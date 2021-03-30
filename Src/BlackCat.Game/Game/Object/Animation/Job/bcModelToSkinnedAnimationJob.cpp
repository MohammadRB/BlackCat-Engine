// [01/15/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Mesh/bcSubMesh.h"
#include "Game/Object/Animation/Job/bcModelToSkinnedAnimationJob.h"

namespace black_cat
{
	namespace game
	{
		bc_model_to_skinned_animation_job::bc_model_to_skinned_animation_job(core::bc_shared_ptr<bc_local_to_model_animation_job> p_model_job, bc_sub_mesh_mat4_transform& p_transforms)
			: bci_animation_job(p_model_job->get_skeleton()),
			m_model_job(std::move(p_model_job)),
			m_transforms(&p_transforms),
			m_world(core::bc_matrix4f::identity())
		{
			if (m_transforms->size() < m_skeleton->get_num_joints())
			{
				throw bc_invalid_argument_exception("Mesh and animation bones count does not match");
			}
		}

		bool bc_model_to_skinned_animation_job::run(const core_platform::bc_clock::update_param& p_clock)
		{
			const auto& l_mesh = m_model_job->get_mesh();
			const auto& l_animation_model_transforms = m_model_job->get_transforms();
			const auto l_bone_names = get_skeleton().get_joint_names();
			auto l_bone_name_ite = l_bone_names.begin();

			core::bc_vector3f l_min(std::numeric_limits<bcFLOAT>::max());
			core::bc_vector3f l_max(std::numeric_limits<bcFLOAT>::lowest());

			for (auto l_bone_ite = 0U; l_bone_ite < l_bone_names.size(); ++l_bone_ite, ++l_bone_name_ite)
			{
				const auto* l_bone_name = *l_bone_name_ite;
				const auto* l_mesh_node = l_mesh.find_node(l_bone_name);
				const auto& l_mesh_node_inverse_transform = l_mesh.get_node_inverse_bind_pose_transform(*l_mesh_node);
				const auto& l_animation_model_transform = l_animation_model_transforms.get_node_transform(*l_mesh_node);
				auto& l_output_transform = m_transforms->get_node_transform(*l_mesh_node);

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
			
			return true;
		}
	}
}