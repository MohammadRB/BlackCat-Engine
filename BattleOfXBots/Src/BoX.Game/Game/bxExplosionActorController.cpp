// [12/19/2020 MRB]

#include "Core/Messaging/Query/bcQueryManager.h"
#include "Core/Utility/bcEnumOperand.h"
#include "Core/Utility/bcRandom.h"
#include "Core/Utility/bcNullable.h"
#include "Core/bcUtility.h"
#include "Game/System/bcGameSystem.h"
#include "Game/System/Physics/bcPxWrap.h"
#include "Game/System/Physics/bcActorGroup.h"
#include "Game/System/Render/Particle/bcParticleManager.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/Object/Scene/Component/bcLightComponent.h"
#include "Game/Object/Scene/Component/bcSoundComponent.h"
#include "Game/Object/Scene/Component/bcHeightMapComponent.h"
#include "Game/Object/Scene/Component/bcParticleEmitterComponent.h"
#include "Game/Object/Scene/Component/bcDecalComponent.h"
#include "Game/Object/Scene/Component/Event/bcExplosionActorEvent.h"
#include "Game/Object/Scene/bcEntityManager.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/bcConstant.h"
#include "BoX.Game/Game/bxExplosionActorController.h"

namespace box
{
	void bx_explosion_actor_controller::initialize(const game::bc_actor_component_initialize_context& p_context)
	{
		m_light_component = p_context.m_actor.get_component<game::bc_light_component>();
		if (!m_light_component->get_light()->as_point_light())
		{
			m_light_component = nullptr;
		}

		m_emitter_name = p_context.m_parameters.get_value<core::bc_string>(constant::g_param_particle_emitter_name)->c_str();
		m_decal_name = p_context.m_parameters.get_value<core::bc_string>(constant::g_param_decal_name)->c_str();
		m_force_amount = *p_context.m_parameters.get_value<bcFLOAT>(constant::g_param_rigid_force_amount);
		m_force_radius = *p_context.m_parameters.get_value<bcFLOAT>(constant::g_param_rigid_force_radius);

		if(m_light_component)
		{
			const auto* l_point_light = m_light_component->get_light()->as_point_light();
			m_light_intensity = l_point_light->get_intensity();
			m_light_particle_intensity = l_point_light->get_particle_intensity();
			m_light_flare_intensity = l_point_light->get_flare() ? l_point_light->get_flare()->get_intensity() : 0.f;
			m_light_radius = l_point_light->get_radius();
			m_light_rise_per_second = m_light_radius * 0.7f / m_light_lifetime_second;
		}

		const auto* l_sound_component = p_context.m_actor.get_component<game::bc_sound_component>();
		if(l_sound_component)
		{
			m_sound_lifetime_second = l_sound_component->get_max_length() / 1000.f;
		}

		const auto* l_player_id = p_context.m_instance_parameters.get_value<game::bc_network_client_id>(constant::g_param_player_id);
		if (l_player_id)
		{
			m_player_id = *l_player_id;
		}
	}

