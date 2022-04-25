// [09/27/2019 MRB]

#include "App/AppPCH.h"

#include "Core/Container/bcArray.h"
#include "Core/Messaging/Query/bcQueryManager.h"
#include "GraphicImp/Resource/bcResourceBuilder.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/bcDefaultRenderThread.h"
#include "Game/System/Input/bcCamera.h"
#include "Game/System/Input/bcCameraFrustum.h"
#include "Game/System/Input/bcGlobalConfig.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/Object/Scene/Component/bcLightComponent.h"
#include "Game/Object/Scene/SceneGraph/bcSceneGraphBuffer.h"
#include "Game/bcConstant.h"
#include "App/RenderPass/ShadowMap/bcBaseCascadedShadowMapPass.h"

namespace black_cat
{
	struct _bc_parameters_cbuffer
	{
		BC_CBUFFER_ALIGN
		bcUINT32 m_shadow_map_size;
		bcUINT32 m_shadow_map_cascade_count;
		bcUINT32 m_shadow_map_current_cascade;
	};

	bc_base_cascaded_shadow_map_pass::bc_base_cascaded_shadow_map_pass(const bc_base_cascaded_shadow_map_pass* p_other_instance)
		: m_my_index(p_other_instance->m_state->m_instance_count),
		m_state(p_other_instance->m_state)
	{
		m_state->m_instance_count++;
		_reset_cascade_sizes(m_state->m_shadow_map_multiplier, core::bc_make_cspan(m_state->m_cascade_sizes));
	}
	
	bc_base_cascaded_shadow_map_pass::bc_base_cascaded_shadow_map_pass(game::bc_render_pass_variable_t p_output_depth_buffers, 
		bcFLOAT p_shadow_map_multiplier,
		std::initializer_list<bc_cascade_shadow_map_trait> p_cascade_sizes)
		: m_my_index(0)
	{
		m_state = core::bc_make_shared<_bc_cascaded_shadow_map_pass_state>(core::bc_alloc_type::program);
		m_state->m_instance_count = 1;
		m_state->m_back_buffer_width = 0;
		m_state->m_back_buffer_height = 0;
		m_state->m_output_depth_buffers_share_slot = p_output_depth_buffers;
		m_state->m_shadow_map_size = 1;
		
		core::bc_vector<core::bc_any> l_cascade_sizes;
		l_cascade_sizes.reserve(p_cascade_sizes.size() * 2);

		for (auto l_cascade : p_cascade_sizes)
		{
			l_cascade_sizes.push_back(core::bc_any(l_cascade.m_distance));
			l_cascade_sizes.push_back(core::bc_any(l_cascade.m_update_interval));
		}

		auto& l_global_config = bc_get_global_config();
		l_global_config.add_if_not_exist_config_key("render_shadow_map_multiplier", core::bc_any(p_shadow_map_multiplier))
			.add_if_not_exist_config_key("render_shadow_map_cascades", core::bc_any(l_cascade_sizes))
			.flush_changes();

		_reset_cascade_sizes(p_shadow_map_multiplier, core::bc_const_span<bc_cascade_shadow_map_trait>(p_cascade_sizes.begin(), p_cascade_sizes.size()));
	}
	
	void bc_base_cascaded_shadow_map_pass::initialize_resources(game::bc_render_system& p_render_system)
	{
		auto& l_device = p_render_system.get_device();
		auto& l_device_swap_buffer = p_render_system.get_device_swap_buffer();

		m_command_list = p_render_system.get_device().create_command_list();
		initialize_pass(p_render_system);

		if(m_my_index == 0)
		{
			share_resource(m_state->m_output_depth_buffers_share_slot, bc_cascaded_shadow_map_buffer_container());
		}

		after_reset(game::bc_render_pass_reset_context
		(
			p_render_system,
			l_device,
			l_device_swap_buffer,
			graphic::bc_device_parameters
			(
				0,
				0,
				graphic::bc_format::unknown,
				graphic::bc_texture_ms_config(1, 0)
			),
			graphic::bc_device_parameters
			(
				l_device_swap_buffer.get_back_buffer_width(),
				l_device_swap_buffer.get_back_buffer_height(),
				l_device_swap_buffer.get_back_buffer_format(),
				l_device_swap_buffer.get_back_buffer_texture().get_sample_count()
			)
		));
	}
	
	void bc_base_cascaded_shadow_map_pass::update(const game::bc_render_pass_update_context& p_context)
	{
	}

