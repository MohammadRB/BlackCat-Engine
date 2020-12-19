// [12/19/2020 MRB]

#include "Game/GamePCH.h"

#include "Core/Utility/bcUtility.h"
#include "Core/Utility/bcRandom.h"
#include "Game/System/bcGameSystem.h"
#include "Game/System/Render/Particle/bcParticleManager.h"
#include "Game/Object/Scene/bcEntityManager.h"
#include "Game/Object/Scene/Component/bcLightComponent.h"
#include "Game/Object/Scene/Component/bcParticleEmitterComponent.h"
#include "Game/Object/Scene/Component/Controller/bcExplosionActorController.h"

namespace black_cat
{
	namespace game
	{
		void bc_explosion_actor_controller::initialize(bc_actor& p_actor)
		{
			auto* l_light_component = p_actor.get_component< bc_light_component >();
			auto* l_emitter_component = p_actor.get_component< bc_particle_emitter_component >();

			if (!l_light_component || !l_emitter_component)
			{
				throw bc_invalid_operation_exception("explosion actor must have light and emitter components");
			}

			m_light_intensity = l_light_component->get_light()->as_point_light()->get_intensity();
			m_light_particle_intensity = l_light_component->get_light()->as_point_light()->get_particle_intensity();
			m_light_radius = l_light_component->get_light()->as_point_light()->get_radius();
			m_light_rise_per_second = m_light_radius * 0.8f / m_light_lifetime_second;
		}

		void bc_explosion_actor_controller::added_to_scene(bc_actor& p_actor, bc_scene& p_scene)
		{
			m_scene = &p_scene;
						
			auto* l_emitter_component = p_actor.get_component< bc_particle_emitter_component >();
			l_emitter_component->spawn_emitter();
		}

		void bc_explosion_actor_controller::update(bc_actor& p_actor, const core_platform::bc_clock::update_param& p_clock)
		{
			auto* l_light_component = p_actor.get_component< bc_light_component >();
			auto* l_point_light = l_light_component->get_light()->as_point_light();

			const auto l_normal_age = pow(1 - (m_age / m_light_lifetime_second), 2);
			l_point_light->set_intensity(m_light_intensity * l_normal_age);
			l_point_light->set_particle_intensity(m_light_particle_intensity * l_normal_age);
			l_point_light->set_position
			(
				l_point_light->get_position() + core::bc_vector3f::up() * m_light_rise_per_second * p_clock.
				m_elapsed_second
			);

			m_age += p_clock.m_elapsed_second;
			if (m_age > m_light_lifetime_second)
			{
				m_scene->remove_actor(p_actor);
			}
		}
	}
}