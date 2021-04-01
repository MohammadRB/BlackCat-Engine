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

			bcFLOAT get_local_time() const noexcept;
			
			void set_local_time(bcFLOAT p_time) noexcept;

			void set_speed(bcFLOAT p_speed) noexcept;
			
			void reset_local_time() noexcept;
			
			virtual bool run(const core_platform::bc_clock::update_param& p_clock) = 0;

		protected:
			explicit bci_animation_job(bc_animation_skeleton& p_skeleton);

			bci_animation_job(bci_animation_job&&) noexcept = default;

			bci_animation_job& operator=(bci_animation_job&&) noexcept = default;

			bcFLOAT update_time(const core_platform::bc_clock::update_param& p_clock, bcFLOAT p_animation_time);
			
			bc_animation_skeleton* m_skeleton;
			bcFLOAT m_local_time;
			bcFLOAT m_speed;
		};

		inline bci_animation_job::~bci_animation_job() = default;

		inline bci_animation_job::bci_animation_job(bc_animation_skeleton& p_skeleton)
			: m_skeleton(&p_skeleton),
			m_local_time(0),
			m_speed(1)
		{
		}

		inline bcFLOAT bci_animation_job::update_time(const core_platform::bc_clock::update_param& p_clock, bcFLOAT p_animation_time)
		{
			m_local_time += p_clock.m_elapsed_second * m_speed;
			auto l_local_time = m_local_time / p_animation_time;
			l_local_time = l_local_time - std::floorf(l_local_time);

			return l_local_time;
		}

		inline bc_animation_skeleton& bci_animation_job::get_skeleton() noexcept
		{
			return *m_skeleton;
		}

		inline const bc_animation_skeleton& bci_animation_job::get_skeleton() const noexcept
		{
			return *m_skeleton;
		}

		inline bcFLOAT bci_animation_job::get_local_time() const noexcept
		{
			return m_local_time;
		}

		inline void bci_animation_job::set_local_time(bcFLOAT p_time) noexcept
		{
			m_local_time = p_time;
		}

		inline void bci_animation_job::set_speed(bcFLOAT p_speed) noexcept
		{
			m_speed = p_speed;
		}

		inline void bci_animation_job::reset_local_time() noexcept
		{
			m_local_time = 0;
		}
	}	
}