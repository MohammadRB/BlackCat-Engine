// [03/12/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/System/bcGameSystem.h"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/Object/Scene/ActorController/bcXBotCameraController.h"

namespace black_cat
{
	namespace game
	{
		bc_xbot_camera_controller::bc_xbot_camera_controller()
			: m_input_system(nullptr),
			m_camera(nullptr),
			m_camera_y_offset(0),
			m_camera_z_offset(0),
			m_camera_look_at_offset(0),
			m_forward_velocity(0, 1, 0.5)
		{
		}

		bc_xbot_camera_controller::bc_xbot_camera_controller(bc_xbot_camera_controller&& p_other) noexcept = default;

		bc_xbot_camera_controller::~bc_xbot_camera_controller()
		{
			if (m_camera)
			{
				m_input_system->remove_camera(m_camera);
			}
		}
		
		bc_xbot_camera_controller& bc_xbot_camera_controller::operator=(bc_xbot_camera_controller&& p_other) noexcept = default;

		void bc_xbot_camera_controller::initialize(const bc_actor_component_initialize_context& p_context)
		{
			bc_xbot_controller::initialize(p_context);
			m_input_system = &p_context.m_game_system.get_input_system();
			m_forward = get_local_forward();
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
			m_camera = static_cast<bc_chasing_camera*>(m_input_system->add_camera(std::move(l_camera)));

			const auto* l_mediate_component = p_context.m_actor.get_component<bc_mediate_component>();
			const auto l_bound_box_extends = l_mediate_component->get_bound_box().get_half_extends();
			const auto l_max_side_length = std::max(std::max(l_bound_box_extends.x, l_bound_box_extends.y), l_bound_box_extends.z);
			m_camera_y_offset = l_max_side_length * 5;
			m_camera_z_offset = l_max_side_length * -2.5f;
			m_camera_look_at_offset = l_max_side_length * 2.5f;
		}

		void bc_xbot_camera_controller::update(const bc_actor_component_update_content& p_context)
		{
			if(!m_camera)
			{
				return;
			}
			
			bc_xbot_controller::update(p_context);
			m_camera->update
			(
				m_position + m_forward * m_camera_z_offset + core::bc_vector3f(0, m_camera_y_offset, 0), 
				m_position + m_forward * m_camera_look_at_offset
			);
		}

		void bc_xbot_camera_controller::handle_event(const bc_actor_component_event_context& p_context)
		{
			bc_xbot_controller::handle_event(p_context);
			
			const auto* l_world_transform_event = core::bci_message::as<bc_world_transform_actor_event>(p_context.m_event);
			if (l_world_transform_event)
			{
				const auto& l_world_transform = l_world_transform_event->get_transform();
				auto l_world_forward = l_world_transform.get_rotation() * m_forward;
				l_world_forward.y = 0;
				l_world_forward.normalize();

				m_position = l_world_transform.get_translation();
				m_forward = l_world_forward;
			}
		}

		void bc_xbot_camera_controller::_update_input(const core_platform::bc_clock::update_param& p_clock)
		{
			const auto& l_key_device = m_input_system->get_key_device();

			const auto l_w_pressed = l_key_device.get_key_state(platform::bc_key::kb_W) == platform::bc_key_state::pressed;

			if(l_w_pressed)
			{
				m_forward_velocity.push(p_clock.m_elapsed_second);
			}
			else
			{
				m_forward_velocity.release(p_clock.m_elapsed_second);
			}
		}
	}	
}