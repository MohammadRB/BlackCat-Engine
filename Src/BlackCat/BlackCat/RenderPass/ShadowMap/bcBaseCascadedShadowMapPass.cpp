// [09/27/2019 MRB]

#include "BlackCat/BlackCatPCH.h"

#include "Core/Container/bcArray.h"
#include "Core/Messaging/Query/bcQueryManager.h"
#include "GraphicImp/Resource/bcResourceBuilder.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Input/bcCamera.h"
#include "Game/System/Input/bcCameraFrustum.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/Object/Scene/Component/bcLightComponent.h"
#include "Game/Object/Scene/SceneGraph/bcSceneGraphBuffer.h"
#include "BlackCat/RenderPass/ShadowMap/bcBaseCascadedShadowMapPass.h"

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
	}
	
	bc_base_cascaded_shadow_map_pass::bc_base_cascaded_shadow_map_pass(constant::bc_render_pass_variable_t p_output_depth_buffers, 
		bcSIZE p_shadow_map_size,
		std::initializer_list<std::tuple<bcSIZE, bcUBYTE>> p_cascade_sizes)
		: m_my_index(0)
	{
		m_state = core::bc_make_shared<_bc_cascaded_shadow_map_pass_state>(core::bc_alloc_type::program);
		m_state->m_instance_count = 1;
		m_state->m_output_depth_buffers_share_slot = p_output_depth_buffers;
		m_state->m_shadow_map_size = p_shadow_map_size;
		m_state->m_cascade_sizes.assign(p_cascade_sizes.size(), 0);
		m_state->m_cascade_update_intervals.assign(p_cascade_sizes.size(), {0,0});

		std::transform
		(
			std::cbegin(p_cascade_sizes),
			std::cend(p_cascade_sizes),
			std::begin(m_state->m_cascade_sizes),
			[](const std::tuple< bcSIZE, bcUBYTE >& p_cascade_size)
			{
				return std::get< 0 >(p_cascade_size);
			}
		);
		std::transform
		(
			std::cbegin(p_cascade_sizes),
			std::cend(p_cascade_sizes),
			std::begin(m_state->m_cascade_update_intervals),
			[](const std::tuple< bcSIZE, bcUBYTE >& p_cascade_size)
			{
				return std::make_tuple(std::get< 1 >(p_cascade_size), std::get< 1 >(p_cascade_size));
			}
		);
	}
	
	void bc_base_cascaded_shadow_map_pass::initialize_resources(game::bc_render_system& p_render_system)
	{
		m_state->m_command_list = p_render_system.get_device().create_command_list();

		initialize_pass(p_render_system);

		if(m_my_index == 0)
		{
			share_resource(m_state->m_output_depth_buffers_share_slot, bc_cascaded_shadow_map_buffer_container());
		}
	}
	
	void bc_base_cascaded_shadow_map_pass::update(const game::bc_render_pass_update_param& p_param)
	{
	}

	void bc_base_cascaded_shadow_map_pass::initialize_frame(const game::bc_render_pass_render_param& p_param)
	{
	}

	void bc_base_cascaded_shadow_map_pass::execute(const game::bc_render_pass_render_param& p_param)
	{
		if(m_state->m_scene_query.is_executed())
		{
			m_state->m_scene_query_result = m_state->m_scene_query.get().get_scene_buffer();
		}

		if(!m_state->m_scene_query_result.is_set())
		{
			return;
		}
		
		core::bc_vector_frame<game::bc_direct_light> l_direct_lights;

		auto& l_light_actors = m_state->m_scene_query_result.get();
		for (auto& l_actor : l_light_actors)
		{
			auto* l_light_component = l_actor.get_component<game::bc_light_component>();
			auto* l_light = l_light_component->get_light();

			if (l_light->get_type() == game::bc_light_type::direct)
			{
				l_direct_lights.push_back(*l_light->as_direct_light());
			}
		}

		while (m_state->m_light_instance_states.size() < l_direct_lights.size())
		{
			m_state->m_light_instance_states.push_back(_create_light_instance(p_param.m_render_system));
		}

		while (m_state->m_light_instance_states.size() > l_direct_lights.size())
		{
			m_state->m_light_instance_states.pop_back();
		}

		p_param.m_render_thread.start(m_state->m_command_list.get());

		for (bcSIZE l_light_ite = 0; l_light_ite < l_direct_lights.size(); ++l_light_ite)
		{
			auto& l_light = l_direct_lights[l_light_ite];
			auto& l_light_state = m_state->m_light_instance_states[l_light_ite];
			auto& l_shadow_maps_container = get_shared_resource_throw< bc_cascaded_shadow_map_buffer_container >(m_state->m_output_depth_buffers_share_slot);

			// Create render pass state instance
			if(l_light_state.m_render_pass_states.size() < m_my_index + 1 || l_light_state.m_render_pass_states[m_my_index].empty())
			{
				l_light_state.m_render_pass_states.resize(std::max(m_my_index + 1, l_light_state.m_render_pass_states.size()));
				l_light_state.m_render_pass_states[m_my_index] = create_render_pass_states
				(
					p_param.m_render_system, 
					l_light_state.m_depth_buffer.get(), 
					l_light_state.m_depth_buffer_views
				);
			}
			
			auto l_cascade_ite = 0U;
			auto l_light_cascade_cameras = _get_light_stabilized_cascades(p_param.m_camera, l_light);
			
			if (m_my_index == 0)
			{
				bc_cascaded_shadow_map_buffer_container::entry l_shadow_map_buffer;
				l_shadow_map_buffer.m_shadow_map_size = m_state->m_shadow_map_size;
				l_shadow_map_buffer.m_shadow_map_count = m_state->m_cascade_sizes.size();
				l_shadow_map_buffer.m_resource_view = l_light_state.m_depth_buffer_resource_view.get();
				std::copy(std::begin(m_state->m_cascade_sizes), std::end(m_state->m_cascade_sizes), std::back_inserter(l_shadow_map_buffer.m_cascade_sizes));

				for (auto& l_light_cascade_camera : l_light_cascade_cameras)
				{
					const auto l_update_interval = --std::get<1>(m_state->m_cascade_update_intervals[l_cascade_ite]);
					if (l_update_interval == 0)
					{
						l_light_state.m_last_view_projections[l_cascade_ite] = l_light_cascade_camera.get_view() * l_light_cascade_camera.get_projection();
					}
					
					l_shadow_map_buffer.m_view_projections.push_back(l_light_state.m_last_view_projections[l_cascade_ite]);
					++l_cascade_ite;
				}
				
				l_shadow_maps_container.add(std::make_pair
				(
					l_light.get_direction(),
					l_shadow_map_buffer
				));
			}

			for(l_cascade_ite = 0U; l_cascade_ite < l_light_cascade_cameras.size(); ++l_cascade_ite)
			{
				auto l_light_cascade_camera = game::bc_camera_instance(l_light_cascade_cameras[l_cascade_ite]);
				auto& l_update_interval = std::get<1>(m_state->m_cascade_update_intervals[l_cascade_ite]);
				if (l_update_interval == 0)
				{
					if (m_my_index == m_state->m_instance_count - 1) // If this is the last instance
					{
						l_update_interval = std::get<0>(m_state->m_cascade_update_intervals[l_cascade_ite]);
					}
					
					// Update global cbuffer with cascade camera
					p_param.m_frame_renderer.update_global_cbuffer(p_param.m_render_thread, p_param.m_clock, l_light_cascade_camera);

					bc_cascaded_shadow_map_pass_render_param l_param
					(
						p_param,
						l_light_state.m_render_pass_states[m_my_index],
						l_light_cascade_camera,
						l_light_ite,
						l_cascade_ite,
						l_light_cascade_cameras.size()
					);
					execute_pass(l_param);
				}
			}

			if (m_state->m_capture_debug_shapes)
			{
				m_state->m_captured_camera = p_param.m_camera.get_extends();

				for (auto& l_cascade_camera : l_light_cascade_cameras)
				{
					game::bc_icamera::extend l_extends;
					l_cascade_camera.get_extend_points(l_extends);
					
					m_state->m_captured_cascades.push_back(l_extends);
				}

				m_state->m_capture_debug_shapes = false;
			}
		}

		// Restore global cbuffer to its default state
		p_param.m_frame_renderer.update_global_cbuffer(p_param.m_render_thread, p_param.m_clock, p_param.m_camera);

		p_param.m_render_thread.finish();
		m_state->m_command_list->finished();

		if(m_my_index == 0)
		{
			for (auto& l_captured_camera : m_state->m_captured_cascades)
			{
				p_param.m_render_system.get_shape_drawer().render_wired_frustum(l_captured_camera);
			}
			for (auto& l_captured_box : m_state->m_captured_boxes)
			{
				p_param.m_render_system.get_shape_drawer().render_wired_box(l_captured_box);
			}
			//p_param.m_render_system.get_shape_drawer().render_wired_frustum(m_captured_camera);	
		}
	}

	void bc_base_cascaded_shadow_map_pass::cleanup_frame(const game::bc_render_pass_render_param& p_param)
	{
		if(m_my_index == 0)
		{
			get_shared_resource_throw<bc_cascaded_shadow_map_buffer_container>(m_state->m_output_depth_buffers_share_slot).clear();

			m_state->m_scene_query = core::bc_get_service<core::bc_query_manager>()->queue_query
			(
				game::bc_scene_graph_query().only<game::bc_light_component>()
			);
			m_state->m_scene_query_result.reset();
		}
	}

	void bc_base_cascaded_shadow_map_pass::before_reset(const game::bc_render_pass_reset_param& p_param)
	{
	}

	void bc_base_cascaded_shadow_map_pass::after_reset(const game::bc_render_pass_reset_param& p_param)
	{
	}

	void bc_base_cascaded_shadow_map_pass::destroy(game::bc_render_system& p_render_system)
	{
		if (m_my_index == 0)
		{
			get_shared_resource<bc_cascaded_shadow_map_buffer_container>(m_state->m_output_depth_buffers_share_slot)->clear();
			unshare_resource(m_state->m_output_depth_buffers_share_slot);
		}
		
		m_state->m_light_instance_states.clear();
		m_state->m_command_list.reset();

		destroy_pass(p_render_system);

		m_state.reset();
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
	
	_bc_cascaded_shadow_map_light_state bc_base_cascaded_shadow_map_pass::_create_light_instance(game::bc_render_system& p_render_system)
	{
		_bc_cascaded_shadow_map_light_state l_instance;

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
				core::bc_enum:: or ({ graphic::bc_resource_view_type::depth_stencil, graphic::bc_resource_view_type::shader }),
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

		l_instance.m_depth_buffer = p_render_system.get_device().create_texture2d(l_depth_buffer_config, nullptr);
		l_instance.m_depth_buffer_resource_view = p_render_system.get_device().create_resource_view(l_instance.m_depth_buffer.get(), l_depth_buffer_resource_view_config);
		l_instance.m_last_view_projections.reserve(l_cascade_count);
		l_instance.m_depth_buffer_views.reserve(l_cascade_count);

		for (auto l_ite = 0U; l_ite < l_cascade_count; ++l_ite)
		{
			auto l_depth_stencil_view_config = l_depth_buffer_view_builder.as_tex2d_array_depth_stencil_view(0, l_ite, 1);
			auto l_depth_stencil_view = p_render_system.get_device().create_depth_stencil_view(l_instance.m_depth_buffer.get(), l_depth_stencil_view_config);
			
			l_instance.m_last_view_projections.push_back(core::bc_matrix4f::identity());
			l_instance.m_depth_buffer_views.push_back(std::move(l_depth_stencil_view));
		}

		return l_instance;
	}

	core::bc_vector_frame<bc_cascaded_shadow_map_camera> bc_base_cascaded_shadow_map_pass::_get_light_cascades(const game::bc_icamera& p_camera, const game::bc_direct_light& p_light)
	{
		game::bc_icamera::extend l_camera_frustum_corners;
		p_camera.get_extend_points(l_camera_frustum_corners);

		core::bc_vector_frame<bcFLOAT> l_cascade_break_points;
		l_cascade_break_points.push_back(p_camera.get_near_clip());
		l_cascade_break_points.insert(std::cend(l_cascade_break_points), std::begin(m_state->m_cascade_sizes), std::end(m_state->m_cascade_sizes));

		core::bc_vector_frame<bc_cascaded_shadow_map_camera> l_cascade_cameras;

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

			game::bc_icamera::extend l_frustum_points
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
			game::bc_icamera::extend l_frustum_points_vs
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
			l_cascade_cameras.push_back(bc_cascaded_shadow_map_camera(l_cascade_camera_pos, l_frustum_center, l_max_vs.x - l_min_vs.x, l_max_vs.y - l_min_vs.y, 0.1, l_max_vs.z));
			//l_cascade_cameras.push_back(bc_cascaded_shadow_map_camera(l_light_camera_pos, l_light_frustum_center, l_light_cascade_extends.x, l_light_cascade_extends.y, 0.1, l_light_cascade_extends.z));

			if (m_state->m_capture_debug_shapes)
			{
				m_state->m_captured_boxes.push_back(physics::bc_bound_box::from_min_max(l_min, l_max));
				//m_captured_cascades.push_back(l_frustum_points);
			}
		}

		return l_cascade_cameras;
	}

	core::bc_vector_frame<bc_cascaded_shadow_map_camera> bc_base_cascaded_shadow_map_pass::_get_light_stabilized_cascades(const game::bc_camera_instance& p_camera, const game::bc_direct_light& p_light)
	{
		const auto& l_camera_extends = p_camera.get_extends();

		core::bc_vector_frame<bcFLOAT> l_cascade_break_points;
		l_cascade_break_points.push_back(p_camera.get_near_clip());
		l_cascade_break_points.insert(std::cend(l_cascade_break_points), std::begin(m_state->m_cascade_sizes), std::end(m_state->m_cascade_sizes));

		core::bc_vector_frame<bc_cascaded_shadow_map_camera> l_cascade_cameras;

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

			game::bc_icamera::extend l_frustum_points
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
			const auto l_width = (l_max.x - l_min.x) * .92f;
			const auto l_height = (l_max.y - l_min.y) * .92f;
			const auto l_depth = (l_frustum_sphere_radius) * .92f;
			//const auto l_camera_back_distance = -l_min.z;
			const auto l_camera_back_distance = std::max(l_depth, m_state->m_cascade_cameras_distance);
			const auto l_camera_pos = l_frustum_center + (-p_light.get_direction() * l_camera_back_distance);
			const auto l_far_plane = (l_frustum_center + (p_light.get_direction() * l_depth) - l_camera_pos).magnitude();
			//const auto l_far_plane = l_max.z - l_min.z;

			auto l_cascade_camera = bc_cascaded_shadow_map_camera(l_camera_pos, l_frustum_center, l_width, l_height, 0.1, l_far_plane);

			const auto l_cascade_camera_view_proj = l_cascade_camera.get_view() * l_cascade_camera.get_projection();
			const auto l_origin = (l_cascade_camera_view_proj * core::bc_vector4f(0, 0, 0, 1)) * (m_state->m_shadow_map_size / 2.0f);
			const auto l_rounded_origin = core::bc_vector4f(std::round(l_origin.x), std::round(l_origin.y), std::round(l_origin.z), std::round(l_origin.w));
			auto l_rounded_offset = (l_rounded_origin - l_origin) * (2.0f / m_state->m_shadow_map_size);

			auto l_cascade_camera_proj = l_cascade_camera.get_projection();
			l_cascade_camera_proj(3, 0) += l_rounded_offset.x;
			l_cascade_camera_proj(3, 1) += l_rounded_offset.y;

			l_cascade_camera.set_projection(l_cascade_camera_proj);

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