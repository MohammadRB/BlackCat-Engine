// [06/30/2016 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Input/bcFreeCamera.h"

namespace black_cat
{
	namespace game
	{
		bc_free_camera::bc_free_camera(bcFLOAT p_aspect_ratio, 
			bcFLOAT p_height_fov,
			bcFLOAT p_near_clip, 
			bcFLOAT p_far_clip,
			bcFLOAT p_move_speed,
			bcFLOAT p_rotate_speed)
			: bc_perspective_camera(p_aspect_ratio, p_height_fov, p_near_clip, p_far_clip),
			m_move_speed(p_move_speed),
			m_rotate_speed(p_rotate_speed)
		{
		}

		void bc_free_camera::update(core_platform::bc_clock::update_param p_clock_update_param,
			const platform::bc_pointing_device& p_pointing_device,
			const platform::bc_key_device& p_key_device)
		{
			bool l_w_pressed = p_key_device.get_key_state(platform::bc_key::kb_W) == platform::bc_key_state::pressed;
			bool l_s_pressed = p_key_device.get_key_state(platform::bc_key::kb_S) == platform::bc_key_state::pressed;
			bool l_a_pressed = p_key_device.get_key_state(platform::bc_key::kb_A) == platform::bc_key_state::pressed;
			bool l_d_pressed = p_key_device.get_key_state(platform::bc_key::kb_D) == platform::bc_key_state::pressed;
			bool l_e_pressed = p_key_device.get_key_state(platform::bc_key::kb_E) == platform::bc_key_state::pressed;
			bool l_q_pressed = p_key_device.get_key_state(platform::bc_key::kb_Q) == platform::bc_key_state::pressed;
			bool l_shift_pressed = p_key_device.get_key_state(platform::bc_key::kb_shift) == platform::bc_key_state::pressed;
			bool l_ctrl_pressed = p_key_device.get_key_state(platform::bc_key::kb_ctrl) == platform::bc_key_state::pressed;
			bool l_rmb_pressed = p_key_device.get_key_state(platform::bc_key::ms_right_button) == platform::bc_key_state::pressed;

			graphic::bc_vector3f l_position = get_position();
			bcFLOAT l_move_speed = l_shift_pressed ? m_move_speed * 6 : l_ctrl_pressed ? m_move_speed * 0.25 : m_move_speed;
			bcFLOAT l_rotate_speed = m_rotate_speed;

			l_move_speed *= p_clock_update_param.m_elapsed_second;
			l_rotate_speed *= p_clock_update_param.m_elapsed_second;

			if (l_w_pressed)
			{
				l_position += get_forward() * l_move_speed;
			}
			if (l_s_pressed)
			{
				l_position += get_back() * l_move_speed;
			}
			if (l_a_pressed)
			{
				l_position += get_left() * l_move_speed;
			}
			if (l_d_pressed)
			{
				l_position += get_right() * l_move_speed;
			}
			if (l_e_pressed)
			{
				l_position += get_down() * l_move_speed;
			}
			if (l_q_pressed)
			{
				l_position += get_up() * l_move_speed;
			}

			graphic::bc_vector3f l_direction = get_direction();

			if(l_rmb_pressed)
			{
				bcFLOAT l_pi = 3.14159265358979323846 / 2;
				auto l_dx = -p_pointing_device.get_state().m_dx * l_rotate_speed;
				auto l_dy = p_pointing_device.get_state().m_dy * l_rotate_speed;

				if(l_dy > l_pi)
				{
					l_dy = l_pi;
				}
				if(l_dy < -l_pi)
				{
					l_dy = -l_pi;
				}

				graphic::bc_matrix3f l_rotation_y;
				graphic::bc_matrix3f l_rotation_x;
				graphic::bc_matrix3f l_rotation;

				graphic::bc_vector3f l_right = get_right();
				graphic::bc_vector3f l_up = get_up();

				l_rotation_y.rotation_euler(l_up, -l_dx);
				l_rotation_x.rotation_euler(l_right, l_dy);
				l_rotation = l_rotation_y * l_rotation_x;

				l_direction = l_rotation * l_direction;
				l_direction.normalize();
			}

			auto l_lookat = l_position + l_direction;

			look_at(l_position, l_lookat, graphic::bc_vector3f(0, 1, 0));
		}
	}
}