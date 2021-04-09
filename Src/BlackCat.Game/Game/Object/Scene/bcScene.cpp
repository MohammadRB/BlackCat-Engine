// [01/13/2017 MRB]

#include "Game/GamePCH.h"

#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "PhysicsImp/Body/bcRigidBody.h"
#include "Core/Concurrency/bcConcurrency.h"
#include "Core/Content/bcContentStreamManager.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/Object/Scene/Component/bcRigidStaticComponent.h"
#include "Game/Object/Scene/Component/bcRigidDynamicComponent.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcAddedToSceneActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcRemovedFromSceneActorEvent.h"

namespace black_cat
{
	namespace game
	{
		bc_scene::bc_scene(core::bc_estring p_path,
			core::bc_string p_name,
			core::bc_vector<core::bc_string> p_stream_files,
			core::bc_vector<core::bc_string> p_entity_files,
			core::bc_vector<core::bc_string> p_material_files,
			core::bc_vector<core::bc_string> p_loaded_streams,
			bc_scene_graph p_scene_graph,
			bc_physics_system& p_physics,
			physics::bc_scene_ref p_px_scene)
			: m_path(std::move(p_path)),
			m_name(std::move(p_name)),
			m_stream_files(std::move(p_stream_files)),
			m_entity_files(std::move(p_entity_files)),
			m_material_files(std::move(p_material_files)),
			m_loaded_streams(std::move(p_loaded_streams)),
			m_scene_graph(std::move(p_scene_graph)),
			m_physics(&p_physics),
			m_px_scene(std::move(p_px_scene)),
			m_bullet_manager(p_physics)
		{
		}

		bc_scene::bc_scene(bc_scene&& p_other) noexcept
			: m_path(std::move(p_other.m_path)),
			m_name(std::move(p_other.m_name)),
			m_stream_files(std::move(p_other.m_stream_files)),
			m_entity_files(std::move(p_other.m_entity_files)),
			m_material_files(std::move(p_other.m_material_files)),
			m_loaded_streams(std::move(p_other.m_loaded_streams)),
			m_scene_graph(std::move(p_other.m_scene_graph)),
			m_physics(p_other.m_physics),
			m_px_scene(std::move(p_other.m_px_scene)),
			m_bullet_manager(std::move(p_other.m_bullet_manager))
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
			m_loaded_streams = std::move(p_other.m_loaded_streams);
			m_scene_graph = std::move(p_other.m_scene_graph);
			m_physics = p_other.m_physics;
			m_px_scene = std::move(p_other.m_px_scene);
			m_bullet_manager = std::move(p_other.m_bullet_manager);

			return *this;
		}

		void bc_scene::add_actor(bc_actor& p_actor)
		{
			{
				core_platform::bc_lock_guard<core_platform::bc_hybrid_mutex> l_lock_guard
				(
					m_changed_actors_lock, core_platform::bc_lock_operation::light
				);
				m_changed_actors.push_back(std::make_tuple(_bc_scene_actor_operation::add, p_actor));
			}
		}

		void bc_scene::update_actor(bc_actor& p_actor)
		{
			{
				core_platform::bc_lock_guard<core_platform::bc_hybrid_mutex> l_lock_guard
				(
					m_changed_actors_lock, core_platform::bc_lock_operation::light
				);

				const auto l_to_remove_list_ite = std::find_if
				(
					std::begin(m_to_remove_actors),
					std::end(m_to_remove_actors),
					[&p_actor](decltype(m_to_remove_actors)::reference p_entry)
					{
						return std::get< bc_actor >(p_entry) == p_actor;
					}
				);
				
				if (l_to_remove_list_ite == std::end(m_to_remove_actors))
				{
					m_changed_actors.push_back(std::make_tuple(_bc_scene_actor_operation::update, p_actor));
				}
			}
		}

		void bc_scene::remove_actor(bc_actor& p_actor)
		{
			{
				core_platform::bc_lock_guard<core_platform::bc_hybrid_mutex> l_lock_guard
				(
					m_changed_actors_lock, core_platform::bc_lock_operation::light
				);
				m_changed_actors.push_back(std::make_tuple(_bc_scene_actor_operation::remove, p_actor));
			}
		}

