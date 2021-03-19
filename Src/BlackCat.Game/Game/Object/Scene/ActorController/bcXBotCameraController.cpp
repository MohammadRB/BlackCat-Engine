// [03/12/2021 MRB]

#include "Game/GamePCH.h"

#include "Core/Messaging/Event/bcEventManager.h"
#include "Core/Math/bcCoordinate.h"
#include "Core/Utility/bcLogger.h"
#include "Platform/bcEvent.h"
#include "GraphicImp/bcRenderApiInfo.h"
#include "Game/System/bcGameSystem.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/Object/Scene/ActorController/bcXBotCameraController.h"
#include "Game/Object/Animation/bcAnimationJobHelper.h"
#include "Game/Object/Animation/Job/bcBlendingAnimationJob.h"

namespace black_cat
{
	namespace game
	{
		bc_xbot_camera_controller::bc_xbot_camera_controller()
			: m_input_system(nullptr),
			m_scene(nullptr),
			m_camera(nullptr),
			m_camera_y_offset(0),
			m_camera_z_offset(0),
			m_camera_look_at_offset(0),
			m_look_speed(0),
			m_run_speed(0),
			m_walk_speed(0),
			m_current_move_speed(0),
			m_pointing_last_x(0),
			m_pointing_delta_x(0),
			m_forward_pressed(false),
			m_backward_pressed(false),
			m_right_pressed(false),
			m_left_pressed(false),
			m_ctrl_pressed(false),
			m_look_velocity(0, 1, 0.25),
			m_forward_velocity(0, 1, 0.25),
			m_backward_velocity(m_forward_velocity),
			m_right_velocity(m_forward_velocity),
			m_left_velocity(m_forward_velocity),
			m_position(),
			m_look_direction(),
			m_move_direction(),
			m_move_speed(0)
		{
			auto* l_event_manager = core::bc_get_service<core::bc_event_manager>();
			m_key_listener_handle = l_event_manager->register_event_listener< platform::bc_app_event_key >
			(
				core::bc_event_manager::delegate_type(*this, &bc_xbot_camera_controller::_on_event)
			);
			m_pointing_listener_handle = l_event_manager->register_event_listener< platform::bc_app_event_pointing >
			(
				core::bc_event_manager::delegate_type(*this, &bc_xbot_camera_controller::_on_event)
			);
		}

		bc_xbot_camera_controller::bc_xbot_camera_controller(bc_xbot_camera_controller&& p_other) noexcept
			: m_look_velocity(p_other.m_look_velocity),
			m_forward_velocity(p_other.m_forward_velocity),
			m_backward_velocity(p_other.m_backward_velocity),
			m_right_velocity(p_other.m_right_velocity),
			m_left_velocity(p_other.m_left_velocity)
		{
			operator=(std::move(p_other));
		}

		bc_xbot_camera_controller::~bc_xbot_camera_controller() = default;
		
		bc_xbot_camera_controller& bc_xbot_camera_controller::operator=(bc_xbot_camera_controller&& p_other) noexcept
		{
			m_key_listener_handle = std::move(p_other.m_key_listener_handle);
			m_pointing_listener_handle = std::move(p_other.m_pointing_listener_handle);
			m_input_system = p_other.m_input_system;
			m_scene = p_other.m_scene;
			m_camera = p_other.m_camera;

			m_camera_y_offset = p_other.m_camera_y_offset;
			m_camera_z_offset = p_other.m_camera_z_offset;
			m_camera_look_at_offset = p_other.m_camera_look_at_offset;
			m_look_speed = p_other.m_look_speed;
			m_run_speed = p_other.m_run_speed;
			m_walk_speed = p_other.m_walk_speed;
			m_current_move_speed = p_other.m_current_move_speed;
			
			m_pointing_last_x = p_other.m_pointing_last_x;
			m_pointing_delta_x = p_other.m_pointing_delta_x;
			m_forward_pressed = p_other.m_forward_pressed;
			m_backward_pressed = p_other.m_backward_pressed;
			m_right_pressed = p_other.m_right_pressed;
			m_left_pressed = p_other.m_left_pressed;
			m_ctrl_pressed = p_other.m_ctrl_pressed;
			m_look_velocity = p_other.m_look_velocity;
			m_forward_velocity = p_other.m_forward_velocity;
			m_backward_velocity = p_other.m_backward_velocity;
			m_right_velocity = p_other.m_right_velocity;
			m_left_velocity = p_other.m_left_velocity;
			m_position = p_other.m_position;
			m_look_direction = p_other.m_look_direction;
			m_move_direction = p_other.m_move_direction;
			m_move_speed = p_other.m_move_speed;

			m_key_listener_handle.reassign(core::bc_event_manager::delegate_type(*this, &bc_xbot_camera_controller::_on_event));
			m_pointing_listener_handle.reassign(core::bc_event_manager::delegate_type(*this, &bc_xbot_camera_controller::_on_event));

			return *this;
		}

