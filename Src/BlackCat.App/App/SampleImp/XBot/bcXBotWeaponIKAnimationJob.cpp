// [03/29/2021 MRB]

#include "App/AppPCH.h"

#include "Game/bcException.h"
#include "Game/bcUtility.h"
#include "App/SampleImp/XBot/bcXBotWeaponIKAnimationJob.h"

namespace black_cat
{
	bc_xbot_weapon_ik_animation_job::bc_xbot_weapon_ik_animation_job(const core::bc_vector3f& p_local_forward,
		core::bc_shared_ptr<game::bci_local_transform_animation_job> p_local_job,
		core::bc_shared_ptr<game::bc_local_to_model_animation_job> p_model_job,
		const bcCHAR* p_main_hand_first_joint,
		const bcCHAR* p_main_hand_middle_joint,
		const bcCHAR* p_main_hand_last_joint,
		const bcCHAR* p_second_hand_first_joint,
		const bcCHAR* p_second_hand_middle_joint,
		const bcCHAR* p_second_hand_last_joint,
		const bcCHAR* p_offset_joint,
		const core::bc_vector3f& p_offset_join_offset)
		: bci_animation_job(p_local_job->get_skeleton()),
		m_local_forward(p_local_forward),
		m_local_right(core::bc_vector3f::cross(core::bc_vector3f::up(), p_local_forward)),
		m_offset_joint_index(0),
		m_rotation(0),
		m_weapon(nullptr),
		m_model_job(p_model_job),
		m_main_hand_job
		(
			p_local_job,
			p_model_job,
			p_main_hand_first_joint,
			p_main_hand_middle_joint,
			p_main_hand_last_joint,
			core::bc_vector3f(-.2f, 0, -1),
			core::bc_vector3f(0, 1, 0)
		),
		m_second_hand_job
		(
			std::move(p_local_job),
			std::move(p_model_job),
			p_second_hand_first_joint,
			p_second_hand_middle_joint,
			p_second_hand_last_joint,
			core::bc_vector3f(.2f, 0, 1),
			core::bc_vector3f(0, 1, 0)
		)
	{
		set_offset_joint(p_offset_joint, p_offset_join_offset);
		
		m_main_hand_job.set_twist_angle(-30);
		m_second_hand_job.set_twist_angle(30);
	}

	bc_xbot_weapon_ik_animation_job::bc_xbot_weapon_ik_animation_job(bc_xbot_weapon_ik_animation_job&&) noexcept = default;

	bc_xbot_weapon_ik_animation_job::~bc_xbot_weapon_ik_animation_job() = default;

	bc_xbot_weapon_ik_animation_job& bc_xbot_weapon_ik_animation_job::operator=(bc_xbot_weapon_ik_animation_job&&) noexcept = default;

	bool bc_xbot_weapon_ik_animation_job::run(const platform::bc_clock::update_param& p_clock)
	{
		if(!m_weapon || !m_enabled)
		{
			return true;
		}

		const auto l_weapon_up_rotation_ms = bc_matrix3f_rotation_between_two_vector(core::bc_vector3f::up(), m_weapon->m_local_up);
		const auto l_weapon_forward_rotation_ms = bc_matrix3f_rotation_between_two_vector(m_local_forward, m_weapon->m_local_forward);
		const auto l_weapon_right_rotation_ms = bc_matrix3f_rotation_euler(m_weapon->m_local_right, core::bc_to_radian(m_rotation));
		const auto l_weapon_rotation_ms = l_weapon_right_rotation_ms * l_weapon_up_rotation_ms * l_weapon_forward_rotation_ms;
		const auto& l_offset_joint = m_model_job->get_transforms()[m_offset_joint_index];
		const auto l_weapon_main_hand_offset_ms = l_offset_joint.get_translation() + m_offset_joint_offset;
		const auto l_weapon_second_hand_offset_ms = l_weapon_main_hand_offset_ms + (l_weapon_rotation_ms * m_weapon->m_second_hand_offset);
		
		m_main_hand_job.set_target_ms(l_weapon_main_hand_offset_ms);
		m_second_hand_job.set_target_ms(l_weapon_second_hand_offset_ms);

		return m_main_hand_job.run(p_clock) && m_second_hand_job.run(p_clock);
	}
}