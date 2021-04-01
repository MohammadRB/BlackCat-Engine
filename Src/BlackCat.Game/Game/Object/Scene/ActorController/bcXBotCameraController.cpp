// [03/12/2021 MRB]

#include "Game/GamePCH.h"

#include "Core/Messaging/Event/bcEventManager.h"
#include "Core/Utility/bcLogger.h"
#include "Platform/bcEvent.h"
#include "Game/System/bcGameSystem.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/Object/Scene/bcEntityManager.h"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/Object/Scene/Component/bcRigidDynamicComponent.h"
#include "Game/Object/Scene/ActorController/bcXBotCameraController.h"

namespace black_cat
{
	namespace game
	{
		bc_xbot_camera_controller::bc_xbot_camera_controller() noexcept
			: m_input_system(nullptr),
			m_camera(nullptr),
			m_camera_y_offset(0),
			m_camera_z_offset(0),
			m_camera_look_at_offset(0),
			m_pointing_delta_x(0),
			m_pointing_last_x(0),
			m_rifle_name(nullptr),
			m_detached_rifle_name(nullptr),
			m_weapon_shoot_time(0)
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
			: bc_xbot_controller(std::move(p_other))
		{
			operator=(std::move(p_other));
		}

		bc_xbot_camera_controller::~bc_xbot_camera_controller() = default;
		
		bc_xbot_camera_controller& bc_xbot_camera_controller::operator=(bc_xbot_camera_controller&& p_other) noexcept
		{
			bc_xbot_controller::operator=(std::move(p_other));
			
			m_key_listener_handle = std::move(p_other.m_key_listener_handle);
			m_pointing_listener_handle = std::move(p_other.m_pointing_listener_handle);
			m_input_system = p_other.m_input_system;
			m_camera = p_other.m_camera;

			m_camera_y_offset = p_other.m_camera_y_offset;
			m_camera_z_offset = p_other.m_camera_z_offset;
			m_camera_look_at_offset = p_other.m_camera_look_at_offset;
			m_pointing_delta_x = p_other.m_pointing_delta_x;
			m_pointing_last_x = p_other.m_pointing_last_x;

			m_rifle_name = p_other.m_rifle_name;
			m_detached_rifle_name = p_other.m_detached_rifle_name;
			
			m_key_listener_handle.reassign(core::bc_event_manager::delegate_type(*this, &bc_xbot_camera_controller::_on_event));
			m_pointing_listener_handle.reassign(core::bc_event_manager::delegate_type(*this, &bc_xbot_camera_controller::_on_event));

			return *this;
		}

		void bc_xbot_camera_controller::initialize(const bc_actor_component_initialize_context& p_context)
		{
			bc_xbot_controller::initialize(p_context);
			m_input_system = &p_context.m_game_system.get_input_system();

			m_rifle_name = p_context.m_parameters.get_value_throw<core::bc_string>("rifle_name").c_str();
			m_detached_rifle_name = p_context.m_parameters.get_value_throw<core::bc_string>("detached_rifle_name").c_str();
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
			const auto l_max_side_length = std::max(std::max(l_bound_box_extends.x, l_bound_box_extends.y), l_bound_box_extends.z) * 2;
			m_camera_y_offset = l_max_side_length * 2.5f;
			m_camera_z_offset = l_max_side_length * -1.5f;
			m_camera_look_at_offset = l_max_side_length * 1.75f;
		}

		void bc_xbot_camera_controller::update(const bc_actor_component_update_content& p_context)
		{
			bc_xbot_controller::update(p_context);
			
			m_pointing_delta_x = 0;
			set_look_delta(m_pointing_delta_x);

			if(m_weapon_shoot_time <= -1) // Start of weapon shoot
			{
				m_weapon_shoot_time = p_context.m_clock.m_elapsed_second;
			}
			if(m_weapon_shoot_time > 0)
			{
				m_weapon_shoot_time += p_context.m_clock.m_elapsed_second;
			}
			if(m_current_weapon.is_valid() && m_weapon_shoot_time >= m_current_weapon.get_component<bc_weapon_component>()->get_rate_of_fire_seconds())
			{
				m_weapon_shoot_time = 0;
			}
		}

		void bc_xbot_camera_controller::removed_from_scene(const bc_actor_component_event_context& p_context, bc_scene& p_scene)
		{
			bc_xbot_controller::removed_from_scene(p_context, p_scene);
			
			m_input_system->remove_camera(m_camera);
		}

