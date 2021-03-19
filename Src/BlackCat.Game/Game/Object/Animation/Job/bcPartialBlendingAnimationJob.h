// [03/12/2021 MRB]

#pragma once

#include "Core/Memory/bcPtr.h"
#include "Core/Container/bcVector.h"
#include "Game/Object/Animation/Job/bcLocalTransformAnimationJob.h"
#include "Game/bcExport.h"
#include "3rdParty/Ozz/Include/ozz/base/maths/simd_math.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_partial_blending_animation_job : bci_local_transform_animation_job
		{
		public:
			/**
			 * \brief Partial blend of two sampled animations.
			 * \param p_skeleton 
			 * \param p_layer1 First animation which will get weight 1 by default
			 * \param p_layer2 Second animation which will get weight 1 for all children of passed root node
			 * \param p_layer_2_root_joint Root node which layer 2 will get weight 1 from this joint
			 */
			bc_partial_blending_animation_job(bc_animation_skeleton& p_skeleton, 
				core::bc_shared_ptr<bci_local_transform_animation_job> p_layer1,
				core::bc_shared_ptr<bci_local_transform_animation_job> p_layer2,
				const bcCHAR* p_layer_2_root_joint);

			bc_partial_blending_animation_job(bc_partial_blending_animation_job&&) noexcept = default;

			~bc_partial_blending_animation_job() override = default;

			bc_partial_blending_animation_job& operator=(bc_partial_blending_animation_job&&) noexcept = default;

			bc_animation_local_transform& get_local_transforms() noexcept override;
			
			const bc_animation_local_transform& get_local_transforms() const noexcept override;
			
			bool run(const core_platform::bc_clock::update_param& p_clock) override;

		private:
			core::bc_shared_ptr<bci_local_transform_animation_job> m_layer1;
			core::bc_shared_ptr<bci_local_transform_animation_job> m_layer2;
			core::bc_vector_movable<ozz::math::SimdFloat4> m_layer1_weights;
			core::bc_vector_movable<ozz::math::SimdFloat4> m_layer2_weights;
			
			bc_animation_local_transform m_locals;
		};

		inline bc_animation_local_transform& bc_partial_blending_animation_job::get_local_transforms() noexcept
		{
			return m_locals;
		}
		
		inline const bc_animation_local_transform& bc_partial_blending_animation_job::get_local_transforms() const noexcept
		{
			return m_locals;
		}
	}
}