		void bc_scene::add_bullet(const bc_bullet& p_bullet)
		{
			m_bullet_manager.add_bullet(p_bullet);
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
				[]() { return true; },
				[&](bool, physics::bc_updated_actor& p_px_actor)
				{
					bc_actor l_actor = m_physics->get_game_actor(p_px_actor.m_actor);
					if(l_actor.is_valid())
					{
						l_actor.add_event(bc_world_transform_actor_event(p_px_actor.m_global_pose.get_matrix4(), true));
					}
				},
				[](bool) {}
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
			m_bullet_manager.update(*this, p_clock);
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

		void bc_scene::update_graph()
		{
			{
				core_platform::bc_hybrid_mutex_guard l_lock(m_changed_actors_lock, core_platform::bc_lock_operation::heavy);
				
				for(auto& l_to_removed_actor : m_to_remove_actors)
				{
					auto l_changed_list_ite = std::find_if
					(
						std::begin(m_changed_actors),
						std::end(m_changed_actors),
						[l_to_removed_actor](decltype(m_changed_actors)::reference p_entry)
						{
							return std::get< bc_actor >(p_entry) == std::get< bc_actor >(l_to_removed_actor);
						}
					);
					if(l_changed_list_ite != std::end(m_changed_actors))
					{
						m_changed_actors.erase(l_changed_list_ite);
					}
					
					_remove_actor(std::get< _bc_scene_actor_remove_state >(l_to_removed_actor), std::get< bc_actor >(l_to_removed_actor));
				}
				
				m_to_remove_actors.clear();
				
				const auto l_num_thread = std::min(core::bc_concurrency::worker_count(), m_changed_actors.size() / 10U + 1);
				
				core::bc_concurrency::concurrent_for_each
				(
					l_num_thread,
					std::begin(m_changed_actors),
					std::end(m_changed_actors),
					[]() { return true; },
					[&](bool, decltype(m_changed_actors)::reference p_actor)
					{
						switch (std::get<_bc_scene_actor_operation>(p_actor))
						{
						case _bc_scene_actor_operation::add:
							_add_actor(std::get<bc_actor>(p_actor));
							break;
						case _bc_scene_actor_operation::update:
							_update_actor(std::get<bc_actor>(p_actor));
							break;
						case _bc_scene_actor_operation::remove:
							_remove_actor_event(_bc_scene_actor_remove_state::to_remove_from_graph, std::get<bc_actor>(p_actor));
							break;
						default:
							BC_ASSERT(false);
						}
					},
					[](bool) {}
				);
				
				m_changed_actors.clear();
				m_changed_actors.shrink_to_fit();
			}
		}

		core::bc_task<void> bc_scene::update_graph_async()
		{
			auto l_task = core::bc_concurrency::start_task
			(
				core::bc_delegate< void() >
				(
					[=]()
					{
						update_graph();
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
				p_actor.destroy();
				return;
			}

			p_actor.add_event(bc_added_to_scene_actor_event(*this));
		}

		void bc_scene::_update_actor(bc_actor& p_actor)
		{
			const bool l_updated = m_scene_graph.update_actor(p_actor);
			if (!l_updated)
			{
				_remove_actor_event(_bc_scene_actor_remove_state::removed_from_graph, p_actor);
			}
		}

		void bc_scene::_remove_actor_event(_bc_scene_actor_remove_state p_state, bc_actor& p_actor)
		{
			p_actor.add_event(bc_removed_from_scene_actor_event(*this));
			m_to_remove_actors.push_back(std::make_tuple(p_state, p_actor));
		}

		void bc_scene::_remove_actor(_bc_scene_actor_remove_state p_state, bc_actor& p_actor)
		{
			if(p_state != _bc_scene_actor_remove_state::removed_from_graph)
			{
				const bool l_removed = m_scene_graph.remove_actor(p_actor);
				BC_ASSERT(l_removed);
			}

			p_actor.destroy();
		}
	}
}