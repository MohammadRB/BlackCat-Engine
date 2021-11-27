// [11/25/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Scene/ActorComponent/bcActor.hpp"
#include "Game/Object/Scene/Component/Event/bcHierarchyTransformActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcBoundBoxChangedActorEvent.h"
#include "Game/Object/Mesh/bcSubMesh.h"
#include "Game/Object/Animation/Job/bcRagdollSkinningAnimationJob.h"

namespace black_cat
{
	namespace game
	{
		bc_ragdoll_animation_job::bc_ragdoll_animation_job(bc_animation_skeleton& p_skeleton,
			bc_actor p_actor,
			bc_skinned_mesh_component& p_component, 
			bc_sub_mesh_mat4_transform& p_skinning_transforms_ws) noexcept
			: bci_skinning_transform_animation_job(p_skeleton),
			m_actor(std::move(p_actor)),
			m_mesh_component(&p_component),
			m_skinning_transforms_ws(&p_skinning_transforms_ws)
		{
		}

		bool bc_ragdoll_animation_job::run(const core_platform::bc_clock::update_param& p_clock)
		{
			const auto& l_mesh = m_mesh_component->get_mesh();
			const auto& l_model_transforms = m_mesh_component->get_model_transforms();
			const auto l_bone_names = get_skeleton().get_joint_names();
			auto l_bone_name_ite = l_bone_names.begin();

			core::bc_vector3f l_min(std::numeric_limits<bcFLOAT>::max());
			core::bc_vector3f l_max(std::numeric_limits<bcFLOAT>::lowest());

			for (auto l_bone_ite = 0U; l_bone_ite < l_bone_names.size(); ++l_bone_ite, ++l_bone_name_ite)
			{
				const auto* l_bone_name = *l_bone_name_ite;
				const auto* l_mesh_node = l_mesh.find_node(l_bone_name);
				const auto& l_mesh_node_inverse_transform = l_mesh.get_node_inverse_bind_pose_transform(*l_mesh_node);
				const auto& l_animation_model_transform = l_model_transforms.get_node_transform(*l_mesh_node);
				auto& l_output_transform = m_skinning_transforms_ws->get_node_transform(*l_mesh_node);

				auto l_animation_model_world_transform = l_animation_model_transform * m_world;
				l_output_transform = l_mesh_node_inverse_transform * l_animation_model_world_transform;

				auto l_translation = l_animation_model_world_transform.get_translation();
				l_min.x = std::min(l_min.x, l_translation.x);
				l_min.y = std::min(l_min.y, l_translation.y);
				l_min.z = std::min(l_min.z, l_translation.z);
				l_max.x = std::max(l_max.x, l_translation.x);
				l_max.y = std::max(l_max.y, l_translation.y);
				l_max.z = std::max(l_max.z, l_translation.z);
			}

			m_bound_box = physics::bc_bound_box::from_min_max(l_min, l_max);

			m_actor.add_event(bc_bound_box_changed_actor_event(m_bound_box));
			m_actor.add_event(bc_hierarchy_transform_actor_event
			(
				m_world,
				&m_mesh_component->get_model_transforms(),
				&m_mesh_component->get_collider_model_transforms()
			));
			
			return true;
		}
	}
}