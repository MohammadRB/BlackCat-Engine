// [11/16/2020 MRB]

#include "BlackCat/BlackCatPCH.h"

#include "Core/Math/bcVector4f.h"
#include "Core/Container/bcVector.h"
#include "Core/Content/bcContentManager.h"
#include "Core/Messaging/Query/bcQueryManager.h"
#include "Core/Utility/bcEnumOperand.h"
#include "GraphicImp/Shader/Parameter/bcResourceViewParameter.h"
#include "GraphicImp/Resource/View/bcRenderTargetViewConfig.h"
#include "GraphicImp/Resource/bcResourceBuilder.h"
#include "Game/System/bcGameSystem.h"
#include "Game/System/Input/bcInputSystem.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/bcDefaultRenderThread.h"
#include "Game/System/Render/Particle/bcParticleManager.h"
#include "Game/System/Render/Particle/bcParticleEmitter.h"
#include "BlackCat/RenderPass/PostProcess/bcParticleSystemPassDx11.h"
#include "BlackCat/bcConstant.h"

namespace black_cat
{
	struct _bc_emitter_struct : public game::bc_particle_emitter_state
	{
	};
	
	struct _bc_particle_struct
	{
		core::bc_vector3f m_position;
		core::bc_vector3f m_direction;
		core::bc_vector3f m_color;
		bcFLOAT m_color_intensity;
		bcFLOAT m_lifetime;
		bcFLOAT m_age;
		bcFLOAT m_force;
		bcFLOAT m_mass;
		bcFLOAT m_start_size;
		bcFLOAT m_end_size;
		bcFLOAT m_size;
		bcFLOAT m_fade;
		bcFLOAT m_rotation;
		bcUINT32 m_sprite_index;
		bcUINT32 m_velocity_curve_index;
		bcFLOAT m_velocity_curve_duration;
		bcUINT32 m_size_curve_index;
		bcUINT32 m_fade_curve_index;
	};

	struct _bc_alive_particle_struct
	{
		bcUINT32 m_index;
		bcFLOAT m_distance;
	};

	struct _bc_sort_cbuffer_struct
	{
		BC_CBUFFER_ALIGN
		bcUINT32 m_num_particles;
		bcUINT32 m_sort_array_size;
		bcUINT32 m_sort_array_size_mask;
		BC_CBUFFER_ALIGN
		bcUINT32 m_matrix_width;
		bcUINT32 m_matrix_height;
	};

	struct _bc_curve_cbuffer_struct
	{
		BC_CBUFFER_ALIGN
		core::bc_vector4f m_curves[game::bc_particle_manager::s_curve_count][game::bc_particle_manager::s_curve_sample_count];
	};

	template<bcSIZE TLightCount>
	struct _bc_lights_cbuffer_struct
	{
		BC_CBUFFER_ALIGN
		bcUINT32 m_light_count;
		BC_CBUFFER_ALIGN
		core::bc_vector4f m_lights[TLightCount * 2];
	};

	bc_particle_system_pass_dx11::bc_particle_system_pass_dx11(game::bc_render_pass_variable_t p_render_target_texture, game::bc_render_pass_variable_t p_render_target_view, const bcECHAR* p_sprites_content_name)
		: m_render_target_texture(p_render_target_texture),
		m_render_target_view(p_render_target_view),
		m_sprites_content_name(p_sprites_content_name)
	{
	}

