// [12/31/2020 MRB]

#pragma once

#include "Core/Memory/bcPtr.h"
#include "Game/System/Animation/bcAnimationSkeleton.h"
#include "Game/System/Animation/bcSkeletonAnimation.h"
#include "Game/System/Animation/bcAnimationLocalTransform.h"
#include "Game/System/Animation/Job/bcLocalTransformAnimationJob.h"
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

			~bc_sampling_animation_job() override = default;

			bc_sampling_animation_job& operator=(bc_sampling_animation_job&&) noexcept = default;

			bc_animation_play_mode get_play_mode() const noexcept;

			void set_play_mode(bc_animation_play_mode p_mode) noexcept;

			bcFLOAT get_local_time() const noexcept;

			void set_local_time(bcFLOAT p_time) noexcept;

			void set_speed(bcFLOAT p_speed) noexcept;

			void reset_local_time() noexcept;
			
			bc_animation_local_transform& get_local_transforms() noexcept override;
			
			const bc_animation_local_transform& get_local_transforms() const noexcept override;

			const bc_skeleton_animation* get_animation() const noexcept;
			
			void change_animation(bc_skeleton_animation& p_animation) noexcept;

			bcFLOAT advance_time(const platform::bc_clock::update_param& p_clock);
			
			bool run(const platform::bc_clock::update_param& p_clock) override;

		private:			
			bc_skeleton_animation* m_animation;
			bc_animation_play_mode m_play_mode;
			bcFLOAT m_local_time;
			bcFLOAT m_speed;
			
			core::bc_unique_ptr<ozz::animation::SamplingJob::Context> m_sampling_cache;
			bc_animation_local_transform m_locals;
		};

		inline bc_animation_play_mode bc_sampling_animation_job::get_play_mode() const noexcept
		{
			return m_play_mode;
		}

		inline void bc_sampling_animation_job::set_play_mode(bc_animation_play_mode p_mode) noexcept
		{
			m_play_mode = p_mode;
		}

		inline bcFLOAT bc_sampling_animation_job::get_local_time() const noexcept
		{
			return m_local_time;
		}

		inline void bc_sampling_animation_job::set_local_time(bcFLOAT p_time) noexcept
		{
			m_local_time = p_time;
		}

		inline void bc_sampling_animation_job::set_speed(bcFLOAT p_speed) noexcept
		{
			m_speed = p_speed;
		}

		inline void bc_sampling_animation_job::reset_local_time() noexcept
		{
			m_local_time = 0;
		}
		
		inline bc_animation_local_transform& bc_sampling_animation_job::get_local_transforms() noexcept
		{
			return m_locals;
		}
		
		inline const bc_animation_local_transform& bc_sampling_animation_job::get_local_transforms() const noexcept
		{
			return m_locals;
		}

		inline const bc_skeleton_animation* bc_sampling_animation_job::get_animation() const noexcept
		{
			return m_animation;
		}

		inline void bc_sampling_animation_job::change_animation(bc_skeleton_animation& p_animation) noexcept
		{
			m_animation = &p_animation;
		}
	}	
}