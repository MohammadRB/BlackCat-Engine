// [12/31/2020 MRB]

#pragma once

#include "Game/Object/Animation/bcAnimationJob.h"
#include "Game/Object/Animation/bcAnimationLocalTransform.h"

namespace black_cat
{
	namespace game
	{
		class bci_animation_job_local_transform : public bci_animation_job
		{
		public:
			virtual ~bci_animation_job_local_transform() = 0;
			
			virtual const bc_animation_local_transform& get_local_transforms() const noexcept = 0;

		protected:
			explicit bci_animation_job_local_transform(bc_animation_skeleton& p_skeleton);
			
			bci_animation_job_local_transform(bci_animation_job_local_transform&&) = default;

			bci_animation_job_local_transform& operator=(bci_animation_job_local_transform&&) = default;
		};

		inline bci_animation_job_local_transform::~bci_animation_job_local_transform() = default;

		inline bci_animation_job_local_transform::bci_animation_job_local_transform(bc_animation_skeleton& p_skeleton)
			: bci_animation_job(p_skeleton)
		{
		}
	}
}