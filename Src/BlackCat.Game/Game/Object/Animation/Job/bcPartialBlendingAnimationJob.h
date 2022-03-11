// [03/12/2021 MRB]

#pragma once

#include "Core/Memory/bcPtr.h"
#include "Core/Container/bcVector.h"
#include "Game/Object/Animation/Job/bcLocalTransformAnimationJob.h"
#include "Game/Object/Animation/Job/bcSamplingAnimationJob.h"
#include "Game/bcExport.h"
#include "3rdParty/Ozz/Include/ozz/base/maths/simd_math.h"

namespace black_cat
{
	namespace game
	{
		/**
		 * \brief Partial blending of two separate samples.
		 * The first job must have been executed but the second job will be executed by blending job.
		 */
		class BC_GAME_DLL bc_partial_blending_animation_job : public bci_local_transform_animation_job
		{
		public:
			/**
			 * \brief Partial blend of two sampled animations.
			 * \param p_skeleton 
			 * \param p_layer1 First animation which will get weight 1 by default. This sampling job must have been executed before the blending job. 
			 * \param p_layer2 Second animation which will get weight 1 for all children of passed root node. This sampling job will be executed by the blending job.
			 * \param p_layer_2_root_joint Root node which layer 2 will get weight 1 from this joint
			 */
			bc_partial_blending_animation_job(bc_animation_skeleton& p_skeleton, 
				core::bc_shared_ptr<bci_local_transform_animation_job> p_layer1,
				core::bc_shared_ptr<bc_sampling_animation_job> p_layer2,
				const bcCHAR* p_layer_2_root_joint);

			bc_partial_blending_animation_job(bc_partial_blending_animation_job&&) noexcept = default;

			~bc_partial_blending_animation_job() override = default;

			bc_partial_blending_animation_job& operator=(bc_partial_blending_animation_job&&) noexcept = default;

			bc_animation_local_transform& get_local_transforms() noexcept override;
			
			const bc_animation_local_transform& get_local_transforms() const noexcept override;

			const core::bc_shared_ptr<bci_local_transform_animation_job>& get_layer1() const noexcept;
			
			const core::bc_shared_ptr<bc_sampling_animation_job>& get_layer2() const noexcept;
			
			void set_weights(bcFLOAT p_layer1_weight, bcFLOAT p_layer2_weight) noexcept;
			
			bool run(const platform::bc_clock::update_param& p_clock) override;

		private:
			bcFLOAT m_layer1_weight;
			bcFLOAT m_layer2_weight;
			core::bc_shared_ptr<bci_local_transform_animation_job> m_layer1;
			core::bc_shared_ptr<bc_sampling_animation_job> m_layer2;
			core::bc_vector_movable_aligned<ozz::math::SimdFloat4, 16> m_layer1_weights;
			core::bc_vector_movable_aligned<ozz::math::SimdFloat4, 16> m_layer2_weights;
			bc_animation_local_transform m_locals;
		};

		inline bc_animation_local_transform& bc_partial_blending_animation_job::get_local_transforms() noexcept
		{
			if(m_enabled)
			{
				return m_locals;
			}

			return m_layer1->get_local_transforms();
		}
		
		inline const bc_animation_local_transform& bc_partial_blending_animation_job::get_local_transforms() const noexcept
		{
			if (m_enabled)
			{
				return m_locals;
			}

			return m_layer1->get_local_transforms();
		}

		inline const core::bc_shared_ptr<bci_local_transform_animation_job>& bc_partial_blending_animation_job::get_layer1() const noexcept
		{
			return m_layer1;
		}

		inline const core::bc_shared_ptr<bc_sampling_animation_job>& bc_partial_blending_animation_job::get_layer2() const noexcept
		{
			return m_layer2;
		}

		inline void bc_partial_blending_animation_job::set_weights(bcFLOAT p_layer1_weight, bcFLOAT p_layer2_weight) noexcept
		{
			m_layer1_weight = p_layer1_weight;
			m_layer2_weight = p_layer2_weight;
		}
	}
}