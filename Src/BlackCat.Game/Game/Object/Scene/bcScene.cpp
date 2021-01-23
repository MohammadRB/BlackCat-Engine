// [01/13/2017 MRB]

#include "Game/GamePCH.h"

#include "PhysicsImp/Body/bcRigidBody.h"
#include "Core/Concurrency/bcConcurrency.h"
#include "Core/Content/bcContentStreamManager.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/Object/Scene/Component/bcRigidStaticComponent.h"
#include "Game/Object/Scene/Component/bcRigidDynamicComponent.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/Object/Scene/Component/Event/bcActorEventWorldTransform.h"
#include "Game/Object/Scene/Component/Event/bcActorEventAddedToScene.h"
#include "Game/Object/Scene/Component/Event/bcActorEventRemovedFromScene.h"

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
			physics::bc_scene_ref p_px_scene)
			: m_path(std::move(p_path)),
			m_name(std::move(p_name)),
			m_stream_files(std::move(p_stream_files)),
			m_entity_files(std::move(p_entity_files)),
			m_material_files(std::move(p_material_files)),
			m_loaded_streams(std::move(p_loaded_streams)),
			m_scene_graph(std::move(p_scene_graph)),
			m_px_scene(std::move(p_px_scene))
		{
		}

		bc_scene::bc_scene(bc_scene&& p_other) noexcept
			: bc_scene
			(
				std::move(p_other.m_path),
				std::move(p_other.m_name),
				std::move(p_other.m_stream_files),
				std::move(p_other.m_entity_files),
				std::move(p_other.m_material_files),
				std::move(p_other.m_loaded_streams),
				std::move(p_other.m_scene_graph),
				std::move(p_other.m_px_scene)
			)
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
			m_px_scene = std::move(p_other.m_px_scene);

			return *this;
		}

		void bc_scene::add_actor(bc_actor& p_actor)
		{
			{
				core_platform::bc_lock_guard<core_platform::bc_hybrid_mutex> l_lock_guard
				(
					m_new_actors_lock, core_platform::bc_lock_operation::light
				);
				m_new_actors.push_back(std::make_tuple(_bc_scene_actor_operation::add, p_actor));
			}
		}

		void bc_scene::update_actor(bc_actor& p_actor)
		{
			{
				core_platform::bc_lock_guard<core_platform::bc_hybrid_mutex> l_lock_guard
				(
					m_new_actors_lock, core_platform::bc_lock_operation::light
				);
				m_new_actors.push_back(std::make_tuple(_bc_scene_actor_operation::update, p_actor));
			}
		}

		void bc_scene::remove_actor(bc_actor& p_actor)
		{
			{
				core_platform::bc_lock_guard<core_platform::bc_hybrid_mutex> l_lock_guard
				(
					m_new_actors_lock, core_platform::bc_lock_operation::light
				);
				m_new_actors.push_back(std::make_tuple(_bc_scene_actor_operation::remove, p_actor));
			}
		}

		void bc_scene::draw_debug_shapes(bc_shape_drawer& p_shape_drawer) const
		{
			m_scene_graph.draw_debug_shapes(p_shape_drawer);
		}

		void bc_scene::update_px(bc_physics_system& p_physics, const core_platform::bc_clock::update_param& p_clock)
		{
			m_px_scene->update(p_clock);
			m_px_scene->wait();

			auto l_px_actors = m_px_scene->get_active_actors();
			const auto l_num_thread = std::min(core::bc_concurrency::worker_count(), l_px_actors.size() / 25U + 1);

			core::bc_concurrency::concurrent_for_each
			(
				l_num_thread,
				std::begin(l_px_actors),
				std::end(l_px_actors),
				[]() { return true; },
				[&](bool, physics::bc_updated_actor& p_px_actor)
				{
					bc_actor l_actor = p_physics.get_game_actor(p_px_actor.m_actor);
					l_actor.add_event(bc_actor_event_world_transform(p_px_actor.m_global_pose.get_matrix4(), true));
				},
				[](bool) {}
			);
		}

		void bc_scene::update_graph()
		{
			{
				core_platform::bc_lock_guard<core_platform::bc_hybrid_mutex> l_lock
				(
					m_new_actors_lock, core_platform::bc_lock_operation::heavy
				);

				const auto l_num_thread = std::min(core::bc_concurrency::worker_count(), m_new_actors.size() / 10U + 1);
				
				core::bc_concurrency::concurrent_for_each
				(
					l_num_thread,
					std::begin(m_new_actors),
					std::end(m_new_actors),
					[]() { return true; },
					[&](bool, decltype(m_new_actors)::reference p_actor)
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
							_remove_actor(std::get<bc_actor>(p_actor));
							break;
						default:
							BC_ASSERT(false);
						}
					},
					[](bool) {}
				);

				m_new_actors.clear();
				m_new_actors.shrink_to_fit();
			}
		}

		void bc_scene::_add_actor(bc_actor& p_actor)
		{
			const bool l_added = m_scene_graph.add_actor(p_actor);
			if (!l_added)
			{
				p_actor.destroy();
				return;
			}

			auto* l_rigid_component = p_actor.get_component<bc_rigid_body_component>();
			if (l_rigid_component)
			{
				physics::bc_rigid_body l_rigid_body = l_rigid_component->get_body();
				m_px_scene->add_actor(l_rigid_body);
			}

			p_actor.add_event(bc_actor_event_added_to_scene(*this));
		}

		void bc_scene::_update_actor(bc_actor& p_actor)
		{
			const bool l_updated = m_scene_graph.update_actor(p_actor);
			if (!l_updated)
			{
				auto* l_rigid_component = p_actor.get_component<bc_rigid_body_component>();
				if (l_rigid_component)
				{
					physics::bc_rigid_body l_rigid_body = l_rigid_component->get_body();
					m_px_scene->remove_actor(l_rigid_body);
				}

				p_actor.destroy();
			}
		}

		void bc_scene::_remove_actor(bc_actor& p_actor)
		{
			auto* l_rigid_component = p_actor.get_component<bc_rigid_body_component>();
			if (l_rigid_component)
			{
				physics::bc_rigid_body l_rigid_body = l_rigid_component->get_body();
				m_px_scene->remove_actor(l_rigid_body);
			}

			const bool l_removed = m_scene_graph.remove_actor(p_actor);
			p_actor.destroy();

			BC_ASSERT(l_removed);
		}
	}
}