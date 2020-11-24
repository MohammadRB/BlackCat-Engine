// [11/16/2020 MRB]

#include "BlackCat/BlackCatPCH.h"

#include "Core/Container/bcVector.h"
#include "Core/Messaging/Query/bcQueryManager.h"
#include "Core/Utility/bcEnumOperand.h"
#include "GraphicImp/Shader/Parameter/bcResourceViewParameter.h"
#include "GraphicImp/Resource/bcResourceBuilder.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/Particle/bcParticleEmitter.h"
#include "BlackCat/RenderPass/PostProcess/bcParticleSystemDx11.h"
#include "BlackCat/bcConstant.h"

namespace black_cat
{
	struct _bc_emitter_struct : public game::bc_particle_emitter
	{
	};
	
	struct _bc_particle_struct
	{
		core::bc_vector3f m_position;
		bcFLOAT m_lifetime;
		core::bc_vector3f m_direction;
		bcFLOAT m_age;
		bcFLOAT m_force;
		bcFLOAT m_mass;
		bcUINT32 m_texture_index;
		bcUINT32 m_sprite_index;
	};

	struct _bc_alive_particle_struct
	{
		bcUINT32 m_index;
		bcFLOAT m_distance;
	};
	
	void bc_particle_system_dx11::initialize_resources(game::bc_render_system& p_render_system)
	{
		auto& l_device = p_render_system.get_device();

		m_command_list = l_device.create_command_list();
		
		auto l_emitters_buffer_config = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_buffer(m_emitters_count, sizeof(_bc_emitter_struct), graphic::bc_resource_usage::gpu_r_cpu_w, graphic::bc_resource_view_type::shader)
			.with_structured_buffer(sizeof(_bc_emitter_struct))
			.as_buffer();
		auto l_particles_buffer_config = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_buffer
			(
				m_particles_count, 
				sizeof(_bc_particle_struct), 
				graphic::bc_resource_usage::gpu_rw, 
				core::bc_enum::or({ graphic::bc_resource_view_type::shader, graphic::bc_resource_view_type::unordered })
			)
			.with_structured_buffer(sizeof(_bc_particle_struct))
			.as_buffer();
		auto l_alive_particles_buffer = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_buffer
			(
				m_particles_count,
				sizeof(_bc_alive_particle_struct), 
				graphic::bc_resource_usage::gpu_rw, 
				core::bc_enum::or({ graphic::bc_resource_view_type::shader, graphic::bc_resource_view_type::unordered })
			)
			.with_structured_buffer(sizeof(_bc_alive_particle_struct))
			.as_buffer();
		auto l_dead_particles_buffer = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_buffer(m_particles_count, sizeof(bcUINT32), graphic::bc_resource_usage::gpu_rw, graphic::bc_resource_view_type::unordered)
			.with_structured_buffer(sizeof(bcUINT32))
			.with_append_consume(sizeof(bcUINT32))
			.as_buffer();
		auto l_draw_args_buffer = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_buffer(4, sizeof(bcUINT32), graphic::bc_resource_usage::gpu_rw, graphic::bc_resource_view_type::shader)
			.with_indirect_args_buffer()
			.as_buffer();

		bcUINT32 l_dead_particle_index = 0;
		core::bc_vector_frame<bcUINT32> l_dead_particle_indices(m_particles_count);
		std::for_each(std::begin(l_dead_particle_indices), std::end(l_dead_particle_indices), [&l_dead_particle_index](bcUINT32& p_index)
		{
			p_index = l_dead_particle_index++;
		});
		core::bc_array<bcUINT32, 4> l_draw_args = { 0,0,0,0 };
		
		graphic::bc_subresource_data l_dead_particles_data(l_dead_particle_indices.data(), 0, 0);
		graphic::bc_subresource_data l_draw_args_data(l_draw_args.data(), 0, 0);

		m_emitters_buffer = l_device.create_buffer(l_emitters_buffer_config, nullptr);
		m_particles_buffer = l_device.create_buffer(l_particles_buffer_config, nullptr);
		m_alive_particles_buffer = l_device.create_buffer(l_alive_particles_buffer, nullptr);
		m_dead_particles_buffer = l_device.create_buffer(l_dead_particles_buffer, &l_dead_particles_data);
		m_draw_args_buffer = l_device.create_buffer(l_draw_args_buffer, &l_draw_args_data);
		
		auto l_emitters_buffer_view_config = graphic::bc_graphic_resource_builder()
			.as_resource_view()
			.as_buffer_view(graphic::bc_format::unknown)
			.as_shader_view(0, m_emitters_count)
			.as_structured_buffer();
		auto l_particles_buffer_view_config = graphic::bc_graphic_resource_builder()
			.as_resource_view()
			.as_buffer_view(graphic::bc_format::unknown)
			.as_unordered_view(0, m_particles_count)
			.as_structured_buffer();
		auto l_alive_particles_buffer_view_config = graphic::bc_graphic_resource_builder()
			.as_resource_view()
			.as_buffer_view(graphic::bc_format::unknown)
			.as_unordered_view(0, m_particles_count)
			.as_structured_buffer();
		auto l_dead_particles_buffer_view_config = graphic::bc_graphic_resource_builder()
			.as_resource_view()
			.as_buffer_view(graphic::bc_format::unknown)
			.as_unordered_view(0, m_particles_count)
			.as_append_consume();
		auto l_draw_args_buffer_view_config = graphic::bc_graphic_resource_builder()
			.as_resource_view()
			.as_buffer_view(graphic::bc_format::R32_UINT)
			.as_shader_view(0, 4)
			.as_indirect_args_buffer();

		m_emitters_buffer_view = l_device.create_resource_view(m_emitters_buffer.get(), l_emitters_buffer_view_config);
		m_particles_buffer_view = l_device.create_resource_view(m_particles_buffer.get(), l_particles_buffer_view_config);
		m_alive_particles_buffer_view = l_device.create_resource_view(m_alive_particles_buffer.get(), l_alive_particles_buffer_view_config);
		m_dead_particles_buffer_view = l_device.create_resource_view(m_dead_particles_buffer.get(), l_dead_particles_buffer_view_config);
		m_draw_args_buffer_view = l_device.create_resource_view(m_draw_args_buffer.get(), l_draw_args_buffer_view_config);

		m_emission_compute_state = p_render_system.create_device_compute_state("particle_emission");
		m_simulation_compute_state = p_render_system.create_device_compute_state("particle_simulation");
		m_sort_compute_state = p_render_system.create_device_compute_state("particle_sort");

		graphic::bc_device_parameters l_old_parameters
		(
			0,
			0,
			graphic::bc_format::unknown,
			graphic::bc_texture_ms_config(1, 0)
		);
		graphic::bc_device_parameters l_new_parameters
		(
			l_device.get_back_buffer_width(),
			l_device.get_back_buffer_height(),
			l_device.get_back_buffer_format(),
			l_device.get_back_buffer_texture().get_sample_count()
		);

		after_reset(game::bc_render_pass_reset_param(p_render_system, l_device, l_old_parameters, l_new_parameters));
	}

