// [12/30/2020 MRB]

#pragma once

#include "Game/Object/Animation/bcSkinnedAnimation.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_animation_manager
		{
		public:
			bc_animation_manager();

			bc_animation_manager(bc_animation_manager&&) noexcept;

			~bc_animation_manager();

			bc_animation_manager& operator=(bc_animation_manager&&) noexcept;
		};
	}
}