	void bx_explosion_actor_controller::added_to_scene(const game::bc_actor_component_event_context& p_context, game::bc_scene& p_scene)
	{
		m_scene = &p_scene;
		m_scene_terrain_query = p_context.m_query_manager.queue_query(game::bc_scene_query().with_callable
		(
			[this, l_scene = &p_scene, l_actor = p_context.m_actor](const game::bc_scene_query_context& p_query_context)
			{
				auto& l_px_scene = l_scene->get_px_scene();
				const auto l_position = l_actor.get_component<game::bc_mediate_component>()->get_position();

				bool l_has_collided;
				const physics::bc_ray l_ray(l_position + core::bc_vector3f(0,1,0), core::bc_vector3f(0, -1, 0), 2 * l_scene->get_global_scale());
				physics::bc_scene_ray_query_buffer l_query_buffer;

				{
					physics::bc_scene_shared_lock l_lock(&l_px_scene);

					l_has_collided = l_px_scene.raycast
					(
						l_ray,
						l_query_buffer,
						core::bc_enum::mask_or({ physics::bc_hit_flag::position, physics::bc_hit_flag::normal, physics::bc_hit_flag::face_index }),
						physics::bc_query_flags::statics,
						static_cast<physics::bc_query_group>(game::bc_actor_physics_group::terrain)
					);
				}

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
		m_scene_dynamics_query = p_context.m_query_manager.queue_query(game::bc_scene_query().with_callable
		(
			[this, l_scene = &p_scene, l_actor = p_context.m_actor](const game::bc_scene_query_context& p_query_context)
			{
				auto& l_px_scene = l_scene->get_px_scene();
				const auto l_position = l_actor.get_component<game::bc_mediate_component>()->get_position();

				physics::bc_scene_overlap_query_buffer l_query_buffer(100);

				{
					physics::bc_scene_shared_lock l_lock(&l_px_scene);

					l_px_scene.overlap
					(
						physics::bc_shape_sphere(m_force_radius),
						physics::bc_transform(l_position),
						l_query_buffer,
						physics::bc_query_flags::dynamics
					);
				}

				const auto l_num_touches = l_query_buffer.get_touch_count();
				core::bc_vector<physics::bc_overlap_hit> l_result;
				l_result.reserve(l_num_touches);

				for(auto l_ite = 0U; l_ite < l_num_touches; ++l_ite)
				{
					const auto l_hit = l_query_buffer.get_touch(l_ite);
					const auto l_find_ite = std::find_if
					(
						std::begin(l_result),
						std::end(l_result),
						[&](const physics::bc_overlap_hit& p_entry)
						{
							return p_entry.get_actor() == l_hit.get_actor();
						}
					);
					if(l_find_ite != std::end(l_result))
					{
						continue;
					}

					l_result.push_back(l_hit);
				}

				return core::bc_any(std::move(l_result));
			}
		));
	}

	void bx_explosion_actor_controller::update(const game::bc_actor_component_update_content& p_context)
	{
		if(!m_has_started)
		{
			m_has_started = m_scene_terrain_query.is_executed() && m_scene_dynamics_query.is_executed();

			if(!m_has_started)
			{
				return;
			}

			const auto& l_explosion_mediate_component = *p_context.m_actor.get_component<game::bc_mediate_component>();
			m_position = l_explosion_mediate_component.get_position();
			m_direction = l_explosion_mediate_component.get_world_transform().get_basis_z();

			const auto l_terrain_hit_result = *m_scene_terrain_query.get().get_result().as<core::bc_nullable<game::bc_ray_hit>>();
			if (l_terrain_hit_result.has_value())
			{
				auto* l_height_map_component = l_terrain_hit_result->get_actor().get_component<game::bc_height_map_component>();
				const auto l_px_height_map = l_height_map_component->get_height_map().get_px_height_field();
				const auto l_height_map_material_index = l_px_height_map.get_triangle_material(l_terrain_hit_result->get_face_index());
				const auto& l_material = l_height_map_component->get_height_map().get_material(l_height_map_material_index);
				const auto l_color = l_material.m_mesh_material->get_diffuse().xyz();

				if (m_emitter_name)
				{
					auto& l_particle_manager = m_scene->get_particle_manager();
					l_particle_manager.spawn_emitter(m_emitter_name, m_position, m_direction, &l_color);
				}
				if (m_decal_name)
				{
					l_height_map_component->add_decal(m_decal_name, l_terrain_hit_result->get_position(), m_direction);
				}
			}
			else
			{
				if (m_emitter_name)
				{
					auto& l_particle_manager = m_scene->get_particle_manager();
					l_particle_manager.spawn_emitter(m_emitter_name, m_position, m_direction);
				}
			}

			const auto l_dynamics_hit_result = std::move(*m_scene_dynamics_query.get().get_result().as<core::bc_vector<physics::bc_overlap_hit>>());
			for(game::bc_overlap_hit l_hit : l_dynamics_hit_result)
			{
				auto l_actor = l_hit.get_actor();
				if(l_actor.is_valid())
				{
					l_hit.get_actor().add_event(game::bc_explosion_actor_event(m_position, m_force_radius, m_force_amount, m_player_id));
				}
			}
		}

		if(m_light_component)
		{
			auto* l_point_light = m_light_component->get_light()->as_point_light();

			const auto l_normal_age = pow(1 - std::min(1.f, m_age / m_light_lifetime_second), 2);
			l_point_light->set_intensity(m_light_intensity * l_normal_age);
			l_point_light->set_particle_intensity(m_light_particle_intensity * l_normal_age);
			l_point_light->set_flare_intensity(m_light_flare_intensity * l_normal_age);
			l_point_light->set_position
			(
				l_point_light->get_position() + m_direction * m_light_rise_per_second * p_context.m_clock.
				m_elapsed_second
			);
		}

		m_age += p_context.m_clock.m_elapsed_second;
		if (m_age > m_light_lifetime_second && m_age > m_sound_lifetime_second)
		{
			m_scene->remove_actor(p_context.m_actor);
			m_has_started = false;
		}
	}
}