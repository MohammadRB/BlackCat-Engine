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
			: bc_orthographic_camera(p_width, p_height, p_near_clip, p_far_clip),
			m_offset_x(0),
			m_offset_y(0)
		{
			set_look_at(p_position, p_look_at);
		}

		bc_cascaded_shadow_map_camera(bc_cascaded_shadow_map_camera&& p_other) noexcept = default;

		~bc_cascaded_shadow_map_camera() = default;
		
		bc_cascaded_shadow_map_camera& operator=(bc_cascaded_shadow_map_camera&& p_other) noexcept = default;

		void set_offset(bcFLOAT p_offset_x, bcFLOAT p_offset_y) noexcept
		{
			m_offset_x = p_offset_x;
			m_offset_y = p_offset_y;

			bci_camera::set_projection(get_screen_width(), get_screen_height());
		}
		
		void update(const core_platform::bc_clock::update_param& p_clock_update_param) noexcept override
		{
		}

	protected:
		core::bc_matrix4f create_projection_matrix() noexcept override
		{
			auto l_proj = bc_orthographic_camera::create_projection_matrix();
			l_proj(3, 0) += m_offset_x;
			l_proj(3, 1) += m_offset_y;

			return l_proj;
		}

	private:
		bcFLOAT m_offset_x;
		bcFLOAT m_offset_y;
	};
}