		void bc_xbot_camera_controller::initialize(const bc_actor_component_initialize_context& p_context)
		{
			bc_xbot_controller::initialize(p_context);
			m_input_system = &p_context.m_game_system.get_input_system();
			m_look_direction = get_local_forward();
			m_move_direction = get_local_forward();
		}

		void bc_xbot_camera_controller::added_to_scene(const bc_actor_component_event_context& p_context, bc_scene& p_scene)
		{
			bc_xbot_controller::added_to_scene(p_context, p_scene);

			auto* l_current_camera = m_input_system->get_camera();
			auto l_camera = core::bc_make_unique<bc_chasing_camera>(bc_chasing_camera
			(
				l_current_camera->get_screen_width(),
				l_current_camera->get_screen_height(),
				1.2f,
				l_current_camera->get_near_clip(),
				l_current_camera->get_far_clip()
			));
			m_scene = &p_scene;
			m_camera = static_cast<bc_chasing_camera*>(m_input_system->add_camera(std::move(l_camera)));

			const auto* l_mediate_component = p_context.m_actor.get_component<bc_mediate_component>();
			const auto l_bound_box_extends = l_mediate_component->get_bound_box().get_half_extends();
			const auto l_max_side_length = std::max(std::max(l_bound_box_extends.x, l_bound_box_extends.y), l_bound_box_extends.z) * 2;
			m_camera_y_offset = l_max_side_length * 3.5f;
			m_camera_z_offset = l_max_side_length * -1.25f;
			m_camera_look_at_offset = l_max_side_length * 1.75f;
			m_look_speed = 10.0f;
			m_run_speed = l_max_side_length * 1.3f;
			m_walk_speed = l_max_side_length * 0.6f;
		}

		void bc_xbot_camera_controller::update(const bc_actor_component_update_content& p_context)
		{
			if(!m_scene) // Has not added to scene yet
			{
				return;
			}
			
			_update_input(p_context.m_clock);
			_update_direction(p_context.m_clock);

			auto* l_active_animation = _select_active_animation(p_context.m_clock);

			_update_world_transform(p_context.m_actor);
			
			bc_xbot_controller::set_active_animation(l_active_animation);
			bc_xbot_controller::update(p_context);

			m_pointing_delta_x = 0;
		}

		void bc_xbot_camera_controller::removed_from_scene(const bc_actor_component_event_context& p_context, bc_scene& p_scene)
		{
			bc_xbot_controller::removed_from_scene(p_context, p_scene);
			
			m_input_system->remove_camera(m_camera);
			m_scene = nullptr;
		}

		void bc_xbot_camera_controller::handle_event(const bc_actor_component_event_context& p_context)
		{
			bc_xbot_controller::handle_event(p_context);
			
			const auto* l_world_transform_event = core::bci_message::as<bc_world_transform_actor_event>(p_context.m_event);
			if (l_world_transform_event)
			{
				const auto& l_world_transform = l_world_transform_event->get_transform();
				
				m_position = l_world_transform.get_translation();
				if(m_camera)
				{
					m_camera->update
					(
						m_position + m_look_direction * m_camera_z_offset + core::bc_vector3f(0, m_camera_y_offset, 0),
						m_position + m_look_direction * m_camera_look_at_offset
					);
				}
			}
		}

		bool bc_xbot_camera_controller::_on_event(core::bci_event& p_event) noexcept
		{
			if (core::bci_message::is<platform::bc_app_event_pointing>(p_event))
			{
				return _on_pointing(static_cast<platform::bc_app_event_pointing&>(p_event));
			}

			if (core::bci_message::is<platform::bc_app_event_key>(p_event))
			{
				return _on_key(static_cast<platform::bc_app_event_key&>(p_event));
			}

			return false;
		}

		bool bc_xbot_camera_controller::_on_pointing(platform::bc_app_event_pointing& p_pointing_event) noexcept
		{
			m_pointing_delta_x = p_pointing_event.get_state().m_x - m_pointing_last_x;
			m_pointing_last_x = p_pointing_event.get_state().m_x;

			return true;
		}

