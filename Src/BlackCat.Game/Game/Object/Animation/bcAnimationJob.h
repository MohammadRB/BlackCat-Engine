// [12/31/2020 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Game/Object/Animation/bcAnimationSkeleton.h"

namespace black_cat
{
	namespace game
	{
		enum class bc_animation_play_mode : bcUBYTE
		{
			once_disable,
			loop
		};
		
		class bci_animation_job
		{
		public:
			virtual ~bci_animation_job() = 0;

			bc_animation_skeleton& get_skeleton() noexcept;

			const bc_animation_skeleton& get_skeleton() const noexcept;

			bool get_enabled() const noexcept;

			virtual void set_enabled(bool p_enabled) noexcept;

			virtual bool run(const platform::bc_clock::update_param& p_clock) = 0;

		protected:
			explicit bci_animation_job(bc_animation_skeleton& p_skeleton);

			bci_animation_job(bci_animation_job&&) noexcept = default;

			bci_animation_job& operator=(bci_animation_job&&) noexcept = default;
			
			bc_animation_skeleton* m_skeleton;
			bool m_enabled;
		};

		inline bci_animation_job::~bci_animation_job() = default;

		inline bci_animation_job::bci_animation_job(bc_animation_skeleton& p_skeleton)
			: m_skeleton(&p_skeleton),
			m_enabled(true)
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

		inline bool bci_animation_job::get_enabled() const noexcept
		{
			return m_enabled;
		}

		inline void bci_animation_job::set_enabled(bool p_enabled) noexcept
		{
			m_enabled = p_enabled;
		}
	}	
}