	void bc_particle_system_pass_dx11::initialize_resources(game::bc_render_system& p_render_system)
	{
		auto& l_device = p_render_system.get_device();
		auto& l_device_swap_buffer = p_render_system.get_device_swap_buffer();
		auto& l_content_manager = *core::bc_get_service<core::bc_content_manager>();
		auto& l_game_system = *core::bc_get_service<game::bc_game_system>();

		const auto l_sprites_path = l_game_system.get_file_system().get_content_texture_path(m_sprites_content_name);
		m_sprites_texture = l_content_manager.load<graphic::bc_texture2d_content>
		(
			core::bc_alloc_type::program,
			l_sprites_path.c_str(),
			nullptr,
			core::bc_content_loader_parameter()
		);
		auto l_sprites_texture = m_sprites_texture->get_resource();
		auto l_sprites_view_config = graphic::bc_graphic_resource_builder()
			.as_resource_view()
			.as_texture_view(m_sprites_texture->get_resource().get_format())
			.as_tex2d_shader_view(0, -1)
			.on_texture2d();
		m_sprites_view = l_device.create_resource_view(l_sprites_texture, l_sprites_view_config);
		
		auto l_emitters_buffer_config = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_buffer(s_emitters_count, sizeof(_bc_emitter_struct), graphic::bc_resource_usage::gpu_rw, graphic::bc_resource_view_type::shader)
			.with_structured_buffer(sizeof(_bc_emitter_struct))
			.as_buffer();
		auto l_particles_buffer_config = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_buffer
			(
				s_particles_count, 
				sizeof(_bc_particle_struct), 
				graphic::bc_resource_usage::gpu_rw, 
				core::bc_enum::mask_or({ graphic::bc_resource_view_type::shader, graphic::bc_resource_view_type::unordered })
			)
			.with_structured_buffer(sizeof(_bc_particle_struct))
			.as_buffer();
		auto l_alive_particles_buffer_config = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_buffer
			(
				s_particles_count,
				sizeof(_bc_alive_particle_struct), 
				graphic::bc_resource_usage::gpu_rw, 
				core::bc_enum::mask_or({ graphic::bc_resource_view_type::shader, graphic::bc_resource_view_type::unordered })
			)
			.with_structured_buffer(sizeof(_bc_alive_particle_struct))
			.as_buffer();
		auto l_dead_particles_buffer_config = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_buffer(s_particles_count, sizeof(bcUINT32), graphic::bc_resource_usage::gpu_rw, graphic::bc_resource_view_type::unordered)
			.with_structured_buffer(sizeof(bcUINT32))
			.with_append_consume(sizeof(bcUINT32))
			.as_buffer();
		auto l_draw_args_buffer_config = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_buffer(4, sizeof(bcUINT32), graphic::bc_resource_usage::gpu_rw, graphic::bc_resource_view_type::shader)
			.with_indirect_args_buffer()
			.as_buffer();
		auto l_sort_cbuffer_config = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_buffer(1, sizeof(_bc_sort_cbuffer_struct), graphic::bc_resource_usage::gpu_rw)
			.as_constant_buffer();
		auto l_curves_cbuffer_config = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_buffer(1, sizeof(_bc_curve_cbuffer_struct), graphic::bc_resource_usage::gpu_r)
			.as_constant_buffer();
		auto l_lights_cbuffer_config = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_buffer(1, sizeof(_bc_lights_cbuffer_struct<s_lights_count>), graphic::bc_resource_usage::gpu_rw)
			.as_constant_buffer();

		bcUINT32 l_dead_particle_index = s_particles_count - 1;
		core::bc_vector_frame<bcUINT32> l_dead_particle_indices(s_particles_count);
		std::for_each(std::begin(l_dead_particle_indices), std::end(l_dead_particle_indices), [&l_dead_particle_index](bcUINT32& p_index)
		{
			p_index = l_dead_particle_index--;
		});

		core::bc_array<bcUINT32, 4> l_draw_args = { 1,0,0,0 };

		const auto& l_curves = game::bc_particle_manager::get_curves();
		_bc_curve_cbuffer_struct l_curve_cbuffer_struct;
		for(bcSIZE l_curve_i = 0; l_curve_i < l_curves.size(); ++l_curve_i)
		{
			for(bcSIZE l_curve_sample_i = 0; l_curve_sample_i < l_curves[l_curve_i].size(); ++l_curve_sample_i)
			{
				l_curve_cbuffer_struct.m_curves[l_curve_i][l_curve_sample_i].x = l_curves[l_curve_i][l_curve_sample_i];
			}
		}
		
		graphic::bc_subresource_data l_dead_particles_data(l_dead_particle_indices.data(), 0, 0);
		graphic::bc_subresource_data l_draw_args_data(l_draw_args.data(), 0, 0);
		graphic::bc_subresource_data l_curves_cbuffer_data(&l_curve_cbuffer_struct, 0, 0);

		m_emitters_buffer = l_device.create_buffer(l_emitters_buffer_config, nullptr);
		m_particles_buffer = l_device.create_buffer(l_particles_buffer_config, nullptr);
		m_alive_particles_buffer1 = l_device.create_buffer(l_alive_particles_buffer_config, nullptr);
		m_alive_particles_buffer2 = l_device.create_buffer(l_alive_particles_buffer_config, nullptr);
		m_dead_particles_buffer = l_device.create_buffer(l_dead_particles_buffer_config, &l_dead_particles_data);
		m_draw_args_buffer = l_device.create_buffer(l_draw_args_buffer_config, &l_draw_args_data);
		m_sort_cbuffer = l_device.create_buffer(l_sort_cbuffer_config, nullptr);
		m_curves_cbuffer = l_device.create_buffer(l_curves_cbuffer_config, &l_curves_cbuffer_data);
		m_lights_cbuffer = l_device.create_buffer(l_lights_cbuffer_config, nullptr);
		
		auto l_emitters_unordered_view_config = graphic::bc_graphic_resource_builder()
			.as_resource_view()
			.as_buffer_view(graphic::bc_format::unknown)
			.as_shader_view(0, s_emitters_count)
			.as_structured_buffer();
		auto l_particles_shader_view_config = graphic::bc_graphic_resource_builder()
			.as_resource_view()
			.as_buffer_view(graphic::bc_format::unknown)
			.as_shader_view(0, s_particles_count)
			.as_structured_buffer();
		auto l_particles_unordered_view_config = graphic::bc_graphic_resource_builder()
			.as_resource_view()
			.as_buffer_view(graphic::bc_format::unknown)
			.as_unordered_view(0, s_particles_count)
			.as_structured_buffer();
		auto l_alive_particles_shader_view_config = graphic::bc_graphic_resource_builder()
			.as_resource_view()
			.as_buffer_view(graphic::bc_format::unknown)
			.as_shader_view(0, s_particles_count)
			.as_structured_buffer();
		auto l_alive_particles_unordered_view_config = graphic::bc_graphic_resource_builder()
			.as_resource_view()
			.as_buffer_view(graphic::bc_format::unknown)
			.as_unordered_view(0, s_particles_count)
			.as_structured_buffer();
		auto l_dead_particles_unordered_view_config = graphic::bc_graphic_resource_builder()
			.as_resource_view()
			.as_buffer_view(graphic::bc_format::unknown)
			.as_unordered_view(0, s_particles_count)
			.as_append_consume();
		auto l_draw_args_shader_view_config = graphic::bc_graphic_resource_builder()
			.as_resource_view()
			.as_buffer_view(graphic::bc_format::R32_UINT)
			.as_shader_view(0, 4)
			.as_indirect_args_buffer();

		m_emitters_shader_view = l_device.create_resource_view(m_emitters_buffer.get(), l_emitters_unordered_view_config);
		m_particles_shader_view = l_device.create_resource_view(m_particles_buffer.get(), l_particles_shader_view_config);
		m_particles_unordered_view = l_device.create_resource_view(m_particles_buffer.get(), l_particles_unordered_view_config);
		m_alive_particles1_shader_view = l_device.create_resource_view(m_alive_particles_buffer1.get(), l_alive_particles_shader_view_config);
		m_alive_particles1_unordered_view = l_device.create_resource_view(m_alive_particles_buffer1.get(), l_alive_particles_unordered_view_config);
		m_alive_particles2_shader_view = l_device.create_resource_view(m_alive_particles_buffer2.get(), l_alive_particles_shader_view_config);
		m_alive_particles2_unordered_view = l_device.create_resource_view(m_alive_particles_buffer2.get(), l_alive_particles_unordered_view_config);
		m_dead_particles_unordered_view = l_device.create_resource_view(m_dead_particles_buffer.get(), l_dead_particles_unordered_view_config);
		m_draw_args_shader_view = l_device.create_resource_view(m_draw_args_buffer.get(), l_draw_args_shader_view_config);

		m_emission_compute_state = p_render_system.create_device_compute_state("particle_emission");
		m_simulation_compute_state = p_render_system.create_device_compute_state("particle_simulation");
		m_sort_group_compute_state = p_render_system.create_device_compute_state("bitonic_group_sort");
		m_sort_compute_state = p_render_system.create_device_compute_state("bitonic_sort");
		m_sort_transpose_compute_state = p_render_system.create_device_compute_state("bitonic_matrix_transpose");

		auto l_linear_sampler_config = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_sampler_state
			(
				graphic::bc_filter::min_mag_mip_linear,
				graphic::bc_texture_address_mode::clamp,
				graphic::bc_texture_address_mode::clamp,
				graphic::bc_texture_address_mode::clamp
			).as_sampler_state();
		auto l_point_sampler_config = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_sampler_state
			(
				graphic::bc_filter::min_mag_mip_point,
				graphic::bc_texture_address_mode::clamp,
				graphic::bc_texture_address_mode::clamp,
				graphic::bc_texture_address_mode::clamp
			).as_sampler_state();
		m_linear_sampler = l_device.create_sampler_state(l_linear_sampler_config);
		m_point_sampler = l_device.create_sampler_state(l_point_sampler_config);
		
		graphic::bc_device_parameters l_old_parameters
		(
			0,
			0,
			graphic::bc_format::unknown,
			graphic::bc_texture_ms_config(1, 0)
		);
		graphic::bc_device_parameters l_new_parameters
		(
			l_device_swap_buffer.get_back_buffer_width(),
			l_device_swap_buffer.get_back_buffer_height(),
			l_device_swap_buffer.get_back_buffer_format(),
			l_device_swap_buffer.get_back_buffer_texture().get_sample_count()
		);

		after_reset(game::bc_render_pass_reset_context(p_render_system, l_device, l_device_swap_buffer, l_old_parameters, l_new_parameters));
	}

