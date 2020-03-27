// [02/28/2020 MRB]

#pragma once

#include "Core/Event/bcEventManager.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_frame_render_state
		{
		public:
			bc_frame_render_state() noexcept;

			bc_frame_render_state(bc_frame_render_state&&) noexcept;

			~bc_frame_render_state();

			bc_frame_render_state& operator=(bc_frame_render_state&&) noexcept;

		private:
			
		};
	}
}