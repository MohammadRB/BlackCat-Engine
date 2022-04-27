// [11/06/2021 MRB]

#include "Game/Object/Scene/Component/bcParticleEmitterComponent.h"
#include "Game/Object/Scene/Component/bcSoundComponent.h"
#include "Game/Object/Scene/Component/bcRigidDynamicComponent.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/bcJsonParse.h"
#include "Game/bcUtility.h"
#include "BoX.Game/Game/bxSmokeGrenadeActorController.h"

namespace box
{
	void bx_smoke_grenade_actor_controller::initialize(const game::bc_actor_component_initialize_context& p_context)
	{
		bc_rigid_dynamic_network_actor_controller::initialize(p_context);

		m_color = core::bc_vector3f(.7f);

		json_parse::bc_load_throw(p_context.m_parameters, "lifetime", m_lifetime);
		json_parse::bc_load_throw(p_context.m_parameters, "smoke_time", m_smoke_time);
		json_parse::bc_load(p_context.m_parameters, "color", m_color);
	}

	void bx_smoke_grenade_actor_controller::update_origin_instance(const game::bc_actor_component_update_content& p_context)
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

	void bx_smoke_grenade_actor_controller::update_replicated_instance(const game::bc_actor_component_update_content& p_context)
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
				.with_emission_direction_deviation(180)
				.with_texture(1)
				.with_particles_color(m_color, 1)
				.with_particle_size(2.0f, 10.0f)
				.with_particle_size_curve(game::bc_particle_builder::s_curve_fast_step5)
				.with_particles_rotation(20)
				.with_particle_velocity_curve(game::bc_particle_builder::s_curve_fast_step2, 4.5)
				.emit_particles_with_per_second_count(8, 20, 10, 0.1f)
				.duplicate_last(core::bc_vector3f(0), core::bc_vector3f::up(), m_smoke_time, 0, 0)
				.with_texture(4)
				.emit_particles_with_per_second_count(8, 20, 21, 0.1f)
				.duplicate_last(core::bc_vector3f(0), core::bc_vector3f::up(), m_smoke_time, 0, 0)
				.with_texture(5)
				.emit_particles_with_per_second_count(8, 20, 19, 0.1f);

			const auto l_actor_position = p_context.m_actor.get_component<game::bc_mediate_component>()->get_position();
			auto* l_emitter_component = p_context.m_actor.get_create_component<game::bc_particle_emitter_component>();
			l_emitter_component->add_emitter(l_emitter, l_actor_position);

			if(const auto* l_sound_component = p_context.m_actor.get_component<game::bc_sound_component>())
			{
				l_sound_component->play_sounds();
			}

			m_particles_added = true;
		}

		if (m_smoke_time > 0)
		{
			m_smoke_time -= p_context.m_clock.m_elapsed_second;
			if (m_smoke_time <= 0)
			{
				// actor will be removed by its network component if network has started
				if (get_network_type() == game::bc_network_type::not_started)
				{
					get_scene()->remove_actor(p_context.m_actor);
				}
				return;
			}
		}
	}

	void bx_smoke_grenade_actor_controller::handle_event(const game::bc_actor_component_event_context& p_context)
	{
	}
}
