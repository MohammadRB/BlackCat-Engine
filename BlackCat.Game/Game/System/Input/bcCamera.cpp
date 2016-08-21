// [06/29/2016 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Input/bcCamera.h"

namespace black_cat
{
	namespace game
	{
		// -- bc_icamera --------------------------------------------------------------------------------

		bc_icamera::bc_icamera(bcFLOAT p_near_clip, bcFLOAT p_far_clip) noexcept
			: m_near(p_near_clip),
			m_far(p_far_clip),
			m_position(graphic::bc_vector3f(0, 0, 0)),
			m_lookat(graphic::bc_vector3f(0, 0, 1))
		{
			create_view_matrix();
		}

		bc_icamera::~bc_icamera()
		{
		}

		void bc_icamera::set_far_clip(bcFLOAT p_far_clip) noexcept
		{
			m_far = p_far_clip;
			create_projection_matrix();
		}

		void bc_icamera::set_near_clip(bcFLOAT p_near_clip) noexcept
		{
			m_near = p_near_clip;
			create_projection_matrix();
		}

		void bc_icamera::set_position(graphic::bc_vector3f p_position) noexcept
		{
			m_position = p_position;
			create_view_matrix();
		}

		void bc_icamera::set_lookat(graphic::bc_vector3f p_lookat) noexcept
		{
			m_lookat = p_lookat;
			create_view_matrix();
		}

		graphic::bc_vector3f bc_icamera::get_forward() const noexcept
		{
			return get_direction();
		}

		graphic::bc_vector3f bc_icamera::get_back() const noexcept
		{
			return get_forward() * -1;
		}

		graphic::bc_vector3f bc_icamera::get_up() const noexcept
		{
			auto l_direction = get_direction();

			auto l_right = l_direction.cross(graphic::bc_vector3f(0, 1, 0));
			auto l_up = l_direction.cross(l_right);

			l_up.normalize();

			return l_up;
		}

		graphic::bc_vector3f bc_icamera::get_down() const noexcept
		{
			return get_up() * -1;
		}

		graphic::bc_vector3f bc_icamera::get_right() const noexcept
		{
			auto l_direction = get_direction();
			auto l_right = graphic::bc_vector3f(0, 1, 0).cross(l_direction);

			l_right.normalize();

			return l_right;
		}

		graphic::bc_vector3f bc_icamera::get_left() const noexcept
		{
			return get_right() * -1;
		}

		graphic::bc_vector3f bc_icamera::get_direction() const noexcept
		{
			auto l_direction = (m_lookat - m_position);
			l_direction.normalize();

			return l_direction;
		}

		void bc_icamera::set_direction(graphic::bc_vector3f p_direction) noexcept
		{
			p_direction.normalize();
			m_lookat = m_position + p_direction;

			create_view_matrix();
		}

		void bc_icamera::look_at(const graphic::bc_vector3f& p_position, const graphic::bc_vector3f& p_lookat, const graphic::bc_vector3f& up)
		{
			m_position = p_position;
			m_lookat = p_lookat;

			create_view_matrix();
		}

		void bc_icamera::create_view_matrix(const graphic::bc_vector3f& p_up)
		{
			m_view = graphic::bc_matrix4f::look_at_lh_matrix(m_position, m_lookat, p_up);
		}

		// -- bc_orthographic_camera --------------------------------------------------------------------------------

		bc_orthographic_camera::bc_orthographic_camera(bcFLOAT p_min_x,
			bcFLOAT p_min_y,
			bcFLOAT p_max_x,
			bcFLOAT p_max_y,
			bcFLOAT p_near_clip,
			bcFLOAT p_far_clip)
			: bc_icamera(p_far_clip, p_near_clip), 
			m_min_x(p_min_x),
			m_max_x(p_min_y),
			m_min_y(p_max_x),
			m_max_y(p_max_y)
		{
			create_projection_matrix();
		}

		void bc_orthographic_camera::set_min_x(bcFLOAT p_min_x) noexcept
		{
			m_min_x = p_min_x;
			create_projection_matrix();
		}

		void bc_orthographic_camera::set_max_x(bcFLOAT p_max_x) noexcept
		{
			m_max_x = p_max_x;
			create_projection_matrix();
		}

		void bc_orthographic_camera::set_min_y(bcFLOAT p_min_y) noexcept
		{
			m_min_y = p_min_y;
			create_projection_matrix();
		}

		void bc_orthographic_camera::set_max_y(bcFLOAT p_max_y) noexcept
		{
			m_max_y = p_max_y;
			create_projection_matrix();
		}

		void bc_orthographic_camera::set_projection(bcUINT16 p_back_buffer_width, bcUINT16 p_back_buffer_height, bcFLOAT p_near_clip, bcFLOAT p_far_clip) noexcept
		{
			set_near_clip(p_near_clip);
			set_far_clip(p_far_clip);

			m_min_x = 0;
			m_max_x = p_back_buffer_width;
			m_min_y = 0;
			m_max_y = p_back_buffer_height;

			create_projection_matrix();
		}

		void bc_orthographic_camera::create_projection_matrix()
		{
			set_projection(graphic::bc_matrix4f::orthographic_lh_matrix(get_near_clip(), get_far_clip(), m_max_x - m_min_x, m_max_y - m_min_y));
		}

		// -- bc_perspective_camera --------------------------------------------------------------------------------

		bc_perspective_camera::bc_perspective_camera(bcFLOAT p_aspect_ratio, 
			bcFLOAT p_field_of_view, 
			bcFLOAT p_near_clip, 
			bcFLOAT p_far_clip)
			: bc_icamera(p_near_clip, p_far_clip),
			m_aspect_ratio(p_aspect_ratio),
			m_field_of_view(p_field_of_view)
		{
			create_projection_matrix();
		}

		void bc_perspective_camera::set_aspect_ratio(bcFLOAT p_aspect_ratio) noexcept
		{
			m_aspect_ratio = p_aspect_ratio;
			create_projection_matrix();
		}

		void bc_perspective_camera::set_field_of_view(bcFLOAT p_field_of_view) noexcept
		{
			m_field_of_view = p_field_of_view;
			create_projection_matrix();
		}

		void bc_perspective_camera::set_projection(bcUINT16 p_back_buffer_width, bcUINT16 p_back_buffer_height, bcFLOAT p_near_clip, bcFLOAT p_far_clip) noexcept
		{
			set_near_clip(p_near_clip);
			set_far_clip(p_far_clip);
			m_aspect_ratio = bcFLOAT(p_back_buffer_width) / p_back_buffer_height;

			create_projection_matrix();
		}

		void bc_perspective_camera::create_projection_matrix()
		{
			set_projection(graphic::bc_matrix4f::perspective_fov_lh_matrix(m_field_of_view, m_aspect_ratio, get_near_clip(), get_far_clip()));
		}
	}
}