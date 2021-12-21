// [03/12/2021 MRB]

#include "BlackCat/BlackCatPCH.h"

#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "Core/Messaging/Event/bcEventManager.h"
#include "Core/Utility/bcJsonParse.h"
#include "Core/Utility/bcLogger.h"
#include "Platform/bcEvent.h"
#include "Game/System/bcGameSystem.h"
#include "Game/System/Input/bcInputSystem.h"
#include "Game/System/Network/bcNetworkSystem.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/Object/Scene/bcEntityManager.h"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/Object/Scene/Component/bcRigidDynamicComponent.h"
#include "BlackCat/SampleImp/XBot/bcXBotPlayerActorController.h"
#include "BlackCat/SampleImp/XBot/bcXBotWeaponNetworkMessage.h"
#include "BlackCat/SampleImp/XBot/bcXBotGrenadeNetworkMessage.h"

namespace black_cat
{
	bc_xbot_player_actor_controller::bc_xbot_player_actor_controller() noexcept
		: bc_xbot_actor_controller(),
		m_input_system(nullptr),
		m_network_system(nullptr),
		m_camera(nullptr),
		m_camera_y_offset(0),
		m_camera_z_offset(0),
		m_camera_look_at_offset(0),
		m_pointing_delta_x(0),
		m_pointing_last_x(0),
		m_forward_pressed(false),
		m_backward_pressed(false),
		m_right_pressed(false),
		m_left_pressed(false),
		m_walk_pressed(false),
		m_grenade_pressed(false),
		m_grenade_throw_passed_time(0),
		m_rifle_name(nullptr),
		m_grenade_name(nullptr),
		m_threw_grenade_name(nullptr),
		m_smoke_grenade_name(nullptr),
		m_threw_smoke_grenade_name(nullptr),
		m_grenade_throw_time(0),
		m_grenade_throw_power(0)
	{
	}

	bc_xbot_player_actor_controller::bc_xbot_player_actor_controller(bc_xbot_player_actor_controller&& p_other) noexcept
		: bc_xbot_actor_controller(std::move(p_other))
	{
		operator=(std::move(p_other));
	}

	bc_xbot_player_actor_controller::~bc_xbot_player_actor_controller() = default;
	
	bc_xbot_player_actor_controller& bc_xbot_player_actor_controller::operator=(bc_xbot_player_actor_controller&& p_other) noexcept
	{
		bc_xbot_actor_controller::operator=(std::move(p_other));
		
		m_input_system = p_other.m_input_system;
		m_network_system = p_other.m_network_system;
		m_camera = p_other.m_camera;
		m_key_listener_handle = std::move(p_other.m_key_listener_handle);
		m_pointing_listener_handle = std::move(p_other.m_pointing_listener_handle);

		m_camera_y_offset = p_other.m_camera_y_offset;
		m_camera_z_offset = p_other.m_camera_z_offset;
		m_camera_look_at_offset = p_other.m_camera_look_at_offset;
		m_pointing_delta_x = p_other.m_pointing_delta_x;
		m_pointing_last_x = p_other.m_pointing_last_x;

		m_rifle_name = p_other.m_rifle_name;
		
		m_key_listener_handle.reassign(core::bc_event_manager::delegate_type(*this, &bc_xbot_player_actor_controller::_on_event));
		m_pointing_listener_handle.reassign(core::bc_event_manager::delegate_type(*this, &bc_xbot_player_actor_controller::_on_event));

		return *this;
	}

