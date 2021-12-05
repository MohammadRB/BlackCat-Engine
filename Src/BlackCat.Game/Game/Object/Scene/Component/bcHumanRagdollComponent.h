// [11/13/2021 MRB]

#pragma once

#include "Core/Container/bcArray.h"
#include "Core/Container/bcVector.h"
#include "PhysicsImp/Body/bcRigidDynamic.h"
#include "PhysicsImp/Joint/bcJoint.h"
#include "Game/Object/Mesh/bcMeshNode.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/Object/Animation/Job/bcRagdollSkinningAnimationJob.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace physics
	{
		template<bc_physics_api TApi>
		class bc_platform_scene;
		using bc_scene = bc_platform_scene<g_current_physics_api>;
	}
	
	namespace game
	{
		class bc_physics_system;
		class bc_skinned_mesh_component;
		class bc_rigid_controller_component;
		
		struct _bc_ragdoll_collider_entry
		{
			struct affected_node
			{
				bc_mesh_node::node_index_t m_node_index;
				
				/**
				 * \brief Collider affected nodes have an offset in relation to collider transform which must be applied in calculation of node transforms.
				 */
				physics::bc_transform m_collider_offset_transform;

				/**
				 * \brief Collider affected nodes have an offset in relation to collider attached node which must be applied in calculation of node transforms.
				 */
				physics::bc_transform m_parent_offset_transform;
			};
			
			std::string_view m_attached_node_name;
			bc_mesh_node::node_index_t m_attached_node_index;
			core::bc_vector<affected_node> m_affected_nodes;
		};
		
		class BC_GAME_DLL bc_human_ragdoll_component : public bci_actor_component
		{
			BC_COMPONENT(ragdoll, true, true)
			
		public:
			bc_human_ragdoll_component(bc_actor_id p_actor_index, bc_actor_component_id p_index) noexcept;

			bc_human_ragdoll_component(bc_human_ragdoll_component&&) noexcept;

			~bc_human_ragdoll_component() override;

			bc_human_ragdoll_component& operator=(bc_human_ragdoll_component&&) noexcept;

			bc_actor get_actor() const noexcept override;
			
			void initialize(const bc_actor_component_initialize_context& p_context) override;
			
			void initialize_entity(const bc_actor_component_initialize_entity_context& p_context) override;

			void update(const bc_actor_component_update_content& p_context) override;
			
			void handle_event(const bc_actor_component_event_context& p_context) override;

			void set_enable(bool p_enable);
			
		private:
			void _fill_colliders_map();

			void _fill_joints_map(const core::bc_json_key_value& p_joint_parameters);
			
			void _create_physics_joints();

			void _destroy_physics_joints();

			constexpr static bcUINT32 s_head_index = 0;
			constexpr static bcUINT32 s_body_index = 1;
			constexpr static bcUINT32 s_left_arm_index = 2;
			constexpr static bcUINT32 s_left_fore_arm_index = 3;
			constexpr static bcUINT32 s_left_hand_index = 4;
			constexpr static bcUINT32 s_right_arm_index = 5;
			constexpr static bcUINT32 s_right_fore_arm_index = 6;
			constexpr static bcUINT32 s_right_hand_index = 7;
			constexpr static bcUINT32 s_left_up_leg_index = 8;
			constexpr static bcUINT32 s_left_leg_index = 9;
			constexpr static bcUINT32 s_left_foot_index = 10;
			constexpr static bcUINT32 s_right_up_leg_index = 11;
			constexpr static bcUINT32 s_right_leg_index = 12;
			constexpr static bcUINT32 s_right_foot_index = 13;
			
			bc_physics_system* m_physics_system;
			physics::bc_scene* m_px_scene;
			bc_skinned_mesh_component* m_mesh_component;
			bc_rigid_controller_component* m_rigid_controller_component;

			core::bc_array<std::pair<std::string_view, physics::bc_rigid_dynamic_ref>, 14> m_joint_actors;
			core::bc_vector<_bc_ragdoll_collider_entry> m_colliders_map;
			core::bc_vector<physics::bc_joint_ref> m_joints;
			core::bc_unique_ptr<bc_ragdoll_animation_job> m_ragdoll_animation_job;
		};
	}	
}