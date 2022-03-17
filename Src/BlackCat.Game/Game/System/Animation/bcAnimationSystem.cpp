// [01/01/2021 MRB]

#include "Game/GamePCH.h"

#include "Core/Concurrency/bcConcurrency.h"
#include "Game/System/Animation/bcAnimationSystem.h"

namespace black_cat
{
	namespace game
	{
		bc_animation_system::bc_animation_system() = default;

		bc_animation_system::bc_animation_system(bc_animation_system&& p_other) noexcept
			: m_jobs(std::move(p_other.m_jobs))
		{
		}

		bc_animation_system::~bc_animation_system() = default;

		bc_animation_system& bc_animation_system::operator=(bc_animation_system&& p_other) noexcept
		{
			m_jobs = std::move(p_other.m_jobs);
			
			return *this;
		}

		void bc_animation_system::schedule_job(bci_animation_job& p_job)
		{
			{
				platform::bc_hybrid_mutex_guard l_lock(m_jobs_lock, platform::bc_lock_operation::light);
				
				m_jobs.push_back(&p_job);
			}
		}

		void bc_animation_system::run_scheduled_jobs(const platform::bc_clock::update_param& p_clock)
		{
			{
				platform::bc_hybrid_mutex_guard l_lock(m_jobs_lock, platform::bc_lock_operation::heavy);

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

		core::bc_task<void> bc_animation_system::run_scheduled_jobs_async(const platform::bc_clock::update_param& p_clock)
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

		void bc_animation_system::_initialize()
		{
		}

		void bc_animation_system::_destroy()
		{
		}
	}	
}