	void bc_xbot_player_actor_controller::initialize(const game::bc_actor_component_initialize_context& p_context)
	{
		bc_xbot_actor_controller::initialize(p_context);
		m_input_system = &p_context.m_game_system.get_input_system();
		m_network_system = &p_context.m_game_system.get_network_system();
		m_rifle_name = p_context.m_parameters.get_value_throw<core::bc_string>("rifle_name").c_str();
		m_grenade_name = p_context.m_parameters.get_value_throw<core::bc_string>("grenade_name").c_str();
		m_threw_grenade_name = p_context.m_parameters.get_value_throw<core::bc_string>("threw_grenade_name").c_str();
		m_smoke_grenade_name = p_context.m_parameters.get_value_throw<core::bc_string>("smoke_grenade_name").c_str();
		m_threw_smoke_grenade_name = p_context.m_parameters.get_value_throw<core::bc_string>("threw_smoke_grenade_name").c_str();
		m_grenade_throw_time = p_context.m_parameters.get_value_throw<bcFLOAT>("grenade_throw_time");
		m_grenade_throw_power = p_context.m_parameters.get_value_throw<bcFLOAT>("grenade_throw_power");

		if(get_replication_side() == game::bc_actor_replication_side::origin)
		{
			auto* l_event_manager = core::bc_get_service<core::bc_event_manager>();
			m_key_listener_handle = l_event_manager->register_event_listener<platform::bc_app_event_key>
			(
				core::bc_event_manager::delegate_type(*this, &bc_xbot_player_actor_controller::_on_event)
			);
			m_pointing_listener_handle = l_event_manager->register_event_listener<platform::bc_app_event_pointing>
			(
				core::bc_event_manager::delegate_type(*this, &bc_xbot_player_actor_controller::_on_event)
			);
		}
	}

	void bc_xbot_player_actor_controller::load_origin_network_instance(const game::bc_actor_component_network_load_context& p_context)
	{
	}

	void bc_xbot_player_actor_controller::load_replicated_network_instance(const game::bc_actor_component_network_load_context& p_context)
	{
	}

	void bc_xbot_player_actor_controller::write_origin_network_instance(const game::bc_actor_component_network_write_context& p_context)
	{
		const auto l_velocity = get_velocity();
		json_parse::bc_write(p_context.m_parameters, "pos", get_position());
		json_parse::bc_write(p_context.m_parameters, "lk_dir", get_look_direction());
		p_context.m_parameters.add("lk_sd", core::bc_any(l_velocity.m_look_side));
		p_context.m_parameters.add
		(
			"keys", 
			core::bc_any(core::bc_vector<core::bc_any>
			({
				core::bc_any(l_velocity.m_forward_velocity > 0.f),
				core::bc_any(l_velocity.m_backward_velocity > 0.f),
				core::bc_any(l_velocity.m_right_velocity > 0.f),
				core::bc_any(l_velocity.m_left_velocity > 0.f),
				core::bc_any(l_velocity.m_walk_velocity > 0.f),
			}))
		);

		if(p_context.m_is_replication_write)
		{
			const auto* l_weapon = get_weapon();
			if(l_weapon)
			{
				const auto* l_mediate_component = l_weapon->m_actor.get_component<game::bc_mediate_component>();
				core::bc_string l_weapon_entity_name = l_mediate_component->get_entity_name();
				p_context.m_parameters.add("wpn", core::bc_any(l_weapon_entity_name));
			}
		}
	}
	
	void bc_xbot_player_actor_controller::write_replicated_network_instance(const game::bc_actor_component_network_write_context& p_context)
	{
	}

	void bc_xbot_player_actor_controller::added_to_scene(const game::bc_actor_component_event_context& p_context, game::bc_scene& p_scene)
	{
		bc_xbot_actor_controller::added_to_scene(p_context, p_scene);

		if(get_replication_side() == game::bc_actor_replication_side::origin)
		{
			auto* l_current_camera = m_input_system->get_camera();
			auto l_camera = core::bc_make_unique<game::bc_chasing_camera>(game::bc_chasing_camera
			(
				l_current_camera->get_screen_width(),
				l_current_camera->get_screen_height(),
				1.2f,
				l_current_camera->get_near_clip(),
				l_current_camera->get_far_clip()
			));
			m_camera = static_cast<game::bc_chasing_camera*>(m_input_system->add_camera(std::move(l_camera)));

			const auto* l_mediate_component = p_context.m_actor.get_component<game::bc_mediate_component>();
			const auto l_bound_box_extends = l_mediate_component->get_bound_box().get_half_extends();
			const auto l_max_side_length = std::max(std::max(l_bound_box_extends.x, l_bound_box_extends.y), l_bound_box_extends.z) * 2;
			m_camera_y_offset = l_max_side_length * 3.5f;
			m_camera_z_offset = l_max_side_length * -1.5f;
			m_camera_look_at_offset = l_max_side_length * 1.25f;
		}
	}

