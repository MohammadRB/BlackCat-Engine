// [06/30/2016 MRB]

#include "Game/GamePCH.h"

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
			m_w_pressed(false),
			m_s_pressed(false),
			m_a_pressed(false),
			m_d_pressed(false),
			m_e_pressed(false),
			m_q_pressed(false),
			m_shift_pressed(false),
			m_ctrl_pressed(false),
			m_rmb_pressed(false),
			m_dx(0),
			m_dy(0)
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
			m_w_pressed = p_other.m_w_pressed;
			m_s_pressed = p_other.m_s_pressed;
			m_a_pressed = p_other.m_a_pressed;
			m_d_pressed = p_other.m_d_pressed;
			m_e_pressed = p_other.m_e_pressed;
			m_q_pressed = p_other.m_q_pressed;
			m_shift_pressed = p_other.m_shift_pressed;
			m_ctrl_pressed = p_other.m_ctrl_pressed;
			m_rmb_pressed = p_other.m_rmb_pressed;
			m_dx = p_other.m_dx;
			m_dy = p_other.m_dy;
			m_key_listener_handle = std::move(p_other.m_key_listener_handle);
			m_pointing_listener_handle = std::move(p_other.m_pointing_listener_handle);

			m_key_listener_handle.reassign(core::bc_event_manager::delegate_type(*this, &bc_free_camera::_on_event));
			m_pointing_listener_handle.reassign(core::bc_event_manager::delegate_type(*this, &bc_free_camera::_on_event));

			return *this;
		}

		void bc_free_camera::update(const core_platform::bc_clock::update_param& p_clock_update_param) noexcept
		{
			core::bc_vector3f l_position = get_position();
			bcFLOAT l_move_speed = m_shift_pressed ? m_move_speed * 6 : m_ctrl_pressed ? m_move_speed * 0.25 : m_move_speed;
			bcFLOAT l_rotate_speed = m_rotate_speed;

			l_move_speed *= p_clock_update_param.m_elapsed_second;
			l_rotate_speed *= p_clock_update_param.m_elapsed_second;

			if (m_w_pressed)
			{
				l_position += get_forward() * l_move_speed;
			}
			if (m_s_pressed)
			{
				l_position += get_back() * l_move_speed;
			}
			if (m_a_pressed)
			{
				l_position += get_left() * l_move_speed;
			}
			if (m_d_pressed)
			{
				l_position += get_right() * l_move_speed;
			}
			if (m_e_pressed)
			{
				l_position += get_down() * l_move_speed;
			}
			if (m_q_pressed)
			{
				l_position += get_up() * l_move_speed;
			}

			core::bc_vector3f l_direction = get_direction();

			if (m_rmb_pressed)
			{
				bcFLOAT l_pi = 3.14159265358979323846 / 2;
				auto l_dx = -m_dx * l_rotate_speed;
				auto l_dy = m_dy * l_rotate_speed;

				if (m_dy > l_pi)
				{
					m_dy = l_pi;
				}
				if (m_dy < -l_pi)
				{
					m_dy = -l_pi;
				}

				core::bc_matrix3f l_rotation_y;
				core::bc_matrix3f l_rotation_x;
				core::bc_matrix3f l_rotation;

				core::bc_vector3f l_right = get_right();
				core::bc_vector3f l_up = get_up();

				if(graphic::bc_render_api_info::is_left_handed())
				{
					l_rotation_y.rotation_euler_lh(l_up, -l_dx);
					l_rotation_x.rotation_euler_lh(l_right, l_dy);
				}
				else
				{
					l_rotation_y.rotation_euler_rh(l_up, -l_dx);
					l_rotation_x.rotation_euler_rh(l_right, l_dy);
				}
				l_rotation = l_rotation_y * l_rotation_x;

				l_direction = l_rotation * l_direction;
				l_direction.normalize();
			}

			const auto l_look_at = l_position + l_direction;

			set_look_at(l_position, l_look_at, core::bc_vector3f(0, 1, 0));

			m_dx = 0;
			m_dy = 0;
		}

		bool bc_free_camera::_on_event(core::bc_ievent& p_event) noexcept
		{
			if(core::bc_imessage::is<platform::bc_app_event_pointing>(p_event))
			{
				return _on_pointing(static_cast<platform::bc_app_event_pointing&>(p_event));
			}

			if(core::bc_imessage::is<platform::bc_app_event_key>(p_event))
			{
				return _on_key(static_cast<platform::bc_app_event_key&>(p_event));
			}

			return false;
		}

		bool bc_free_camera::_on_pointing(platform::bc_app_event_pointing& p_pointing_event) noexcept
		{
			m_dx = p_pointing_event.get_state().m_dx;
			m_dy = p_pointing_event.get_state().m_dy;

			return true;
		}
		
		bool bc_free_camera::_on_key(platform::bc_app_event_key& p_key_event) noexcept
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

			return true;
		}
	}
}