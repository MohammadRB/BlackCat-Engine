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
#include "Game/bcJsonParse.h"
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

		const core::bc_string* l_emitter_name;
		const core::bc_string* l_decal_name;

		json_parse::bc_load(p_context.m_parameters, constant::g_param_particle_emitter_name, l_emitter_name);
		json_parse::bc_load(p_context.m_parameters, constant::g_param_decal_name, l_decal_name);
		json_parse::bc_load(p_context.m_parameters, constant::g_param_rigid_force_amount, m_force_amount);
		json_parse::bc_load(p_context.m_parameters, constant::g_param_rigid_force_radius, m_force_radius);

		m_emitter_name = l_emitter_name->c_str();
		m_decal_name = l_decal_name->c_str();

		if(m_light_component)
		{
			const auto* l_point_light = m_light_component->get_light()->as_point_light();
			m_light_intensity = l_point_light->get_intensity();
			m_light_particle_intensity = l_point_light->get_particle_intensity();
			m_light_flare_intensity = l_point_light->get_flare() ? l_point_light->get_flare()->get_intensity() : 0.f;
			m_light_radius = l_point_light->get_radius();
			m_light_rise_per_second = m_light_radius * 0.7f / m_light_lifetime_second;
		}

		if(const auto* l_sound_component = p_context.m_actor.get_component<game::bc_sound_component>())
		{
			m_sound_lifetime_second = l_sound_component->get_max_length() / 1000.f;
		}

		json_parse::bc_load(p_context.m_instance_parameters, constant::g_param_player_id, m_player_id);
	}

	void bx_explosion_actor_controller::added_to_scene(const game::bc_actor_component_event_context& p_context, game::bc_scene& p_scene)
	{
		m_scene = &p_scene;
		const auto& l_mediate_component = *p_context.m_actor.get_component<game::bc_mediate_component>();
		const auto l_position = l_mediate_component.get_position();

		m_scene_terrain_query = _build_terrain_query(p_context.m_query_manager, l_position);
		m_scene_dynamics_query = _build_dynamics_query(p_context.m_query_manager, l_position);
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

			const auto l_dynamics_hit_result = std::move(*m_scene_dynamics_query.get().get_result().as<core::bc_vector<game::bc_overlap_hit>>());
			for(const auto& l_hit : l_dynamics_hit_result)
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

	core::bc_query_result<game::bc_scene_query> bx_explosion_actor_controller::_build_terrain_query(core::bc_query_manager& p_query_manager, const core::bc_vector3f& p_explosion_position) const
	{
		auto l_scene_query = game::bc_scene_query
		(
			[&, this, p_explosion_position](const game::bc_scene_query_context& p_query_context)
			{
				auto& l_px_scene = m_scene->get_px_scene();
				const auto l_ray = physics::bc_ray(p_explosion_position + core::bc_vector3f(0, 1, 0), core::bc_vector3f(0, -1, 0), 2 * m_scene->get_global_scale());
				auto l_query_buffer = physics::bc_scene_ray_query_buffer();
				bool l_has_collided;

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

				if (l_has_collided)
				{
					const auto l_ray_hit = l_query_buffer.get_block();
					if ((l_ray_hit.get_position() - p_explosion_position).magnitude() < m_light_radius * 0.6f) // If explosion is close to height map
					{
						l_result.reset(l_ray_hit);
					}
				}

				return core::bc_any(l_result);
			}
		);

		return p_query_manager.queue_query(std::move(l_scene_query));
	}

	core::bc_query_result<game::bc_scene_query> bx_explosion_actor_controller::_build_dynamics_query(core::bc_query_manager& p_query_manager, const core::bc_vector3f& p_explosion_position) const
	{
		auto l_scene_query = game::bc_scene_query
		(
			[&, this, p_explosion_position](const game::bc_scene_query_context& p_query_context)
			{
				auto& l_px_scene = m_scene->get_px_scene();
				auto l_query_buffer = physics::bc_scene_overlap_query_buffer(100);
				core::bc_vector<game::bc_overlap_hit> l_hits;

				{
					physics::bc_scene_shared_lock l_lock(&l_px_scene);

					l_px_scene.overlap
					(
						physics::bc_shape_sphere(m_force_radius),
						physics::bc_transform(p_explosion_position),
						l_query_buffer,
						physics::bc_query_flags::dynamics
					);

					const auto l_num_touches = l_query_buffer.get_touch_count();
					l_hits.reserve(l_num_touches);

					for (auto l_ite = 0U; l_ite < l_num_touches; ++l_ite)
					{
						const auto l_hit = l_query_buffer.get_touch(l_ite);
						const auto l_find_ite = std::find_if
						(
							std::begin(l_hits),
							std::end(l_hits),
							[&](const game::bc_overlap_hit& p_entry)
							{
								return p_entry.get_px_actor() == l_hit.get_actor();
							}
						);
						if (l_find_ite != std::end(l_hits))
						{
							continue;
						}

						l_hits.push_back(l_hit);
					}

					const auto l_remove_ite = std::remove_if
					(
						std::begin(l_hits),
						std::end(l_hits),
						[&](const auto& l_hit)
						{
							const auto l_actor_is_visible = _test_actor_visibility(l_px_scene, l_hit, p_explosion_position);
							return !l_actor_is_visible;
						}
					);
					l_hits.erase(l_remove_ite, std::end(l_hits));
				}

				return core::bc_any(std::move(l_hits));
			}
		);
		return p_query_manager.queue_query(std::move(l_scene_query));
	}

	bool bx_explosion_actor_controller::_test_actor_visibility(const physics::bc_scene& p_px_scene, const game::bc_overlap_hit& p_hit, const core::bc_vector3f& p_explosion_position) const
	{
		const auto l_hit_actor = p_hit.get_actor();
		const auto& l_hit_mediate_component = *l_hit_actor.get_component<game::bc_mediate_component>();
		const auto& l_hit_bound_box = l_hit_mediate_component.get_bound_box();
		const auto l_ray_target = l_hit_bound_box.get_center();
		const auto l_ray_direction = l_ray_target - p_explosion_position;
		const auto l_ray = physics::bc_ray(p_explosion_position, core::bc_vector3f::normalize(l_ray_direction), l_ray_direction.magnitude());
		auto l_ray_hit_buffer = physics::bc_scene_ray_query_buffer();

		const auto l_hit_result = p_px_scene.raycast
		(
			l_ray,
			l_ray_hit_buffer,
			physics::bc_hit_flag::hit_info,
			physics::bc_query_flags::statics
		);
		
		// if there is no hit it means actor is visible because we search for static actors
		return !l_hit_result || (l_hit_result && static_cast<game::bc_ray_hit>(l_ray_hit_buffer.get_block()).get_actor() == l_hit_actor);
	}
}