	void bc_base_cascaded_shadow_map_pass::initialize_frame(const game::bc_concurrent_render_pass_render_context& p_context)
	{
		if (m_state->m_cascade_sizes.empty())
		{
			return;
		}

		m_last_cameras.clear();

		if (m_my_index == 0)
		{
			if (m_state->m_lights_query.is_executed())
			{
				m_state->m_lights = m_state->m_lights_query.get().get_lights();
			}
			
			m_state->m_lights_query = p_context.m_query_manager.queue_query
			(
				game::bc_scene_light_query(game::bc_light_type::direct)
			);

			while (m_state->m_light_states.size() < m_state->m_lights.size())
			{
				m_state->m_light_states.push_back(_create_light_state(p_context.m_render_system));
			}

			while (m_state->m_light_states.size() > m_state->m_lights.size())
			{
				m_state->m_light_states.pop_back();
			}

			if(!m_state->m_lights.empty())
			{
				const auto l_cascade_count = m_state->m_cascade_sizes.size();
				for (auto l_cascade_ite = 0U; l_cascade_ite < l_cascade_count; ++l_cascade_ite)
				{
					--std::get<1>(m_state->m_cascade_update_intervals[l_cascade_ite]);
				}
			}
		}

		m_state->wait_for_sync_flag(1);

		if (m_state->m_lights.empty())
		{
			return;
		}

		for (bcSIZE l_light_ite = 0; l_light_ite < m_state->m_lights.size(); ++l_light_ite)
		{
			const auto* l_light = m_state->m_lights[l_light_ite].as_direct_light();
			const auto l_cascade_count = m_state->m_cascade_sizes.size();

			m_last_update_cascade_cameras = _get_light_stabilized_cascades(p_context.m_update_camera, *l_light);
			m_last_render_cascade_cameras = _get_light_stabilized_cascades(p_context.m_render_camera, *l_light);

			for (auto l_cascade_ite = 0U; l_cascade_ite < l_cascade_count; ++l_cascade_ite)
			{
				const auto l_update_interval = std::get<0>(m_state->m_cascade_update_intervals[l_cascade_ite]);
				const auto l_current_update_interval = std::get<1>(m_state->m_cascade_update_intervals[l_cascade_ite]);
				m_last_cameras.push_back(game::bc_camera_instance(m_last_update_cascade_cameras[l_cascade_ite]));
				auto& l_update_cascade_camera = m_last_cameras.back();
				m_last_cameras.push_back(game::bc_camera_instance(m_last_render_cascade_cameras[l_cascade_ite]));
				auto& l_render_cascade_camera = m_last_cameras.back();

				bc_cascaded_shadow_map_pass_init_frame_context l_param
				(
					p_context,
					l_update_cascade_camera,
					l_render_cascade_camera,
					l_light_ite,
					l_cascade_ite,
					l_cascade_count,
					l_update_interval == 1 || (l_update_interval > 1 && l_current_update_interval == 1),
					l_current_update_interval == 0
				);
				initialize_frame_pass(l_param);
			}
		}
	}

