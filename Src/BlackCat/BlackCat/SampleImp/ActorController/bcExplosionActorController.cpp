// [12/19/2020 MRB]

#include "BlackCat/BlackCatPCH.h"

#include "Core/Messaging/Query/bcQueryManager.h"
#include "Core/Utility/bcEnumOperand.h"
#include "Core/Utility/bcRandom.h"
#include "Core/Utility/bcNullable.h"
#include "Core/bcUtility.h"
#include "Game/System/bcGameSystem.h"
#include "Game/System/Physics/bcPxWrap.h"
#include "Game/System/Render/Particle/bcParticleManager.h"
#include "Game/Object/Scene/bcEntityManager.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/Object/Scene/Component/bcLightComponent.h"
#include "Game/Object/Scene/Component/bcHeightMapComponent.h"
#include "Game/Object/Scene/Component/bcParticleEmitterComponent.h"
#include "Game/Object/Scene/Component/bcDecalComponent.h"
#include "BlackCat/SampleImp/ActorController/bcExplosionActorController.h"

namespace black_cat
{
	void bc_explosion_actor_controller::initialize(const game::bc_actor_component_initialize_context& p_context)
	{
		auto* l_light_component = p_context.m_actor.get_component<game::bc_light_component>();
		auto* l_emitter_component = p_context.m_actor.get_component<game::bc_particle_emitter_component>();

		if (!l_light_component || !l_emitter_component)
		{
			throw bc_invalid_operation_exception("explosion actor must have light and emitter components");
		}

		m_light_intensity = l_light_component->get_light()->as_point_light()->get_intensity();
		m_light_particle_intensity = l_light_component->get_light()->as_point_light()->get_particle_intensity();
		m_light_radius = l_light_component->get_light()->as_point_light()->get_radius();
		m_light_rise_per_second = m_light_radius * 0.8f / m_light_lifetime_second;
	}

	void bc_explosion_actor_controller::added_to_scene(const game::bc_actor_component_event_context& p_context, game::bc_scene& p_scene)
	{
		m_scene = &p_scene;
		m_scene_terrain_query = p_context.m_query_manager.queue_query(game::bc_scene_query().with_callable
		(
			[this, l_scene = &p_scene, l_actor = p_context.m_actor](const game::bc_scene_query_context& p_query_context)
			{
				const auto& l_px_scene = l_scene->get_px_scene();
				const auto l_position = l_actor.get_component<game::bc_mediate_component>()->get_position();

				const physics::bc_ray l_ray(core::bc_vector3f(l_position.x, l_position.y + 10, l_position.z), core::bc_vector3f(0, -1, 0), 20);
				physics::bc_scene_ray_query_buffer l_query_buffer(0);

				const bool l_has_collided = l_px_scene.raycast
				(
					l_ray,
					l_query_buffer,
					core::bc_enum::mask_or({physics::bc_hit_flag::position, physics::bc_hit_flag::normal, physics::bc_hit_flag::face_index}),
					physics::bc_query_flags::statics,
					static_cast<physics::bc_query_group>(game::bc_actor_group::terrain)
				);

				core::bc_nullable<game::bc_ray_hit> l_result;
				
				if(l_has_collided)
				{
					const auto l_ray_hit = l_query_buffer.get_block();
					if((l_ray_hit.get_position() - l_position).magnitude() < m_light_radius * 0.6f) // If explosion is close to height map
					{
						l_result.reset(l_ray_hit);
					}
				}
				
				return core::bc_any(l_result);
			}
		));
	}

	void bc_explosion_actor_controller::update(const game::bc_actor_component_update_content& p_context)
	{
		if(m_scene_terrain_query.is_executed())
		{
			auto l_hit_result = *m_scene_terrain_query.get().get_result().as<core::bc_nullable<game::bc_ray_hit>>();

			auto* l_mediate_component = p_context.m_actor.get_component<game::bc_mediate_component>();
			auto* l_emitter_component = p_context.m_actor.get_component<game::bc_particle_emitter_component>();

			if(l_hit_result.is_set())
			{
				m_light_direction = l_hit_result->get_normal();

				auto* l_height_map_component = l_hit_result->get_actor().get_component<game::bc_height_map_component>();
				const auto l_px_height_map = l_height_map_component->get_height_map().get_px_height_field();
				const auto l_height_map_material_index = l_px_height_map.get_triangle_material(l_hit_result->get_face_index());
				const auto& l_material = l_height_map_component->get_height_map().get_material(l_height_map_material_index);
				const auto l_color = l_material.m_mesh_material->get_diffuse().xyz();
				
				l_emitter_component->spawn_emitter(l_mediate_component->get_position(), m_light_direction, &l_color);

				const auto* l_decal_component = p_context.m_actor.get_component<game::bc_decal_component>();
				if(l_decal_component)
				{
					l_height_map_component->add_decal(l_decal_component->get_decal_name(), l_hit_result->get_position(), m_light_direction);
				}
			}
			else
			{
				l_emitter_component->spawn_emitter(l_mediate_component->get_position(), core::bc_vector3f::up());
			}

			m_has_started = true;
		}

		if(!m_has_started)
		{
			return;
		}
		
		auto* l_light_component = p_context.m_actor.get_component< game::bc_light_component >();
		auto* l_point_light = l_light_component->get_light()->as_point_light();

		const auto l_normal_age = pow(1 - (m_age / m_light_lifetime_second), 2);
		l_point_light->set_intensity(m_light_intensity * l_normal_age);
		l_point_light->set_particle_intensity(m_light_particle_intensity * l_normal_age);
		l_point_light->set_position
		(
			l_point_light->get_position() + m_light_direction * m_light_rise_per_second * p_context.m_clock.
			m_elapsed_second
		);

		m_age += p_context.m_clock.m_elapsed_second;
		if (m_age > m_light_lifetime_second)
		{
			m_scene->remove_actor(p_context.m_actor);
		}
	}
}