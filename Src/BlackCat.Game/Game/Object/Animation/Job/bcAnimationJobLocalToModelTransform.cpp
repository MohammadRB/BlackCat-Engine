// [01/05/2021 MRB]

#include "Game/GamePCH.h"

#include "Core/Math/bcCoordinate.h"
#include "GraphicImp/bcRenderApiInfo.h"
#include "Game/Object/Animation/Job/bcAnimationJobLocalToModelTransform.h"
#include "Game/bcException.h"

#include "3rdParty/Ozz/Include/ozz/animation/runtime/local_to_model_job.h"

namespace black_cat
{
	namespace game
	{
		bc_animation_job_local_to_model_transform::bc_animation_job_local_to_model_transform(bci_animation_job_local_transform& p_local_transform_job,
			const bc_sub_mesh& p_sub_mesh,
			bc_sub_mesh_transform& p_model_transforms)
			: bci_animation_job(p_local_transform_job.get_skeleton()),
			m_local_transform_job(&p_local_transform_job),
			m_mesh(&p_sub_mesh),
			m_ozz_model_transforms(p_local_transform_job.get_skeleton().get_num_bones()),
			m_model_transforms(&p_model_transforms),
			m_world(core::bc_matrix4f::identity())
		{
			if (m_model_transforms->size() < m_skeleton->get_num_bones())
			{
				throw bc_invalid_argument_exception("Passed sub mesh transform bones count is not equal to animation skeletons join count");
			}
		}

		bool bc_animation_job_local_to_model_transform::run(const core_platform::bc_clock::update_param& p_clock)
		{
			ozz::animation::LocalToModelJob l_ltm_job;
			const auto& l_local_transforms = m_local_transform_job->get_local_transforms();

			l_ltm_job.skeleton = &get_skeleton().get_native_handle();
			l_ltm_job.input = { &*l_local_transforms.begin(), l_local_transforms.size() };
			l_ltm_job.output = ozz::make_span(m_ozz_model_transforms);

			if(!l_ltm_job.Run())
			{
				return false;
			}

			auto l_bone_names = get_skeleton().get_bone_names();
			auto l_bone_name_ite = l_bone_names.begin();
			for(auto l_bone_ite = 0U; l_bone_ite < l_bone_names.size(); ++l_bone_ite, ++l_bone_name_ite)
			{
				const auto* l_bone_name = *l_bone_name_ite;
				const auto* l_mesh_node = m_mesh->find_node(l_bone_name);
				const auto& l_ozz_model_transform = m_ozz_model_transforms[l_bone_ite];
				auto& l_model_transform = m_model_transforms->get_node_transform(*l_mesh_node);
				auto& l_model_transform_ptr = l_model_transform(0, 0);

				// Store columns into rows to transpose matrix
				ozz::math::StorePtr(l_ozz_model_transform.cols[0], &l_model_transform_ptr);
				ozz::math::StorePtr(l_ozz_model_transform.cols[1], &l_model_transform_ptr + 4);
				ozz::math::StorePtr(l_ozz_model_transform.cols[2], &l_model_transform_ptr + 8);
				ozz::math::StorePtr(l_ozz_model_transform.cols[3], &l_model_transform_ptr + 12);

				auto* l_root_node = m_mesh->get_root_node();
				//l_model_transform = l_model_transform * m_mesh->get_mesh()->get_inverse_bind_poses()[l_mesh_node->get_transform_index()] * m_world;
				//l_model_transform = m_mesh->get_node_transform(*l_root_node).inverse() * m_mesh->get_mesh()->get_bone_offsets()[l_mesh_node->get_transform_index()] * m_world;
				l_model_transform = m_mesh->get_mesh()->get_bone_offsets()[l_mesh_node->get_transform_index()] * l_model_transform * m_world;
				//l_model_transform = m_mesh->get_mesh()->get_inverse_bind_poses()[l_mesh_node->get_transform_index()] * l_model_transform * m_world;
				//l_model_transform = l_model_transform * m_world;
			}
			
			//const auto l_root_inverse_transform = m_mesh->get_node_transform(*m_mesh->get_root_node()).inverse();
			//const auto l_identity_transform = core::bc_matrix4f::identity();
			//m_mesh->iterate_over_nodes(l_identity_transform, [this, &l_root_inverse_transform](const bc_mesh_node& p_node, const core::bc_matrix4f& p_parent_transform)
			//{
			//	const auto l_global_transform = p_parent_transform * m_model_transforms->get_node_transform(p_node);
			//	//const auto l_final_transform = l_root_inverse_transform * l_global_transform * m_mesh->get_node_offset_transform(p_node);
			//	const auto l_final_transform = m_model_transforms->get_node_transform(p_node) * m_world * m_mesh->get_node_offset_transform(p_node);

			//	m_model_transforms->set_node_transformation(p_node, l_final_transform);

			//	return l_global_transform;
			//});
			
			return true;
		}
	}
}