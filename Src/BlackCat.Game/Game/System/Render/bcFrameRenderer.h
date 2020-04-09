// [02/28/2020 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Concurrency/bcTask.h"
#include "Game/bcExport.h"
#include "Game/System/Render/bcRenderThreadManager.h"
#include "Game/System/Render/Pass/bcRenderPassManager.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_frame_renderer
		{
		public:
			bc_frame_renderer(bc_render_thread_manager& p_thread_manager, bc_render_pass_manager& p_render_pass_manager) noexcept;

			bc_frame_renderer(bc_frame_renderer&&) noexcept;

			~bc_frame_renderer();

			bc_frame_renderer& operator=(bc_frame_renderer&&) noexcept;

			void update(const core_platform::bc_clock::update_param& p_update_param);

			core::bc_task<void> render(const core_platform::bc_clock::update_param& p_update_param);

		private:
			bc_render_thread_manager& m_thread_manager;
			bc_render_pass_manager& m_bc_render_pass_manager;
		};
	}
}