	void bc_xbot_player_actor_controller::update_origin_instance(const game::bc_actor_component_update_content& p_context)
	{
		bc_xbot_actor_controller::update(bc_xbot_input_update_context
		{
			p_context.m_clock,
			m_pointing_delta_x,
			m_forward_pressed,
			m_backward_pressed,
			m_right_pressed,
			m_left_pressed,
			m_walk_pressed
		});

		m_pointing_delta_x = 0; // Zero previous delta

		if(m_grenade_pressed)
		{
			m_grenade_throw_passed_time += p_context.m_clock.m_elapsed_second;
		}
	}

	void bc_xbot_player_actor_controller::update_replicated_instance(const game::bc_actor_component_update_content& p_context)
	{
	}

	void bc_xbot_player_actor_controller::removed_from_scene(const game::bc_actor_component_event_context& p_context, game::bc_scene& p_scene)
	{
		bc_xbot_actor_controller::removed_from_scene(p_context, p_scene);

		if (get_replication_side() == game::bc_actor_replication_side::origin)
		{
			m_input_system->remove_camera(m_camera);
			m_camera = nullptr;
		}
	}

	void bc_xbot_player_actor_controller::handle_event(const game::bc_actor_component_event_context& p_context)
	{
		bc_xbot_actor_controller::handle_event(p_context);
		
		const auto* l_world_transform_event = core::bci_message::as<game::bc_world_transform_actor_event>(p_context.m_event);
		if (l_world_transform_event)
		{
			if(m_camera)
			{
				m_camera->update
				(
					get_position() + get_look_direction() * m_camera_z_offset + core::bc_vector3f(0, m_camera_y_offset, 0),
					get_position() + get_look_direction() * m_camera_look_at_offset
				);
				/*m_camera->update
				(
					get_position() + core::bc_vector3f(1,0,0) * m_camera_z_offset + core::bc_vector3f(0, get_bound_box_max_side_length() * 2.5f, 0),
					get_position()
				);*/
			}
		}
	}

	void bc_xbot_player_actor_controller::_on_event(core::bci_event& p_event) noexcept
	{
		auto* l_pointing_event = core::bci_message::as<platform::bc_app_event_pointing>(p_event);
		if (l_pointing_event)
		{
			_on_pointing(*l_pointing_event);
			return;
		}

		auto* l_key_event = core::bci_message::as<platform::bc_app_event_key>(p_event);
		if (l_key_event)
		{
			_on_key(*l_key_event);
			return;
		}
	}

	void bc_xbot_player_actor_controller::_on_pointing(platform::bc_app_event_pointing& p_pointing_event) noexcept
	{
		m_pointing_delta_x = p_pointing_event.get_state().m_x - m_pointing_last_x;
		m_pointing_last_x = p_pointing_event.get_state().m_x;
	}