		void bc_xbot_camera_controller::handle_event(const bc_actor_component_event_context& p_context)
		{
			bc_xbot_controller::handle_event(p_context);
			
			const auto* l_world_transform_event = core::bci_message::as<bc_world_transform_actor_event>(p_context.m_event);
			if (l_world_transform_event)
			{
				if(m_camera)
				{
					m_camera->update
					(
						get_position() + get_look_direction() * m_camera_z_offset + core::bc_vector3f(0, m_camera_y_offset, 0),
						get_position() + get_look_direction() * m_camera_look_at_offset
					);
					m_camera->update
					(
						get_position() + core::bc_vector3f(1,0,0) * m_camera_z_offset + core::bc_vector3f(0, m_camera_y_offset, 0),
						get_position()
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
			
			set_look_delta(m_pointing_delta_x);
			
			return true;
		}

		bool bc_xbot_camera_controller::_on_key(platform::bc_app_event_key& p_key_event) noexcept
		{
			if (p_key_event.get_key() == platform::bc_key::kb_W)
			{
				if (p_key_event.get_key_state() == platform::bc_key_state::pressing)
				{
					set_move_forward(true);
				}
				else if (p_key_event.get_key_state() == platform::bc_key_state::releasing)
				{
					set_move_forward(false);
				}
			}
			else if (p_key_event.get_key() == platform::bc_key::kb_S)
			{
				if (p_key_event.get_key_state() == platform::bc_key_state::pressing)
				{
					set_move_backward(true);
				}
				else if (p_key_event.get_key_state() == platform::bc_key_state::releasing)
				{
					set_move_backward(false);
				}
			}
			else if (p_key_event.get_key() == platform::bc_key::kb_D)
			{
				if (p_key_event.get_key_state() == platform::bc_key_state::pressing)
				{
					set_move_right(true);
				}
				else if (p_key_event.get_key_state() == platform::bc_key_state::releasing)
				{
					set_move_right(false);
				}
			}
			else if (p_key_event.get_key() == platform::bc_key::kb_A)
			{
				if (p_key_event.get_key_state() == platform::bc_key_state::pressing)
				{
					set_move_left(true);
				}
				else if (p_key_event.get_key_state() == platform::bc_key_state::releasing)
				{
					set_move_left(false);
				}
			}
			if (p_key_event.get_key() == platform::bc_key::kb_ctrl)
			{
				if (p_key_event.get_key_state() == platform::bc_key_state::pressing)
				{
					set_walk(true);
				}
				else if (p_key_event.get_key_state() == platform::bc_key_state::releasing)
				{
					set_walk(false);
				}
			}

			if (p_key_event.get_key() == platform::bc_key::kb_d1)
			{
				if (p_key_event.get_key_state() == platform::bc_key_state::pressing)
				{
					_attach_weapon(m_rifle_name);
				}
			}
			else if (p_key_event.get_key() == platform::bc_key::kb_d0)
			{
				if (p_key_event.get_key_state() == platform::bc_key_state::pressing)
				{
					_detach_weapon(m_detached_rifle_name);
				}
			}

			if (p_key_event.get_key() == platform::bc_key::ms_left_button)
			{
				if (p_key_event.get_key_state() == platform::bc_key_state::pressing || p_key_event.get_key_state() == platform::bc_key_state::pressed)
				{
					if(m_current_weapon.is_valid() && m_weapon_shoot_time <= 0)
					{
						fire_weapon();
						m_weapon_shoot_time = -1; // Signal start of weapon shoot
					}
				}
			}
			
			return true;
		}

		void bc_xbot_camera_controller::_attach_weapon(const bcCHAR* p_entity)
		{
			if(m_current_weapon.is_valid())
			{
				_detach_weapon(m_detached_rifle_name);
			}
			
			m_current_weapon = core::bc_get_service<bc_entity_manager>()->create_entity(p_entity);
			m_current_weapon.add_event(bc_world_transform_actor_event(get_position()));

			get_scene()->add_actor(m_current_weapon);
			bc_xbot_controller::attach_weapon(m_current_weapon);
		}

		void bc_xbot_camera_controller::_detach_weapon(const bcCHAR* p_detached_entity)
		{
			if (!m_current_weapon.is_valid())
			{
				return;
			}
			
			detach_weapon();

			auto l_current_weapon_world_transform = m_current_weapon.get_component<bc_mediate_component>()->get_world_transform();
			l_current_weapon_world_transform.set_translation(l_current_weapon_world_transform.get_translation() + get_look_direction());

			auto l_detached_rifle_actor = core::bc_get_service<bc_entity_manager>()->create_entity(p_detached_entity);
			l_detached_rifle_actor.add_event(bc_world_transform_actor_event(l_current_weapon_world_transform));

			auto* l_rigid_dynamic_component = l_detached_rifle_actor.get_component<bc_rigid_dynamic_component>();
			if (l_rigid_dynamic_component)
			{
				auto l_rigid_dynamic = l_rigid_dynamic_component->get_dynamic_body();
				l_rigid_dynamic.set_linear_velocity(get_look_direction() * 2);
			}

			get_scene()->remove_actor(m_current_weapon);
			get_scene()->add_actor(l_detached_rifle_actor);

			m_current_weapon = bc_actor();
		}
	}	
}