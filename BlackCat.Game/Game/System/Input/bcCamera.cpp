// [06/29/2016 MRB]

#include "Game/GamePCH.h"
#include "Core/Event/bcEventManager.h"
#include "Platform/bcEvent.h"
#include "GraphicImp/bcRenderApiInfo.h"
#include "Game/System/Input/bcCamera.h"

namespace black_cat
{
	namespace game
	{
		// -- bc_icamera --------------------------------------------------------------------------------

		bc_icamera::bc_icamera(bcFLOAT p_near_clip, bcFLOAT p_far_clip) noexcept
			: m_near(p_near_clip),
			m_far(p_far_clip),
			m_position(core::bc_vector3f(0, 0, 0)),
			m_lookat(core::bc_vector3f(0, 0, 1))
		{
			create_view_matrix();

			auto* l_event_manager = core::bc_get_service<core::bc_event_manager>();

			m_key_listener_handle = l_event_manager->register_event_listener< platform::bc_app_event_key >
			(
				core::bc_event_manager::delegate_type(this, &bc_icamera::on_key)
			);
			m_pointing_listener_handle = l_event_manager->register_event_listener< platform::bc_app_event_pointing >
			(
				core::bc_event_manager::delegate_type(this, &bc_icamera::on_pointing)
			);
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

		void bc_icamera::set_position(core::bc_vector3f p_position) noexcept
		{
			m_position = p_position;
			create_view_matrix();
		}

		void bc_icamera::set_lookat(core::bc_vector3f p_lookat) noexcept
		{
			m_lookat = p_lookat;
			create_view_matrix();
		}

		void bc_icamera::set_position_lookat(const core::bc_vector3f& p_position, const core::bc_vector3f& p_lookat)
		{
			set_position(p_position);
			set_lookat(p_lookat);
		}

		core::bc_vector3f bc_icamera::get_forward() const noexcept
		{
			return get_direction();
		}

		core::bc_vector3f bc_icamera::get_back() const noexcept
		{
			return get_forward() * -1;
		}

		core::bc_vector3f bc_icamera::get_up() const noexcept
		{
			auto l_direction = get_direction();

			auto l_right = core::bc_vector3f(0, 1, 0).cross(l_direction);
			auto l_up = l_direction.cross(l_right);

			l_up.normalize();

			return l_up;
		}

		core::bc_vector3f bc_icamera::get_down() const noexcept
		{
			return get_up() * -1;
		}

		core::bc_vector3f bc_icamera::get_right() const noexcept
		{
			auto l_direction = get_direction();
			auto l_right = core::bc_vector3f(0, 1, 0).cross(l_direction);

			l_right.normalize();

			return l_right;
		}

		core::bc_vector3f bc_icamera::get_left() const noexcept
		{
			return get_right() * -1;
		}

		core::bc_vector3f bc_icamera::project_clip_point_to_3d_ray(bcUINT16 p_screen_width, bcUINT16 p_screen_height, bcUINT16 p_left, bcUINT16 p_top) const
		{
			auto l_view = get_view();
			auto l_proj = get_projection();

			// Compute picking ray in view space.
			bcFLOAT l_vx = (+2.0f * p_left / p_screen_width - 1.0f) / l_proj(0, 0);
			bcFLOAT l_vy = (-2.0f * p_top / p_screen_height + 1.0f) / l_proj(1, 1);

			// Ray definition in view space.
			auto l_ray_origin = core::bc_vector4f(0.0f, 0.0f, 0.0f, 1.0f);
			auto l_ray_dir = core::bc_vector4f(l_vx, l_vy, 1.0f, 0.0f);

			// Transform ray to world space
			auto l_inv_view = l_view.inverse();

			l_ray_origin = l_inv_view * l_ray_origin;
			l_ray_dir = l_inv_view * l_ray_dir;

			// Make the ray direction unit length for the intersection tests.
			auto l_dir = l_ray_dir.xyz();
			l_dir.normalize();

			return l_dir;

			/*bcFLOAT l_left = static_cast<bcFLOAT>(p_left) / p_screen_width;
			bcFLOAT l_botton = 1 - (static_cast<bcFLOAT>(p_top) / p_screen_height);
			extend l_camera_extends;

			get_extend_points(l_camera_extends);

			bcFLOAT l_near_width = std::abs((l_camera_extends[0] - l_camera_extends[3]).magnitude());
			bcFLOAT l_near_height = std::abs((l_camera_extends[0] - l_camera_extends[1]).magnitude());
			bcFLOAT l_far_width = std::abs((l_camera_extends[4] - l_camera_extends[7]).magnitude());
			bcFLOAT l_far_height = std::abs((l_camera_extends[4] - l_camera_extends[5]).magnitude());

			bcFLOAT l_near_width_ratio = l_near_width * l_left;
			bcFLOAT l_near_height_ratio = l_near_height * l_botton;
			bcFLOAT l_far_width_ratio = l_far_width * l_left;
			bcFLOAT l_far_height_ratio = l_far_height * l_botton;

			auto l_near_point = l_camera_extends[0];
			auto l_far_point = l_camera_extends[4];

			l_near_point += get_right() * l_near_width_ratio;
			l_near_point += get_up() * l_near_height_ratio;
			l_far_point += get_right() * l_far_width_ratio;
			l_far_point += get_up() * l_far_height_ratio;

			auto l_ray = l_far_point - l_near_point;
			l_ray.normalize();

			return l_ray;*/
		}

		core::bc_vector3f bc_icamera::get_direction() const noexcept
		{
			auto l_direction = (m_lookat - m_position);
			l_direction.normalize();

			return l_direction;
		}

		void bc_icamera::set_direction(core::bc_vector3f p_direction) noexcept
		{
			p_direction.normalize();
			m_lookat = m_position + p_direction;

			create_view_matrix();
		}

		void bc_icamera::look_at(const core::bc_vector3f& p_position, const core::bc_vector3f& p_lookat, const core::bc_vector3f& up)
		{
			m_position = p_position;
			m_lookat = p_lookat;

			create_view_matrix();
		}

		void bc_icamera::create_view_matrix(const core::bc_vector3f& p_up)
		{
			if(graphic::bc_render_api_info::is_left_handed())
			{
				m_view = core::bc_matrix4f::look_at_matrix_lh(m_position, m_lookat, p_up);
			}
			else
			{
				m_view = core::bc_matrix4f::look_at_matrix_rh(m_position, m_lookat, p_up);
			}
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
			core::bc_matrix4f l_proj;

			if(graphic::bc_render_api_info::is_left_handed())
			{
				l_proj = core::bc_matrix4f::orthographic_matrix_lh(get_near_clip(), get_far_clip(), m_max_x - m_min_x, m_max_y - m_min_y);
			}
			else
			{
				l_proj = core::bc_matrix4f::orthographic_matrix_rh(get_near_clip(), get_far_clip(), m_max_x - m_min_x, m_max_y - m_min_y);
			}

			set_projection(l_proj);
		}

		// -- bc_perspective_camera --------------------------------------------------------------------------------

		bc_perspective_camera::bc_perspective_camera(bcFLOAT p_aspect_ratio, 
			bcFLOAT p_height_fov,
			bcFLOAT p_near_clip, 
			bcFLOAT p_far_clip)
			: bc_icamera(p_near_clip, p_far_clip),
			m_aspect_ratio(p_aspect_ratio),
			m_field_of_view(p_height_fov)
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

		void bc_perspective_camera::get_extend_points(extend& p_points) const
		{
			auto l_fov_tan = 2 * std::tanf(m_field_of_view / 2);
			auto l_near_clip_height = l_fov_tan * get_near_clip();
			auto l_near_clip_width = l_near_clip_height * m_aspect_ratio;
			auto l_far_clip_height = l_fov_tan * get_far_clip();
			auto l_far_clip_width = l_far_clip_height * m_aspect_ratio;

			auto l_near_clip_center = get_position() + get_forward() * get_near_clip();
			auto l_far_clip_center = get_position() + get_forward() * get_far_clip();

			p_points[0] = l_near_clip_center + get_left() * (l_near_clip_width / 2) + get_down() * (l_near_clip_height / 2);
			p_points[1] = l_near_clip_center + get_left() * (l_near_clip_width / 2) + get_up() * (l_near_clip_height / 2);
			p_points[2] = l_near_clip_center + get_right() * (l_near_clip_width / 2) + get_up() * (l_near_clip_height / 2);
			p_points[3] = l_near_clip_center + get_right() * (l_near_clip_width / 2) + get_down() * (l_near_clip_height / 2);
			p_points[4] = l_far_clip_center + get_left() * (l_far_clip_width / 2) + get_down() * (l_far_clip_height / 2);
			p_points[5] = l_far_clip_center + get_left() * (l_far_clip_width / 2) + get_up() * (l_far_clip_height / 2);
			p_points[6] = l_far_clip_center + get_right() * (l_far_clip_width / 2) + get_up() * (l_far_clip_height / 2);
			p_points[7] = l_far_clip_center + get_right() * (l_far_clip_width / 2) + get_down() * (l_far_clip_height / 2);
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
			core::bc_matrix4f l_proj;

			if (graphic::bc_render_api_info::is_left_handed())
			{
				l_proj = core::bc_matrix4f::perspective_fov_matrix_lh(m_field_of_view, m_aspect_ratio, get_near_clip(), get_far_clip());
			}
			else
			{
				l_proj = core::bc_matrix4f::perspective_fov_matrix_rh(m_field_of_view, m_aspect_ratio, get_near_clip(), get_far_clip());
			}

			set_projection(l_proj);
		}
	}
}