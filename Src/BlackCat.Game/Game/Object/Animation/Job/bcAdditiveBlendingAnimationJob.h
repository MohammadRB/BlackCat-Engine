// [03/31/2021 MRB]

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
		 * \brief Additive blending of two separate samples.
		 * The first job must have been executed but the second job will be executed by blending job.
		 */
		class BC_GAME_DLL bc_additive_blending_animation_job : public bci_local_transform_animation_job
		{
		public:
			/**
			 * \brief Partial blend of two sampled animations.
			 * \param p_skeleton
			 * \param p_main_layer First animation which will get weight 1 by default. This sampling job must have been executed before the blending job.
			 * \param p_additive_layer Second animation which will get weight 1 for all children of passed root node. This sampling job will be executed by the blending job.
			 * \param p_additive_layer_root_joint Root node which layer 2 will get weight 1 from this joint. Can be Null.
			 */
			bc_additive_blending_animation_job(bc_animation_skeleton& p_skeleton,
				core::bc_shared_ptr<bci_local_transform_animation_job> p_main_layer,
				core::bc_shared_ptr<bc_sampling_animation_job> p_additive_layer,
				const bcCHAR* p_additive_layer_root_joint);

			bc_additive_blending_animation_job(bc_additive_blending_animation_job&&) noexcept = default;

			~bc_additive_blending_animation_job() override = default;

			bc_additive_blending_animation_job& operator=(bc_additive_blending_animation_job&&) noexcept = default;

			bc_animation_local_transform& get_local_transforms() noexcept override;

			const bc_animation_local_transform& get_local_transforms() const noexcept override;

			const core::bc_shared_ptr<bci_local_transform_animation_job>& get_main_layer() const noexcept;

			const core::bc_shared_ptr<bc_sampling_animation_job>& get_additive_layer() const noexcept;
			
			void set_weights(bcFLOAT p_main_layer_weight, bcFLOAT p_additive_layer_weight) noexcept;
			
			bool run(const core_platform::bc_clock::update_param& p_clock) override;

		private:
			bcFLOAT m_main_layer_weight;
			bcFLOAT m_additive_layer_weight;
			core::bc_shared_ptr<bci_local_transform_animation_job> m_main_layer;
			core::bc_shared_ptr<bc_sampling_animation_job> m_additive_layer;
			core::bc_vector_aligned<ozz::math::SimdFloat4, 16> m_additive_layer_weights;
			bc_animation_local_transform m_locals;
		};

		inline bc_animation_local_transform& bc_additive_blending_animation_job::get_local_transforms() noexcept
		{
			if (m_enabled)
			{
				return m_locals;
			}

			return m_main_layer->get_local_transforms();
		}

		inline const bc_animation_local_transform& bc_additive_blending_animation_job::get_local_transforms() const noexcept
		{
			if (m_enabled)
			{
				return m_locals;
			}

			return m_main_layer->get_local_transforms();
		}

		inline const core::bc_shared_ptr<bci_local_transform_animation_job>& bc_additive_blending_animation_job::get_main_layer() const noexcept
		{
			return m_main_layer;
		}

		inline const core::bc_shared_ptr<bc_sampling_animation_job>& bc_additive_blending_animation_job::get_additive_layer() const noexcept
		{
			return m_additive_layer;
		}

		inline void bc_additive_blending_animation_job::set_weights(bcFLOAT p_main_layer_weight, bcFLOAT p_additive_layer_weight) noexcept
		{
			m_main_layer_weight = p_main_layer_weight;
			m_additive_layer_weight = p_additive_layer_weight;
		}
	}
}