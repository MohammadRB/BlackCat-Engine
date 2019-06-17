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
	struct bc_parameters_cbuffer
	{
		BC_CBUFFER_ALIGN
		bcUINT32 m_shadow_map_size;
		bcUINT32 m_shadow_map_cascade_count;
		bcUINT32 m_shadow_map_current_cascade;
	};

	bc_cascaded_shadow_map_pass::bc_cascaded_shadow_map_pass(bcSIZE p_shadow_map_size, std::initializer_list<bcSIZE> p_cascade_sizes)
		: m_shadow_map_size(p_shadow_map_size),
		m_cascade_sizes(p_cascade_sizes)
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
			"csm_ps",
			game::bc_vertex_type::pos_tex_nor_tan,
			game::bc_blend_type::opaque,
			game::bc_depth_stencil_type::depth_less_stencil_off,
			game::bc_rasterizer_type::fill_solid_cull_back,
			0x01,
			{
				graphic::bc_format::D32_FLOAT
			},
			get_shared_resource<graphic::bc_texture2d>(game::bc_render_pass_resource_variable::depth_stencil_texture)->get_format(),
			game::bc_multi_sample_type::c1_q1
		);

		bc_parameters_cbuffer l_parameters{ m_shadow_map_size,m_cascade_sizes.size() };
		auto l_parameters_cbuffer_config = graphic::bc_graphic_resource_configure()
			.as_resource()
			.as_buffer(1, sizeof(bc_parameters_cbuffer), graphic::bc_resource_usage::gpu_r)
			.as_constant_buffer();
		auto l_parameters_data = graphic::bc_subresource_data(&l_parameters, 0, 0);
		m_parameters_cbuffer = p_render_system.get_device().create_buffer(l_parameters_cbuffer_config, &l_parameters_data);
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

		if(l_direct_lights.size() > m_light_instance_states.size())
		{
			m_light_instance_states.push_back(_create_light_instance(p_param.m_render_system));
		}

		for(bcSIZE l_light_ite = 0; l_light_ite < l_direct_lights.size(); ++l_light_ite)
		{
			auto& l_light = l_direct_lights[l_light_ite];
			auto& l_light_state = m_light_instance_states[l_light_ite];
			auto* l_light_render_pass_state = l_light_state.m_render_pass_state.get();

			p_param.m_render_thread.bind_render_pass_state(l_light_render_pass_state);

			auto l_light_cascade_cameras = _get_light_cascades(p_param.m_camera, l_light);
			for(auto& l_light_cascade_camera : l_light_cascade_cameras)
			{
				game::bc_camera_frustum l_camera_frustum(l_light_cascade_camera);
				auto l_scene_buffer = p_param.m_scene.get_actors<game::bc_mesh_component>(l_camera_frustum);

				l_scene_buffer.render_actors(p_param.m_render_system);
				
				p_param.m_render_system.render_all_instances(p_param.m_render_thread, p_param.m_clock, p_param.m_camera);
				p_param.m_render_system.clear_render_instances();
			}

			p_param.m_render_thread.unbind_render_pass_state(l_light_render_pass_state);
		}
		
		p_param.m_render_thread.finish();
		m_command_list->finished();
	}

	void bc_cascaded_shadow_map_pass::before_reset(const game::bc_render_pass_reset_param& p_param)
	{
	}

	void bc_cascaded_shadow_map_pass::after_reset(const game::bc_render_pass_reset_param& p_param)
	{
	}

	void bc_cascaded_shadow_map_pass::destroy(game::bc_render_system& p_render_system)
	{
		m_light_instance_states.clear();
		m_parameters_cbuffer.reset();
		m_device_pipeline.reset();
		m_command_list.reset();
	}

	_bc_cascaded_shadow_map_light_state bc_cascaded_shadow_map_pass::_create_light_instance(game::bc_render_system& p_render_system)
	{
		_bc_cascaded_shadow_map_light_state l_instance;

		auto l_resource_configure = graphic::bc_graphic_resource_configure();
		auto l_shadow_map_config = l_resource_configure.as_resource()
			.as_texture2d
			(
				m_shadow_map_size * m_cascade_sizes.size(),
				m_shadow_map_size,
				false,
				0,
				graphic::bc_format::D32_FLOAT,
				graphic::bc_resource_usage::gpu_rw,
				graphic::bc_resource_view_type::render_target
			)
			.as_render_target_texture();
		auto l_render_target_config = l_resource_configure.as_resource_view()
			.as_texture_view(graphic::bc_format::D32_FLOAT)
			.as_tex2d_render_target_view(0);

		l_instance.m_shadow_map = p_render_system.get_device().create_texture2d(l_shadow_map_config, nullptr);
		l_instance.m_render_target = p_render_system.get_device().create_render_target_view(l_instance.m_shadow_map.get(), l_render_target_config);
		l_instance.m_render_pass_state = p_render_system.create_render_pass_state
		(
			m_device_pipeline.get(),
			graphic::bc_viewport::default_config(l_shadow_map_config.get_width(), l_shadow_map_config.get_height()),
			{ l_instance.m_render_target.get() },
			*get_shared_resource<graphic::bc_depth_stencil_view>(game::bc_render_pass_resource_variable::depth_stencil_view),
			{},
			{},
			{ graphic::bc_constant_buffer_parameter(0, m_parameters_cbuffer.get()) }
		);

		return l_instance;
	}

	core::bc_vector_frame<_bc_cascaded_shadow_map_camera> bc_cascaded_shadow_map_pass::_get_light_cascades(const game::bc_icamera& p_camera, const game::bc_direct_light& p_light)
	{
		game::bc_icamera::extend l_camera_frustum_corners;
		p_camera.get_extend_points(l_camera_frustum_corners);

		core::bc_vector_frame<bcFLOAT> l_cascade_break_points;
		l_cascade_break_points.push_back(p_camera.get_near_clip());
		l_cascade_break_points.assign(std::begin(m_cascade_sizes), std::end(m_cascade_sizes));

		core::bc_vector_frame<std::tuple<core::bc_matrix4f, core::bc_matrix4f>> l_cascade_cameras;

		const auto l_lower_left_ray = l_camera_frustum_corners[4] - l_camera_frustum_corners[0];
		const auto l_upper_left_ray = l_camera_frustum_corners[5] - l_camera_frustum_corners[1];
		const auto l_upper_right_ray = l_camera_frustum_corners[6] - l_camera_frustum_corners[2];
		const auto l_lower_right_ray = l_camera_frustum_corners[7] - l_camera_frustum_corners[3];
		const auto l_forward_vector = p_camera.get_forward();

		for (bcUINT32 l_cascade_index = 0; l_cascade_index < l_cascade_break_points.size() - 1; ++l_cascade_index)
		{
			const auto l_min_z = l_cascade_break_points[l_cascade_index];
			const auto l_max_z = l_cascade_break_points[l_cascade_index + 1];

			auto l_min_z_ray = l_forward_vector * (l_min_z - p_camera.get_near_clip());
			auto l_max_z_ray = l_forward_vector * (l_max_z - p_camera.get_near_clip());

			const auto l_lower_left_min_z_proj = l_min_z_ray.dot(l_lower_left_ray);
			const auto l_lower_left_max_z_proj = l_max_z_ray.dot(l_lower_left_ray);
			const auto l_upper_left_min_z_proj = l_min_z_ray.dot(l_upper_left_ray);
			const auto l_upper_left_max_z_proj = l_max_z_ray.dot(l_upper_left_ray);
			const auto l_upper_right_min_z_proj = l_min_z_ray.dot(l_upper_right_ray);
			const auto l_upper_right_max_z_proj = l_max_z_ray.dot(l_upper_right_ray);
			const auto l_lower_right_min_z_proj = l_min_z_ray.dot(l_lower_right_ray);
			const auto l_lower_right_max_z_proj = l_max_z_ray.dot(l_lower_right_ray);

			auto l_camera_position = p_camera.get_position();
			auto l_frustum_center = l_camera_position + ((l_min_z_ray + l_max_z_ray) / 2);
			core::bc_array<core::bc_vector3f, 8> l_frustum_points
			{
				l_lower_left_ray * l_lower_left_min_z_proj + l_camera_position,
				l_lower_left_ray * l_lower_left_max_z_proj + l_camera_position,
				l_upper_left_ray * l_upper_left_min_z_proj + l_camera_position,
				l_upper_left_ray * l_upper_left_max_z_proj + l_camera_position,
				l_upper_right_ray * l_upper_right_min_z_proj + l_camera_position,
				l_upper_right_ray * l_upper_right_max_z_proj + l_camera_position,
				l_lower_right_ray * l_lower_right_min_z_proj + l_camera_position,
				l_lower_right_ray * l_lower_right_max_z_proj + l_camera_position
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

			const auto l_frustum_height = l_max.y - l_min.y;
			const auto l_camera_pos = l_frustum_center + (-p_light.get_direction() * (l_frustum_height + 300));
			const auto l_frustum_far_plan = (l_camera_pos - l_frustum_center).magnitude() + l_frustum_height;

			l_cascade_cameras.push_back(std::make_tuple
			(
				core::bc_matrix4f::look_at_matrix_lh(l_camera_pos, l_frustum_center, core::bc_vector3f::up()),
				core::bc_matrix4f::orthographic_matrix_lh(0.1, l_frustum_far_plan, l_max.x - l_min.x, l_max.y - l_min.y)
			));
		}

		return l_cascade_cameras;
	}
}