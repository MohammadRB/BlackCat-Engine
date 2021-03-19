// [12/31/2020 MRB]

#pragma once

#include "Core/Memory/bcPtr.h"
#include "Game/Object/Animation/bcAnimationSkeleton.h"
#include "Game/Object/Animation/bcSkeletonAnimation.h"
#include "Game/Object/Animation/bcAnimationLocalTransform.h"
#include "Game/Object/Animation/Job/bcLocalTransformAnimationJob.h"
#include "Game/bcExport.h"
#include "3rdParty/Ozz/Include/ozz/animation/runtime/sampling_job.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_sampling_animation_job : public bci_local_transform_animation_job
		{
		public:
			bc_sampling_animation_job(bc_animation_skeleton& p_skeleton, bc_skeleton_animation& p_animation);

			bc_sampling_animation_job(bc_sampling_animation_job&&) noexcept = default;

			~bc_sampling_animation_job() = default;

			bc_sampling_animation_job& operator=(bc_sampling_animation_job&&) noexcept = default;

			bc_animation_local_transform& get_local_transforms() noexcept override;
			
			const bc_animation_local_transform& get_local_transforms() const noexcept override;

			void change_animation(bc_skeleton_animation& p_animation) noexcept;
			
			bool run(const core_platform::bc_clock::update_param& p_clock) override;

		private:
			bc_skeleton_animation* m_animation;
			
			core::bc_unique_ptr<ozz::animation::SamplingCache> m_sampling_cache;
			bc_animation_local_transform m_locals;
		};

		inline bc_animation_local_transform& bc_sampling_animation_job::get_local_transforms() noexcept
		{
			return m_locals;
		}
		
		inline const bc_animation_local_transform& bc_sampling_animation_job::get_local_transforms() const noexcept
		{
			return m_locals;
		}

		inline void bc_sampling_animation_job::change_animation(bc_skeleton_animation& p_animation) noexcept
		{
			m_animation = &p_animation;
		}
	}	
}