		bool bc_xbot_camera_controller::_on_key(platform::bc_app_event_key& p_key_event) noexcept
		{
			if (p_key_event.get_key() == platform::bc_key::kb_W)
			{
				if (p_key_event.get_key_state() == platform::bc_key_state::pressing)
				{
					m_forward_pressed = true;
				}
				else if (p_key_event.get_key_state() == platform::bc_key_state::releasing)
				{
					m_forward_pressed = false;
				}
			}
			else if (p_key_event.get_key() == platform::bc_key::kb_S)
			{
				if (p_key_event.get_key_state() == platform::bc_key_state::pressing)
				{
					m_backward_pressed = true;
				}
				else if (p_key_event.get_key_state() == platform::bc_key_state::releasing)
				{
					m_backward_pressed = false;
				}
			}
			else if (p_key_event.get_key() == platform::bc_key::kb_D)
			{
				if (p_key_event.get_key_state() == platform::bc_key_state::pressing)
				{
					m_right_pressed = true;
				}
				else if (p_key_event.get_key_state() == platform::bc_key_state::releasing)
				{
					m_right_pressed = false;
				}
			}
			else if (p_key_event.get_key() == platform::bc_key::kb_A)
			{
				if (p_key_event.get_key_state() == platform::bc_key_state::pressing)
				{
					m_left_pressed = true;
				}
				else if (p_key_event.get_key_state() == platform::bc_key_state::releasing)
				{
					m_left_pressed = false;
				}
			}
			if (p_key_event.get_key() == platform::bc_key::kb_ctrl)
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
			
			return true;
		}

		void bc_xbot_camera_controller::_update_input(const core_platform::bc_clock::update_param& p_clock)
		{
			if(m_pointing_delta_x != 0)
			{
				m_look_velocity.push(p_clock.m_elapsed_second);
			}
			else
			{
				m_look_velocity.release(p_clock.m_elapsed_second);
			}
			
			if(m_forward_pressed)
			{
				m_forward_velocity.push(p_clock.m_elapsed_second);
			}
			else
			{
				m_forward_velocity.release(p_clock.m_elapsed_second);
			}

			if (m_right_pressed)
			{
				m_right_velocity.push(p_clock.m_elapsed_second);
			}
			else
			{
				m_right_velocity.release(p_clock.m_elapsed_second);
			}

			if (m_left_pressed)
			{
				m_left_velocity.push(p_clock.m_elapsed_second);
			}
			else
			{
				m_left_velocity.release(p_clock.m_elapsed_second);
			}

			const bool l_is_moving_forward = m_forward_velocity.get_value() || m_right_velocity.get_value() || m_left_velocity.get_value();
			const bool l_is_moving_backward = m_backward_velocity.get_value();
			
			if (m_backward_pressed && (!l_is_moving_forward || l_is_moving_backward))
			{
				m_backward_velocity.push(p_clock.m_elapsed_second);
			}
			else
			{
				m_backward_velocity.release(p_clock.m_elapsed_second);
			}
		}

