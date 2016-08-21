// [06/30/2016 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "PlatformImp/Application/bcPointingDevice.h"
#include "PlatformImp/Application/bcKeyDevice.h"
#include "Game/System/Input/bcCamera.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_free_camera : public bc_perspective_camera
		{
		public:
			bc_free_camera(bcFLOAT p_aspect_ratio,
				bcFLOAT p_field_of_view,
				bcFLOAT p_near_clip,
				bcFLOAT p_far_clip,
				bcFLOAT p_move_speed = 2,
				bcFLOAT p_rotate_speed = 2);

			bc_free_camera(const bc_free_camera&) = default;

			~bc_free_camera() = default;

			bc_free_camera& operator=(const bc_free_camera&) = default;

			void update(core_platform::bc_clock::update_param p_clock_update_param,
				const platform::bc_pointing_device& p_pointing_device,
				const platform::bc_key_device& p_key_device) override;

		protected:

		private:
			bcFLOAT m_move_speed;
			bcFLOAT m_rotate_speed;
		};
	}
}