	void bc_base_cascaded_shadow_map_pass::execute(const game::bc_concurrent_render_pass_render_context& p_context)
	{
		if(m_state->m_lights.empty() || m_state->m_cascade_sizes.empty())
		{
			return;
		}
		
		p_context.m_child_render_thread.start(*m_command_list);

		for (bcSIZE l_light_ite = 0; l_light_ite < m_state->m_lights.size(); ++l_light_ite)
		{
			auto& l_light_state = m_state->m_light_states[l_light_ite];
			const auto l_cascade_count = m_state->m_cascade_sizes.size();
			
			if (l_light_state.m_render_pass_states[m_my_index].empty())
			{
				l_light_state.m_render_pass_states[m_my_index] = create_render_pass_states
				(
					p_context.m_render_system,
					l_light_state.m_depth_buffer.get(),
					l_light_state.m_depth_buffer_views
				);
			}

			// Update shared resource so gbuffer light map pass can do its job
			if (m_my_index == 0)
			{
				bc_cascaded_shadow_map_buffer_container::entry l_shadow_map_buffer;
				l_shadow_map_buffer.m_shadow_map_size = m_state->m_shadow_map_size;
				l_shadow_map_buffer.m_shadow_map_count = m_state->m_cascade_sizes.size();
				l_shadow_map_buffer.m_resource_view = l_light_state.m_depth_buffer_resource_view.get();
				std::transform
				(
					std::begin(m_state->m_cascade_sizes),
					std::end(m_state->m_cascade_sizes),
					std::back_inserter(l_shadow_map_buffer.m_cascade_sizes),
					[](const bc_cascade_shadow_map_trait& p_cascade)
					{
						return p_cascade.m_distance;
					}
				);

				for (auto l_cascade_ite = 0U; l_cascade_ite < l_cascade_count; ++l_cascade_ite)
				{
					const auto l_update_interval = std::get<1>(m_state->m_cascade_update_intervals[l_cascade_ite]);
					if (l_update_interval == 0)
					{
						const auto& l_render_cascade_camera = m_last_render_cascade_cameras[l_cascade_ite];
						l_light_state.m_last_view_projections[l_cascade_ite] = l_render_cascade_camera.get_view() * l_render_cascade_camera.get_projection();
					}

					l_shadow_map_buffer.m_view_projections.push_back(l_light_state.m_last_view_projections[l_cascade_ite]);
				}

				auto& l_shadow_maps_container = get_shared_resource_throw<bc_cascaded_shadow_map_buffer_container>(m_state->m_output_depth_buffers_share_slot);
				l_shadow_maps_container.add(std::make_pair
				(
					m_state->m_lights[l_light_ite].get_id(),
					l_shadow_map_buffer
				));
			}

			for(auto l_cascade_ite = 0U; l_cascade_ite < l_cascade_count; ++l_cascade_ite)
			{
				const auto l_update_interval = std::get<1>(m_state->m_cascade_update_intervals[l_cascade_ite]);
				if (l_update_interval == 0)
				{
					m_last_cameras.push_back(game::bc_camera_instance(m_last_update_cascade_cameras[l_cascade_ite]));
					auto& l_update_cascade_camera = m_last_cameras.back();
					m_last_cameras.push_back(game::bc_camera_instance(m_last_render_cascade_cameras[l_cascade_ite]));
					auto& l_render_cascade_camera = m_last_cameras.back();

					bc_cascaded_shadow_map_pass_render_context l_param
					(
						p_context,
						l_light_state.m_render_pass_states[m_my_index],
						l_update_cascade_camera,
						l_render_cascade_camera,
						l_light_ite,
						l_cascade_ite,
						l_cascade_count
					);
					execute_pass(l_param);
				}
			}

			if (m_state->m_capture_debug_shapes)
			{
				m_state->m_captured_camera = p_context.m_render_camera.get_extends();

				for (auto& l_cascade_camera : m_last_render_cascade_cameras)
				{
					game::bci_camera::extend l_extends;
					l_cascade_camera.get_extend_points(l_extends);
					
					m_state->m_captured_cascades.push_back(l_extends);
				}

				m_state->m_capture_debug_shapes = false;
			}
		}
		
		p_context.m_child_render_thread.finish();
		
		m_state->wait_for_sync_flag(2);

		if (m_my_index == 0)
		{
			// Restore global cbuffer to its default state
			p_context.m_frame_renderer.update_global_cbuffer(p_context.m_render_thread, p_context.m_clock, p_context.m_render_camera);

			for (auto& l_captured_camera : m_state->m_captured_cascades)
			{
				p_context.m_render_system.get_shape_drawer().draw_wired_frustum(l_captured_camera);
			}
			/*for (auto& l_captured_box : m_state->m_captured_boxes)
			{
				p_param.m_render_system.get_shape_drawer().draw_wired_bound_box(l_captured_box);
			}*/
			p_context.m_render_system.get_shape_drawer().draw_wired_frustum(m_state->m_captured_camera);
		}
	}

