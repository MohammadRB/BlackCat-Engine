// [17/03/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Mesh/bcSubMesh.h"
#include "Game/System/Animation/Job/bcAimAnimationJob.h"
#include "Game/bcException.h"
#include "3rdParty/Ozz/Include/ozz/animation/runtime/ik_aim_job.h"
#include "3rdParty/Ozz/Include/ozz/base/maths/simd_quaternion.h"

namespace black_cat::game
{
	void multiply_soa_transform_quaternion(bcINT32 p_index, 
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

	bc_aim_animation_job::bc_aim_animation_job(core::bc_shared_ptr<bci_local_transform_animation_job> p_local_job,
	                                           core::bc_shared_ptr<bc_local_to_model_animation_job> p_model_job,
	                                           core::bc_shared_ptr<bc_model_to_skinning_animation_job> p_skinned_job,
	                                           core::bc_span<bc_aim_animation_bone> p_joint_chains,
	                                           bcFLOAT p_aim_weight)
		: bci_animation_job(p_local_job->get_skeleton()),
		  m_local_job(std::move(p_local_job)),
		  m_model_job(std::move(p_model_job)),
		  m_skinned_job(std::move(p_skinned_job)),
		  m_bone_chains(std::begin(p_joint_chains), std::end(p_joint_chains)),
		  m_aim_weight(p_aim_weight)
	{
		_assign_bone_indices();
	}

	bc_aim_animation_job::bc_aim_animation_job(core::bc_shared_ptr<bci_local_transform_animation_job> p_local_job,
	                                           core::bc_shared_ptr<bc_local_to_model_animation_job> p_model_job,
	                                           core::bc_shared_ptr<bc_model_to_skinning_animation_job> p_skinned_job,
	                                           const core::bc_span<const bcCHAR*>& p_joint_chains,
	                                           const core::bc_vector3f& p_local_forward,
	                                           bcFLOAT p_aim_weight)
		: bc_aim_animation_job
		(
			std::move(p_local_job),
			std::move(p_model_job),
			std::move(p_skinned_job),
			core::bc_span<bc_aim_animation_bone>(),
			p_aim_weight
		)
	{
		m_bone_chains.reserve(p_joint_chains.size());
			
		std::transform
		(
			std::begin(p_joint_chains),
			std::end(p_joint_chains),
			std::back_inserter(m_bone_chains),
			[&](const bcCHAR* p_bone_name)
			{
				return bc_aim_animation_bone(p_bone_name, core::bc_vector3f::up(), p_local_forward);
			}
		);

		_assign_bone_indices();
	}

	bc_aim_animation_job::bc_aim_animation_job(bc_aim_animation_job&&) noexcept = default;

	bc_aim_animation_job::~bc_aim_animation_job() = default;

	bc_aim_animation_job& bc_aim_animation_job::operator=(bc_aim_animation_job&&) noexcept = default;

	bool bc_aim_animation_job::run(const platform::bc_clock::update_param& p_clock)
	{
		if(m_bone_chains.size() < 2)
		{
			throw bc_invalid_operation_exception("Number of bones in aim bone chain must be greater than 1");
		}
			
		if(!m_enabled)
		{
			return true;
		}
			
		ozz::animation::IKAimJob l_aim_job;
		ozz::math::SimdQuaternion l_correction;
		auto l_ozz_model_transforms = m_model_job->get_native_transforms();
		const auto l_first_chain_bone_index = m_bone_chains[0].m_bone_index;
		const auto l_last_chain_bone_index = m_bone_chains[m_bone_chains.size() - 1].m_bone_index;
		l_aim_job.pole_vector = ozz::math::Normalize3
		(
			l_ozz_model_transforms[l_last_chain_bone_index].cols[3] - l_ozz_model_transforms[l_first_chain_bone_index].cols[3]
		);
		l_aim_job.target = ozz::math::simd_float4::Load3PtrU(&m_model_target.x);
		l_aim_job.joint_correction = &l_correction;

		bcINT32 l_previous_bone = ozz::animation::Skeleton::kNoParent;
		for(bcINT32 l_ite = m_bone_chains.size() - 1; l_ite >= 0 ; --l_ite)
		{
			const auto& l_bone_chain = m_bone_chains[l_ite];

			l_aim_job.joint = &l_ozz_model_transforms[l_bone_chain.m_bone_index];
			l_aim_job.up = ozz::math::simd_float4::Load3PtrU(&l_bone_chain.m_local_up.x);

			const bool l_is_last_bone = l_ite == 0;
			l_aim_job.weight = m_aim_weight * (l_is_last_bone ? 1.f : 0.1f);
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
			multiply_soa_transform_quaternion(l_bone_chain.m_bone_index, l_correction, ozz::make_span(m_local_job->get_local_transforms()));
				
			l_previous_bone = l_bone_chain.m_bone_index;
		}

		return m_model_job->run(p_clock, l_first_chain_bone_index);
	}

	void bc_aim_animation_job::_assign_bone_indices()
	{
		for (auto& l_bone_chain : m_bone_chains)
		{
			const auto l_real_bone_name = get_skeleton().find_joint_by_name(l_bone_chain.m_bone_name);
			if(!l_real_bone_name.second)
			{
				throw bc_invalid_argument_exception("Bone chain bone were not found in skeleton");
			}
				
			l_bone_chain.m_bone_index = l_real_bone_name.first;
		}
	}
}
