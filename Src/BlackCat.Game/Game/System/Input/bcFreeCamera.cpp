// [06/30/2016 MRB]

#include "Game/GamePCH.h"

#include "Core/Math/bcCoordinate.h"
#include "GraphicImp/bcRenderApiInfo.h"
#include "Game/System/Input/bcFreeCamera.h"

namespace black_cat
{
	namespace game
	{
		bc_free_camera::bc_free_camera(bcUINT16 p_back_buffer_width,
			bcUINT16 p_back_buffer_height,
			bcFLOAT p_height_fov,
			bcFLOAT p_near_clip,
			bcFLOAT p_far_clip,
			bcFLOAT p_move_speed,
			bcFLOAT p_rotate_speed)
			: bc_perspective_camera(p_back_buffer_width, p_back_buffer_height, p_height_fov, p_near_clip, p_far_clip),
			m_move_speed(p_move_speed),
			m_rotate_speed(p_rotate_speed),
			m_pointing_smooth_frames(2),
			m_movement_drag_time(0.25),
			m_movement_drag_timer(0),
			m_w_pressed(false),
			m_s_pressed(false),
			m_a_pressed(false),
			m_d_pressed(false),
			m_e_pressed(false),
			m_q_pressed(false),
			m_shift_pressed(false),
			m_ctrl_pressed(false),
			m_rmb_pressed(false),
			m_pointing_delta(0),
			m_new_pointing_delta(0),
			m_movement_direction(0),
			m_pointing_velocity(0),
			m_pointing_angle(0),
			m_movement_velocity(0),
			m_movement_velocity_drag(0)
		{
			auto* l_event_manager = core::bc_get_service<core::bc_event_manager>();
			m_key_listener_handle = l_event_manager->register_event_listener< platform::bc_app_event_key >
			(
				core::bc_event_manager::delegate_type(*this, &bc_free_camera::_on_event)
			);
			m_pointing_listener_handle = l_event_manager->register_event_listener< platform::bc_app_event_pointing >
			(
				core::bc_event_manager::delegate_type(*this, &bc_free_camera::_on_event)
			);
		}

		bc_free_camera::bc_free_camera(bc_free_camera&& p_other) noexcept
			: bc_perspective_camera(std::move(p_other))
		{
			operator=(std::move(p_other));
		}

		bc_free_camera& bc_free_camera::operator=(bc_free_camera&& p_other) noexcept
		{
			bc_perspective_camera::operator=(std::move(p_other));
			m_move_speed = p_other.m_move_speed;
			m_rotate_speed = p_other.m_rotate_speed;
			m_pointing_smooth_frames = p_other.m_pointing_smooth_frames;
			m_movement_drag_time = p_other.m_movement_drag_time;
			m_movement_drag_timer = p_other.m_movement_drag_timer;
			m_w_pressed = p_other.m_w_pressed;
			m_s_pressed = p_other.m_s_pressed;
			m_a_pressed = p_other.m_a_pressed;
			m_d_pressed = p_other.m_d_pressed;
			m_e_pressed = p_other.m_e_pressed;
			m_q_pressed = p_other.m_q_pressed;
			m_shift_pressed = p_other.m_shift_pressed;
			m_ctrl_pressed = p_other.m_ctrl_pressed;
			m_rmb_pressed = p_other.m_rmb_pressed;
			m_pointing_delta = p_other.m_pointing_delta;
			m_new_pointing_delta = p_other.m_new_pointing_delta;
			m_movement_direction = p_other.m_movement_direction;
			m_pointing_velocity = p_other.m_pointing_velocity;
			m_pointing_angle = p_other.m_pointing_angle;
			m_movement_velocity = p_other.m_movement_velocity;
			m_movement_velocity_drag = p_other.m_movement_velocity_drag;
			m_key_listener_handle = std::move(p_other.m_key_listener_handle);
			m_pointing_listener_handle = std::move(p_other.m_pointing_listener_handle);

			m_key_listener_handle.reassign(core::bc_event_manager::delegate_type(*this, &bc_free_camera::_on_event));
			m_pointing_listener_handle.reassign(core::bc_event_manager::delegate_type(*this, &bc_free_camera::_on_event));

			return *this;
		}

		void bc_free_camera::set_look_at(const core::bc_vector3f& p_position, const core::bc_vector3f& p_look_at) noexcept
		{
			bci_camera::set_look_at(p_position, p_look_at);
		}

