// [10/29/2021 MRB]

#pragma once

#include "Game/Object/Animation/bcAnimationJob.h"

namespace black_cat
{
	namespace game
	{
		template<typename TJob>
		class bc_execute_once_animation_job : public TJob
		{
		public:
			bc_execute_once_animation_job(TJob&& p_job)
				: TJob(std::move(p_job)),
				m_total_elapsed(0)
			{
				static_assert(std::is_base_of_v<bci_animation_job, TJob>, "TJob must inherit from bci_animation_job");
			}

			bc_execute_once_animation_job(bc_execute_once_animation_job&&) noexcept = default;

			~bc_execute_once_animation_job() = default;

			bc_execute_once_animation_job& operator=(bc_execute_once_animation_job&&) noexcept = default;

			bool run(const core_platform::bc_clock::update_param& p_clock) override
			{
				if(m_total_elapsed == p_clock.m_total_elapsed)
				{
					return true;
				}

				m_total_elapsed = p_clock.m_total_elapsed;
				return TJob::run(p_clock);
			}

		private:
			core_platform::bc_clock::big_delta_time m_total_elapsed;
		};
	}
}