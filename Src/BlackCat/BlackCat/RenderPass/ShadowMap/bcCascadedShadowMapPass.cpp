// [06/07/2019 MRB]

#include "BlackCat/BlackCatPCH.h"

#include "Core/Container/bcArray.h"
#include "GraphicImp/Resource/bcResourceConfig.h"
#include "Game/System/Input/bcCamera.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"
#include "Game/Object/Scene/Component/bcLightComponent.h"
#include "BlackCat/RenderPass/ShadowMap/bcCascadedShadowMapPass.h"

namespace black_cat
{
	struct _bc_parameters_cbuffer
	{
		BC_CBUFFER_ALIGN
		bcUINT32 m_shadow_map_size;
		bcUINT32 m_shadow_map_cascade_count;
		bcUINT32 m_shadow_map_current_cascade;
	};

	bc_cascaded_shadow_map_pass::bc_cascaded_shadow_map_pass(constant::bc_render_pass_variable_t p_output_depth_buffers, bcSIZE p_shadow_map_size, std::initializer_list<bcSIZE> p_cascade_sizes)
		: m_shadow_map_size(p_shadow_map_size),
		m_cascade_sizes(p_cascade_sizes),
		m_depth_buffers_share_slot(p_output_depth_buffers)
	{
	}

	void bc_cascaded_shadow_map_pass::initialize_resources(game::bc_render_system& p_render_system)
	{
		m_command_list = p_render_system.get_device().create_command_list();
		m_device_pipeline = p_render_system.create_device_pipeline_state
		(
			"csm_vs",
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			game::bc_vertex_type::pos_tex_nor_tan,
			game::bc_blend_type::opaque,
			game::bc_depth_stencil_type::depth_less_stencil_off,
			game::bc_rasterizer_type::fill_solid_cull_back,
			0x01,
			{},
			graphic::bc_format::D32_FLOAT,
			game::bc_multi_sample_type::c1_q1
		);

		auto l_parameters_cbuffer_config = graphic::bc_graphic_resource_configure()
			.as_resource()
			.as_buffer(1, sizeof(_bc_parameters_cbuffer), graphic::bc_resource_usage::gpu_rw)
			.as_constant_buffer();
		m_parameters_cbuffer = p_render_system.get_device().create_buffer(l_parameters_cbuffer_config, nullptr);

		share_resource(m_depth_buffers_share_slot, bc_cascaded_shadow_map_buffer_container());
	}

	void bc_cascaded_shadow_map_pass::update(const game::bc_render_pass_update_param& p_param)
	{
	}

	void bc_cascaded_shadow_map_pass::initialize_frame(const game::bc_render_pass_render_param& p_param)
	{
		p_param.m_render_thread.start(m_command_list.get());
	}