		void bc_xbot_camera_controller::_update_direction(const core_platform::bc_clock::update_param& p_clock)
		{
			const bcFLOAT l_look_velocity = m_look_velocity.get_value();
			const bcFLOAT l_forward_velocity = m_forward_velocity.get_value();
			const bcFLOAT l_backward_velocity = m_backward_velocity.get_value();
			const bcFLOAT l_right_velocity = m_right_velocity.get_value();
			const bcFLOAT l_left_velocity = m_left_velocity.get_value();

			core::bc_matrix3f l_look_rotation;
			core::bc_vector3f l_right_vector;
			core::bc_vector3f l_left_vector;

			if (graphic::bc_render_api_info::use_left_handed())
			{
				l_look_rotation.rotation_y_lh(core::bc_to_radian(m_pointing_delta_x * l_look_velocity * m_look_speed * p_clock.m_elapsed_second));
			}
			else
			{
				l_look_rotation.rotation_y_rh(core::bc_to_radian(m_pointing_delta_x * l_look_velocity * m_look_speed * p_clock.m_elapsed_second));
			}
			
			m_look_direction = core::bc_vector3f::normalize(l_look_rotation * m_look_direction);
			
			if(graphic::bc_render_api_info::use_left_handed())
			{
				core::bc_matrix3f l_right_left_direction;
				
				l_right_left_direction.rotation_y_lh(core::bc_to_radian(l_right_velocity * 90));
				l_right_vector = core::bc_vector3f::normalize(l_right_left_direction * m_look_direction);
				
				l_right_left_direction.rotation_y_lh(core::bc_to_radian(l_left_velocity * -90));
				l_left_vector = core::bc_vector3f::normalize(l_right_left_direction * m_look_direction);
			}
			else
			{
				core::bc_matrix3f l_right_left_direction;

				l_right_left_direction.rotation_y_rh(core::bc_to_radian(l_right_velocity * 90));
				l_right_vector = core::bc_vector3f::normalize(l_right_left_direction * m_look_direction);

				l_right_left_direction.rotation_y_rh(core::bc_to_radian(l_left_velocity * -90));
				l_left_vector = core::bc_vector3f::normalize(l_right_left_direction * m_look_direction);
			}

			m_current_move_speed = !m_ctrl_pressed ? m_run_speed : m_walk_speed;
			m_move_direction = m_look_direction * l_forward_velocity +
					m_look_direction * -l_backward_velocity +
					l_right_vector * l_right_velocity +
					l_left_vector * l_left_velocity;
			m_move_speed = std::max({ l_forward_velocity, l_backward_velocity, l_right_velocity, l_left_velocity }) * m_current_move_speed * p_clock.m_elapsed_second;

			if(m_move_speed <= 0)
			{
				m_move_direction = m_look_direction;
			}

			m_move_direction.normalize();

			// if two vectors are exactly opposite of each other 'rotation_between_two_vector' method in matrix3x3 will have undefined result
			if(m_move_direction.dot(m_look_direction) == -1.f)
			{
				m_move_direction.x -= 0.01f;
				m_move_direction.normalize();
			}
		}

		bci_animation_job* bc_xbot_camera_controller::_select_active_animation(const core_platform::bc_clock::update_param& p_clock)
		{
			bci_animation_job* l_active_animation = nullptr;

			if(m_move_speed <= 0)
			{
				l_active_animation = get_idle_animation();
				auto* l_blending_job = bc_animation_job_helper::find_job<bc_blending_animation_job>(static_cast<bc_sequence_animation_job&>(*l_active_animation));

				const auto l_look_velocity = m_look_velocity.get_value();
				bcFLOAT l_weights[3] = { l_look_velocity, 1 - l_look_velocity, l_look_velocity };
				
				l_blending_job->set_weights(&l_weights[0]);

				if(l_look_velocity == 0.f)
				{
					bcFLOAT l_times[3] = { 0, -1, 0 };
					l_blending_job->set_local_times(&l_times[0]);
				}
				else
				{
					bcFLOAT l_times[3] = { -1, 0, -1 };
					l_blending_job->set_local_times(&l_times[0]);
				}
			}
			else
			{
				l_active_animation = get_running_animation();
				auto* l_blending_job = bc_animation_job_helper::find_job<bc_blending_animation_job>(static_cast<bc_sequence_animation_job&>(*l_active_animation));

				const auto l_move_speed_normalize = m_move_speed / (m_current_move_speed * p_clock.m_elapsed_second);
				bcFLOAT l_weights[3] = { 1 - l_move_speed_normalize, l_move_speed_normalize, l_move_speed_normalize };

				const auto l_move_direction_dot = m_look_direction.dot(m_move_direction);
				if (l_move_direction_dot >= -0.01f)
				{
					l_weights[2] = 0;
				}
				else
				{
					l_weights[1] = 0;
				}

				l_blending_job->set_weights(&l_weights[0]);
			}

			return l_active_animation;
		}

		void bc_xbot_camera_controller::_update_world_transform(bc_actor& p_actor)
		{
			core::bc_matrix4f l_world_transform;
			core::bc_matrix3f l_rotation;

			auto l_move_direction_sign = m_look_direction.dot(m_move_direction) >= -0.01f ? 1 : -1;
			
			if (graphic::bc_render_api_info::use_left_handed())
			{
				l_rotation.rotation_between_two_vector_lh(get_local_forward(), m_move_direction * l_move_direction_sign);
			}
			else
			{
				l_rotation.rotation_between_two_vector_rh(get_local_forward(), m_move_direction * l_move_direction_sign);
			}

			l_world_transform.make_identity();
			l_world_transform.set_rotation(l_rotation);
			l_world_transform.set_translation(m_position + m_move_direction * m_move_speed);

			p_actor.add_event(bc_world_transform_actor_event(l_world_transform));
		}
	}	
}