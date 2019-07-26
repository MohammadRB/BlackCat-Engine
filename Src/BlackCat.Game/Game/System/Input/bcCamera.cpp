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
		// -- bc_icamera ------------------------------------------------------------------------------------------

		bc_icamera::bc_icamera(bcUINT16 p_back_buffer_width, bcUINT16 p_back_buffer_height, bcFLOAT p_near_clip, bcFLOAT p_far_clip) noexcept
			: m_screen_width(p_back_buffer_width),
			m_screen_height(p_back_buffer_height),
			m_near(p_near_clip),
			m_far(p_far_clip),
			m_position(core::bc_vector3f(0, 0, 0)),
			m_look_at(core::bc_vector3f(0, 0, 1))
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

		bc_icamera::bc_icamera(bc_icamera&& p_other) noexcept
			: m_screen_width(p_other.m_screen_width),
			m_screen_height(p_other.m_screen_height),
			m_near(p_other.m_near),
			m_far(p_other.m_far),
			m_position(p_other.m_position),
			m_look_at(p_other.m_look_at),
			m_view(p_other.m_view),
			m_projection(p_other.m_projection),
			m_key_listener_handle(std::move(p_other.m_key_listener_handle)),
			m_pointing_listener_handle(std::move(p_other.m_pointing_listener_handle))
		{
			m_key_listener_handle.reassign(core::bc_event_manager::delegate_type(this, &bc_icamera::on_key));
			m_pointing_listener_handle.reassign(core::bc_event_manager::delegate_type(this, &bc_icamera::on_pointing));
		}

		bc_icamera& bc_icamera::operator=(bc_icamera&& p_other) noexcept
		{
			m_screen_width = p_other.m_screen_width;
			m_screen_height = p_other.m_screen_height;
			m_near = p_other.m_near;
			m_far = p_other.m_far;
			m_position = p_other.m_position;
			m_look_at = p_other.m_look_at;
			m_view = p_other.m_view;
			m_projection = p_other.m_projection;
			m_key_listener_handle = std::move(p_other.m_key_listener_handle);
			m_pointing_listener_handle = std::move(p_other.m_pointing_listener_handle);

			m_key_listener_handle.reassign(core::bc_event_manager::delegate_type(this, &bc_icamera::on_key));
			m_pointing_listener_handle.reassign(core::bc_event_manager::delegate_type(this, &bc_icamera::on_pointing));

			return *this;
		}

		bc_icamera::~bc_icamera() noexcept
		{
		}

		core::bc_vector3f bc_icamera::get_direction() const noexcept
		{
			auto l_direction = (m_look_at - m_position);
			l_direction.normalize();

			return l_direction;
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

		void bc_icamera::set_look_at(const core::bc_vector3f& p_position, const core::bc_vector3f& p_look_at, const core::bc_vector3f& p_up) noexcept
		{
			m_position = p_position;
			m_look_at = p_look_at;

			create_view_matrix(p_up);
		}

		void bc_icamera::set_projection(bcUINT16 p_back_buffer_width, bcUINT16 p_back_buffer_height) noexcept
		{
			set_projection(p_back_buffer_width, p_back_buffer_height, m_near, m_far);
		}

		core::bc_vector3f bc_icamera::project_clip_point_to_3d_ray(bcUINT16 p_screen_width, bcUINT16 p_screen_height, bcUINT16 p_left, bcUINT16 p_top) const noexcept
		{
			//auto l_view = get_view();
			//auto l_proj = get_projection();

			//// Compute picking ray in view space.
			//bcFLOAT l_vx = (+2.0f * p_left / p_screen_width - 1.0f) / l_proj(0, 0);
			//bcFLOAT l_vy = (-2.0f * p_top / p_screen_height + 1.0f) / l_proj(1, 1);

			//// Ray definition in view space.
			//auto l_ray_origin = core::bc_vector4f(0.0f, 0.0f, 0.0f, 1.0f);
			//auto l_ray_dir = core::bc_vector4f(l_vx, l_vy, 1.0f, 0.0f);

			//// Transform ray to world space
			//auto l_inv_view = l_view.inverse();

			//l_ray_origin = l_inv_view * l_ray_origin;
			//l_ray_dir = l_inv_view * l_ray_dir;

			//// Make the ray direction unit length for the intersection tests.
			//auto l_dir = l_ray_dir.xyz();
			//l_dir.normalize();

			//return l_dir;

			const bcFLOAT l_left = static_cast<bcFLOAT>(p_left) / p_screen_width;
			const bcFLOAT l_bottom = 1 - (static_cast<bcFLOAT>(p_top) / p_screen_height);
			extend l_camera_extends;

			get_extend_points(l_camera_extends);

			const bcFLOAT l_near_width = std::abs((l_camera_extends[0] - l_camera_extends[3]).magnitude());
			const bcFLOAT l_near_height = std::abs((l_camera_extends[0] - l_camera_extends[1]).magnitude());
			const bcFLOAT l_far_width = std::abs((l_camera_extends[4] - l_camera_extends[7]).magnitude());
			const bcFLOAT l_far_height = std::abs((l_camera_extends[4] - l_camera_extends[5]).magnitude());

			const bcFLOAT l_near_width_ratio = l_near_width * l_left;
			const bcFLOAT l_near_height_ratio = l_near_height * l_bottom;
			const bcFLOAT l_far_width_ratio = l_far_width * l_left;
			const bcFLOAT l_far_height_ratio = l_far_height * l_bottom;

			auto l_near_point = l_camera_extends[0];
			auto l_far_point = l_camera_extends[4];

			auto l_right_vec = l_camera_extends[3] - l_camera_extends[0];
			auto l_up_vec = l_camera_extends[1] - l_camera_extends[0];

			l_right_vec.normalize();
			l_up_vec.normalize();

			l_near_point += l_right_vec * l_near_width_ratio;
			l_near_point += l_up_vec * l_near_height_ratio;
			l_far_point += l_right_vec * l_far_width_ratio;
			l_far_point += l_up_vec * l_far_height_ratio;

			auto l_ray = l_far_point - l_near_point;
			l_ray.normalize();

			return l_ray;
		}
		
		void bc_icamera::create_view_matrix(const core::bc_vector3f& p_up) noexcept
		{
			if(graphic::bc_render_api_info::is_left_handed())
			{
				m_view = core::bc_matrix4f::look_at_matrix_lh(m_position, m_look_at, p_up);
			}
			else
			{
				m_view = core::bc_matrix4f::look_at_matrix_rh(m_position, m_look_at, p_up);
			}
		}

		// -- bc_orthographic_camera -------------------------------------------------------------------------------

		bc_orthographic_camera::bc_orthographic_camera(bcUINT16 p_back_buffer_width,
			bcUINT16 p_back_buffer_height,
			bcFLOAT p_near_clip,
			bcFLOAT p_far_clip) noexcept
			: bc_icamera(p_back_buffer_width, p_back_buffer_height, p_far_clip, p_near_clip)
		{
			set_projection(p_back_buffer_width, p_back_buffer_height, p_near_clip, p_far_clip);
		}

		bc_orthographic_camera::bc_orthographic_camera(bc_orthographic_camera&& p_other) noexcept
			: bc_icamera(std::move(p_other))
			, m_min_x(p_other.m_min_x),
			m_max_x(p_other.m_max_x),
			m_min_y(p_other.m_min_y),
			m_max_y(p_other.m_max_y)
		{
		}

		bc_orthographic_camera& bc_orthographic_camera::operator=(bc_orthographic_camera&& p_other) noexcept
		{
			bc_icamera::operator=(std::move(p_other));
			m_min_x = p_other.m_min_x;
			m_max_x = p_other.m_max_x;
			m_min_y = p_other.m_min_y;
			m_max_y = p_other.m_max_y;

			return *this;
		}

		void bc_orthographic_camera::get_extend_points(extend& p_points) const noexcept
		{
			const auto l_near_clip_height = m_max_y - m_min_y;
			const auto l_near_clip_width = m_max_x - m_min_x;
			const auto l_far_clip_height = l_near_clip_height;
			const auto l_far_clip_width = l_near_clip_width;

			const auto l_near_clip_center = get_position() + get_forward() * get_near_clip();
			const auto l_far_clip_center = get_position() + get_forward() * get_far_clip();

			p_points[0] = l_near_clip_center + get_left() * (l_near_clip_width / 2) + get_down() * (l_near_clip_height / 2);
			p_points[1] = l_near_clip_center + get_left() * (l_near_clip_width / 2) + get_up() * (l_near_clip_height / 2);
			p_points[2] = l_near_clip_center + get_right() * (l_near_clip_width / 2) + get_up() * (l_near_clip_height / 2);
			p_points[3] = l_near_clip_center + get_right() * (l_near_clip_width / 2) + get_down() * (l_near_clip_height / 2);
			p_points[4] = l_far_clip_center + get_left() * (l_far_clip_width / 2) + get_down() * (l_far_clip_height / 2);
			p_points[5] = l_far_clip_center + get_left() * (l_far_clip_width / 2) + get_up() * (l_far_clip_height / 2);
			p_points[6] = l_far_clip_center + get_right() * (l_far_clip_width / 2) + get_up() * (l_far_clip_height / 2);
			p_points[7] = l_far_clip_center + get_right() * (l_far_clip_width / 2) + get_down() * (l_far_clip_height / 2);
		}

		void bc_orthographic_camera::set_projection(bcUINT16 p_back_buffer_width, bcUINT16 p_back_buffer_height, bcFLOAT p_near_clip, bcFLOAT p_far_clip) noexcept
		{
			m_screen_width = p_back_buffer_width;
			m_screen_height = p_back_buffer_height;
			m_near = p_near_clip;
			m_far = p_far_clip;
			m_min_x = 0;
			m_max_x = p_back_buffer_width;
			m_min_y = 0;
			m_max_y = p_back_buffer_height;

			create_projection_matrix();
		}

		void bc_orthographic_camera::create_projection_matrix() noexcept
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

			m_projection = l_proj;
		}

		// -- bc_perspective_camera --------------------------------------------------------------------------------

		bc_perspective_camera::bc_perspective_camera(bcUINT16 p_back_buffer_width, 
			bcUINT16 p_back_buffer_height, 
			bcFLOAT p_height_fov, 
			bcFLOAT p_near_clip, 
			bcFLOAT p_far_clip) noexcept
			: bc_icamera(p_back_buffer_width, p_back_buffer_height, p_near_clip, p_far_clip)
		{
			set_projection(p_back_buffer_width, p_back_buffer_height, p_height_fov, p_near_clip, p_far_clip);
		}

		bc_perspective_camera::bc_perspective_camera(bc_perspective_camera&& p_other) noexcept
			: bc_icamera(std::move(p_other)),
			m_aspect_ratio(p_other.m_aspect_ratio),
			m_field_of_view(p_other.m_field_of_view)
		{
		}

		bc_perspective_camera& bc_perspective_camera::operator=(bc_perspective_camera&& p_other) noexcept
		{
			bc_icamera::operator=(std::move(p_other));
			m_aspect_ratio = p_other.m_aspect_ratio;
			m_field_of_view = p_other.m_field_of_view;

			return *this;
		}

		void bc_perspective_camera::get_extend_points(extend& p_points) const noexcept
		{
			const auto l_fov_tan = 2 * std::tanf(m_field_of_view / 2);
			const auto l_near_clip_height = l_fov_tan * get_near_clip();
			const auto l_near_clip_width = l_near_clip_height * m_aspect_ratio;
			const auto l_far_clip_height = l_fov_tan * get_far_clip();
			const auto l_far_clip_width = l_far_clip_height * m_aspect_ratio;

			const auto l_near_clip_center = get_position() + get_forward() * get_near_clip();
			const auto l_far_clip_center = get_position() + get_forward() * get_far_clip();
			
			const auto l_near_clip_half_width = l_near_clip_width / 2;
			const auto l_near_clip_half_height = l_near_clip_height / 2;
			const auto l_far_clip_half_width = l_far_clip_width / 2;
			const auto l_far_clip_half_height = l_far_clip_height / 2;

			const auto l_left = get_left();
			const auto l_right = get_right();
			const auto l_down = get_down();
			const auto l_up = get_up();

			p_points[0] = l_near_clip_center + l_left * l_near_clip_half_width + l_down * l_near_clip_half_height;
			p_points[1] = l_near_clip_center + l_left * l_near_clip_half_width + l_up * l_near_clip_half_height;
			p_points[2] = l_near_clip_center + l_right * l_near_clip_half_width + l_up * l_near_clip_half_height;
			p_points[3] = l_near_clip_center + l_right * l_near_clip_half_width + l_down * l_near_clip_half_height;
			p_points[4] = l_far_clip_center + l_left * l_far_clip_half_width + l_down * l_far_clip_half_height;
			p_points[5] = l_far_clip_center + l_left * l_far_clip_half_width + l_up * l_far_clip_half_height;
			p_points[6] = l_far_clip_center + l_right * l_far_clip_half_width + l_up * l_far_clip_half_height;
			p_points[7] = l_far_clip_center + l_right * l_far_clip_half_width + l_down * l_far_clip_half_height;
		}

		void bc_perspective_camera::set_projection(bcUINT16 p_back_buffer_width, bcUINT16 p_back_buffer_height, bcFLOAT p_near_clip, bcFLOAT p_far_clip) noexcept
		{
			m_screen_width = p_back_buffer_width;
			m_screen_height = p_back_buffer_height;
			m_near = p_near_clip;
			m_far = p_far_clip;
			m_aspect_ratio = bcFLOAT(p_back_buffer_width) / p_back_buffer_height;

			create_projection_matrix();
		}

		void bc_perspective_camera::set_projection(bcUINT16 p_back_buffer_width, bcUINT16 p_back_buffer_height, bcFLOAT p_height_fov, bcFLOAT p_near_clip, bcFLOAT p_far_clip) noexcept
		{
			m_field_of_view = p_height_fov;
			set_projection(p_back_buffer_width, p_back_buffer_height, p_near_clip, p_far_clip);
		}

		void bc_perspective_camera::create_projection_matrix() noexcept
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

			m_projection = l_proj;
		}
	}
}