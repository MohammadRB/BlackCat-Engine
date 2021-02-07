// [12/30/2020 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Container/bcVector.h"
#include "Game/Object/Animation/bcAnimationJob.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace core
	{
		template<typename T>
		class bc_task;
	}
	
	namespace game
	{
		class BC_GAME_DLL bc_animation_manager
		{
		public:
			bc_animation_manager();

			bc_animation_manager(bc_animation_manager&& p_other) noexcept;

			~bc_animation_manager();

			bc_animation_manager& operator=(bc_animation_manager&& p_other) noexcept;

			void schedule_job(bci_animation_job& p_job);

			void run_scheduled_jobs(const core_platform::bc_clock::update_param& p_clock);
			
			core::bc_task<void> run_scheduled_jobs_async(const core_platform::bc_clock::update_param& p_clock);
			
		private:
			core_platform::bc_hybrid_mutex m_jobs_lock;
			core::bc_vector_movable<bci_animation_job*> m_jobs;
		};
	}
}