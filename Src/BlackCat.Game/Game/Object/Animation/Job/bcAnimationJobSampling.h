// [12/31/2020 MRB]

#pragma once

#include "Core/Memory/bcPtr.h"
#include "Game/Object/Animation/bcAnimationSkeleton.h"
#include "Game/Object/Animation/bcSkeletonAnimation.h"
#include "Game/Object/Animation/bcAnimationLocalTransform.h"
#include "Game/Object/Animation/Job/bcAnimationJobLocalTransform.h"
#include "Game/bcExport.h"

#include "3rdParty/Ozz/Include/ozz/animation/runtime/sampling_job.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_animation_job_sampling : public bci_animation_job_local_transform
		{
		public:
			bc_animation_job_sampling(bc_animation_skeleton& p_skeleton, bc_skeleton_animation& p_animation);

			bc_animation_job_sampling(bc_animation_job_sampling&&) noexcept = default;

			~bc_animation_job_sampling() = default;

			bc_animation_job_sampling& operator=(bc_animation_job_sampling&&) noexcept = default;

			const bc_animation_local_transform& get_local_transforms() const noexcept override;

			bool run(const core_platform::bc_clock::update_param& p_clock) override;
			
		private:
			bc_skeleton_animation* m_animation;
			
			core::bc_unique_ptr<ozz::animation::SamplingCache> m_sampling_cache;
			bc_animation_local_transform m_locals;
		};

		inline const bc_animation_local_transform& bc_animation_job_sampling::get_local_transforms() const noexcept
		{
			return m_locals;
		}
	}	
}