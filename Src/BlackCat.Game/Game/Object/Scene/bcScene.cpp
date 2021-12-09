// [01/13/2017 MRB]

#include "Game/GamePCH.h"

#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "PhysicsImp/Body/bcRigidBody.h"
#include "Core/Concurrency/bcConcurrency.h"
#include "Core/Content/bcContentStreamManager.h"
#include "Game/System/Input/bcGlobalConfig.h"
#include "Game/Object/Scene/bcEntityManager.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/Object/Scene/Component/bcRigidStaticComponent.h"
#include "Game/Object/Scene/Component/bcRigidDynamicComponent.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcAddedToSceneActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcRemovedFromSceneActorEvent.h"
#include "Game/Object/Scene/Bullet/bcBulletManager.h"
#include "Game/System/bcGameSystem.h"
#include "Game/System/Render/Light/bcLightManager.h"
#include "Game/System/Render/Particle/bcParticleManager.h"
#include "Game/System/Render/Decal/bcDecalManager.h"

namespace black_cat
{
	namespace game
	{
		bc_scene::bc_scene(bc_entity_manager& p_entity_manager,
			bc_game_system& p_game_system,
			core::bc_estring p_path,
			core::bc_string p_name,
			core::bc_vector<core::bc_string> p_stream_files,
			core::bc_vector<core::bc_string> p_entity_files,
			core::bc_vector<core::bc_string> p_material_files,
			core::bc_vector<core::bc_string> p_decal_files,
			core::bc_vector<core::bc_string> p_loaded_streams,
			bc_scene_graph p_scene_graph,
			physics::bc_scene_ref p_px_scene)
			: m_path(std::move(p_path)),
			m_name(std::move(p_name)),
			m_stream_files(std::move(p_stream_files)),
			m_entity_files(std::move(p_entity_files)),
			m_material_files(std::move(p_material_files)),
			m_decal_files(std::move(p_decal_files)),
			m_loaded_streams(std::move(p_loaded_streams)),
			m_entity_manager(&p_entity_manager),
			m_physics(&p_game_system.get_physics_system()),
			m_scene_graph(std::move(p_scene_graph)),
			m_px_scene(std::move(p_px_scene))
		{
			m_global_scale = bc_get_global_config().get_global_scale();
			m_bullet_manager = core::bc_make_unique<bc_bullet_manager>(bc_bullet_manager(*m_physics));
			m_light_manager = core::bc_make_unique<bc_light_manager>(bc_light_manager());
			m_particle_manager = core::bc_make_unique<bc_particle_manager>(bc_particle_manager());
			m_decal_manager = core::bc_make_unique<bc_decal_manager>(bc_decal_manager(p_game_system.get_render_system().get_material_manager()));
		}

		bc_scene::bc_scene(bc_scene&& p_other) noexcept
			: m_path(std::move(p_other.m_path)),
			m_name(std::move(p_other.m_name)),
			m_stream_files(std::move(p_other.m_stream_files)),
			m_entity_files(std::move(p_other.m_entity_files)),
			m_material_files(std::move(p_other.m_material_files)),
			m_decal_files(std::move(p_other.m_decal_files)),
			m_loaded_streams(std::move(p_other.m_loaded_streams)),
			m_global_scale(p_other.m_global_scale),
			m_entity_manager(p_other.m_entity_manager),
			m_physics(p_other.m_physics),
			m_scene_graph(std::move(p_other.m_scene_graph)),
			m_px_scene(std::move(p_other.m_px_scene)),
			m_bullet_manager(std::move(p_other.m_bullet_manager)),
			m_light_manager(std::move(p_other.m_light_manager)),
			m_particle_manager(std::move(p_other.m_particle_manager)),
			m_decal_manager(std::move(p_other.m_decal_manager))
		{
		}

		bc_scene::~bc_scene()
		{
			m_scene_graph.clear();
			m_px_scene.reset();

			auto* l_content_stream_manager = core::bc_get_service<core::bc_content_stream_manager>();
			for (auto& l_stream : m_loaded_streams)
			{
				l_content_stream_manager->unload_content_stream(l_stream.c_str());
			}
		}

