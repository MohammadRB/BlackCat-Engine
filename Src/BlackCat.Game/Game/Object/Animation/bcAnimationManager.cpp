// [01/01/2021 MRB]

#include "Game/GamePCH.h"

#include "Core/Concurrency/bcConcurrency.h"
#include "Game/Object/Animation/bcAnimationManager.h"

namespace black_cat
{
	namespace game
	{
		bc_animation_manager::bc_animation_manager() = default;

		bc_animation_manager::bc_animation_manager(bc_animation_manager&& p_other) noexcept
			: m_jobs(std::move(p_other.m_jobs))
		{
		}

		bc_animation_manager::~bc_animation_manager() = default;

		bc_animation_manager& bc_animation_manager::operator=(bc_animation_manager&& p_other) noexcept
		{
			m_jobs = std::move(p_other.m_jobs);
			
			return *this;
		}

		void bc_animation_manager::schedule_job(bci_animation_job& p_job)
		{
			{
				core_platform::bc_hybrid_mutex_guard l_lock(m_jobs_lock, core_platform::bc_lock_operation::light);
				
				m_jobs.push_back(&p_job);
			}
		}

		void bc_animation_manager::run_scheduled_jobs(const core_platform::bc_clock::update_param& p_clock)
		{
			{
				core_platform::bc_hybrid_mutex_guard l_lock(m_jobs_lock, core_platform::bc_lock_operation::heavy);

				core::bc_concurrency::concurrent_for_each
				(
					std::begin(m_jobs),
					std::end(m_jobs),
					[=](bci_animation_job* p_job)
					{
						const auto l_result = p_job->run(p_clock);
						BC_ASSERT(l_result);
					}
				);

				m_jobs.clear();
			}
		}

		core::bc_task<void> bc_animation_manager::run_scheduled_jobs_async(const core_platform::bc_clock::update_param& p_clock)
		{
			auto l_task = core::bc_concurrency::start_task
			(
				core::bc_delegate<void()>
				(
					[=, &p_clock]()
					{
						run_scheduled_jobs(p_clock);
					}
				)
			);

			return l_task;
		}
	}	
}