// [30/12/2020 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Container/bcVector.h"
#include "Game/System/Animation/bcAnimationJob.h"
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
		class BC_GAME_DLL bc_animation_system : public core::bc_initializable<>
		{
		public:
			bc_animation_system();

			bc_animation_system(bc_animation_system&& p_other) noexcept;

			~bc_animation_system();

			bc_animation_system& operator=(bc_animation_system&& p_other) noexcept;

			void schedule_job(bci_animation_job& p_job);

			void run_scheduled_jobs(const platform::bc_clock::update_param& p_clock);
			
			core::bc_task<void> run_scheduled_jobs_async(const platform::bc_clock::update_param& p_clock);

		private:
			void _initialize() override;

			void _destroy() override;

			platform::bc_hybrid_mutex m_jobs_lock;
			core::bc_vector_movable<bci_animation_job*> m_jobs;
		};
	}
}