		void bc_free_camera::update(const core_platform::bc_clock::update_param& p_clock) noexcept
		{
			_update_pointing();
			_update_movement();
			_update_velocity(p_clock.m_elapsed_second);

			if (m_rmb_pressed)
			{
				const bcFLOAT l_pi = (core::g_pi / 2.0f) - 0.01f;
				m_pointing_angle += core::bc_vector2f(m_pointing_velocity.y, m_pointing_velocity.x);
				m_pointing_angle.x = std::min(l_pi, m_pointing_angle.x);
				m_pointing_angle.x = std::max(-l_pi, m_pointing_angle.x);
			}
			
			core::bc_matrix3f l_y_rotation;
			core::bc_matrix3f l_x_rotation;
			core::bc_matrix3f l_rotation;

			core::bc_vector3f l_up, l_forward;
			
			if (graphic::bc_render_api_info::use_left_handed())
			{
				l_y_rotation.rotation_euler_lh(core::bc_vector3f::up(), m_pointing_angle.y);
				l_x_rotation.rotation_euler_lh(core::bc_vector3f::right(), m_pointing_angle.x);
				l_rotation = l_x_rotation * l_y_rotation;

				l_up = core::bc_vector3f::up();
				l_forward = core::bc_vector3f::forward();
			}
			else
			{
				l_y_rotation.rotation_euler_rh(core::bc_vector3f::up(), m_pointing_angle.y);
				l_x_rotation.rotation_euler_rh(core::bc_vector3f::right(), m_pointing_angle.x);
				l_rotation = l_x_rotation * l_y_rotation;

				l_up = core::bc_vector3f::up();
				l_forward = core::bc_vector3f::forward() * -1;
			}

			l_up = core::bc_vector3f::normalize(l_rotation * l_up);
			l_forward = core::bc_vector3f::normalize(l_rotation * l_forward);
			const auto l_movement_velocity = l_rotation * m_movement_velocity;
			const auto l_position = get_position() + l_movement_velocity;

			m_update_pointing_angle = false;
			bci_camera::set_look_at(l_position, l_position + l_forward, l_up);

			m_new_pointing_delta = core::bc_vector2f(0);
			m_movement_direction = core::bc_vector3f(0);
		}

		core::bc_matrix4f bc_free_camera::create_view_matrix(const core::bc_vector3f& p_up) noexcept
		{
			if(!m_update_pointing_angle) // TODO remove this magical flag
			{
				m_update_pointing_angle = true;
				return bci_camera::create_view_matrix(p_up);
			}
			
			const auto l_source_vec = core::bc_vector3f::normalize(get_look_at() - get_position());
			core::bc_matrix3f l_rotation;
			l_rotation.rotation_between_two_vector_lh(core::bc_vector3f::forward(), l_source_vec);

			bcFLOAT l_x_angle = std::acos(core::bc_vector3f::dot(core::bc_vector3f::right(), l_rotation.get_row(0)));
			bcFLOAT l_y_angle = std::acos(core::bc_vector3f::dot(core::bc_vector3f::up(), l_rotation.get_row(1)));

			if (l_source_vec.x < 0)
			{
				l_x_angle *= -1;
			}

			m_pointing_angle.x = l_y_angle;
			m_pointing_angle.y = l_x_angle;

			return bci_camera::create_view_matrix(p_up);
			
			/*core::bc_matrix3f l_new_rotation;
			l_new_rotation.rotation_zyx_lh(core::bc_vector3f(l_y_angle, l_x_angle, 0));
			auto l_new_vec = core::bc_vector3f::normalize(l_rotation * core::bc_vector3f::forward());
			auto l_new_vec1 = core::bc_vector3f::normalize(l_new_rotation * core::bc_vector3f::forward());*/
		}

		void bc_free_camera::_update_pointing()
		{
			const bcFLOAT l_percent_of_new = 1.0f / m_pointing_smooth_frames;
			const bcFLOAT l_percent_of_old = 1.0f - l_percent_of_new;
			m_pointing_delta.x = m_pointing_delta.x * l_percent_of_old + m_new_pointing_delta.x * l_percent_of_new;
			m_pointing_delta.y = m_pointing_delta.y * l_percent_of_old + m_new_pointing_delta.y * l_percent_of_new;
		}

		void bc_free_camera::_update_movement()
		{
			if (m_w_pressed)
			{
				m_movement_direction.z += 1;
			}
			if (m_s_pressed)
			{
				m_movement_direction.z -= 1;
			}
			if (m_a_pressed)
			{
				m_movement_direction.x -= 1;
			}
			if (m_d_pressed)
			{
				m_movement_direction.x += 1;
			}
			if (m_e_pressed)
			{
				m_movement_direction.y -= 1;
			}
			if (m_q_pressed)
			{
				m_movement_direction.y += 1;
			}
		}

