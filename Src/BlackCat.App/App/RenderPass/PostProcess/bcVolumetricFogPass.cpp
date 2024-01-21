// [23/07/2023 MRB]

#include "App/AppPCH.h"

#include "Core/Math/bcMatrix4f.h"
#include "Core/Messaging/Query/bcQueryManager.h"
#include "GraphicImp/Resource/bcResourceBuilder.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/Object/Scene/Component/bcFogComponent.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/bcDefaultRenderThread.h"
#include "App/RenderPass/PostProcess/bcVolumetricFogPass.h"
#include "App/bcConstant.h"

namespace black_cat
{
	struct _bc_fog_instance_parameters
	{
		core::bc_matrix4f m_world;
		core::bc_vector3f m_half_extend;
		core::bc_vector3f m_min;
		core::bc_vector3f m_max;
		core::bc_vector3f m_color;
		bcFLOAT m_visibility;
		bcFLOAT m_center_fade;
		bcFLOAT m_intensity;
		bcUINT32 m_global_light;
	};

	struct _bc_params_struct
	{
		BC_CBUFFER_ALIGN
		core::bc_vector4f m_frustum_vectors[4];
		BC_CBUFFER_ALIGN
		core::bc_vector3f m_frustum_hor_diff;
		BC_CBUFFER_ALIGN
		core::bc_vector3f m_frustum_ver_diff;
	};
	
	core::bc_vector<_bc_fog_query_instance> _fog_query_callback(const game::bc_scene& p_scene)
	{
		const auto l_scene_buffer = p_scene.get_scene_graph().get_actors<game::bc_fog_component>();

		core::bc_vector<_bc_fog_query_instance> l_fog_instances;
		l_fog_instances.reserve(l_scene_buffer.size());

		std::transform
		(
			std::begin(l_scene_buffer),
			std::end(l_scene_buffer),
			std::back_inserter(l_fog_instances),
			[](const game::bc_actor& p_actor)
			{
				const auto* l_fog_component = p_actor.get_component<game::bc_fog_component>();
				return _bc_fog_query_instance
				{
					l_fog_component->get_center(),
					l_fog_component->get_half_extend(),
					l_fog_component->get_min(),
					l_fog_component->get_max(),
					l_fog_component->get_color(),
					l_fog_component->get_visibility(),
					l_fog_component->get_center_fade(),
					l_fog_component->get_intensity(),
					l_fog_component->get_apply_global_light()
				};
			}
		);

		return l_fog_instances;
	}

	bc_volumetric_fog_pass::bc_volumetric_fog_pass(game::bc_render_pass_variable_t p_render_target_texture,
		game::bc_render_pass_variable_t p_render_target_view)
		: m_render_target_texture(p_render_target_texture),
		  m_render_target_view(p_render_target_view),
		  m_update_fog_instances(false)
	{
	}

	void bc_volumetric_fog_pass::initialize_resources(game::bc_render_system& p_render_system)
	{
		auto& l_device = p_render_system.get_device();

		core::bc_array<core::bc_vector3f, 8> l_cube_vertices
		{
			core::bc_vector3f(-.5, -.5, -.5),
			core::bc_vector3f(-.5, -.5, .5),
			core::bc_vector3f(.5, -.5, .5),
			core::bc_vector3f(.5, -.5, -.5),
			core::bc_vector3f(-.5, .5, -.5),
			core::bc_vector3f(-.5, .5, .5),
			core::bc_vector3f(.5, .5, .5),
			core::bc_vector3f(.5, .5, -.5),
		};
		core::bc_array<bcINT16, 36> l_cube_indices
		{
			0, 2, 1, // Bottom
			0, 3, 2,
			4, 5, 6, // Top
			4, 6, 7,
			0, 4, 7, // Front
			0, 7, 3,
			1, 4, 0, // Left
			1, 5, 4,
			1, 6, 5, // Back
			1, 2, 6,
			2, 7, 6, // Right
			2, 3, 7,
		};

		auto l_cube_vb_config = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_buffer(8, sizeof(core::bc_vector3f), graphic::bc_resource_usage::gpu_r)
			.as_vertex_buffer();
		auto l_cube_ib_config = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_buffer(36, sizeof(bcINT16), graphic::bc_resource_usage::gpu_r)
			.as_index_buffer();
		auto l_params_buffer_config = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_buffer(1, sizeof(_bc_params_struct), graphic::bc_resource_usage::gpu_rw, graphic::bc_resource_view_type::none)
			.as_constant_buffer();
		auto l_instance_buffer_config = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_buffer(s_max_instance_per_draw, sizeof(_bc_fog_instance_parameters), graphic::bc_resource_usage::gpu_rw, graphic::bc_resource_view_type::shader)
			.with_structured_buffer(sizeof(_bc_fog_instance_parameters))
			.as_buffer();
		auto l_instance_buffer_view_config = graphic::bc_graphic_resource_builder()
			.as_resource_view()
			.as_buffer_view(graphic::bc_format::unknown)
			.as_shader_view(0, s_max_instance_per_draw)
			.as_structured_buffer();
		auto l_cube_vb_data = graphic::bc_subresource_data(l_cube_vertices.data(), 0, 0);
		auto l_cube_ib_data = graphic::bc_subresource_data(l_cube_indices.data(), 0, 0);

		m_cube_vb = l_device.create_buffer(l_cube_vb_config, &l_cube_vb_data);
		m_cube_ib = l_device.create_buffer(l_cube_ib_config, &l_cube_ib_data);
		m_params_buffer = l_device.create_buffer(l_params_buffer_config, nullptr);
		m_fog_instances_buffer = l_device.create_buffer(l_instance_buffer_config, nullptr);
		m_fog_instances_buffer_view = l_device.create_resource_view(m_fog_instances_buffer.get(), l_instance_buffer_view_config);

		const auto l_point_sampler_config = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_sampler_state
			(
				graphic::bc_filter::min_mag_mip_point,
				graphic::bc_texture_address_mode::wrap,
				graphic::bc_texture_address_mode::wrap,
				graphic::bc_texture_address_mode::wrap
			)
			.as_sampler_state();

		m_point_sampler = l_device.create_sampler_state(l_point_sampler_config);

		after_reset(game::bc_render_pass_reset_context::create_default_instance(p_render_system));
	}

