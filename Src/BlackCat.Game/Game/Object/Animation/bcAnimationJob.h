// [12/31/2020 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Game/Object/Animation/bcAnimationSkeleton.h"

namespace black_cat
{
	namespace game
	{
		class bci_animation_job
		{
		public:
			virtual ~bci_animation_job() = 0;

			bc_animation_skeleton& get_skeleton() noexcept;

			const bc_animation_skeleton& get_skeleton() const noexcept;

			void reset_local_time() noexcept;
			
			virtual bool run(const core_platform::bc_clock::update_param& p_clock) = 0;

		protected:
			explicit bci_animation_job(bc_animation_skeleton& p_skeleton);

			bci_animation_job(bci_animation_job&&) noexcept = default;

			bci_animation_job& operator=(bci_animation_job&&) noexcept = default;

			bc_animation_skeleton* m_skeleton;
			bcFLOAT m_local_time;
		};

		inline bci_animation_job::~bci_animation_job() = default;

		inline bci_animation_job::bci_animation_job(bc_animation_skeleton& p_skeleton)
			: m_skeleton(&p_skeleton),
			m_local_time(0)
		{
		}

		inline bc_animation_skeleton& bci_animation_job::get_skeleton() noexcept
		{
			return *m_skeleton;
		}

		inline const bc_animation_skeleton& bci_animation_job::get_skeleton() const noexcept
		{
			return *m_skeleton;
		}

		inline void bci_animation_job::reset_local_time() noexcept
		{
			m_local_time = 0;
		}
	}	
}