	void bc_cascaded_shadow_map_pass::execute(const game::bc_render_pass_render_param& p_param)
	{
		core::bc_vector_frame<game::bc_direct_light> l_direct_lights;
		
		auto l_light_actors = p_param.m_scene.get_actors<game::bc_light_component>();
		for(auto& l_actor : l_light_actors)
		{
			auto* l_light_component = l_actor.get_component<game::bc_light_component>();
			auto* l_light = l_light_component->get_light();
			
			if(l_light->get_type() == game::bc_light_type::direct)
			{
				l_direct_lights.push_back(*l_light->as_direct_light());
			}
		}

		while(m_light_instance_states.size() < l_direct_lights.size())
		{
			m_light_instance_states.push_back(_create_light_instance(p_param.m_render_system));
		}

		while(m_light_instance_states.size() > l_direct_lights.size())
		{
			m_light_instance_states.pop_back();
		}

		for(bcSIZE l_light_ite = 0; l_light_ite < l_direct_lights.size(); ++l_light_ite)
		{
			auto& l_light = l_direct_lights[l_light_ite];
			auto& l_light_state = m_light_instance_states[l_light_ite];
			auto* l_light_render_pass_state = l_light_state.m_render_pass_state.get();

			p_param.m_render_thread.bind_render_pass_state(l_light_render_pass_state);

			if(l_light_ite == 0)
			{
				p_param.m_render_thread.clear_buffers(core::bc_vector4f(1));
			}

			auto& l_depth_maps_container = get_shared_resource_throw<bc_cascaded_shadow_map_buffer_container>(m_depth_buffers_share_slot);
			bc_cascaded_shadow_map_buffer_container::entry l_csm_buffer;
			l_csm_buffer.m_shadow_map_size = m_shadow_map_size;
			l_csm_buffer.m_shadow_map_count = m_cascade_sizes.size();
			l_csm_buffer.m_resource_view = l_light_state.m_depth_buffer_resource_view.get();
			std::copy(std::begin(m_cascade_sizes), std::end(m_cascade_sizes), std::back_inserter(l_csm_buffer.m_cascade_sizes));

			auto l_cascade_ite = 0U;
			auto l_light_cascade_cameras = _get_light_cascades(p_param.m_camera, l_light);

			for(auto& l_light_cascade_camera : l_light_cascade_cameras)
			{
				_bc_parameters_cbuffer l_parameters{ m_shadow_map_size, m_cascade_sizes.size(), l_cascade_ite };
				p_param.m_render_thread.update_subresource(m_parameters_cbuffer.get(), 0, &l_parameters, 0, 0);
				
				game::bc_camera_frustum l_camera_frustum(l_light_cascade_camera);
				auto l_scene_buffer = p_param.m_scene.get_actors<game::bc_mesh_component>(l_camera_frustum);

				l_scene_buffer.render_actors(p_param.m_render_system);
				
				p_param.m_render_system.render_all_instances(p_param.m_render_thread, p_param.m_clock, l_light_cascade_camera);
				p_param.m_render_system.clear_render_instances();

				l_csm_buffer.m_view_projections.push_back(l_light_cascade_camera.get_view() * l_light_cascade_camera.get_projection());
				++l_cascade_ite;
			}
			
			p_param.m_render_thread.unbind_render_pass_state(l_light_render_pass_state);

			l_depth_maps_container.add(std::make_pair
			(
				l_light.get_direction(),
				l_csm_buffer
			));

			if (m_capture_debug_shapes)
			{
				m_captured_cascades.assign(std::make_move_iterator(std::begin(l_light_cascade_cameras)), std::make_move_iterator(std::end(l_light_cascade_cameras)));
				m_capture_debug_shapes = false;
			}
		}
		
		p_param.m_render_thread.finish();
		m_command_list->finished();
		
		for(auto& l_captured_camera : m_captured_cascades)
		{
			//p_param.m_render_system.get_shape_drawer().render_wired_frustum(l_captured_camera);
		}
		for(auto& l_captured_box : m_captured_boxes)
		{
			p_param.m_render_system.get_shape_drawer().render_wired_box(l_captured_box);
		}
	}

	void bc_cascaded_shadow_map_pass::cleanup_frame(const game::bc_render_pass_render_param& p_param)
	{
		get_shared_resource_throw<bc_cascaded_shadow_map_buffer_container>(m_depth_buffers_share_slot).clear();
	}

	void bc_cascaded_shadow_map_pass::before_reset(const game::bc_render_pass_reset_param& p_param)
	{
	}

	void bc_cascaded_shadow_map_pass::after_reset(const game::bc_render_pass_reset_param& p_param)
	{
	}

	void bc_cascaded_shadow_map_pass::destroy(game::bc_render_system& p_render_system)
	{
		get_shared_resource<bc_cascaded_shadow_map_buffer_container>(m_depth_buffers_share_slot)->clear();
		m_light_instance_states.clear();
		m_parameters_cbuffer.reset();
		m_device_pipeline.reset();
		m_command_list.reset();

		unshare_resource(m_depth_buffers_share_slot);
	}

	void bc_cascaded_shadow_map_pass::capture_debug_shapes()
	{
		m_capture_debug_shapes = true;
		m_captured_cascades.clear();
		m_captured_boxes.clear();
	}

	_bc_cascaded_shadow_map_light_state bc_cascaded_shadow_map_pass::_create_light_instance(game::bc_render_system& p_render_system)
	{
		_bc_cascaded_shadow_map_light_state l_instance;

		auto l_depth_buffer_config = graphic::bc_graphic_resource_configure()
			.as_resource()
			.as_texture2d
			(
				m_shadow_map_size * m_cascade_sizes.size(),
				m_shadow_map_size,
				false,
				1,
				graphic::bc_format::D32_FLOAT,
				graphic::bc_resource_usage::gpu_rw,
				core::bc_enum:: or ({ graphic::bc_resource_view_type::depth_stencil, graphic::bc_resource_view_type::shader })
			).as_depth_stencil_texture();
		auto l_depth_buffer_view_config = graphic::bc_graphic_resource_configure()
			.as_resource_view()
			.as_texture_view(graphic::bc_format::D32_FLOAT)
			.as_tex2d_depth_stencil_view(0);
		auto l_depth_buffer_resource_view_config = graphic::bc_graphic_resource_configure()
			.as_resource_view()
			.as_texture_view(graphic::bc_format::D32_FLOAT)
			.as_tex2d_shader_view(0, 1)
			.on_depth_stencil_texture2d();

		l_instance.m_depth_buffer = p_render_system.get_device().create_texture2d(l_depth_buffer_config, nullptr);
		l_instance.m_depth_buffer_view = p_render_system.get_device().create_depth_stencil_view(l_instance.m_depth_buffer.get(), l_depth_buffer_view_config);
		l_instance.m_depth_buffer_resource_view = p_render_system.get_device().create_resource_view(l_instance.m_depth_buffer.get(), l_depth_buffer_resource_view_config);
		l_instance.m_render_pass_state = p_render_system.create_render_pass_state
		(
			m_device_pipeline.get(),
			graphic::bc_viewport::default_config(l_depth_buffer_config.get_width(), l_depth_buffer_config.get_height()),
			{ },
			l_instance.m_depth_buffer_view.get(),
			{},
			{},
			{
				p_render_system.get_global_cbuffer(),
				graphic::bc_constant_buffer_parameter(1, graphic::bc_shader_type::vertex, m_parameters_cbuffer.get())
			}
			);

		return l_instance;
	}