	void bc_volumetric_fog_pass::update(const game::bc_render_pass_update_context& p_context)
	{
	}
	
	void bc_volumetric_fog_pass::initialize_frame(const game::bc_render_pass_render_context& p_context)
	{
		if (m_fog_query.is_executed())
		{
			m_fog_instances = std::move(m_fog_query.get().get_result_as_throw<core::bc_vector<_bc_fog_query_instance>>());
			m_update_fog_instances = true;
		}

		m_last_query_execution_elapsed += p_context.m_clock.m_elapsed;
		if (m_last_query_execution_elapsed < s_query_execution_delay_ms)
		{
			return;
		}
		m_last_query_execution_elapsed = 0;

		auto l_query = game::bc_scene_query();
		l_query.with_callback
		(
			[](const game::bc_scene_query_context& p_context)
			{
				if (!p_context.m_scene)
				{
					return core::bc_any(core::bc_vector<_bc_fog_query_instance>());
				}

				auto l_fog_instances = _fog_query_callback(*p_context.m_scene);
				return core::bc_any(std::move(l_fog_instances));
			}
		);

		m_fog_query = p_context.m_query_manager.queue_query(std::move(l_query));
	}

	void bc_volumetric_fog_pass::execute(const game::bc_render_pass_render_context& p_context)
	{
		p_context.m_render_thread.start();

		if (m_update_fog_instances)
		{
			_copy_fog_instances_to_buffer(p_context, core::bc_make_cspan(m_fog_instances));
			m_update_fog_instances = false;
		}

		_update_params(p_context.m_render_thread, p_context.m_render_camera);

		p_context.m_render_thread.bind_render_pass_state(*m_render_pass_state);
		p_context.m_render_thread.bind_render_state(*m_render_state);
		p_context.m_render_thread.draw_indexed_instanced(m_render_state->get_index_count(), m_fog_instances.size(), 0, 0, 0);
		p_context.m_render_thread.unbind_render_state(*m_render_state);
		p_context.m_render_thread.unbind_render_pass_state(*m_render_pass_state);

		p_context.m_render_thread.finish();
	}

	void bc_volumetric_fog_pass::before_reset(const game::bc_render_pass_reset_context& p_context)
	{
	}

