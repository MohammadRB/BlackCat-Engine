// [03/29/2021 MRB]

#pragma once

#include "Core/Math/bcCoordinate.h"
#include "Game/Object/Animation/bcAnimationJob.h"
#include "Game/Object/Animation/Job/bcLocalTransformAnimationJob.h"
#include "Game/Object/Animation/Job/bcLocalToModelAnimationJob.h"
#include "Game/Object/Animation/Job/bcModelToSkinnedAnimationJob.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_two_bone_ik_animation_job : public bci_animation_job
		{
		public:
			bc_two_bone_ik_animation_job(core::bc_shared_ptr<bci_local_transform_animation_job> p_local_job,
				core::bc_shared_ptr<bc_local_to_model_animation_job> p_model_job, 
				const bcCHAR* p_first_joint,
				const bcCHAR* p_middle_joint,
				const bcCHAR* p_last_joint,
				const core::bc_vector3f& p_middle_joint_rotation_ls,
				const core::bc_vector3f& p_pole_vector_ms,
				bcFLOAT p_aim_weight = 1.f);

			bc_two_bone_ik_animation_job(bc_two_bone_ik_animation_job&&) noexcept;

			~bc_two_bone_ik_animation_job() override;

			bc_two_bone_ik_animation_job& operator=(bc_two_bone_ik_animation_job&&) noexcept;

			void set_enabled(bool p_enabled) noexcept;
			
			void set_target_ms(const core::bc_vector3f& p_target) noexcept;

			void set_weight(bcFLOAT p_weight) noexcept;

			void set_twist_angle(bcINT32 p_angle) noexcept;
			
			bool run(const core_platform::bc_clock::update_param& p_clock) override;
			
		private:
			bool m_enabled;
			core::bc_shared_ptr<bci_local_transform_animation_job> m_local_job;
			core::bc_shared_ptr<bc_local_to_model_animation_job> m_model_job;
			bcUINT32 m_first_joint_index;
			bcUINT32 m_middle_joint_index;
			bcUINT32 m_last_joint_index;
			core::bc_vector3f m_middle_joint_rotation_ls;
			core::bc_vector3f m_pole_vector_ms;
			bcFLOAT m_aim_weight;
			bcFLOAT m_twist_angle;
			core::bc_vector3f m_target_ms;
		};

		inline void bc_two_bone_ik_animation_job::set_enabled(bool p_enabled) noexcept
		{
			m_enabled = p_enabled;
		}

		inline void bc_two_bone_ik_animation_job::set_target_ms(const core::bc_vector3f& p_target) noexcept
		{
			m_target_ms = p_target;
		}

		inline void bc_two_bone_ik_animation_job::set_weight(bcFLOAT p_weight) noexcept
		{
			m_aim_weight = p_weight;
		}

		inline void bc_two_bone_ik_animation_job::set_twist_angle(bcINT32 p_angle) noexcept
		{
			m_twist_angle = core::bc_to_radian(p_angle);
		}
	}	
}