	void bc_base_cascaded_shadow_map_pass::cleanup_frame(const game::bc_render_pass_render_context& p_context)
	{
		const auto l_cascade_count = m_state->m_cascade_sizes.size();

		if (m_my_index == 0)
		{
			get_shared_resource_throw<bc_cascaded_shadow_map_buffer_container>(m_state->m_output_depth_buffers_share_slot).clear();
			m_state->reset_sync_flag();

			for (auto l_cascade_ite = 0U; l_cascade_ite < l_cascade_count; ++l_cascade_ite)
			{
				auto& l_update_interval = std::get<1>(m_state->m_cascade_update_intervals[l_cascade_ite]);
				if (l_update_interval == 0)
				{
					l_update_interval = std::get<0>(m_state->m_cascade_update_intervals[l_cascade_ite]);
				}
			}
		}

		for (auto l_light_ite = 0U; l_light_ite < m_state->m_lights.size(); ++l_light_ite)
		{
			auto& l_light_state = m_state->m_light_states[l_light_ite];

			for (auto l_cascade_ite = 0U; l_cascade_ite < l_cascade_count; ++l_cascade_ite)
			{
				const auto l_update_interval = std::get<1>(m_state->m_cascade_update_intervals[l_cascade_ite]);

				if(l_update_interval == 1)
				{
					// Cascade will be rendered next frame and its depth buffer must be cleared
					p_context.m_render_thread.start();
					p_context.m_render_thread.clear_depth_stencil_view(l_light_state.m_depth_buffer_views[l_cascade_ite].get());
					p_context.m_render_thread.finish();
				}
				
				cleanup_frame_pass(bc_cascaded_shadow_map_pass_cleanup_context
				(
					p_context, 
					l_light_state.m_render_pass_states[m_my_index], 
					l_light_ite, 
					l_cascade_ite, 
					l_cascade_count
				));
			}
		}
	}

	void bc_base_cascaded_shadow_map_pass::before_reset(const game::bc_render_pass_reset_context& p_context)
	{
	}

	void bc_base_cascaded_shadow_map_pass::after_reset(const game::bc_render_pass_reset_context& p_context)
	{
		if(my_index() != 0)
		{
			return;
		}

		const auto l_light_state_count = m_state->m_light_states.size();

		m_state->m_back_buffer_width = p_context.m_device_swap_buffer.get_back_buffer_width();
		m_state->m_back_buffer_height = p_context.m_device_swap_buffer.get_back_buffer_height();
		m_state->m_shadow_map_size = (m_state->m_back_buffer_width + m_state->m_back_buffer_height) / 2 * m_state->m_shadow_map_multiplier;
		m_state->m_light_states.clear();

		for(auto l_ite = 0U; l_ite < l_light_state_count; ++l_ite)
		{
			m_state->m_light_states.push_back(_create_light_state(p_context.m_render_system));
		}
	}

	void bc_base_cascaded_shadow_map_pass::destroy(game::bc_render_system& p_render_system)
	{
		destroy_pass(p_render_system);

		if (m_my_index == 0)
		{
			get_shared_resource<bc_cascaded_shadow_map_buffer_container>(m_state->m_output_depth_buffers_share_slot)->clear();
			unshare_resource(m_state->m_output_depth_buffers_share_slot);
		}

		m_command_list.reset();
		m_state->m_light_states.clear();
		m_state.reset();
	}

	void bc_base_cascaded_shadow_map_pass::config_changed(const game::bc_render_pass_config_change_context& p_context)
	{
		if(m_my_index != 0)
		{
			return;
		}

		core::bc_any l_shadow_map_multiplier_param;
		core::bc_any l_cascade_sizes_param;

		p_context.m_global_config
			.read_config_key("render_shadow_map_multiplier", l_shadow_map_multiplier_param)
			.read_config_key("render_shadow_map_cascades", l_cascade_sizes_param);

		if (!l_shadow_map_multiplier_param.cast_to_double().first || !l_cascade_sizes_param.is<core::bc_vector<core::bc_any>>())
		{
			return;
		}

		const auto l_shadow_map_multiplier = static_cast<bcFLOAT>(l_shadow_map_multiplier_param.cast_to_double_throw());
		const auto& l_cascade_sizes = l_cascade_sizes_param.as_throw<core::bc_vector<core::bc_any>>();

		if(l_cascade_sizes.size() % 2 != 0)
		{
			return;
		}
			
		core::bc_vector_frame<bc_cascade_shadow_map_trait> l_cascade_sizes_int;
		l_cascade_sizes_int.reserve(l_cascade_sizes.size());

		for(auto l_ite = 0U; l_ite < l_cascade_sizes.size(); l_ite += 2)
		{
			const auto l_cascade_distance = static_cast<bcUINT32>(l_cascade_sizes[l_ite].cast_to_int_throw());
			const auto l_cascade_update_interval = static_cast<bcUINT32>(l_cascade_sizes[l_ite + 1].cast_to_int_throw());
			l_cascade_sizes_int.push_back(bc_cascade_shadow_map_trait{ l_cascade_distance, l_cascade_update_interval });
		}

		_reset_cascade_sizes(l_shadow_map_multiplier, core::bc_make_cspan(l_cascade_sizes_int));
	}

