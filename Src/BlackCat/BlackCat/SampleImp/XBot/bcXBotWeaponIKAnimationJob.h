// [03/29/2021 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Core/Math/bcMatrix4f.h"
#include "Game/Object/Animation/Job/bcTwoBoneIKAnimationJob.h"
#include "BlackCat/SampleImp/XBot/bcXBotWeapon.h"
#include "BlackCat/bcExport.h"

namespace black_cat
{
	class BC_DLL bc_xbot_weapon_ik_animation_job : public game::bci_animation_job
	{
	public:
		bc_xbot_weapon_ik_animation_job(const core::bc_vector3f& p_local_forward,
			core::bc_shared_ptr<game::bci_local_transform_animation_job> p_local_job,
			core::bc_shared_ptr<game::bc_local_to_model_animation_job> p_model_job,
			const bcCHAR* p_main_hand_first_joint,
			const bcCHAR* p_main_hand_middle_joint,
			const bcCHAR* p_main_hand_last_joint,
			const bcCHAR* p_second_hand_first_joint,
			const bcCHAR* p_second_hand_middle_joint,
			const bcCHAR* p_second_hand_last_joint,
			const bcCHAR* p_offset_joint,
			const core::bc_vector3f& p_offset_join_offset);

		bc_xbot_weapon_ik_animation_job(bc_xbot_weapon_ik_animation_job&&) noexcept;

		~bc_xbot_weapon_ik_animation_job() override;

		bc_xbot_weapon_ik_animation_job& operator=(bc_xbot_weapon_ik_animation_job&&) noexcept;

		void set_enabled(bool p_enabled) noexcept override;

		void set_offset_joint(const bcCHAR* p_offset_joint, const core::bc_vector3f& p_offset);
		
		void set_weight(bcFLOAT p_weight) noexcept;

		bcFLOAT get_weight() const noexcept;
		
		/**
		 * \brief Pass null to disable job
		 * \param p_weapon 
		 */
		void set_weapon(bc_xbot_weapon* p_weapon) noexcept;

		bool run(const core_platform::bc_clock::update_param& p_clock) override;
		
	private:
		core::bc_vector3f m_local_forward;
		core::bc_shared_ptr<game::bc_local_to_model_animation_job> m_model_job;
		game::bc_two_bone_ik_animation_job m_main_hand_job;
		game::bc_two_bone_ik_animation_job m_second_hand_job;
		bcUINT32 m_offset_joint_index;
		core::bc_vector3f m_offset_joint_offset;
		bc_xbot_weapon* m_weapon;
	};

	inline void bc_xbot_weapon_ik_animation_job::set_enabled(bool p_enabled) noexcept
	{
		bci_animation_job::set_enabled(p_enabled);
		m_main_hand_job.set_enabled(p_enabled);
		m_second_hand_job.set_enabled(p_enabled);
	}

	inline void bc_xbot_weapon_ik_animation_job::set_offset_joint(const bcCHAR* p_offset_joint, const core::bc_vector3f& p_offset)
	{
		const auto l_offset_joint = get_skeleton().find_joint_by_name(p_offset_joint);
		if (!l_offset_joint.second)
		{
			throw bc_invalid_argument_exception("Joint to place weapon were not found");
		}

		m_offset_joint_index = l_offset_joint.first;
		m_offset_joint_offset = p_offset;
	}

	inline void bc_xbot_weapon_ik_animation_job::set_weight(bcFLOAT p_weight) noexcept
	{
		m_main_hand_job.set_weight(p_weight);
		m_second_hand_job.set_weight(p_weight);
	}

	inline bcFLOAT bc_xbot_weapon_ik_animation_job::get_weight() const noexcept
	{
		return m_main_hand_job.get_weight();
	}
	
	inline void bc_xbot_weapon_ik_animation_job::set_weapon(bc_xbot_weapon* p_weapon) noexcept
	{
		m_weapon = p_weapon;

		if(m_weapon)
		{
			// if two vectors are exactly opposite of each other 'rotation_between_two_vector' method in matrix3x3 will have undefined result
			if (m_local_forward.dot(m_weapon->m_local_forward) == -1.f)
			{
				m_local_forward.x -= 0.01f;
				m_local_forward.normalize();
			}
		}
	}
}