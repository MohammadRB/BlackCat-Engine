// [01/01/2021 MRB]

#include "Game/GamePCH.h"

#include "Core/Concurrency/bcConcurrency.h"
#include "Game/Object/Animation/bcAnimationManager.h"

namespace black_cat
{
	namespace game
	{
		bc_animation_manager::bc_animation_manager()
			: m_current_clock(0, 0)
		{
		}

		bc_animation_manager::bc_animation_manager(bc_animation_manager&& p_other) noexcept
			: m_jobs(std::move(p_other.m_jobs)),
			m_current_clock(0, 0)
		{
		}

		bc_animation_manager& bc_animation_manager::operator=(bc_animation_manager&& p_other) noexcept
		{
			m_jobs = std::move(p_other.m_jobs);
			m_current_clock = p_other.m_current_clock;
			
			return *this;
		}

		void bc_animation_manager::schedule_job(bci_animation_job& p_job)
		{
			{
				core_platform::bc_hybrid_mutex_guard l_lock(m_jobs_lock, core_platform::bc_lock_operation::light);
				
				m_jobs.push_back(&p_job);
			}
		}

		core::bc_task<void> bc_animation_manager::run_scheduled_jobs(const core_platform::bc_clock::update_param& p_clock)
		{
			m_current_clock = p_clock;
			
			auto l_task = core::bc_concurrency::start_task
			(
				core::bc_delegate< void() >
				(
					[=]()
					{
						{
							core_platform::bc_hybrid_mutex_guard l_lock(m_jobs_lock, core_platform::bc_lock_operation::heavy);

							const auto l_num_thread = std::min(core::bc_concurrency::worker_count(), m_jobs.size() / 1U + 1);
							core::bc_concurrency::concurrent_for_each
							(
								l_num_thread,
								std::begin(m_jobs),
								std::end(m_jobs),
								[=](bci_animation_job* p_job)
								{
									BC_ASSERT(p_job->run(m_current_clock));
								}
							);

							m_jobs.clear();
						}
					}
				)
			);

			return l_task;
		}
	}	
}