	void bc_base_cascaded_shadow_map_pass::capture_debug_shapes()
	{
		m_state->m_capture_debug_shapes = true;
		m_state->m_captured_cascades.clear();
		m_state->m_captured_boxes.clear();
	}

	bcSIZE bc_base_cascaded_shadow_map_pass::my_index() const
	{
		return m_my_index;
	}

	void bc_base_cascaded_shadow_map_pass::_reset_cascade_sizes(bcFLOAT p_shadow_map_multiplier, core::bc_const_span<bc_cascade_shadow_map_trait> p_cascades)
	{
		m_state->m_shadow_map_multiplier = p_shadow_map_multiplier;
		m_state->m_shadow_map_size = (m_state->m_back_buffer_width + m_state->m_back_buffer_height) / 2 * m_state->m_shadow_map_multiplier;
		m_state->m_cascade_sizes.assign(std::begin(p_cascades), std::end(p_cascades));
		m_state->m_light_states.clear();
		m_state->m_cascade_update_intervals.assign(p_cascades.size(), { 0,0 });

		std::transform
		(
			std::cbegin(p_cascades),
			std::cend(p_cascades),
			std::begin(m_state->m_cascade_update_intervals),
			[](const bc_cascade_shadow_map_trait& p_cascade_size)
			{
				return std::make_tuple(p_cascade_size.m_update_interval, p_cascade_size.m_update_interval);
			}
		);
	}

	_bc_cascaded_shadow_map_light_state bc_base_cascaded_shadow_map_pass::_create_light_state(game::bc_render_system& p_render_system)
	{
		const auto l_cascade_count = m_state->m_cascade_sizes.size();

		auto l_depth_buffer_config = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_texture2d
			(
				m_state->m_shadow_map_size,
				m_state->m_shadow_map_size,
				false,
				1,
				graphic::bc_format::D16_UNORM,
				graphic::bc_resource_usage::gpu_rw,
				core::bc_enum::mask_or({ graphic::bc_resource_view_type::depth_stencil, graphic::bc_resource_view_type::shader }),
				1,
				0,
				l_cascade_count
			).as_depth_stencil_texture();
		auto l_depth_buffer_view_builder = graphic::bc_graphic_resource_builder()
			.as_resource_view()
			.as_texture_view(graphic::bc_format::D16_UNORM);
		auto l_depth_buffer_resource_view_config = graphic::bc_graphic_resource_builder()
			.as_resource_view()
			.as_texture_view(graphic::bc_format::D16_UNORM)
			.as_tex2d_array_shader_view(0, 1, 0, l_cascade_count)
			.on_depth_stencil_texture2d();

		_bc_cascaded_shadow_map_light_state l_instance;
		l_instance.m_depth_buffer = p_render_system.get_device().create_texture2d(l_depth_buffer_config, nullptr);
		l_instance.m_depth_buffer_resource_view = p_render_system.get_device().create_resource_view(l_instance.m_depth_buffer.get(), l_depth_buffer_resource_view_config);
		l_instance.m_depth_buffer_views.reserve(l_cascade_count);
		l_instance.m_render_pass_states.resize(m_state->m_instance_count);
		l_instance.m_last_view_projections.reserve(l_cascade_count);

		for (auto l_ite = 0U; l_ite < l_cascade_count; ++l_ite)
		{
			auto l_depth_stencil_view_config = l_depth_buffer_view_builder.as_tex2d_array_depth_stencil_view(0, l_ite, 1);
			auto l_depth_stencil_view = p_render_system.get_device().create_depth_stencil_view(l_instance.m_depth_buffer.get(), l_depth_stencil_view_config);
			
			l_instance.m_last_view_projections.push_back(core::bc_matrix4f::identity());
			l_instance.m_depth_buffer_views.push_back(std::move(l_depth_stencil_view));
		}
		
		return l_instance;
	}

