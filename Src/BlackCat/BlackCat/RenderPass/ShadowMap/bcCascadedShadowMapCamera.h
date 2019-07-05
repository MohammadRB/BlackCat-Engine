// [07/04/2019 MRB]

#pragma once

#include "Game/System/Input/bcCamera.h"

namespace black_cat
{
	class bc_cascaded_shadow_map_camera : public game::bc_orthographic_camera
	{
	public:
		bc_cascaded_shadow_map_camera(const core::bc_vector3f& p_position,
			const core::bc_vector3f& p_look_at,
			bcSIZE p_width,
			bcSIZE p_height,
			bcFLOAT p_near_clip,
			bcFLOAT p_far_clip)
			: bc_orthographic_camera(p_width, p_height, p_near_clip, p_far_clip)
		{
			set_look_at(p_position, p_look_at);
		}

		bc_cascaded_shadow_map_camera(bc_cascaded_shadow_map_camera&& p_other) noexcept
			: bc_orthographic_camera(std::move(p_other))
		{
		}

		bc_cascaded_shadow_map_camera& operator=(bc_cascaded_shadow_map_camera&& p_other) noexcept
		{
			bc_orthographic_camera::operator=(std::move(p_other));
			return *this;
		}

		void update(core_platform::bc_clock::update_param p_clock_update_param,
			const platform::bc_pointing_device& p_pointing_device,
			const platform::bc_key_device& p_key_device) noexcept override
		{
		}

	protected:
		bool on_key(core::bc_ievent& p_key_event) noexcept override
		{
			return false;
		}

		bool on_pointing(core::bc_ievent& p_pointing_event) noexcept override
		{
			return false;
		}
	};
}