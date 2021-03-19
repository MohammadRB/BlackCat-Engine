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
		class bc_sub_mesh;
		
		struct bc_aim_animation_bone
		{
			const bcCHAR* m_bone_name;
			bcUINT32 m_bone_index;
			core::bc_vector3f m_local_up;
			core::bc_vector3f m_local_forward;
		};
		
		class BC_GAME_DLL bc_aim_animation_job : public bci_animation_job
		{
		public:
			bc_aim_animation_job(const bc_sub_mesh& p_mesh,
				core::bc_shared_ptr<bci_local_transform_animation_job> p_local_job, 
				core::bc_shared_ptr<bc_local_to_model_animation_job> p_model_job,
				core::bc_shared_ptr<bc_model_to_skinned_animation_job> p_skinned_job,
				core::bc_span<bc_aim_animation_bone> p_joint_chains,
				bcFLOAT p_aim_weight = 1.f);

			bc_aim_animation_job(bc_aim_animation_job&&) noexcept;

			~bc_aim_animation_job() override;

			bc_aim_animation_job& operator=(bc_aim_animation_job&&) noexcept;

			void set_world_target(const core::bc_vector3f& p_target) noexcept;

			void set_aim_weight(bcFLOAT p_aim_weight) noexcept;
			
			bool run(const core_platform::bc_clock::update_param& p_clock) override;
			
		private:
			core::bc_shared_ptr< bci_local_transform_animation_job > m_local_job;
			core::bc_shared_ptr< bc_local_to_model_animation_job > m_model_job;
			core::bc_shared_ptr< bc_model_to_skinned_animation_job > m_skinned_job;
			core::bc_vector< bc_aim_animation_bone > m_bone_chains;
			bcFLOAT m_aim_weight;
			core::bc_vector3f m_model_target;
		};

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