// [01/05/2021 MRB]

#include "Game/GamePCH.h"

#include "Core/Math/bcMatrix4f.h"
#include "Game/Object/Animation/Job/bcLocalToModelTransformAnimationJob.h"
#include "Game/bcException.h"

#include "3rdParty/Ozz/Include/ozz/animation/runtime/local_to_model_job.h"

namespace black_cat
{
	namespace game
	{
		bc_animation_job_local_to_model_transform::bc_animation_job_local_to_model_transform(bci_local_transform_animation_job& p_local_transform_job, 
			const bc_sub_mesh& p_sub_mesh,
			bc_sub_mesh_mat4_transform& p_transforms)
			: bci_animation_job(p_local_transform_job.get_skeleton()),
			m_mesh(&p_sub_mesh),
			m_local_transform_job(&p_local_transform_job),
			m_ozz_model_transforms(p_local_transform_job.get_skeleton().get_num_bones()),
			m_model_transforms(&p_transforms)
		{
			if (m_model_transforms->size() < m_skeleton->get_num_bones())
			{
				throw bc_invalid_argument_exception("Mesh and animation bones count does not match");
			}
		}

		bool bc_animation_job_local_to_model_transform::run(const core_platform::bc_clock::update_param& p_clock)
		{
			ozz::animation::LocalToModelJob l_ltm_job;
			const auto& l_local_transforms = m_local_transform_job->get_local_transforms();
			const auto l_scale = ozz::math::Float4x4::Scaling(ozz::math::simd_float4::Load1(m_mesh->get_mesh_scale()));
			
			l_ltm_job.skeleton = &get_skeleton().get_native_handle();
			l_ltm_job.input = { &*l_local_transforms.begin(), l_local_transforms.size() };
			l_ltm_job.output = ozz::make_span(m_ozz_model_transforms);
			l_ltm_job.root = &l_scale;

			if(!l_ltm_job.Run())
			{
				return false;
			}

			const auto l_bone_names = get_skeleton().get_bone_names();
			auto l_bone_name_ite = l_bone_names.begin();
			for(auto l_bone_ite = 0U; l_bone_ite < l_bone_names.size(); ++l_bone_ite, ++l_bone_name_ite)
			{
				const auto* l_bone_name = *l_bone_name_ite;
				const auto* l_mesh_node = m_mesh->find_node(l_bone_name);
				const auto& l_ozz_model_transform = m_ozz_model_transforms[l_bone_ite];
				auto& l_model_transform = m_model_transforms->get_node_transform(*l_mesh_node);

				if(core::bc_matrix4f::use_column_major_storage())
				{
					bcFLOAT l_col[4];
					ozz::math::StorePtr(l_ozz_model_transform.cols[0], l_col);
					
					l_model_transform[0] = l_col[0];
					l_model_transform[1] = l_col[1];
					l_model_transform[2] = l_col[2];
					l_model_transform[3] = l_col[3];

					ozz::math::StorePtr(l_ozz_model_transform.cols[1], l_col);

					l_model_transform[4] = l_col[0];
					l_model_transform[5] = l_col[1];
					l_model_transform[6] = l_col[2];
					l_model_transform[7] = l_col[3];

					ozz::math::StorePtr(l_ozz_model_transform.cols[2], l_col);

					l_model_transform[8] = l_col[0];
					l_model_transform[9] = l_col[1];
					l_model_transform[10] = l_col[2];
					l_model_transform[11] = l_col[3];

					ozz::math::StorePtr(l_ozz_model_transform.cols[3], l_col);

					l_model_transform[12] = l_col[0];
					l_model_transform[13] = l_col[1];
					l_model_transform[14] = l_col[2];
					l_model_transform[15] = l_col[3];
				}
				else
				{
					auto& l_model_transform_ptr = l_model_transform(0, 0);
					
					// Store columns into rows to transpose matrix
					ozz::math::StorePtr(l_ozz_model_transform.cols[0], &l_model_transform_ptr);
					ozz::math::StorePtr(l_ozz_model_transform.cols[1], &l_model_transform_ptr + 4);
					ozz::math::StorePtr(l_ozz_model_transform.cols[2], &l_model_transform_ptr + 8);
					ozz::math::StorePtr(l_ozz_model_transform.cols[3], &l_model_transform_ptr + 12);
				}
			}
			
			return true;
		}
	}
}