	void bc_particle_system_pass_dx11::update(const game::bc_render_pass_update_context& p_context)
	{
	}

	void bc_particle_system_pass_dx11::initialize_frame(const game::bc_render_pass_render_context& p_context)
	{
		if(m_emitters_query.is_executed())
		{
			m_emitters_query_result = m_emitters_query.get().get_emitters();
		}
		if(m_lights_query.is_executed())
		{
			m_lights_query_result = m_lights_query.get().get_lights();
			m_lights_query_result.erase
			(
				std::remove_if
				(
					std::begin(m_lights_query_result), 
					std::end(m_lights_query_result), 
					[](const game::bc_light_instance& p_light)
					{
						return !p_light.as_point_light()->get_particle_cast();
					}
				),
				std::end(m_lights_query_result)
			);
		}

		m_emitters_query = core::bc_get_service<core::bc_query_manager>()->queue_query
		(
			game::bc_scene_particle_emitter_query()
		);
		m_lights_query = core::bc_get_service<core::bc_query_manager>()->queue_query
		(
			game::bc_scene_light_query({game::bc_light_type::point}).with(game::bc_camera_frustum(p_context.m_update_camera))
		);
	}

	void bc_particle_system_pass_dx11::execute(const game::bc_render_pass_render_context& p_context)
	{
		p_context.m_render_thread.start();
		
		if(!m_emitters_query_result.empty())
		{
			const game::bc_compute_state_unordered_view_initial_count_array l_initial_count = { -1, m_dead_particles_initial_count, -1, -1, -1, -1, -1, -1 };
			p_context.m_render_thread.bind_compute_state(*m_emission_compute, &l_initial_count);
			p_context.m_render_thread.update_subresource(*m_emitters_buffer, 0, m_emitters_query_result.data(), m_emitters_query_result.size(), 1);
			p_context.m_render_thread.dispatch(m_emitters_query_result.size(), 1, 1);
			p_context.m_render_thread.unbind_compute_state(*m_emission_compute);
		}

		const game::bc_compute_state_unordered_view_initial_count_array l_initial_count = { -1, 0, m_dead_particles_initial_count, -1, -1, -1, -1, -1 };
		p_context.m_render_thread.bind_compute_state(*m_simulation_compute, &l_initial_count);
		p_context.m_render_thread.dispatch(std::max(1U, s_particles_count / s_simulation_shader_group_size), 1, 1);
		p_context.m_render_thread.unbind_compute_state(*m_simulation_compute);

		p_context.m_render_thread.copy_structure_count(*m_draw_args_buffer, sizeof(bcUINT32), *m_alive_particles1_unordered_view);

		_execute_sort_shader(p_context);

		if(!m_lights_query_result.empty())
		{
			std::partial_sort
			(
				std::begin(m_lights_query_result),
				std::begin(m_lights_query_result) + std::min(s_lights_count, m_lights_query_result.size()),
				std::end(m_lights_query_result),
				[&](const game::bc_light_instance& p_1, const game::bc_light_instance& p_2)
				{
					const auto l_1_distance = (p_context.m_render_camera.get_position() - p_1.as_point_light()->get_position()).magnitude();
					const auto l_2_distance = (p_context.m_render_camera.get_position() - p_2.as_point_light()->get_position()).magnitude();
					return l_1_distance < l_2_distance;
				}
			);
		}
		
		_bc_lights_cbuffer_struct<s_lights_count> l_lights;
		l_lights.m_light_count = std::min(s_lights_count, m_lights_query_result.size());
		for (auto l_light_ite = 0U; l_light_ite < l_lights.m_light_count; ++l_light_ite)
		{
			auto& l_light = *m_lights_query_result[l_light_ite].as_point_light();
			const auto l_cbuffer_ite = l_light_ite * 2;
			
			l_lights.m_lights[l_cbuffer_ite].x = l_light.get_position().x;
			l_lights.m_lights[l_cbuffer_ite].y = l_light.get_position().y;
			l_lights.m_lights[l_cbuffer_ite].z = l_light.get_position().z;
			l_lights.m_lights[l_cbuffer_ite].w = l_light.get_radius();
			l_lights.m_lights[l_cbuffer_ite + 1].x = l_light.get_color().x;
			l_lights.m_lights[l_cbuffer_ite + 1].y = l_light.get_color().y;
			l_lights.m_lights[l_cbuffer_ite + 1].z = l_light.get_color().z;
			l_lights.m_lights[l_cbuffer_ite + 1].w = l_light.get_particle_intensity();
		}

		p_context.m_render_thread.update_subresource(m_lights_cbuffer.get(), 0, &l_lights,1, 1);
		
		p_context.m_render_thread.bind_render_pass_state(*m_render_pass_state);
		p_context.m_render_thread.get_pipeline().bind_ia_primitive_topology(graphic::bc_primitive::pointlist);
		p_context.m_render_thread.get_pipeline().pipeline_apply_states(graphic::bc_pipeline_stage::input_assembler_stage);
		p_context.m_render_thread.draw_instanced_indirect(*m_draw_args_buffer, 0);
		p_context.m_render_thread.get_pipeline().unbind_ia_primitive_topology();
		p_context.m_render_thread.get_pipeline().pipeline_apply_states(graphic::bc_pipeline_stage::input_assembler_stage);
		p_context.m_render_thread.unbind_render_pass_state(*m_render_pass_state);
		
		p_context.m_render_thread.finish();

		m_dead_particles_initial_count = -1;
	}