	void bc_particle_system_dx11::update(const game::bc_render_pass_update_param& p_param)
	{
	}

	void bc_particle_system_dx11::initialize_frame(const game::bc_render_pass_render_param& p_param)
	{
		if(m_emitters_query.is_executed())
		{
			m_emitters_query_result = m_emitters_query.get().get_emitters();
		}

		m_emitters_query = core::bc_get_service<core::bc_query_manager>()->queue_query
		(
			game::bc_particle_emitter_query()
		);
	}

	void bc_particle_system_dx11::execute(const game::bc_render_pass_render_param& p_param)
	{
		p_param.m_render_thread.start(m_command_list.get());
		
		if(!m_emitters_query_result.empty())
		{
			p_param.m_render_thread.bind_compute_state(*m_emission_compute, { -1, m_dead_particles_counter });
			p_param.m_render_thread.update_subresource(*m_emitters_buffer, 0, m_emitters_query_result.data(), m_emitters_query_result.size(), 1);
			p_param.m_render_thread.dispatch(m_emitters_query_result.size(), 1, 1);
			p_param.m_render_thread.unbind_compute_state(*m_emission_compute);
		}

		p_param.m_render_thread.bind_compute_state(*m_simulation_compute, { -1, 0, m_dead_particles_counter });
		p_param.m_render_thread.dispatch(m_particles_count / m_simulation_shader_group_size, 1, 1);
		p_param.m_render_thread.unbind_compute_state(*m_simulation_compute);

		p_param.m_render_thread.copy_structure_count(*m_draw_args_buffer, sizeof(bcUINT32), *m_alive_particles_buffer_view);
		
		p_param.m_render_thread.bind_compute_state(*m_sort_compute);
		p_param.m_render_thread.dispatch((m_particles_count / m_sort_shader_group_size) / 2, 1, 1);
		p_param.m_render_thread.unbind_compute_state(*m_sort_compute);

		p_param.m_render_thread.finish();
		m_command_list->finished();
		
		m_dead_particles_counter = -1;
	}