	core::bc_vector<bc_cascaded_shadow_map_camera> bc_base_cascaded_shadow_map_pass::_get_light_cascades(const game::bci_camera& p_camera, const game::bc_direct_light& p_light)
	{
		game::bci_camera::extend l_camera_frustum_corners;
		p_camera.get_extend_points(l_camera_frustum_corners);

		core::bc_vector_frame<bcFLOAT> l_cascade_break_points;
		l_cascade_break_points.reserve(m_state->m_cascade_sizes.size() + 1);
		l_cascade_break_points.push_back(p_camera.get_near_clip());
		std::transform
		(
			std::begin(m_state->m_cascade_sizes),
			std::end(m_state->m_cascade_sizes),
			std::back_inserter(l_cascade_break_points),
			[](const bc_cascade_shadow_map_trait& p_cascade)
			{
				return p_cascade.m_distance;
			}
		);

		core::bc_vector<bc_cascaded_shadow_map_camera> l_cascade_cameras;
		l_cascade_cameras.reserve(l_cascade_break_points.size());

		const auto l_camera_position = p_camera.get_position();
		const auto l_camera_far_plane_distance = p_camera.get_far_clip();
		const auto l_lower_left_ray = l_camera_frustum_corners[4] - l_camera_frustum_corners[0];
		const auto l_upper_left_ray = l_camera_frustum_corners[5] - l_camera_frustum_corners[1];
		const auto l_upper_right_ray = l_camera_frustum_corners[6] - l_camera_frustum_corners[2];
		const auto l_lower_right_ray = l_camera_frustum_corners[7] - l_camera_frustum_corners[3];

		for (bcUINT32 l_cascade_index = 0; l_cascade_index < l_cascade_break_points.size() - 1; ++l_cascade_index)
		{
			const auto l_mid_z = (l_cascade_break_points[l_cascade_index] + l_cascade_break_points[l_cascade_index + 1]) / 2;
			const auto l_min_z = l_cascade_break_points[l_cascade_index];
			const auto l_max_z = l_cascade_break_points[l_cascade_index + 1];

			const auto l_cascade_camera_min_z_distance_ratio = (l_min_z / l_camera_far_plane_distance);
			const auto l_cascade_camera_max_z_distance_ratio = (l_max_z / l_camera_far_plane_distance);

			game::bci_camera::extend l_frustum_points
			{
				l_lower_left_ray * l_cascade_camera_min_z_distance_ratio + l_camera_position,
				l_upper_left_ray * l_cascade_camera_min_z_distance_ratio + l_camera_position,
				l_upper_right_ray * l_cascade_camera_min_z_distance_ratio + l_camera_position,
				l_lower_right_ray * l_cascade_camera_min_z_distance_ratio + l_camera_position,
				l_lower_left_ray * l_cascade_camera_max_z_distance_ratio + l_camera_position,
				l_upper_left_ray * l_cascade_camera_max_z_distance_ratio + l_camera_position,
				l_upper_right_ray * l_cascade_camera_max_z_distance_ratio + l_camera_position,
				l_lower_right_ray * l_cascade_camera_max_z_distance_ratio + l_camera_position
			};

			core::bc_vector3f l_min(std::numeric_limits<bcFLOAT>::max());
			core::bc_vector3f l_max(std::numeric_limits<bcFLOAT>::lowest());
			for (auto& l_frustum_point : l_frustum_points)
			{
				l_min.x = std::min(l_frustum_point.x, l_min.x);
				l_min.y = std::min(l_frustum_point.y, l_min.y);
				l_min.z = std::min(l_frustum_point.z, l_min.z);
				l_max.x = std::max(l_frustum_point.x, l_max.x);
				l_max.y = std::max(l_frustum_point.y, l_max.y);
				l_max.z = std::max(l_frustum_point.z, l_max.z);
			}

			const auto l_frustum_center = p_camera.get_position() + p_camera.get_direction() * l_mid_z;
			const auto l_cascade_camera_pos = l_frustum_center + (-p_light.get_direction() * m_state->m_cascade_cameras_distance);
			//const auto l_light_frustum_center = std::accumulate(std::cbegin(l_frustum_points), std::cend(l_frustum_points), core::bc_vector3f(0)) / 8.0f;
			//const auto l_light_camera_look = l_light_frustum_center + p_light.get_direction();

			auto l_cascade_view_matrix = core::bc_matrix4f::look_at_matrix_lh(l_cascade_camera_pos, l_frustum_center, core::bc_vector3f::up());
			//auto l_cascade_view_matrix = core::bc_matrix4f::look_at_matrix_lh(l_light_frustum_center, l_light_camera_look, core::bc_vector3f::up());
			game::bci_camera::extend l_frustum_points_vs
			{
				(l_cascade_view_matrix * core::bc_vector4f(l_frustum_points[0], 1)).xyz(),
				(l_cascade_view_matrix * core::bc_vector4f(l_frustum_points[1], 1)).xyz(),
				(l_cascade_view_matrix * core::bc_vector4f(l_frustum_points[2], 1)).xyz(),
				(l_cascade_view_matrix * core::bc_vector4f(l_frustum_points[3], 1)).xyz(),
				(l_cascade_view_matrix * core::bc_vector4f(l_frustum_points[4], 1)).xyz(),
				(l_cascade_view_matrix * core::bc_vector4f(l_frustum_points[5], 1)).xyz(),
				(l_cascade_view_matrix * core::bc_vector4f(l_frustum_points[6], 1)).xyz(),
				(l_cascade_view_matrix * core::bc_vector4f(l_frustum_points[7], 1)).xyz(),
			};

			core::bc_vector3f l_min_vs(std::numeric_limits<bcFLOAT>::max());
			core::bc_vector3f l_max_vs(std::numeric_limits<bcFLOAT>::lowest());
			for (auto& l_frustum_point : l_frustum_points_vs)
			{
				l_min_vs.x = std::min(l_frustum_point.x, l_min_vs.x);
				l_min_vs.y = std::min(l_frustum_point.y, l_min_vs.y);
				l_min_vs.z = std::min(l_frustum_point.z, l_min_vs.z);
				l_max_vs.x = std::max(l_frustum_point.x, l_max_vs.x);
				l_max_vs.y = std::max(l_frustum_point.y, l_max_vs.y);
				l_max_vs.z = std::max(l_frustum_point.z, l_max_vs.z);
			}

			//const auto l_light_camera_pos = l_light_frustum_center + (-p_light.get_direction() * -l_min_vs.z);
			//const auto l_light_cascade_extends = l_max_vs - l_min_vs;
			l_cascade_cameras.push_back(bc_cascaded_shadow_map_camera(l_cascade_camera_pos, l_frustum_center, l_max_vs.x - l_min_vs.x, l_max_vs.y - l_min_vs.y, 0.1f, l_max_vs.z));
			//l_cascade_cameras.push_back(bc_cascaded_shadow_map_camera(l_light_camera_pos, l_light_frustum_center, l_light_cascade_extends.x, l_light_cascade_extends.y, 0.1, l_light_cascade_extends.z));

			if (m_state->m_capture_debug_shapes)
			{
				m_state->m_captured_boxes.push_back(physics::bc_bound_box::from_min_max(l_min, l_max));
				//m_captured_cascades.push_back(l_frustum_points);
			}
		}

		return l_cascade_cameras;
	}

