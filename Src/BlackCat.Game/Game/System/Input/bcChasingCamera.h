// [11/03/2021 MRB]

#pragma once

#include "Game/System/Input/bcCamera.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_chasing_camera : public bc_perspective_camera
		{
		public:
			bc_chasing_camera(bcUINT16 p_back_buffer_width,
				bcUINT16 p_back_buffer_height,
				bcFLOAT p_height_fov,
				bcFLOAT p_near_clip,
				bcFLOAT p_far_clip) noexcept;

			bc_chasing_camera(bc_chasing_camera&&) noexcept = default;

			~bc_chasing_camera() override = default;

			bc_chasing_camera& operator=(bc_chasing_camera&&) noexcept = default;

			void update(const core::bc_vector3f& p_position, const core::bc_vector3f& p_look_at) noexcept;

			void update(const platform::bc_clock::update_param& p_clock) noexcept override;
		};
	}
}