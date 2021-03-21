// [03/17/2021 MRB]

#pragma once

#include "Core/Memory/bcPtr.h"
#include "Core/Math/bcVector3f.h"
#include "Core/Container/bcSpan.h"
#include "Core/Container/bcVector.h"
#include "Game/Object/Animation/bcAnimationJob.h"
#include "Game/Object/Animation/Job/bcLocalTransformAnimationJob.h"
#include "Game/Object/Animation/Job/bcLocalToModelAnimationJob.h"
#include "Game/Object/Animation/Job/bcModelToSkinnedAnimationJob.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		struct bc_aim_animation_bone
		{
			bc_aim_animation_bone(const bcCHAR* p_bone_name)
				: m_bone_index(-1),
				m_bone_name(p_bone_name)
			{
			}

			bc_aim_animation_bone(const bcCHAR* p_bone_name, const core::bc_vector3f& p_local_up, const core::bc_vector3f& p_local_forward)
				: m_bone_index(-1),
				m_bone_name(p_bone_name),
				m_local_up(p_local_up),
				m_local_forward(p_local_forward)
			{	
			}
			
			bcUINT32 m_bone_index;
			const bcCHAR* m_bone_name;
			core::bc_vector3f m_local_up;
			core::bc_vector3f m_local_forward;
		};
		
		class BC_GAME_DLL bc_aim_animation_job : public bci_animation_job
		{
		public:
			bc_aim_animation_job(core::bc_shared_ptr<bci_local_transform_animation_job> p_local_job, 
				core::bc_shared_ptr<bc_local_to_model_animation_job> p_model_job,
				core::bc_shared_ptr<bc_model_to_skinned_animation_job> p_skinned_job,
				core::bc_span<bc_aim_animation_bone> p_joint_chains,
				bcFLOAT p_aim_weight = 1.f);

			bc_aim_animation_job(core::bc_shared_ptr<bci_local_transform_animation_job> p_local_job,
				core::bc_shared_ptr<bc_local_to_model_animation_job> p_model_job,
				core::bc_shared_ptr<bc_model_to_skinned_animation_job> p_skinned_job,
				const core::bc_span<const bcCHAR*>& p_joint_chains,
				const core::bc_vector3f& p_local_forward,
				bcFLOAT p_aim_weight = 1.f);
			
			bc_aim_animation_job(bc_aim_animation_job&&) noexcept;

			~bc_aim_animation_job() override;

			bc_aim_animation_job& operator=(bc_aim_animation_job&&) noexcept;

			void set_enabled(bool p_enabled) noexcept;
			
			void set_aim_weight(bcFLOAT p_aim_weight) noexcept;
			
			void set_world_target(const core::bc_vector3f& p_target) noexcept;
			
			bool run(const core_platform::bc_clock::update_param& p_clock) override;
			
		private:
			void _assign_bone_indices();

			core::bc_shared_ptr<bci_local_transform_animation_job> m_local_job;
			core::bc_shared_ptr<bc_local_to_model_animation_job> m_model_job;
			core::bc_shared_ptr<bc_model_to_skinned_animation_job> m_skinned_job;
			core::bc_vector<bc_aim_animation_bone> m_bone_chains;
			bool m_enabled;
			bcFLOAT m_aim_weight;
			core::bc_vector3f m_model_target;
		};

		inline void bc_aim_animation_job::set_enabled(bool p_enabled) noexcept
		{
			m_enabled = p_enabled;
		}
		
		inline void bc_aim_animation_job::set_world_target(const core::bc_vector3f& p_target) noexcept
		{
			m_model_target = (m_skinned_job->get_world().inverse() * core::bc_vector4f(p_target, 1)).xyz();
		}

		inline void bc_aim_animation_job::set_aim_weight(bcFLOAT p_aim_weight) noexcept
		{
			m_aim_weight = p_aim_weight;
		}
	}
}