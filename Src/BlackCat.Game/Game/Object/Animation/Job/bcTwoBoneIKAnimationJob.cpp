// [03/29/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Animation/Job/bcTwoBoneIKAnimationJob.h"
#include "3rdParty/Ozz/Include/ozz/animation/runtime/ik_two_bone_job.h"
#include "3rdParty/Ozz/Include/ozz/base/maths/simd_quaternion.h"

namespace black_cat
{
	namespace game
	{
		extern void multiply_soa_transform_quaternion(bcINT32 p_index,
			const ozz::math::SimdQuaternion& p_quat,
			const ozz::span<ozz::math::SoaTransform>& p_transforms);
		
		bc_two_bone_ik_animation_job::bc_two_bone_ik_animation_job(core::bc_shared_ptr<bci_local_transform_animation_job> p_local_job,
			core::bc_shared_ptr<bc_local_to_model_animation_job> p_model_job,
			const bcCHAR* p_first_joint,
			const bcCHAR* p_middle_joint,
			const bcCHAR* p_last_joint,
			const core::bc_vector3f& p_middle_joint_rotation_ls,
			const core::bc_vector3f& p_pole_vector_ms,
			bcFLOAT p_aim_weight)
			: bci_animation_job(p_local_job->get_skeleton()),
			m_enabled(true),
			m_local_job(std::move(p_local_job)),
			m_model_job(std::move(p_model_job)),
			m_middle_joint_rotation_ls(core::bc_vector3f::normalize(p_middle_joint_rotation_ls)),
			m_pole_vector_ms(core::bc_vector3f::normalize(p_pole_vector_ms)),
			m_aim_weight(p_aim_weight),
			m_twist_angle(0)
		{
			const auto l_first_joint = get_skeleton().find_joint_by_name(p_first_joint);
			const auto l_middle_joint = get_skeleton().find_joint_by_name(p_middle_joint);
			const auto l_last_joint = get_skeleton().find_joint_by_name(p_last_joint);

			if(!l_first_joint.second || !l_middle_joint.second || !l_last_joint.second)
			{
				throw bc_invalid_argument_exception("Joint name were not found");
			}

			m_first_joint_index = l_first_joint.first;
			m_middle_joint_index = l_middle_joint.first;
			m_last_joint_index = l_last_joint.first;
		}

		bc_two_bone_ik_animation_job::bc_two_bone_ik_animation_job(bc_two_bone_ik_animation_job&&) noexcept = default;

		bc_two_bone_ik_animation_job::~bc_two_bone_ik_animation_job() = default;

		bc_two_bone_ik_animation_job& bc_two_bone_ik_animation_job::operator=(bc_two_bone_ik_animation_job&&) noexcept = default;

		bool bc_two_bone_ik_animation_job::run(const core_platform::bc_clock::update_param& p_clock)
		{
			if(!m_enabled)
			{
				return true;
			}
			
			ozz::animation::IKTwoBoneJob l_ik_job;
			l_ik_job.target = ozz::math::simd_float4::Load3PtrU(&m_target_ms.x);
			l_ik_job.pole_vector = ozz::math::simd_float4::Load3PtrU(&m_pole_vector_ms.x);
			l_ik_job.mid_axis = ozz::math::simd_float4::Load3PtrU(&m_middle_joint_rotation_ls.x);
			l_ik_job.weight = m_aim_weight;
			l_ik_job.soften = 0.95f;
			l_ik_job.twist_angle = m_twist_angle;
			l_ik_job.start_joint = &m_model_job->get_native_transforms()[m_first_joint_index];
			l_ik_job.mid_joint = &m_model_job->get_native_transforms()[m_middle_joint_index];
			l_ik_job.end_joint = &m_model_job->get_native_transforms()[m_last_joint_index];

			ozz::math::SimdQuaternion l_start_correction;
			l_ik_job.start_joint_correction = &l_start_correction;
			ozz::math::SimdQuaternion l_mid_correction;
			l_ik_job.mid_joint_correction = &l_mid_correction;

			if (!l_ik_job.Run())
			{
				return false;
			}

			multiply_soa_transform_quaternion(m_first_joint_index, l_start_correction, ozz::make_span(m_local_job->get_local_transforms()));
			multiply_soa_transform_quaternion(m_middle_joint_index, l_mid_correction, ozz::make_span(m_local_job->get_local_transforms()));

			return m_model_job->run(p_clock, m_first_joint_index);
		}
	}	
}