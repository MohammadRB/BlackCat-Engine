// [11/03/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Input/bcChasingCamera.h"

namespace black_cat
{
	namespace game
	{
		bc_chasing_camera::bc_chasing_camera(bcUINT16 p_back_buffer_width,
			bcUINT16 p_back_buffer_height,
			bcFLOAT p_height_fov,
			bcFLOAT p_near_clip,
			bcFLOAT p_far_clip) noexcept
			: bc_perspective_camera
			(
				p_back_buffer_width, 
				p_back_buffer_height,
				p_height_fov,
				p_near_clip,
				p_far_clip
			)
		{
		}

		void bc_chasing_camera::update(const core::bc_vector3f& p_position, const core::bc_vector3f& p_look_at) noexcept
		{
			set_look_at(p_position, p_look_at);
		}

		void bc_chasing_camera::update(const platform::bc_clock::update_param& p_clock) noexcept
		{
		}
	}
}