		bc_scene& bc_scene::operator=(bc_scene&& p_other) noexcept
		{
			m_path = std::move(p_other.m_path);
			m_name = std::move(p_other.m_name);
			m_stream_files = std::move(p_other.m_stream_files);
			m_entity_files = std::move(p_other.m_entity_files);
			m_material_files = std::move(p_other.m_material_files);
			m_decal_files = std::move(p_other.m_decal_files);
			m_loaded_streams = std::move(p_other.m_loaded_streams);
			m_scene_graph = std::move(p_other.m_scene_graph);
			m_global_scale = p_other.m_global_scale;
			m_entity_manager = p_other.m_entity_manager;
			m_physics = p_other.m_physics;
			m_px_scene = std::move(p_other.m_px_scene);
			m_bullet_manager = std::move(p_other.m_bullet_manager);
			m_light_manager = std::move(p_other.m_light_manager);
			m_particle_manager = std::move(p_other.m_particle_manager);
			m_decal_manager = std::move(p_other.m_decal_manager);

			return *this;
		}

		bc_actor bc_scene::create_actor(const bcCHAR* p_entity_name, const core::bc_matrix4f& p_world_transform)
		{
			auto l_actor = m_entity_manager->create_entity(*this, p_entity_name);
			if(!l_actor.is_valid())
			{
				return l_actor;
			}
			
			{
				core_platform::bc_hybrid_mutex_guard l_lock_guard
				(
					m_changed_actors_lock, core_platform::bc_lock_operation::light
				);
				m_added_actors.push_back(l_actor);
			}

			l_actor.add_event(bc_world_transform_actor_event(p_world_transform));
			return l_actor;
		}

		void bc_scene::update_actor(bc_actor& p_actor)
		{
			{
				core_platform::bc_hybrid_mutex_guard l_lock_guard
				(
					m_changed_actors_lock, core_platform::bc_lock_operation::light
				);

				/*const auto l_to_remove_ite = std::find_if
				(
					std::begin(m_to_remove_actors),
					std::end(m_to_remove_actors),
					[&p_actor](decltype(m_to_remove_actors)::reference p_entry)
					{
						return std::get<bc_actor>(p_entry) == p_actor;
					}
				);
				
				if (l_to_remove_ite == std::end(m_to_remove_actors))
				{
					m_changed_actors.push_back(std::make_tuple(_bc_scene_actor_operation::update, p_actor));
				}*/

				m_changed_actors.push_back(std::make_tuple(_bc_scene_actor_operation::update, p_actor));
			}
		}

		void bc_scene::remove_actor(bc_actor& p_actor)
		{
			{
				core_platform::bc_hybrid_mutex_guard l_lock_guard
				(
					m_changed_actors_lock, core_platform::bc_lock_operation::light
				);
				m_changed_actors.push_back(std::make_tuple(_bc_scene_actor_operation::remove_event, p_actor));
			}
		}

		void bc_scene::add_bullet(const bc_bullet& p_bullet)
		{
			m_bullet_manager->add_bullet(p_bullet);
		}

		void bc_scene::draw_debug_shapes(bc_shape_drawer& p_shape_drawer) const
		{
			m_scene_graph.draw_debug_shapes(p_shape_drawer);
		}

		void bc_scene::update_physics(const core_platform::bc_clock::update_param& p_clock)
		{
			core::bc_vector_frame<physics::bc_updated_actor> l_px_actors;
			
			{
				physics::bc_scene_lock l_lock(&m_px_scene.get());

				m_px_scene->update(p_clock);
				m_px_scene->wait();
				l_px_actors = m_px_scene->get_active_actors();
			}

			const auto l_num_thread = std::min(core::bc_concurrency::worker_count(), l_px_actors.size() / 25U + 1);

			core::bc_concurrency::concurrent_for_each
			(
				l_num_thread,
				std::begin(l_px_actors),
				std::end(l_px_actors),
				[&](physics::bc_updated_actor& p_px_actor)
				{
					auto l_actor = m_physics->get_game_actor(p_px_actor.m_actor);
					if(l_actor.is_valid())
					{
						l_actor.add_event(bc_world_transform_actor_event(p_px_actor.m_global_pose.get_matrix4(), bc_transform_event_type::physics));
					}
				}
			);
		}

		core::bc_task<void> bc_scene::update_physics_async(const core_platform::bc_clock::update_param& p_clock)
		{
			auto l_task = core::bc_concurrency::start_task
			(
				core::bc_delegate< void() >
				(
					[=, &p_clock]()
					{
						update_physics(p_clock);
					}
				)
			);

			return l_task;
		}

		void bc_scene::update_bullets(const core_platform::bc_clock::update_param& p_clock)
		{
			m_bullet_manager->update(*this, p_clock);
		}

