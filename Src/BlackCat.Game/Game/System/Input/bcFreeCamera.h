// [06/30/2016 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Platform/bcEvent.h"
#include "Game/System/Input/bcCamera.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_free_camera : public bc_perspective_camera
		{
		public:
			bc_free_camera(bcUINT16 p_back_buffer_width, 
				bcUINT16 p_back_buffer_height,
				bcFLOAT p_height_fov,
				bcFLOAT p_near_clip,
				bcFLOAT p_far_clip,
				bcFLOAT p_move_speed = 25,
				bcFLOAT p_rotate_speed = 1);

			bc_free_camera(bc_free_camera&& p_other) noexcept;

			~bc_free_camera() = default;

			bc_free_camera& operator=(bc_free_camera&& p_other) noexcept;

			void update(const core_platform::bc_clock::update_param& p_clock_update_param) noexcept override;

		private:
			bool _on_event(core::bc_ievent& p_event) noexcept;

			bool _on_pointing(platform::bc_app_event_pointing& p_pointing_event) noexcept;
			
			bool _on_key(platform::bc_app_event_key& p_key_event) noexcept;
			
			bcFLOAT m_move_speed;
			bcFLOAT m_rotate_speed;

			bool m_w_pressed;
			bool m_s_pressed;
			bool m_a_pressed;
			bool m_d_pressed;
			bool m_e_pressed;
			bool m_q_pressed;
			bool m_shift_pressed;
			bool m_ctrl_pressed;
			bool m_rmb_pressed;
			bcINT16 m_dx;
			bcINT16 m_dy;

			core::bc_event_listener_handle m_key_listener_handle;
			core::bc_event_listener_handle m_pointing_listener_handle;
		};
	}
}