	core::bc_vector<bc_cascaded_shadow_map_camera> bc_base_cascaded_shadow_map_pass::_get_light_stabilized_cascades(const game::bc_camera_instance& p_camera, const game::bc_direct_light& p_light)
	{
		const auto& l_camera_extends = p_camera.get_extends();

		core::bc_vector_frame<bcFLOAT> l_cascade_break_points;
		l_cascade_break_points.reserve(m_state->m_cascade_sizes.size() + 1);
		l_cascade_break_points.push_back(p_camera.get_near_clip());
		std::transform
		(
			std::begin(m_state->m_cascade_sizes),
			std::end(m_state->m_cascade_sizes),
			std::back_inserter(l_cascade_break_points),
			[](const bc_cascade_shadow_map_trait& p_cascade)
			{
				return p_cascade.m_distance;
			}
		);

		core::bc_vector<bc_cascaded_shadow_map_camera> l_cascade_cameras;
		l_cascade_cameras.reserve(l_cascade_break_points.size());

		const auto l_camera_position = p_camera.get_position();
		const auto l_camera_far_plane_distance = p_camera.get_far_clip();
		const auto l_lower_left_ray = l_camera_extends[4] - l_camera_extends[0];
		const auto l_upper_left_ray = l_camera_extends[5] - l_camera_extends[1];
		const auto l_upper_right_ray = l_camera_extends[6] - l_camera_extends[2];
		const auto l_lower_right_ray = l_camera_extends[7] - l_camera_extends[3];

		for (bcUINT32 l_cascade_index = 0; l_cascade_index < l_cascade_break_points.size() - 1; ++l_cascade_index)
		{
			const auto l_mid_z = (l_cascade_break_points[l_cascade_index] + l_cascade_break_points[l_cascade_index + 1]) / 2;
			const auto l_min_z = l_cascade_break_points[l_cascade_index];
			const auto l_max_z = l_cascade_break_points[l_cascade_index + 1];

			const auto l_cascade_camera_min_z_distance_ratio = (l_min_z / l_camera_far_plane_distance);
			const auto l_cascade_camera_max_z_distance_ratio = (l_max_z / l_camera_far_plane_distance);

			game::bci_camera::extend l_frustum_points
			{
				l_lower_left_ray * l_cascade_camera_min_z_distance_ratio + l_camera_position,
				l_upper_left_ray * l_cascade_camera_min_z_distance_ratio + l_camera_position,
				l_upper_right_ray * l_cascade_camera_min_z_distance_ratio + l_camera_position,
				l_lower_right_ray * l_cascade_camera_min_z_distance_ratio + l_camera_position,
				l_lower_left_ray * l_cascade_camera_max_z_distance_ratio + l_camera_position,
				l_upper_left_ray * l_cascade_camera_max_z_distance_ratio + l_camera_position,
				l_upper_right_ray * l_cascade_camera_max_z_distance_ratio + l_camera_position,
				l_lower_right_ray * l_cascade_camera_max_z_distance_ratio + l_camera_position
			};

			const auto l_frustum_center = p_camera.get_position() + p_camera.get_direction() * l_mid_z;

			auto l_frustum_sphere_radius = 0.0f;
			for (const auto& l_frustum_point : l_frustum_points)
			{
				const auto l_frustum_point_distance = (l_frustum_point - l_frustum_center).magnitude();
				l_frustum_sphere_radius = std::max(l_frustum_sphere_radius, l_frustum_point_distance);
			}

			const auto l_max = core::bc_vector3f(l_frustum_sphere_radius);
			const auto l_min = -l_max;
			const auto l_width = l_max.x - l_min.x;
			const auto l_height = l_max.y - l_min.y;
			const auto l_depth = l_frustum_sphere_radius;
			//const auto l_camera_back_distance = -l_min.z;
			const auto l_camera_back_distance = std::max(l_depth, m_state->m_cascade_cameras_distance);
			const auto l_camera_pos = l_frustum_center + (-p_light.get_direction() * l_camera_back_distance);
			const auto l_far_plane = (l_frustum_center + (p_light.get_direction() * l_depth) - l_camera_pos).magnitude();
			//const auto l_far_plane = l_max.z - l_min.z;

			auto l_cascade_camera = bc_cascaded_shadow_map_camera(l_camera_pos, l_frustum_center, l_width, l_height, 0.1f, l_far_plane);

			const auto l_cascade_camera_view_proj = l_cascade_camera.get_view() * l_cascade_camera.get_projection();
			const auto l_origin = (l_cascade_camera_view_proj * core::bc_vector4f(0, 0, 0, 1)) * (m_state->m_shadow_map_size / 2.0f);
			const auto l_rounded_origin = core::bc_vector4f(std::round(l_origin.x), std::round(l_origin.y), std::round(l_origin.z), std::round(l_origin.w));
			auto l_rounded_offset = (l_rounded_origin - l_origin) * (2.0f / m_state->m_shadow_map_size);

			l_cascade_camera.set_offset(l_rounded_offset.x, l_rounded_offset.y);

			l_cascade_cameras.push_back(std::move(l_cascade_camera));

			if (m_state->m_capture_debug_shapes)
			{
				core::bc_vector3f l_frustum_min(std::numeric_limits<bcFLOAT>::max());
				core::bc_vector3f l_frustum_max(std::numeric_limits<bcFLOAT>::lowest());
				for (auto& l_frustum_point : l_frustum_points)
				{
					l_frustum_min.x = std::min(l_frustum_point.x, l_frustum_min.x);
					l_frustum_min.y = std::min(l_frustum_point.y, l_frustum_min.y);
					l_frustum_min.z = std::min(l_frustum_point.z, l_frustum_min.z);
					l_frustum_max.x = std::max(l_frustum_point.x, l_frustum_max.x);
					l_frustum_max.y = std::max(l_frustum_point.y, l_frustum_max.y);
					l_frustum_max.z = std::max(l_frustum_point.z, l_frustum_max.z);
				}

				m_state->m_captured_boxes.push_back(physics::bc_bound_box::from_min_max(l_frustum_min, l_frustum_max));
				//m_captured_cascades.push_back(l_frustum_points);
			}
		}

		return l_cascade_cameras;
	}
}
