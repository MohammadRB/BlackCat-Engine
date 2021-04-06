// [04/03/2021 MRB]

#include "Game/GamePCH.h"

#include "Core/Concurrency/bcConcurrency.h"
#include "PhysicsImp/Fundation/bcScene.h"
#include "Game/System/Physics/bcPhysicsSystem.h"
#include "Game/Object/Scene/Bullet/bcBulletManager.h"
#include "Game/Object/Mesh/bcMeshUtility.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/Object/Scene/Component/Event/bcBulletHitActorEvent.h"

namespace black_cat
{
	namespace game
	{
		bc_bullet_manager::bc_bullet_manager(bc_physics_system& p_physics_system)
			: m_physics_system(&p_physics_system),
			m_bullets(100, core::bc_alloc_type::unknown_movable)
		{
		}

		bc_bullet_manager::bc_bullet_manager(bc_bullet_manager&& p_other) noexcept
			: m_physics_system(p_other.m_physics_system),
			m_bullets(std::move(p_other.m_bullets))
		{
		}

		bc_bullet_manager::~bc_bullet_manager() = default;

		bc_bullet_manager& bc_bullet_manager::operator=(bc_bullet_manager&& p_other) noexcept
		{
			m_physics_system = p_other.m_physics_system;
			m_bullets = std::move(p_other.m_bullets);
			return *this;
		}

		void bc_bullet_manager::add_bullet(const bc_bullet& p_bullet)
		{
			{
				core_platform::bc_spin_mutex_guard l_lock(m_bullets_mutex);
				m_bullets.push_back(p_bullet);
			}
		}

		void bc_bullet_manager::update(bc_scene& p_scene, const core_platform::bc_clock::update_param& p_clock)
		{
			auto l_scene_bound_box = p_scene.get_scene_graph().get_bound_box();
			auto& l_px_scene = p_scene.get_px_scene();
			
			{
				core_platform::bc_spin_mutex_guard l_lock(m_bullets_mutex);
				physics::bc_scene_shared_lock l_px_lock(&l_px_scene);

				physics::bc_scene_ray_query_buffer l_query_buffer(1);
				auto l_gravity = l_px_scene.get_gravity();
				auto l_ite = std::begin(m_bullets);
				const auto l_end = std::end(m_bullets);
				
				while(l_ite != l_end)
				{
					_scene_ray_query
					(
						l_px_scene,
						physics::bc_ray(l_ite->m_position, l_ite->m_direction, l_ite->m_speed * p_clock.m_elapsed_second), 
						l_query_buffer
					);

					if(!l_query_buffer.has_block())
					{
						l_ite->m_direction += (l_gravity * l_ite->m_mass / l_ite->m_speed) * p_clock.m_elapsed_second;
						l_ite->m_direction.normalize();
						l_ite->m_position += l_ite->m_direction * l_ite->m_speed * p_clock.m_elapsed_second;

						if(!l_scene_bound_box.contains(l_ite->m_position))
						{
							l_ite = m_bullets.erase(l_ite);
						}
						else
						{
							++l_ite;
						}

						continue;
					}

					auto l_query_hit = l_query_buffer.get_block();
					auto l_actor = m_physics_system->get_game_actor(l_query_hit.get_actor());
					l_actor.add_event(bc_bullet_hit_actor_event
					(
						l_actor,
						l_query_hit.get_shape(),
						l_query_hit.get_position(),
						l_query_hit.get_normal(),
						l_query_hit.get_face_index(),
						l_ite->get_mass(),
						l_ite->get_speed()
					));
					
					l_ite = m_bullets.erase(l_ite);
				}
			}
		}

		core::bc_task<void> bc_bullet_manager::update_async(bc_scene& p_scene, const core_platform::bc_clock::update_param& p_clock)
		{
			return core::bc_concurrency::start_task
			(
				core::bc_delegate<void()>
				(
					[this, &p_scene ,&p_clock]()
					{
						update(p_scene, p_clock);
					}
				)
			);
		}

		void bc_bullet_manager::_scene_ray_query(const physics::bc_scene& p_px_scene, const physics::bc_ray& p_ray, physics::bc_scene_ray_query_buffer& p_buffer)
		{
			auto l_query_filter = physics::bc_scene_query_post_filter_callback
			(
				[&p_ray, this](const physics::bc_scene_query_post_filter_data& p_filter_data)
				{
					return bc_mesh_utility::skinned_mesh_ray_hit_test(*m_physics_system, p_ray, p_filter_data).first;
				}
			);
			
			const auto l_hit = p_px_scene.raycast
			(
				p_ray,
				p_buffer,
				physics::bc_hit_flag::hit_info,
				core::bc_enum::mask_or({physics::bc_query_flags::statics, physics::bc_query_flags::dynamics}),
				physics::bc_query_group::all,
				&l_query_filter
			);
		}
	}	
}