	void bc_particle_system_pass_dx11::_execute_sort_shader(const game::bc_render_pass_render_context& p_param)
	{
		// DX11 ComputeShaderSort11 Sample

		_bc_sort_cbuffer_struct l_cbuffer;

		/*p_param.m_render_thread.bind_compute_state(*m_sort_compute);
		for(UINT32 l_sort_array_size = 2; l_sort_array_size <= s_sort_shader_group_size; l_sort_array_size *= 2)
		{
			l_cbuffer.m_num_particles = s_particles_count;
			l_cbuffer.m_sort_array_size = l_sort_array_size;
			l_cbuffer.m_sort_array_size_mask = l_sort_array_size;
			l_cbuffer.m_matrix_width = s_sort_transpose_matrix_width;
			l_cbuffer.m_matrix_height = s_sort_transpose_matrix_height;
			p_param.m_render_thread.update_subresource(m_sort_cbuffer.get(), 0, &l_cbuffer, 1, 1);

			p_param.m_render_thread.dispatch(s_particles_count / s_sort_shader_group_size, 1, 1);
		}
		p_param.m_render_thread.unbind_compute_state(*m_sort_compute);*/
		
		p_param.m_render_thread.bind_compute_state(*m_sort_group_compute);
		p_param.m_render_thread.dispatch(s_particles_count / s_sort_shader_group_size, 1, 1);
		p_param.m_render_thread.unbind_compute_state(*m_sort_group_compute);

		for(bcUINT32 l_bitonic_array = s_sort_shader_group_size * 2; l_bitonic_array <= s_particles_count; l_bitonic_array *= 2)
		{
			l_cbuffer.m_num_particles = s_particles_count;
			l_cbuffer.m_sort_array_size = l_bitonic_array / s_sort_shader_group_size;
			l_cbuffer.m_sort_array_size_mask = (l_bitonic_array & ~s_particles_count) / s_sort_shader_group_size;
			l_cbuffer.m_matrix_width = s_sort_transpose_matrix_width;
			l_cbuffer.m_matrix_height = s_sort_transpose_matrix_height;
			p_param.m_render_thread.update_subresource(m_sort_cbuffer.get(), 0, &l_cbuffer, 1, 1);

			p_param.m_render_thread.bind_compute_state(*m_sort_transpose1_compute);
			p_param.m_render_thread.dispatch(s_sort_transpose_matrix_width / s_sort_transpose_shader_group_size, s_sort_transpose_matrix_height / s_sort_transpose_shader_group_size, 1);
			p_param.m_render_thread.unbind_compute_state(*m_sort_transpose1_compute);

			p_param.m_render_thread.bind_compute_state(*m_sort1_after_transpose_compute);
			p_param.m_render_thread.dispatch(s_particles_count / s_sort_shader_group_size, 1, 1);
			p_param.m_render_thread.unbind_compute_state(*m_sort1_after_transpose_compute);

			l_cbuffer.m_num_particles = s_particles_count;
			l_cbuffer.m_sort_array_size = s_sort_shader_group_size;
			l_cbuffer.m_sort_array_size_mask = l_bitonic_array;
			l_cbuffer.m_matrix_width = s_sort_transpose_matrix_height;
			l_cbuffer.m_matrix_height = s_sort_transpose_matrix_width;
			p_param.m_render_thread.update_subresource(m_sort_cbuffer.get(), 0, &l_cbuffer, 1, 1);

			p_param.m_render_thread.bind_compute_state(*m_sort_transpose2_compute);
			p_param.m_render_thread.dispatch(s_sort_transpose_matrix_height / s_sort_transpose_shader_group_size, s_sort_transpose_matrix_width / s_sort_transpose_shader_group_size, 1);
			p_param.m_render_thread.unbind_compute_state(*m_sort_transpose2_compute);

			p_param.m_render_thread.bind_compute_state(*m_sort2_after_transpose_compute);
			p_param.m_render_thread.dispatch(s_particles_count / s_sort_shader_group_size, 1, 1);
			p_param.m_render_thread.unbind_compute_state(*m_sort2_after_transpose_compute);
		}
	}