		void bc_free_camera::_update_velocity(bcFLOAT p_elapsed_second)
		{
			m_pointing_velocity = m_pointing_delta * m_rotate_speed * p_elapsed_second;

			// Normalize vector so if moving 2 dirs (left & forward), 
			// the camera doesn't move faster than if moving in 1 dir
			const auto l_move_speed = m_shift_pressed ? m_move_speed * 4 : m_ctrl_pressed ? m_move_speed * 0.25f : m_move_speed;
			const auto l_acceleration = core::bc_vector3f::normalize(m_movement_direction) * l_move_speed * p_elapsed_second;

			if(m_movement_direction.magnitude() > 0)
			{
				m_movement_drag_timer = m_movement_drag_time;
				m_movement_velocity = l_acceleration;
				m_movement_velocity_drag = l_acceleration / m_movement_drag_timer;
			}
			else
			{
				if(m_movement_drag_timer > 0)
				{
					m_movement_velocity -= m_movement_velocity_drag * p_elapsed_second;
					m_movement_drag_timer -= p_elapsed_second;
				}
				else
				{
					m_movement_velocity = core::bc_vector3f(0);
				}
			}
		}

		void bc_free_camera::_on_event(core::bci_event& p_event) noexcept
		{
			if(core::bci_message::is<platform::bc_app_event_pointing>(p_event))
			{
				_on_pointing(static_cast<platform::bc_app_event_pointing&>(p_event));
				return;
			}

			if(core::bci_message::is<platform::bc_app_event_key>(p_event))
			{
				_on_key(static_cast<platform::bc_app_event_key&>(p_event));
				return;
			}
		}

		void bc_free_camera::_on_pointing(platform::bc_app_event_pointing& p_pointing_event) noexcept
		{
			if(m_rmb_pressed)
			{
				m_new_pointing_delta.x = p_pointing_event.get_state().m_dx;
				m_new_pointing_delta.y = p_pointing_event.get_state().m_dy;
			}
		}
		
		void bc_free_camera::_on_key(platform::bc_app_event_key& p_key_event) noexcept
		{
			if (p_key_event.get_key() == platform::bc_key::kb_W)
			{
				if (p_key_event.get_key_state() == platform::bc_key_state::pressing)
				{
					m_w_pressed = true;
				}
				else if (p_key_event.get_key_state() == platform::bc_key_state::releasing)
				{
					m_w_pressed = false;
				}
			}
			else if (p_key_event.get_key() == platform::bc_key::kb_S)
			{
				if (p_key_event.get_key_state() == platform::bc_key_state::pressing)
				{
					m_s_pressed = true;
				}
				else if (p_key_event.get_key_state() == platform::bc_key_state::releasing)
				{
					m_s_pressed = false;
				}
			}
			else if (p_key_event.get_key() == platform::bc_key::kb_A)
			{
				if (p_key_event.get_key_state() == platform::bc_key_state::pressing)
				{
					m_a_pressed = true;
				}
				else if (p_key_event.get_key_state() == platform::bc_key_state::releasing)
				{
					m_a_pressed = false;
				}
			}
			else if (p_key_event.get_key() == platform::bc_key::kb_D)
			{
				if (p_key_event.get_key_state() == platform::bc_key_state::pressing)
				{
					m_d_pressed = true;
				}
				else if (p_key_event.get_key_state() == platform::bc_key_state::releasing)
				{
					m_d_pressed = false;
				}
			}
			else if (p_key_event.get_key() == platform::bc_key::kb_E)
			{
				if (p_key_event.get_key_state() == platform::bc_key_state::pressing)
				{
					m_e_pressed = true;
				}
				else if (p_key_event.get_key_state() == platform::bc_key_state::releasing)
				{
					m_e_pressed = false;
				}
			}
			else if (p_key_event.get_key() == platform::bc_key::kb_Q)
			{
				if (p_key_event.get_key_state() == platform::bc_key_state::pressing)
				{
					m_q_pressed = true;
				}
				else if (p_key_event.get_key_state() == platform::bc_key_state::releasing)
				{
					m_q_pressed = false;
				}
			}
			else if (p_key_event.get_key() == platform::bc_key::kb_shift)
			{
				if (p_key_event.get_key_state() == platform::bc_key_state::pressing)
				{
					m_shift_pressed = true;
				}
				else if (p_key_event.get_key_state() == platform::bc_key_state::releasing)
				{
					m_shift_pressed = false;
				}
			}
			else if (p_key_event.get_key() == platform::bc_key::kb_ctrl)
			{
				if (p_key_event.get_key_state() == platform::bc_key_state::pressing)
				{
					m_ctrl_pressed = true;
				}
				else if (p_key_event.get_key_state() == platform::bc_key_state::releasing)
				{
					m_ctrl_pressed = false;
				}
			}
			else if (p_key_event.get_key() == platform::bc_key::ms_right_button)
			{
				if (p_key_event.get_key_state() == platform::bc_key_state::pressing)
				{
					m_rmb_pressed = true;
				}
				else if (p_key_event.get_key_state() == platform::bc_key_state::releasing)
				{
					m_rmb_pressed = false;
				}
			}
		}
	}
}