	core::bc_vector_frame<bc_cascaded_shadow_map_camera> bc_cascaded_shadow_map_pass::_get_light_cascades(const game::bc_icamera& p_camera, const game::bc_direct_light& p_light)
	{
		game::bc_icamera::extend l_camera_frustum_corners;
		p_camera.get_extend_points(l_camera_frustum_corners);

		core::bc_vector_frame<bcFLOAT> l_cascade_break_points;
		l_cascade_break_points.push_back(p_camera.get_near_clip());
		l_cascade_break_points.insert(std::cend(l_cascade_break_points), std::begin(m_cascade_sizes), std::end(m_cascade_sizes));

		core::bc_vector_frame<bc_cascaded_shadow_map_camera> l_cascade_cameras;

		const auto l_camera_planes_distance = p_camera.get_far_clip();
		const auto l_lower_left_ray = l_camera_frustum_corners[4] - l_camera_frustum_corners[0];
		const auto l_upper_left_ray = l_camera_frustum_corners[5] - l_camera_frustum_corners[1];
		const auto l_upper_right_ray = l_camera_frustum_corners[6] - l_camera_frustum_corners[2];
		const auto l_lower_right_ray = l_camera_frustum_corners[7] - l_camera_frustum_corners[3];

		for (bcUINT32 l_cascade_index = 0; l_cascade_index < l_cascade_break_points.size() - 1; ++l_cascade_index)
		{
			const auto l_min_z = l_cascade_break_points[l_cascade_index] - p_camera.get_near_clip();
			const auto l_max_z = l_cascade_break_points[l_cascade_index + 1] - p_camera.get_near_clip();

			const auto l_cascade_camera_min_z_distance_ratio = l_min_z / l_camera_planes_distance;
			const auto l_cascade_camera_max_z_distance_ratio = l_max_z / l_camera_planes_distance;

			auto l_camera_position = p_camera.get_position();
			core::bc_array<core::bc_vector3f, 8> l_frustum_points
			{
				l_lower_left_ray * l_cascade_camera_min_z_distance_ratio + l_camera_position,
				l_lower_left_ray * l_cascade_camera_max_z_distance_ratio + l_camera_position,
				l_upper_left_ray * l_cascade_camera_min_z_distance_ratio + l_camera_position,
				l_upper_left_ray * l_cascade_camera_max_z_distance_ratio + l_camera_position,
				l_upper_right_ray * l_cascade_camera_min_z_distance_ratio + l_camera_position,
				l_upper_right_ray * l_cascade_camera_max_z_distance_ratio + l_camera_position,
				l_lower_right_ray * l_cascade_camera_min_z_distance_ratio + l_camera_position,
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
			
			const auto l_frustum_center = (l_min + l_max) / 2;
			const auto l_camera_pos = l_frustum_center + (-p_light.get_direction() * m_cascade_cameras_distance);

			auto l_cascade_view_matrix = core::bc_matrix4f::look_at_matrix_lh(l_camera_pos, l_frustum_center, core::bc_vector3f::up());
			core::bc_array<core::bc_vector3f, 8> l_frustum_points_vs
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

			l_cascade_cameras.push_back(bc_cascaded_shadow_map_camera(l_camera_pos, l_frustum_center, l_max_vs.x - l_min_vs.x, l_max_vs.y - l_min_vs.y, 0.1, l_max_vs.z));

			if(m_capture_debug_shapes)
			{
				m_captured_boxes.push_back(physics::bc_bound_box::from_min_max(l_min, l_max));
			}
		}

		return l_cascade_cameras;
	}
}