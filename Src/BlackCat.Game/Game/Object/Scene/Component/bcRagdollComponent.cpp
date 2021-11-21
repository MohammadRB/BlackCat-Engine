// [11/13/2021 MRB]

#include "Game/GamePCH.h"

#include "Core/Container/bcStack.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/Component/bcRagdollComponent.h"
#include "Game/Object/Scene/Component/bcSkinnedMeshComponent.h"
#include "Game/Object/Scene/Component/bcRigidControllerComponent.h"
#include "Game/Object/Scene/Component/Event/bcAddedToSceneActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcRemovedFromSceneActorEvent.h"

namespace black_cat
{
	namespace game
	{
		bc_ragdoll_component::bc_ragdoll_component(bc_actor_id p_actor_index, bc_actor_component_id p_index) noexcept
			: bci_actor_component(p_actor_index, p_index)
		{
		}

		bc_ragdoll_component::bc_ragdoll_component(bc_ragdoll_component&&) noexcept = default;

		bc_ragdoll_component::~bc_ragdoll_component() = default;

		bc_ragdoll_component& bc_ragdoll_component::operator=(bc_ragdoll_component&&) noexcept = default;

		bc_actor bc_ragdoll_component::get_actor() const noexcept
		{
			return get_manager().component_get_actor(*this);
		}

		void bc_ragdoll_component::initialize(const bc_actor_component_initialize_context& p_context)
		{
		}

		void bc_ragdoll_component::initialize_entity(const bc_actor_component_initialize_entity_context& p_context)
		{
		}

		void bc_ragdoll_component::handle_event(const bc_actor_component_event_context& p_context)
		{
			const auto* l_scene_add_event = core::bci_message::as<bc_added_to_scene_actor_event>(p_context.m_event);
			if(l_scene_add_event)
			{
				m_scene = &l_scene_add_event->get_scene().get_px_scene();
				return;
			}

			const auto* l_scene_remove_event = core::bci_message::as<bc_removed_from_scene_actor_event>(p_context.m_event);
			if(l_scene_remove_event)
			{
				m_scene = nullptr;
				return;
			}
		}

		void bc_ragdoll_component::set_enable(bool p_enable)
		{
			{
				physics::bc_scene_lock l_lock(m_scene);

				if (p_enable)
				{
					_fill_colliders_map(get_actor());
					_create_physics_joints();
				}
				else
				{
					m_joints.clear();

					for (auto& l_actor : m_actors)
					{
						m_scene->remove_actor(*l_actor);
					}

					m_actors.clear();
				}
			}
		}

		void bc_ragdoll_component::_fill_colliders_map(const bc_actor& p_actor)
		{
			const auto* l_mesh_component = p_actor.get_component<bc_skinned_mesh_component>();
			const auto& l_mesh = l_mesh_component->get_mesh();
			const auto& l_mesh_collider = l_mesh.get_mesh_collider();

			core::bc_stack_frame<_bc_ragdoll_collider_entry> l_colliders_stack;
			m_colliders_map.reserve(l_mesh_collider.size());

			bool l_dummy;
			l_mesh.iterate_over_nodes(l_dummy, [&](const bc_mesh_node& p_node, bool)
			{
				if(l_colliders_stack.empty())
				{
					l_colliders_stack.push(_bc_ragdoll_collider_entry());
					l_colliders_stack.top().m_attached_node_index = bc_mesh_node::s_invalid_index;
				}
				
				auto* l_last_collider = &l_colliders_stack.top();
				const auto l_node_colliders = l_mesh_collider.find_mesh_collider(p_node.get_name());

				if (!l_node_colliders.empty())
				{
					if (l_last_collider->m_attached_node_index != bc_mesh_node::s_invalid_index)
					{
						l_colliders_stack.push(_bc_ragdoll_collider_entry());
						l_last_collider = &l_colliders_stack.top();
					}

					l_last_collider->m_attached_node_index = p_node.get_index();
					l_last_collider->m_attached_node_name = p_node.get_name();
				}

				l_last_collider->m_affected_node_indices.push_back(p_node.get_index());

				return false;
			}, 
			[&](const bc_mesh_node& p_node, const bool)
			{
				if(l_colliders_stack.empty())
				{
					return;
				}
				
				if(p_node.get_index() == l_colliders_stack.top().m_attached_node_index)
				{
					m_colliders_map.push_back(std::move(l_colliders_stack.top()));
					l_colliders_stack.pop();
				}
			});
		}

		void bc_ragdoll_component::_create_physics_joints()
		{
			auto* l_mesh_component = get_actor().get_component<bc_skinned_mesh_component>();
			//auto* l_rigid_controller_component = get_actor().get_component<bc_rigid_controller_component>();
			const auto& l_mesh = l_mesh_component->get_mesh();
			const auto& l_mesh_colliders = l_mesh.get_mesh_collider();
			
			m_joints.reserve(l_mesh_colliders.size());
		}
	}	
}