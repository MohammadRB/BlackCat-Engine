// [03/11/2021 MRB]

#pragma once

#include <initializer_list>
#include "Core/Memory/bcPtr.h"
#include "Game/Object/Animation/Job/bcSamplingAnimationJob.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		/**
		 * \brief Blend multiple samples based on their weights.
		 * All sampling jobs will be executed by this job.
		 */
		class BC_GAME_DLL bc_blending_animation_job : public bci_local_transform_animation_job
		{
		public:
			bc_blending_animation_job(bc_animation_skeleton& p_skeleton, std::initializer_list<core::bc_shared_ptr<bc_sampling_animation_job>> p_layers);

			bc_blending_animation_job(bc_blending_animation_job&&) noexcept = default;

			~bc_blending_animation_job() override = default;

			bc_blending_animation_job& operator=(bc_blending_animation_job&&) noexcept = default;

			bc_animation_local_transform& get_local_transforms() noexcept override;
			
			const bc_animation_local_transform& get_local_transforms() const noexcept override;

			/**
			 * \brief Set weight for layers.
			 * \param p_weights Pointer to weights array which must contain as many element as layers
			 */
			void set_weights(const bcFLOAT* p_weights) noexcept;

			/**
			 * \brief Set local time for layers. If provided time is lower than zero the time would not change.
			 * \param p_time Pointer to time array which must contain as many element as layers
			 */
			void set_local_times(const bcFLOAT* p_time) noexcept;
			
			bool run(const core_platform::bc_clock::update_param& p_clock) override;

		private:
			core::bc_vector< std::pair< core::bc_shared_ptr<bc_sampling_animation_job>, bcFLOAT > > m_layers;
			bc_animation_local_transform m_locals;
		};

		inline bc_animation_local_transform& bc_blending_animation_job::get_local_transforms() noexcept
		{
			return m_locals;
		}
		
		inline const bc_animation_local_transform& bc_blending_animation_job::get_local_transforms() const noexcept
		{
			return m_locals;
		}
	}
}