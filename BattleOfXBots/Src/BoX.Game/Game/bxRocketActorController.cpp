// [04/27/2021 MRB]

#include "Core/Messaging/Query/bcQueryManager.h"
#include "Core/Utility/bcNullable.h"
#include "Game/System/Physics/bcPxWrap.h"
#include "Game/System/Physics/bcActorGroup.h"
#include "Game/Object/Scene/bcEntityManager.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/Object/Scene/Component/bcParticleEmitterComponent.h"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Game/Object/Scene/ActorComponent/bcActor.hpp"
#include "Game/Object/Scene/bcScene.h"
#include "Game/bcUtility.h"
#include "Game/bcConstant.h"
#include "BoX.Game/Game/bxRocketActorController.h"

namespace box
{
	void bx_rocket_controller::initialize(const game::bc_actor_component_initialize_context& p_context)
	{
		m_explosion_entity = p_context.m_parameters.get_value_throw<core::bc_string>("explosion_entity").c_str();
		m_speed = p_context.m_parameters.get_value_throw<bcFLOAT>(constant::g_param_weapon_bullet_speed);
	}

	void bx_rocket_controller::added_to_scene(const game::bc_actor_component_event_context& p_context, game::bc_scene& p_scene)
	{
		const auto& l_mediate_component = *p_context.m_actor.get_component<game::bc_mediate_component>();
		auto& l_particle_emitter_component = *p_context.m_actor.get_create_component<game::bc_particle_emitter_component>();
		
		m_scene = &p_scene;
		m_position = l_mediate_component.get_position();
		m_direction = core::bc_vector3f::normalize(l_mediate_component.get_world_transform().get_basis_z());
		m_deviated_position = m_position;

		const auto l_emitter = game::bc_particle_builder()
			.emitter(core::bc_vector3f(0), m_direction, std::numeric_limits<bcFLOAT>::max(), 0, 0)
			.with_emission_deviation(360)
			.with_texture(3)
			.with_particles_reverse_direction()
			.with_particles_color({ 0.8f, 0.8f, 0.8f }, 1)
			.with_particle_size(1.5f * m_scene->get_global_scale(), 4.5f * m_scene->get_global_scale())
			.with_particles_rotation(40)
			.with_particle_velocity_curve(game::bc_particle_builder::s_curve_fast_step3, 1.5f)
			.emit_particles_with_total_count(200, 3, 10, -0.1f);
		l_particle_emitter_component.add_emitter(l_emitter, m_position);
	}

	void bx_rocket_controller::update(const game::bc_actor_component_update_content& p_context)
	{
		if(!m_scene)
		{
			return;
		}
		
		if(m_scene_query.is_executed())
		{
			auto l_query_result = m_scene_query.get().get_result().as_throw<core::bc_nullable<game::bc_ray_hit>>();

			if(l_query_result.has_value())
			{
				auto l_explosion = m_scene->create_actor
				(
					m_explosion_entity,
					bc_matrix4f_from_position_and_direction(l_query_result->get_position(), l_query_result->get_normal())
				);
				l_explosion.mark_for_double_update();

				m_scene->remove_actor(p_context.m_actor);
				m_scene = nullptr;

				return;
			}
		}

		m_scene_query = p_context.m_query_manager.queue_query
		(
			game::bc_scene_query().with_callable([this, l_elapsed = p_context.m_clock.m_elapsed_second](const game::bc_scene_query_context& p_query_context)
			{
				auto& l_px_scene = m_scene->get_px_scene();

				const physics::bc_ray l_ray(m_deviated_position, m_direction, (m_direction* l_elapsed* m_speed).magnitude());
				physics::bc_scene_ray_query_buffer l_query_buffer;
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
					l_result = l_query_buffer.get_block();
				}

				return core::bc_any(l_result);
			})
		);

		constexpr auto l_deviation_cycle = 4;
		const auto l_deviation = 2.5f * m_scene->get_global_scale();
		const auto l_sin = std::sinf(static_cast<bcFLOAT>(p_context.m_clock.m_total_elapsed_second) * l_deviation_cycle);
		const auto l_cos = std::cosf(static_cast<bcFLOAT>(p_context.m_clock.m_total_elapsed_second) * l_deviation_cycle);
		m_position += m_direction * (m_speed * p_context.m_clock.m_elapsed_second);

		auto l_world_transform = bc_matrix4f_from_position_and_direction(m_position, m_direction);
		
		m_deviated_position = m_position;
		m_deviated_position += l_world_transform.get_basis_x() * l_sin * l_deviation;
		m_deviated_position += l_world_transform.get_basis_y() * l_cos * l_deviation;
		l_world_transform.set_translation(m_deviated_position);
		
		p_context.m_actor.add_event(game::bc_world_transform_actor_event(l_world_transform));
	}
}