	void bc_volumetric_fog_pass::after_reset(const game::bc_render_pass_reset_context& p_context)
	{
		const auto l_depth_stencil = get_shared_resource_throw<graphic::bc_texture2d>(constant::g_rpass_depth_stencil_texture);
		const auto l_depth_stencil_view = get_shared_resource_throw<graphic::bc_resource_view>(constant::g_rpass_depth_stencil_read_view);
		const auto l_render_target_texture = get_shared_resource_throw<graphic::bc_texture2d>(m_render_target_texture);
		const auto l_render_target_view = get_shared_resource_throw<graphic::bc_render_target_view>(m_render_target_view);
		const auto l_viewport = graphic::bc_viewport::default_config(l_render_target_texture.get_width(), l_render_target_texture.get_height());

		m_device_pipeline_state = p_context.m_render_system.create_device_pipeline_state
		(
			"vol_fog_vs",
			nullptr,
			nullptr,
			nullptr,
			"vol_fog_ps",
			game::bc_vertex_type::pos,
			game::bc_blend_type::blend_overwrite_alpha,
			core::bc_enum::mask_or({ game::bc_depth_stencil_type::depth_off, game::bc_depth_stencil_type::stencil_off }),
			game::bc_rasterizer_type::fill_solid_cull_front,
			0x1,
			{ l_render_target_texture.get_format() },
			graphic::bc_format::unknown,
			game::bc_multi_sample_type::c1_q1
		);
		m_render_pass_state = p_context.m_render_system.create_render_pass_state
		(
			m_device_pipeline_state.get(),
			l_viewport,
			{
				graphic::bc_render_target_view_parameter(l_render_target_view)
			},
			graphic::bc_depth_stencil_view(),
			{
				graphic::bc_sampler_parameter(0, graphic::bc_shader_type::pixel, m_point_sampler.get())
			},
			{
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::pixel, l_depth_stencil_view),
				graphic::bc_resource_view_parameter
				(
					1,
					graphic::bc_shader_type::vertex,
					m_fog_instances_buffer_view.get()
				)
			},
			{
			},
			{
				p_context.m_render_system.get_global_cbuffer(),
				graphic::bc_constant_buffer_parameter(1, graphic::bc_shader_type::pixel, m_params_buffer.get())
			}
		);
		m_render_state = p_context.m_render_system.create_render_state
		(
			graphic::bc_primitive::trianglelist,
			*m_cube_vb,
			sizeof(core::bc_vector3f),
			0,
			*m_cube_ib,
			game::i16bit,
			m_cube_ib->get_byte_width() / sizeof(bcINT16),
			0,
			{
			},
			{
			}
		);
	}

	void bc_volumetric_fog_pass::destroy(game::bc_render_system& p_render_system)
	{
		m_cube_vb.reset();
		m_cube_ib.reset();
		m_params_buffer.reset();
		m_fog_instances_buffer.reset();
		m_fog_instances_buffer_view.reset();
		m_point_sampler.reset();
		m_device_pipeline_state.reset();
		m_render_pass_state.reset();
	}

	void bc_volumetric_fog_pass::_copy_fog_instances_to_buffer(const game::bc_render_pass_render_context& p_context, 
		core::bc_const_span<_bc_fog_query_instance> p_instances)
	{
		if (p_instances.empty())
		{
			return;
		}

		const auto l_instance_count = std::min<bcUINT>(s_max_instance_per_draw, m_fog_instances.size());
		core::bc_vector_frame<_bc_fog_instance_parameters> l_instances;
		l_instances.reserve(l_instance_count);

		std::transform
		(
			std::begin(m_fog_instances),
			std::next(std::begin(m_fog_instances), l_instance_count),
			std::back_inserter(l_instances),
			[&](const _bc_fog_query_instance& p_instance)
			{
				auto l_world_transform = core::bc_matrix4f::scale_matrix(p_instance.m_half_extend * 2) * core::bc_matrix4f::translation_matrix(p_instance.m_center);

				// Because matrices are put in regular buffer rather than cbuffer they must be stored in row major format
				if constexpr (!p_context.m_frame_renderer.need_matrix_transpose())
				{
					l_world_transform = l_world_transform.transpose();
				}

				return _bc_fog_instance_parameters
				{
					l_world_transform,
					p_instance.m_half_extend,
					p_instance.m_min,
					p_instance.m_max,
					p_instance.m_color,
					p_instance.m_visibility,
					p_instance.m_center_fade,
					p_instance.m_intensity,
					p_instance.m_global_light
				};
			}
		);

		p_context.m_render_thread.update_subresource
		(
			m_fog_instances_buffer.get(), 
			0, 
			l_instances.data(), 
			0, 
			0
		);
	}

	void bc_volumetric_fog_pass::_update_params(game::bc_default_render_thread& p_render_thread, const game::bc_camera_instance& p_camera)
	{
		_bc_params_struct l_params{};

		const auto l_camera_extends = p_camera.get_extend_rays();
		l_params.m_frustum_vectors[0] = core::bc_vector4f(l_camera_extends[1], 1);
		l_params.m_frustum_vectors[1] = core::bc_vector4f(l_camera_extends[2], 1);
		l_params.m_frustum_vectors[2] = core::bc_vector4f(l_camera_extends[3], 1);
		l_params.m_frustum_vectors[3] = core::bc_vector4f(l_camera_extends[0], 1);

		l_params.m_frustum_hor_diff = (l_params.m_frustum_vectors[1] - l_params.m_frustum_vectors[0]).xyz();
		l_params.m_frustum_ver_diff = (l_params.m_frustum_vectors[3] - l_params.m_frustum_vectors[0]).xyz();

		p_render_thread.update_subresource(*m_params_buffer, 0, &l_params, 0, 0);
	}
}