	void bc_particle_system_dx11::before_reset(const game::bc_render_pass_reset_param& p_param)
	{
	}

	void bc_particle_system_dx11::after_reset(const game::bc_render_pass_reset_param& p_param)
	{
		m_depth_buffer_view = get_shared_resource_throw<graphic::bc_resource_view>(constant::g_rpass_depth_stencil_read_view);
		m_emission_compute = p_param.m_render_system.create_compute_state
		(
			m_emission_compute_state.get(),
			{},
			{
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::compute, m_emitters_buffer_view.get()),
			},
			{
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::compute, m_particles_buffer_view.get()),
				graphic::bc_resource_view_parameter(1, graphic::bc_shader_type::compute, m_dead_particles_buffer_view.get())
			},
			{
				p_param.m_render_system.get_global_cbuffer()
			}
		);
		m_simulation_compute = p_param.m_render_system.create_compute_state
		(
			m_simulation_compute_state.get(),
			{},
			{},
			{
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::compute, m_particles_buffer_view.get()),
				graphic::bc_resource_view_parameter(1, graphic::bc_shader_type::compute, m_alive_particles_buffer_view.get()),
				graphic::bc_resource_view_parameter(2, graphic::bc_shader_type::compute, m_dead_particles_buffer_view.get())
			},
			{
				p_param.m_render_system.get_global_cbuffer()
			}
		);
		m_sort_compute = p_param.m_render_system.create_compute_state
		(
			m_sort_compute_state.get(),
			{},
			{},
			{
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::compute, m_alive_particles_buffer_view.get()),
				graphic::bc_resource_view_parameter(3, graphic::bc_shader_type::compute, m_draw_args_buffer_view.get()),
			},
			{
				p_param.m_render_system.get_global_cbuffer()
			}
		);
	}

	void bc_particle_system_dx11::destroy(game::bc_render_system& p_render_system)
	{
		m_emitters_query = core::bc_query_result<game::bc_particle_emitter_query>();
		m_emitters_query_result.clear();
		m_emitters_query_result.shrink_to_fit();
		
		m_sort_compute.reset();
		m_sort_compute_state.reset();
		m_simulation_compute.reset();
		m_simulation_compute_state.reset();
		m_emission_compute.reset();
		m_emission_compute_state.reset();

		m_draw_args_buffer_view.reset();
		m_dead_particles_buffer_view.reset();
		m_alive_particles_buffer_view.reset();
		m_particles_buffer_view.reset();
		m_emitters_buffer_view.reset();

		m_draw_args_buffer.reset();
		m_dead_particles_buffer.reset();
		m_alive_particles_buffer.reset();
		m_particles_buffer.reset();
		m_emitters_buffer.reset();

		m_command_list.reset();
	}
}