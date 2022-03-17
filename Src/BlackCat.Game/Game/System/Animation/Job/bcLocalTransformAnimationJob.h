// [12/31/2020 MRB]

#pragma once

#include "Game/System/Animation/bcAnimationJob.h"
#include "Game/System/Animation/bcAnimationLocalTransform.h"

namespace black_cat
{
	namespace game
	{
		class bci_local_transform_animation_job : public bci_animation_job
		{
		public:
			virtual ~bci_local_transform_animation_job() override = 0;
			
			virtual bc_animation_local_transform& get_local_transforms() noexcept = 0;
			
			virtual const bc_animation_local_transform& get_local_transforms() const noexcept = 0;

		protected:
			explicit bci_local_transform_animation_job(bc_animation_skeleton& p_skeleton);
			
			bci_local_transform_animation_job(bci_local_transform_animation_job&&) = default;

			bci_local_transform_animation_job& operator=(bci_local_transform_animation_job&&) = default;
		};

		inline bci_local_transform_animation_job::~bci_local_transform_animation_job() = default;

		inline bci_local_transform_animation_job::bci_local_transform_animation_job(bc_animation_skeleton& p_skeleton)
			: bci_animation_job(p_skeleton)
		{
		}
	}
}