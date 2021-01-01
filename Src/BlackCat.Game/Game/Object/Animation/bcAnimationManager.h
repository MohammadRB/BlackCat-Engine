// [12/30/2020 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Container/bcVector.h"
#include "Game/Object/Animation/bcAnimationJob.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_animation_manager
		{
		public:
			bc_animation_manager() = default;

			bc_animation_manager(bc_animation_manager&& p_other) noexcept = default;

			~bc_animation_manager() = default;

			bc_animation_manager& operator=(bc_animation_manager&& p_other) noexcept = default;

			void schedule_job(bci_animation_job& p_job);

			void run_animation_jobs(core_platform::bc_clock::update_param& p_clock);
			
		private:
			core::bc_vector_movable<bci_animation_job*> m_jobs;
		};
	}
}