		core::bc_task<void> bc_scene::update_bullets_async(const core_platform::bc_clock::update_param& p_clock)
		{
			auto l_task = core::bc_concurrency::start_task
			(
				core::bc_delegate< void() >
				(
					[=, &p_clock]()
					{
						update_bullets(p_clock);
					}
				)
			);

			return l_task;
		}

		void bc_scene::update_graph(const core_platform::bc_clock::update_param& p_clock)
		{
			{
				core_platform::bc_hybrid_mutex_guard l_lock(m_changed_actors_lock, core_platform::bc_lock_operation::heavy);
				
				for(auto& l_added_actor : m_added_actors)
				{
					auto l_changed_ite = std::find_if
					(
						std::begin(m_changed_actors),
						std::end(m_changed_actors),
						[&](decltype(m_changed_actors)::reference p_entry)
						{
							return std::get<bc_actor>(p_entry) == l_added_actor;
						}
					);
					
					if (l_changed_ite == std::end(m_changed_actors))
					{
						m_changed_actors.push_back(std::make_tuple(_bc_scene_actor_operation::add, l_added_actor));
					}
					else
					{
						*l_changed_ite = std::make_tuple(_bc_scene_actor_operation::add, l_added_actor);
					}
				}

				m_added_actors.clear();
				m_added_actors.shrink_to_fit();

				for (auto& l_to_removed_actor : m_to_remove_actors)
				{
					m_changed_actors.push_back(l_to_removed_actor);
				}

				m_to_remove_actors.clear();
				
				const auto l_num_thread = std::min(core::bc_concurrency::worker_count(), m_changed_actors.size() / 10U + 1);
				core::bc_concurrency::concurrent_for_each
				(
					l_num_thread,
					std::begin(m_changed_actors),
					std::end(m_changed_actors),
					[&](decltype(m_changed_actors)::reference p_actor)
					{
						switch (std::get<_bc_scene_actor_operation>(p_actor))
						{
						case _bc_scene_actor_operation::add:
							_add_actor(std::get<bc_actor>(p_actor));
							break;
						case _bc_scene_actor_operation::update:
							_update_actor(std::get<bc_actor>(p_actor));
							break;
						case _bc_scene_actor_operation::remove_event:
							_send_actor_remove_event(_bc_scene_actor_operation::remove_from_graph, std::get<bc_actor>(p_actor));
							break;
						case _bc_scene_actor_operation::remove_from_graph:
							_remove_actor(_bc_scene_actor_operation::remove_from_graph, std::get<bc_actor>(p_actor));
							break;
						case _bc_scene_actor_operation::removed_from_graph:
							_remove_actor(_bc_scene_actor_operation::removed_from_graph, std::get<bc_actor>(p_actor));
							break;
						default:
							BC_ASSERT(false);
						}
					}
				);

				m_changed_actors.clear();
			}
			
			m_scene_graph.update(p_clock);
		}

		core::bc_task<void> bc_scene::update_graph_async(const core_platform::bc_clock::update_param& p_clock)
		{
			auto l_task = core::bc_concurrency::start_task
			(
				core::bc_delegate<void()>
				(
					[&]()
					{
						update_graph(p_clock);
					}
				)
			);

			return l_task;
		}

		void bc_scene::_add_actor(bc_actor& p_actor)
		{
			const bool l_added = m_scene_graph.add_actor(p_actor);
			if (!l_added)
			{
				_send_actor_remove_event(_bc_scene_actor_operation::removed_from_graph, p_actor);
				return;
			}

			p_actor.add_event(bc_added_to_scene_actor_event(*this));
		}

		void bc_scene::_update_actor(bc_actor& p_actor)
		{
			const bool l_updated = m_scene_graph.update_actor(p_actor);
			if (!l_updated)
			{
				_send_actor_remove_event(_bc_scene_actor_operation::removed_from_graph, p_actor);
			}
		}

		void bc_scene::_send_actor_remove_event(_bc_scene_actor_operation p_state, bc_actor& p_actor)
		{
			p_actor.add_event(bc_removed_from_scene_actor_event(*this));
			
			{
				core_platform::bc_spin_mutex_guard l_lock(m_to_remove_actors_lock);
				m_to_remove_actors.push_back(std::make_tuple(p_state, p_actor));
			}
		}

		void bc_scene::_remove_actor(_bc_scene_actor_operation p_state, bc_actor& p_actor)
		{
			if(p_state != _bc_scene_actor_operation::removed_from_graph)
			{
				const bool l_removed = m_scene_graph.remove_actor(p_actor);
				BC_ASSERT(l_removed);
			}

			p_actor.destroy();
		}
	}
}