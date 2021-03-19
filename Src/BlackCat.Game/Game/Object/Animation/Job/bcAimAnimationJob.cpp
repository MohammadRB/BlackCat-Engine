// [03/17/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Mesh/bcSubMesh.h"
#include "Game/Object/Animation/Job/bcAimAnimationJob.h"
#include "3rdParty/Ozz/Include/ozz/animation/runtime/ik_aim_job.h"
#include "3rdParty/Ozz/Include/ozz/base/maths/simd_quaternion.h"

namespace black_cat
{
	namespace game
	{
		void MultiplySoATransformQuaternion(bcINT32 p_index, 
			const ozz::math::SimdQuaternion& p_quat,
			const ozz::span<ozz::math::SoaTransform>& p_transforms)
		{
			assert(p_index >= 0 && static_cast<size_t>(p_index) < p_transforms.size() * 4 && "joint index out of bound.");

			// Convert soa to aos in order to perform quaternion multiplication, and gets back to soa.
			ozz::math::SoaTransform& l_soa_transform_ref = p_transforms[p_index / 4];
			ozz::math::SimdQuaternion l_aos_quats[4];
			ozz::math::Transpose4x4(&l_soa_transform_ref.rotation.x, &l_aos_quats->xyzw);

			ozz::math::SimdQuaternion& l_aos_quat_ref = l_aos_quats[p_index & 3];
			l_aos_quat_ref = l_aos_quat_ref * p_quat;

			ozz::math::Transpose4x4(&l_aos_quats->xyzw, &l_soa_transform_ref.rotation.x);
		}

		bc_aim_animation_job::bc_aim_animation_job(const bc_sub_mesh& p_mesh,
			core::bc_shared_ptr< bci_local_transform_animation_job > p_local_job,
			core::bc_shared_ptr< bc_local_to_model_animation_job > p_model_job,
			core::bc_shared_ptr< bc_model_to_skinned_animation_job > p_skinned_job,
			core::bc_span< bc_aim_animation_bone > p_joint_chains,
			bcFLOAT p_aim_weight)
			: bci_animation_job(p_local_job->get_skeleton()),
			m_local_job(std::move(p_local_job)),
			m_model_job(std::move(p_model_job)),
			m_skinned_job(std::move(p_skinned_job)),
			m_bone_chains(std::begin(p_joint_chains), std::end(p_joint_chains)),
			m_aim_weight(p_aim_weight)
		{
			physics::bc_bound_box l_mesh_bound_box;
			bc_sub_mesh_mat4_transform l_mesh_absolute_transforms(*p_mesh.get_root_node());
			p_mesh.calculate_absolute_transforms(core::bc_matrix4f::identity(), l_mesh_absolute_transforms, l_mesh_bound_box);
			
			auto l_bone_names = get_skeleton().get_bone_names();
			auto l_bone_index = 0U;
			for(auto l_bone_name : l_bone_names)
			{
				auto l_bone_ite = std::find_if(std::begin(m_bone_chains), std::end(m_bone_chains), [l_bone_name](bc_aim_animation_bone& p_bone)
				{
					return std::strstr(l_bone_name, p_bone.m_bone_name) != nullptr;
				});
				
				if(l_bone_ite != std::end(m_bone_chains))
				{
					const auto* l_mesh_node = p_mesh.find_node(l_bone_ite->m_bone_name);
					BC_ASSERT(l_mesh_node);

					const core::bc_matrix4f& l_bone_transform = p_mesh.get_node_absolute_transform(*l_mesh_node, l_mesh_absolute_transforms);
					l_bone_ite->m_bone_index = l_bone_index;
					l_bone_ite->m_local_up = l_bone_transform.get_basis_y();
					l_bone_ite->m_local_forward = l_bone_transform.get_basis_z();
				}

				++l_bone_index;
			}
		}

		bc_aim_animation_job::bc_aim_animation_job(bc_aim_animation_job&&) noexcept = default;

		bc_aim_animation_job::~bc_aim_animation_job() = default;

		bc_aim_animation_job& bc_aim_animation_job::operator=(bc_aim_animation_job&&) noexcept = default;

		bool bc_aim_animation_job::run(const core_platform::bc_clock::update_param& p_clock)
		{
			ozz::animation::IKAimJob l_aim_job;
			ozz::math::SimdQuaternion l_correction;
			auto l_ozz_model_transforms = m_model_job->get_native_transforms();
			
			l_aim_job.pole_vector = ozz::math::simd_float4::y_axis();
			l_aim_job.target = ozz::math::simd_float4::Load3PtrU(&m_model_target.x);
			l_aim_job.joint_correction = &l_correction;

			bcINT32 l_previous_bone = ozz::animation::Skeleton::kNoParent;
			for(bcINT32 l_ite = m_bone_chains.size() - 1; l_ite >= 0 ; --l_ite)
			{
				const auto& l_bone_chain = m_bone_chains[l_ite];
				
				l_aim_job.joint = &l_ozz_model_transforms[l_bone_chain.m_bone_index];
				l_aim_job.up = ozz::math::simd_float4::Load3PtrU(&l_bone_chain.m_local_up.x);

				const bool l_is_last_bone = l_ite == 0;
				l_aim_job.weight = m_aim_weight * (l_is_last_bone ? 1.f : 0.5f);
				l_aim_job.offset = ozz::math::simd_float4::Load1(0);

				if(l_ite == m_bone_chains.size() - 1)
				{
					// First joint, uses global forward and offset.
					l_aim_job.forward = ozz::math::simd_float4::Load3PtrU(&l_bone_chain.m_local_forward.x);
				}
				else
				{
					// Applies previous correction to "forward" and "offset", before bringing them to model-space (_ms).
					const ozz::math::SimdFloat4 l_corrected_forward_ms = ozz::math::TransformVector
					(
						l_ozz_model_transforms[l_previous_bone],
						ozz::math::TransformVector(l_correction, l_aim_job.forward)
					);

					// Brings "forward" and "offset" to joint local-space
					const ozz::math::Float4x4 l_inv_joint = ozz::math::Invert(l_ozz_model_transforms[l_bone_chain.m_bone_index]);
					l_aim_job.forward = ozz::math::TransformVector(l_inv_joint, l_corrected_forward_ms);
				}

				if (!l_aim_job.Run()) 
				{
					return false;
				}

				// Apply IK quaternion to its respective local-space transforms.
				MultiplySoATransformQuaternion(l_bone_chain.m_bone_index, l_correction, ozz::make_span(m_local_job->get_local_transforms()));
				
				l_previous_bone = l_bone_chain.m_bone_index;
			}

			return m_model_job->run(p_clock, l_previous_bone);
		}
	}
}
