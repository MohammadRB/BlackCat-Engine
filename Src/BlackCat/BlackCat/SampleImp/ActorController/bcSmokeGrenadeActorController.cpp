// [11/06/2021 MRB]

#include "BlackCat/BlackCatPCH.h"

#include "Core/Utility/bcJsonParse.h"
#include "Game/Object/Scene/Component/bcParticleEmitterComponent.h"
#include "Game/Object/Scene/Component/bcRigidDynamicComponent.h"
#include "BlackCat/SampleImp/ActorController/bcSmokeGrenadeActorController.h"

namespace black_cat
{
	void bc_smoke_grenade_actor_controller::initialize(const game::bc_actor_component_initialize_context& p_context)
	{
		bc_rigid_dynamic_network_actor_controller::initialize(p_context);
		
		m_lifetime = p_context.m_parameters.get_value_throw<bcFLOAT>("lifetime");
		m_smoke_time = p_context.m_parameters.get_value_throw<bcFLOAT>("smoke_time");
		m_num_particles_per_second = 20;
	}

	void bc_smoke_grenade_actor_controller::update_origin_instance(const game::bc_actor_component_update_content& p_context)
	{
		if (get_network_type() == game::bc_network_type::not_started)
		{
			update_replicated_instance(p_context);
			return;
		}
		
		if (!get_scene())
		{
			return;
		}

		if(m_lifetime > 0)
		{
			m_lifetime -= p_context.m_clock.m_elapsed_second;
			return;
		}

		if(m_smoke_time > 0)
		{
			m_smoke_time -= p_context.m_clock.m_elapsed_second;
			if(m_smoke_time <= 0)
			{
				get_scene()->remove_actor(p_context.m_actor);
				return;
			}
		}
	}

	void bc_smoke_grenade_actor_controller::update_replicated_instance(const game::bc_actor_component_update_content& p_context)
	{
		if (!get_scene())
		{
			return;
		}

		if (m_lifetime > 0)
		{
			m_lifetime -= p_context.m_clock.m_elapsed_second;
			return;
		}

		if (!m_particles_added)
		{
			const auto l_emitter = game::bc_particle_builder()
				.emitter(core::bc_vector3f(0), core::bc_vector3f::up(), m_smoke_time, 0, 0)
				.with_emission_deviation(180)
				.with_texture(1)
				.with_particles_color({ 0.8f, 0.8f, 0.8f }, 1)
				.with_particle_size(4.0f, 10.0f)
				.with_particles_rotation(10)
				.with_particle_velocity_curve(game::bc_particle_builder::s_curve_fast_step2, 5)
				.emit_particles(0, 20, 10, 0.1f)
				.duplicate_last(core::bc_vector3f(0), core::bc_vector3f::up(), m_smoke_time, 0, 0)
				.with_texture(4)
				.emit_particles(0, 20, 21, 0.1f)
				.duplicate_last(core::bc_vector3f(0), core::bc_vector3f::up(), m_smoke_time, 0, 0)
				.with_texture(5)
				.emit_particles(0, 20, 19, 0.1f);
			auto* l_emitter_component = p_context.m_actor.get_create_component<game::bc_particle_emitter_component>();
			l_emitter_component->add_emitter(l_emitter);

			m_particles_added = true;
		}

		if (m_smoke_time > 0)
		{
			m_smoke_time -= p_context.m_clock.m_elapsed_second;
			if (m_smoke_time <= 0)
			{
				if (get_network_type() == game::bc_network_type::not_started)
				{
					get_scene()->remove_actor(p_context.m_actor);
				}
				return;
			}
		}

		const bcUINT32 l_total_particles_in_current_second = (p_context.m_clock.m_total_elapsed_second - std::floor(p_context.m_clock.m_total_elapsed_second)) * m_num_particles_per_second;
		if (l_total_particles_in_current_second < m_num_spawned_particles_in_current_second)
		{
			m_num_spawned_particles_in_current_second = 0;
		}

		const bcUINT32 l_num_particles_in_current_second = l_total_particles_in_current_second - m_num_spawned_particles_in_current_second;
		const auto l_actor_position = p_context.m_actor.get_component<game::bc_mediate_component>()->get_position();

		auto* l_emitter = p_context.m_actor.get_component<game::bc_particle_emitter_component>()->get_emitter();
		l_emitter->set_positions(l_actor_position);
		l_emitter->set_particle_counts(l_num_particles_in_current_second);

		m_num_spawned_particles_in_current_second += l_num_particles_in_current_second;
	}

	void bc_smoke_grenade_actor_controller::handle_event(const game::bc_actor_component_event_context& p_context)
	{
	}
}
