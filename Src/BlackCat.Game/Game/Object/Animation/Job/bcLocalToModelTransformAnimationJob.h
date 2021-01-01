// [12/31/2020 MRB]

#pragma once

#include "Core/Math/bcMatrix4f.h"
#include "Core/Container/bcVector.h"
#include "Game/Object/Animation/bcAnimationJob.h"
#include "Game/Object/Animation/Job/bcLocalTransformAnimationJob.h"

namespace black_cat
{
	namespace game
	{
		class bc_local_to_model_transform_animation_job : public bci_animation_job
		{
		public:
			explicit bc_local_to_model_transform_animation_job(bci_local_transform_animation_job& p_local_transform_job);

			bc_local_to_model_transform_animation_job(bc_local_to_model_transform_animation_job&&) noexcept = default;

			~bc_local_to_model_transform_animation_job() = default;

			bc_local_to_model_transform_animation_job& operator=(bc_local_to_model_transform_animation_job&&) noexcept = default;

			void set_world() const noexcept;
			
			void run() const override;
			
		private:
			bci_local_transform_animation_job* m_local_transform_job;
			core::bc_vector_movable<core::bc_matrix4f> m_model_transforms;
			core::bc_matrix4f m_world;
		};
	}	
}