	void bc_xbot_player_actor_controller::_on_key(platform::bc_app_event_key& p_key_event) noexcept
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
				m_walk_pressed = true;
			}
			else if (p_key_event.get_key_state() == platform::bc_key_state::releasing)
			{
				m_walk_pressed = false;
			}
		}

		if (p_key_event.get_key() == platform::bc_key::kb_G)
		{
			if(p_key_event.get_key_state() == platform::bc_key_state::pressing && m_grenade_throw_passed_time <= 0)
			{
				m_grenade_pressed = true;
			}
			else if (p_key_event.get_key_state() == platform::bc_key_state::releasing)
			{
				m_grenade_pressed = false;
				_start_grenade_throw(m_grenade_name);
			}
		}

		if (p_key_event.get_key() == platform::bc_key::kb_H)
		{
			if (p_key_event.get_key_state() == platform::bc_key_state::pressing && m_grenade_throw_passed_time <= 0)
			{
				m_grenade_pressed = true;
			}
			else if (p_key_event.get_key_state() == platform::bc_key_state::releasing)
			{
				m_grenade_pressed = false;
				_start_grenade_throw(m_smoke_grenade_name);
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
				_detach_weapon();
			}
		}

		if (p_key_event.get_key() == platform::bc_key::ms_left_button)
		{
			if (p_key_event.get_key_state() == platform::bc_key_state::pressing || p_key_event.get_key_state() == platform::bc_key_state::pressed)
			{
				_shoot_weapon();
			}
		}
	}

	void bc_xbot_player_actor_controller::_start_grenade_throw(const bcCHAR* p_entity)
	{
		bc_xbot_actor_controller::start_grenade_throw(p_entity);

		if (m_network_system->get_network_type() != game::bc_network_type::not_started)
		{
			m_network_system->send_message(bc_xbot_start_grenade_throw_network_message(get_actor(), p_entity));
		}
	}

	void bc_xbot_player_actor_controller::throw_grenade(game::bc_actor& p_grenade) noexcept
	{
		auto* l_scene = get_scene();
		const auto* l_grenade_mediate_component = p_grenade.get_component<game::bc_mediate_component>();
		const auto l_grenade_throw_power = std::max(0.3f, std::min(1.0f, m_grenade_throw_passed_time / m_grenade_throw_time)) *
			m_grenade_throw_power *
			get_bound_box_max_side_length();
		const auto l_throw_direction = core::bc_vector3f::normalize(get_look_direction() + core::bc_vector3f(0, 0.2f, 0)) * l_grenade_throw_power;
		
		const bcCHAR* l_threw_grenade_name = std::strcmp(l_grenade_mediate_component->get_entity_name(), m_grenade_name) == 0 ? m_threw_grenade_name : m_threw_smoke_grenade_name;

		if (m_network_system->get_network_type() != game::bc_network_type::not_started)
		{
			m_network_system->send_message(bc_xbot_grenade_throw_network_message(l_threw_grenade_name, l_grenade_mediate_component->get_world_transform(), l_throw_direction));
		}
		else
		{
			auto l_threw_grenade = l_scene->create_actor(l_threw_grenade_name, l_grenade_mediate_component->get_world_transform());
			l_threw_grenade.mark_for_double_update();

			auto* l_rigid_dynamic_component = l_threw_grenade.get_component<game::bc_rigid_dynamic_component>();
			auto l_rigid_body = l_rigid_dynamic_component->get_dynamic_body();

			{
				game::bc_rigid_component_lock l_lock(*l_rigid_dynamic_component);

				l_rigid_body.set_linear_velocity(l_throw_direction);
			}
		}
		
		l_scene->remove_actor(p_grenade);
		m_grenade_throw_passed_time = 0;
	}

	void bc_xbot_player_actor_controller::_attach_weapon(const bcCHAR* p_entity)
	{
		auto* l_weapon = get_weapon();
		if(l_weapon)
		{
			bc_xbot_actor_controller::detach_weapon();
		}

		auto l_weapon_actor = get_scene()->create_actor(p_entity, core::bc_matrix4f::translation_matrix(get_position()));
		bc_xbot_actor_controller::attach_weapon(l_weapon_actor);

		if(m_network_system->get_network_type() != game::bc_network_type::not_started)
		{
			m_network_system->send_message(bc_xbot_weapon_attach_network_message(get_actor(), l_weapon_actor));
		}
	}

	void bc_xbot_player_actor_controller::_detach_weapon()
	{
		auto* l_weapon = get_weapon();
		if (!l_weapon)
		{
			return;
		}
		
		bc_xbot_actor_controller::detach_weapon();

		if (m_network_system->get_network_type() != game::bc_network_type::not_started)
		{
			m_network_system->send_message(bc_xbot_weapon_detach_network_message(get_actor()));
		}
	}

	void bc_xbot_player_actor_controller::_shoot_weapon()
	{
		const bool l_shoot = bc_xbot_actor_controller::shoot_weapon();

		if (l_shoot && m_network_system->get_network_type() != game::bc_network_type::not_started)
		{
			m_network_system->send_message(bc_xbot_weapon_shoot_network_message(get_actor()));
		}
	}
}