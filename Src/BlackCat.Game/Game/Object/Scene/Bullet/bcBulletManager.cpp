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
		bc_bullet_manager::bc_bullet_manager(bc_physics_system& p_physics_system) noexcept
			: m_physics_system(&p_physics_system),
			m_bullets(100)
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

		core::bc_vector_frame<bc_bullet> bc_bullet_manager::get_bullets() const
		{
			core::bc_vector_frame<bc_bullet> l_bullets_copy;

			{
				core_platform::bc_lock_guard l_lock(m_bullets_mutex);

				l_bullets_copy.reserve(m_bullets.size());

				std::copy(std::begin(m_bullets), std::end(m_bullets), std::back_inserter(l_bullets_copy));
			}

			return l_bullets_copy;
		}

		void bc_bullet_manager::add_bullet(const bc_bullet& p_bullet) noexcept
		{
			{
				core_platform::bc_spin_mutex_guard l_lock(m_bullets_mutex);
				m_bullets.push_back(p_bullet);
			}
		}

		void bc_bullet_manager::update(bc_scene& p_scene, const core_platform::bc_clock::update_param& p_clock) noexcept
		{
			const auto l_scene_bound_box = p_scene.get_scene_graph().get_bound_box();
			auto& l_px_scene = p_scene.get_px_scene();
			
			{
				core_platform::bc_spin_mutex_guard l_lock(m_bullets_mutex);
				physics::bc_scene_shared_lock l_px_lock(&l_px_scene);

				physics::bc_scene_ray_query_buffer l_query_buffer;
				core::bc_vector_frame<physics::bc_ray_hit> l_skinned_mesh_hits_buffer;
				l_skinned_mesh_hits_buffer.reserve(4);

				const auto l_gravity = l_px_scene.get_gravity();
				auto l_ite = std::begin(m_bullets);
				const auto l_end = std::end(m_bullets);
				
				while(l_ite != l_end)
				{
					auto l_modified_hit = _scene_ray_query
					(
						l_px_scene,
						physics::bc_ray(l_ite->m_position, l_ite->m_direction, l_ite->m_speed * p_clock.m_elapsed_second), 
						l_query_buffer,
						l_skinned_mesh_hits_buffer
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
						l_query_hit.get_actor(),
						l_query_hit.get_shape(),
						l_modified_hit.has_value() ? l_modified_hit->get_position() : l_query_hit.get_position(),
						l_modified_hit.has_value() ? l_modified_hit->get_normal() : l_query_hit.get_normal(),
						l_modified_hit.has_value() ? l_modified_hit->get_face_index() : l_query_hit.get_face_index(),
						l_ite->get_player_id(),
						l_ite->get_direction(),
						l_ite->get_speed(),
						l_ite->get_mass()
					));
					
					l_ite = m_bullets.erase(l_ite);
				}
			}
		}

		core::bc_task<void> bc_bullet_manager::update_async(bc_scene& p_scene, const core_platform::bc_clock::update_param& p_clock) noexcept
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

		core::bc_nullable<physics::bc_ray_hit> bc_bullet_manager::_scene_ray_query(const physics::bc_scene& p_px_scene, 
			const physics::bc_ray& p_ray, 
			physics::bc_scene_ray_query_buffer& p_buffer,
			core::bc_vector_frame<physics::bc_ray_hit>& p_modified_hits_buffer)
		{
			p_modified_hits_buffer.clear();
			
			auto l_query_filter = physics::bc_scene_query_post_filter_callback
			(
				[&p_ray, &p_modified_hits_buffer, this](const physics::bc_scene_query_post_filter_data& p_filter_data)
				{
					if (p_filter_data.m_shape.get_query_group() != static_cast<physics::bc_query_group>(bc_actor_group::skinned_mesh))
					{
						const auto l_shape_query_flag = p_filter_data.m_shape.get_query_flags();
						return core::bc_enum::has(l_shape_query_flag, physics::bc_shape_query_flag::touching) ?
							physics::bc_query_hit_type::touch :
							physics::bc_query_hit_type::block;
					}
					
					const auto l_hit = bc_mesh_utility::skinned_mesh_ray_hit_test(*m_physics_system, p_ray, p_filter_data);
					if(l_hit.first == physics::bc_query_hit_type::block)
					{
						p_modified_hits_buffer.push_back(l_hit.second);
					}

					return l_hit.first;
				}
			);
			
			const auto l_hit = p_px_scene.raycast
			(
				p_ray,
				p_buffer,
				physics::bc_hit_flag::hit_info,
				core::bc_enum::mask_or({physics::bc_query_flags::statics, physics::bc_query_flags::dynamics}),
				physics::bc_query_group::all,
				true,
				&l_query_filter
			);

			if(!l_hit || p_modified_hits_buffer.empty())
			{
				return nullptr;
			}
			
			const auto l_block = p_buffer.get_block();
			const auto l_min_distance_ite = std::min_element
			(
				std::cbegin(p_modified_hits_buffer),
				std::cend(p_modified_hits_buffer),
				[&](const physics::bc_ray_hit& p_hit1, const physics::bc_ray_hit& p_hit2)
				{
					return p_hit1.get_distance() - l_block.get_distance() < p_hit2.get_distance() - l_block.get_distance();
				}
			);
			
			return core::bc_nullable<physics::bc_ray_hit>(*l_min_distance_ite);
		}
	}	
}