	void bc_particle_system_pass_dx11::before_reset(const game::bc_render_pass_reset_context& p_context)
	{
	}

	void bc_particle_system_pass_dx11::after_reset(const game::bc_render_pass_reset_context& p_context)
	{
		m_depth_buffer_view = get_shared_resource_throw<graphic::bc_depth_stencil_view>(constant::g_rpass_depth_stencil_render_view);
		m_depth_buffer_shader_view = get_shared_resource_throw<graphic::bc_resource_view>(constant::g_rpass_depth_stencil_read_view);
		
		m_emission_compute = p_context.m_render_system.create_compute_state
		(
			m_emission_compute_state.get(),
			{},
			{
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::compute, m_emitters_shader_view.get()),
			},
			{
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::compute, m_particles_unordered_view.get()),
				graphic::bc_resource_view_parameter(1, graphic::bc_shader_type::compute, m_dead_particles_unordered_view.get())
			},
			{
				p_context.m_render_system.get_global_cbuffer()
			}
		);
		m_simulation_compute = p_context.m_render_system.create_compute_state
		(
			m_simulation_compute_state.get(),
			{},
			{},
			{
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::compute, m_particles_unordered_view.get()),
				graphic::bc_resource_view_parameter(1, graphic::bc_shader_type::compute, m_alive_particles1_unordered_view.get()),
				graphic::bc_resource_view_parameter(2, graphic::bc_shader_type::compute, m_dead_particles_unordered_view.get())
			},
			{
				p_context.m_render_system.get_global_cbuffer(),
				graphic::bc_constant_buffer_parameter(1, graphic::bc_shader_type::compute, m_curves_cbuffer.get())
			}
		);
		m_sort_group_compute = p_context.m_render_system.create_compute_state
		(
			m_sort_group_compute_state.get(),
			{},
			{},
			{
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::compute, m_alive_particles1_unordered_view.get())
			},
			{
				p_context.m_render_system.get_global_cbuffer()
			}
		);
		m_sort_compute = p_context.m_render_system.create_compute_state
		(
			m_sort_compute_state.get(),
			{},
			{},
			{
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::compute, m_alive_particles1_unordered_view.get())
			},
			{
				p_context.m_render_system.get_global_cbuffer(),
				graphic::bc_constant_buffer_parameter(1, graphic::bc_shader_type::compute, m_sort_cbuffer.get())
			}
		);
		m_sort_transpose1_compute = p_context.m_render_system.create_compute_state
		(
			m_sort_transpose_compute_state.get(),
			{},
			{
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::compute, m_alive_particles1_shader_view.get())
			},
			{
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::compute, m_alive_particles2_unordered_view.get())
			},
			{
				p_context.m_render_system.get_global_cbuffer(),
				graphic::bc_constant_buffer_parameter(1, graphic::bc_shader_type::compute, m_sort_cbuffer.get())
			}
		);
		m_sort1_after_transpose_compute = p_context.m_render_system.create_compute_state
		(
			m_sort_compute_state.get(),
			{},
			{},
			{
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::compute, m_alive_particles2_unordered_view.get())
			},
			{
				p_context.m_render_system.get_global_cbuffer(),
				graphic::bc_constant_buffer_parameter(1, graphic::bc_shader_type::compute, m_sort_cbuffer.get())
			}
		);
		m_sort_transpose2_compute = p_context.m_render_system.create_compute_state
		(
			m_sort_transpose_compute_state.get(),
			{},
			{
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::compute, m_alive_particles2_shader_view.get())
			},
			{
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::compute, m_alive_particles1_unordered_view.get())
			},
			{
				p_context.m_render_system.get_global_cbuffer(),
				graphic::bc_constant_buffer_parameter(1, graphic::bc_shader_type::compute, m_sort_cbuffer.get())
			}
		);
		m_sort2_after_transpose_compute = p_context.m_render_system.create_compute_state
		(
			m_sort_compute_state.get(),
			{},
			{},
			{
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::compute, m_alive_particles1_unordered_view.get())
			},
			{
				p_context.m_render_system.get_global_cbuffer(),
				graphic::bc_constant_buffer_parameter(1, graphic::bc_shader_type::compute, m_sort_cbuffer.get())
			}
		);

		const auto l_render_target = get_shared_resource_throw<graphic::bc_texture2d>(m_render_target_texture);
		const auto l_render_render_view = get_shared_resource_throw<graphic::bc_render_target_view>(m_render_target_view);
		const auto l_viewport = graphic::bc_viewport::default_config(l_render_target.get_width(), l_render_target.get_height());
		
		m_device_pipeline_state = p_context.m_render_system.create_device_pipeline_state
		(
			"particle_render_vs",
			nullptr,
			nullptr,
			"particle_render_gs",
			"particle_render_ps",
			game::bc_vertex_type::none,
			game::bc_blend_type::blending_overwrite_alpha,
			core::bc_enum::mask_or({ game::bc_depth_stencil_type::depth_off, game::bc_depth_stencil_type::stencil_off }),
			game::bc_rasterizer_type::fill_solid_cull_none,
			0x1,
			{
				l_render_target.get_format()
			},
			graphic::bc_format::unknown,
			game::bc_multi_sample_type::c1_q1
		);
		m_render_pass_state = p_context.m_render_system.create_render_pass_state
		(
			m_device_pipeline_state.get(),
			l_viewport,
			{
				graphic::bc_render_target_view_parameter(l_render_render_view)
			},
			graphic::bc_depth_stencil_view(),
			{
				graphic::bc_sampler_parameter(0, graphic::bc_shader_type::pixel, m_linear_sampler.get()),
				graphic::bc_sampler_parameter(1, graphic::bc_shader_type::pixel, m_point_sampler.get())
			},
			{
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::geometry, m_particles_shader_view.get()),
				graphic::bc_resource_view_parameter(1, graphic::bc_shader_type::geometry, m_alive_particles1_shader_view.get()),
				graphic::bc_resource_view_parameter(2, graphic::bc_shader_type::pixel, m_depth_buffer_shader_view),
				graphic::bc_resource_view_parameter(3, graphic::bc_shader_type::pixel, m_sprites_view.get()),
			},
			{},
			{
				p_context.m_render_system.get_global_cbuffer(),
				graphic::bc_constant_buffer_parameter(1, graphic::bc_shader_type::geometry, m_lights_cbuffer.get())
			}
		);
	}

	void bc_particle_system_pass_dx11::destroy(game::bc_render_system& p_render_system)
	{
		m_emitters_query = core::bc_query_result<game::bc_scene_particle_emitter_query>();
		m_lights_query = core::bc_query_result<game::bc_scene_light_query>();
		m_emitters_query_result.clear();
		m_emitters_query_result.shrink_to_fit();
		m_lights_query_result.clear();
		m_lights_query_result.shrink_to_fit();

		m_linear_sampler.reset();
		m_point_sampler.reset();
		m_device_pipeline_state.reset();
		m_render_pass_state.reset();

		m_emission_compute_state.reset();
		m_emission_compute.reset();
		m_simulation_compute_state.reset();
		m_simulation_compute.reset();
		m_sort_group_compute_state.reset();
		m_sort_group_compute.reset();
		m_sort_compute_state.reset();
		m_sort_compute.reset();
		m_sort1_after_transpose_compute.reset();
		m_sort2_after_transpose_compute.reset();
		m_sort_transpose_compute_state.reset();
		m_sort_transpose1_compute.reset();
		m_sort_transpose2_compute.reset();

		m_emitters_shader_view.reset();
		m_particles_shader_view.reset();
		m_particles_unordered_view.reset();
		m_alive_particles1_shader_view.reset();
		m_alive_particles1_unordered_view.reset();
		m_alive_particles2_shader_view.reset();
		m_alive_particles2_unordered_view.reset();
		m_dead_particles_unordered_view.reset();
		m_draw_args_shader_view.reset();
		
		m_emitters_buffer.reset();
		m_particles_buffer.reset();
		m_alive_particles_buffer1.reset();
		m_alive_particles_buffer2.reset();
		m_dead_particles_buffer.reset();
		m_draw_args_buffer.reset();
		m_sort_cbuffer.reset();
		m_curves_cbuffer.reset();
		m_lights_cbuffer.reset();
	}
}