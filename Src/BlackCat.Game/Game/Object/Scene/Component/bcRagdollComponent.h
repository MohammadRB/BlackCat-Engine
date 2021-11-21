// [11/13/2021 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "PhysicsImp/Body/bcRigidDynamic.h"
#include "PhysicsImp/Joint/bcJoint.h"
#include "Game/Object/Mesh/bcMeshNode.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		struct _bc_ragdoll_collider_entry
		{
			std::string_view m_attached_node_name;
			bc_mesh_node::node_index_t m_attached_node_index;
			core::bc_vector<bc_mesh_node::node_index_t> m_affected_node_indices;
		};
		
		class BC_GAME_DLL bc_ragdoll_component : public bci_actor_component
		{
			BC_COMPONENT(ragdoll, true, false)
			
		public:
			bc_ragdoll_component(bc_actor_id p_actor_index, bc_actor_component_id p_index) noexcept;

			bc_ragdoll_component(bc_ragdoll_component&&) noexcept;

			~bc_ragdoll_component() override;

			bc_ragdoll_component& operator=(bc_ragdoll_component&&) noexcept;

			bc_actor get_actor() const noexcept override;
			
			void initialize(const bc_actor_component_initialize_context& p_context) override;
			
			void initialize_entity(const bc_actor_component_initialize_entity_context& p_context) override;
			
			void handle_event(const bc_actor_component_event_context& p_context) override;

			void set_enable(bool p_enable);
			
		private:
			void _fill_colliders_map(const bc_actor& p_actor);
			
			void _create_physics_joints();

			physics::bc_scene* m_scene;
			core::bc_vector<physics::bc_joint_ref> m_joints;
			core::bc_vector<physics::bc_rigid_dynamic_ref> m_actors;
			core::bc_vector<_bc_ragdoll_collider_entry> m_colliders_map;
		};
	}	
}