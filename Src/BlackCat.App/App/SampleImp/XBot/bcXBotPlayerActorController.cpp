// [03/12/2021 MRB]

#include "App/AppPCH.h"

#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "Core/Messaging/Event/bcEventManager.h"
#include "Core/Messaging/Query/bcQueryManager.h"
#include "Core/Utility/bcJsonParse.h"
#include "Core/Utility/bcLogger.h"
#include "Platform/bcEvent.h"
#include "Game/System/bcGameSystem.h"
#include "Game/System/Input/bcInputSystem.h"
#include "Game/System/Network/bcNetworkSystem.h"
#include "Game/System/Physics/bcPxWrap.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/Object/Scene/bcEntityManager.h"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/Object/Scene/Component/bcRigidDynamicComponent.h"
#include "Game/Object/Scene/Component/bcSkinnedMeshComponent.h"
#include "Game/Object/Scene/Component/bcRigidControllerComponent.h"
#include "Game/Object/Scene/Component/bcHumanRagdollComponent.h"
#include "Game/bcJsonParse.h"
#include "App/SampleImp/XBot/bcXBotPlayerActorController.h"
#include "App/SampleImp/XBot/bcXBotWeaponNetworkMessage.h"
#include "App/SampleImp/XBot/bcXBotGrenadeNetworkMessage.h"

namespace black_cat
{
	bc_xbot_player_actor_controller::bc_xbot_player_actor_controller() noexcept
		: bc_xbot_actor_controller(),
		m_input_system(nullptr),
		m_physics_system(nullptr),
		m_network_system(nullptr),
		m_camera(nullptr),
		m_rifle_name(nullptr),
		m_grenade_name(nullptr),
		m_threw_grenade_name(nullptr),
		m_smoke_grenade_name(nullptr),
		m_threw_smoke_grenade_name(nullptr),
		m_grenade_throw_time(0),
		m_grenade_throw_force(0),
		m_camera_y_offset(0),
		m_camera_z_offset(0),
		m_camera_look_at_offset(0),
		m_pointing_delta_x(0),
		m_forward_pressed(false),
		m_backward_pressed(false),
		m_right_pressed(false),
		m_left_pressed(false),
		m_walk_pressed(false),
		m_grenade_pressed(false),
		m_grenade_throw_passed_time(0),
		m_weapon_shoot_velocity(0, 1, 0.3f),
		m_weapon_obstacle_rotation_velocity(0, 45, .2f),
		m_weapon_obstacle_distance(0)
	{
	}

	bc_xbot_player_actor_controller::bc_xbot_player_actor_controller(bc_xbot_player_actor_controller&& p_other) noexcept
		: bc_xbot_actor_controller(std::move(p_other)),
		m_weapon_shoot_velocity(p_other.m_weapon_shoot_velocity),
		m_weapon_obstacle_rotation_velocity(p_other.m_weapon_obstacle_rotation_velocity)
	{
		operator=(std::move(p_other));
	}

	bc_xbot_player_actor_controller::~bc_xbot_player_actor_controller() = default;
	
	bc_xbot_player_actor_controller& bc_xbot_player_actor_controller::operator=(bc_xbot_player_actor_controller&& p_other) noexcept
	{
		bc_xbot_actor_controller::operator=(std::move(p_other));

		m_key_listener_handle = std::move(p_other.m_key_listener_handle);
		m_pointing_listener_handle = std::move(p_other.m_pointing_listener_handle);
		m_input_system = p_other.m_input_system;
		m_physics_system = p_other.m_physics_system;
		m_network_system = p_other.m_network_system;
		m_camera = p_other.m_camera;

		m_rifle_name = p_other.m_rifle_name;
		m_grenade_name = p_other.m_grenade_name;
		m_threw_grenade_name = p_other.m_threw_grenade_name;
		m_smoke_grenade_name = p_other.m_smoke_grenade_name;
		m_threw_smoke_grenade_name = p_other.m_threw_smoke_grenade_name;

		m_camera_y_offset = p_other.m_camera_y_offset;
		m_camera_z_offset = p_other.m_camera_z_offset;
		m_camera_look_at_offset = p_other.m_camera_look_at_offset;
		m_pointing_delta_x = p_other.m_pointing_delta_x;

		m_weapon_shoot_velocity = p_other.m_weapon_shoot_velocity;
		m_weapon_obstacle_query = std::move(p_other.m_weapon_obstacle_query);
		m_weapon_obstacle_rotation_velocity = p_other.m_weapon_obstacle_rotation_velocity;
		m_weapon_obstacle_distance = p_other.m_weapon_obstacle_distance;

		m_key_listener_handle.reassign(core::bc_event_manager::delegate_type(*this, &bc_xbot_player_actor_controller::_on_event));
		m_pointing_listener_handle.reassign(core::bc_event_manager::delegate_type(*this, &bc_xbot_player_actor_controller::_on_event));

		return *this;
	}

	void bc_xbot_player_actor_controller::enable_ragdoll(core::bc_string_view p_body_part_force, const core::bc_vector3f& p_force) noexcept
	{
		bc_xbot_actor_controller::enable_ragdoll(p_body_part_force, p_force);
	}

	void bc_xbot_player_actor_controller::initialize(const game::bc_actor_component_initialize_context& p_context)
	{
		bc_xbot_actor_controller::initialize(p_context);
		m_input_system = &p_context.m_game_system.get_input_system();
		m_physics_system = &p_context.m_game_system.get_physics_system();
		m_network_system = &p_context.m_game_system.get_network_system();

		const core::bc_string* l_rifle_name;
		const core::bc_string* l_grenade_name;
		const core::bc_string* l_threw_grenade_name;
		const core::bc_string* l_smoke_grenade_name;
		const core::bc_string* l_threw_smoke_grenade_name;

		json_parse::bc_load_throw(p_context.m_parameters, "rifle_name", l_rifle_name);
		json_parse::bc_load_throw(p_context.m_parameters, "grenade_name", l_grenade_name);
		json_parse::bc_load_throw(p_context.m_parameters, "threw_grenade_name", l_threw_grenade_name);
		json_parse::bc_load_throw(p_context.m_parameters, "smoke_grenade_name", l_smoke_grenade_name);
		json_parse::bc_load_throw(p_context.m_parameters, "threw_smoke_grenade_name", l_threw_smoke_grenade_name);
		json_parse::bc_load_throw(p_context.m_parameters, "grenade_throw_time", m_grenade_throw_time);
		json_parse::bc_load_throw(p_context.m_parameters, "grenade_throw_force", m_grenade_throw_force);

		m_rifle_name = l_rifle_name->c_str();
		m_grenade_name = l_grenade_name->c_str();
		m_threw_grenade_name = l_threw_grenade_name->c_str();
		m_smoke_grenade_name = l_smoke_grenade_name->c_str();
		m_threw_smoke_grenade_name = l_threw_smoke_grenade_name->c_str();

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
	
	void bc_xbot_player_actor_controller::write_origin_network_instance(const game::bc_actor_component_network_write_context& p_context)
	{
		const auto l_velocity = get_velocity();
		json_parse::bc_write(p_context.m_parameters, "pos", get_position());
		json_parse::bc_write(p_context.m_parameters, "lk_dir", get_look_direction());
		json_parse::bc_write(p_context.m_parameters, "lk_vl", core::bc_any(l_velocity.m_look_side * l_velocity.m_look_velocity));
		json_parse::bc_write
		(
			p_context.m_parameters, 
			"keys",
			core::bc_any
			(
				core::bc_vector<core::bc_any>
				(
					{
						core::bc_any(static_cast<bcINT32>(l_velocity.m_forward_velocity > 0.f)),
						core::bc_any(static_cast<bcINT32>(l_velocity.m_backward_velocity > 0.f)),
						core::bc_any(static_cast<bcINT32>(l_velocity.m_right_velocity > 0.f)),
						core::bc_any(static_cast<bcINT32>(l_velocity.m_left_velocity > 0.f)),
						core::bc_any(static_cast<bcINT32>(l_velocity.m_walk_velocity > 0.f)),
					}
				)
			)
		);

		const auto l_weapon_rotation = m_weapon_obstacle_rotation_velocity.get_value();
		if(l_weapon_rotation > 0.f)
		{
			json_parse::bc_write(p_context.m_parameters, "wpn_r", l_weapon_rotation);
		}

		if(p_context.m_is_replication_write)
		{
			if(const auto* l_weapon = get_weapon())
			{
				const auto* l_mediate_component = l_weapon->m_actor.get_component<game::bc_mediate_component>();
				const core::bc_string l_weapon_entity_name = l_mediate_component->get_entity_name();
				json_parse::bc_write(p_context.m_parameters, "wpn", l_weapon_entity_name);
			}

			if(get_ragdoll_enabled())
			{
				const auto* l_ragdoll_component = get_actor().get_component<game::bc_human_ragdoll_component>();
				json_parse::bc_write(p_context.m_parameters, "rag", l_ragdoll_component->get_body_px_transforms());
			}
		}
	}
	
	void bc_xbot_player_actor_controller::added_to_scene(const game::bc_actor_component_event_context& p_context, game::bc_scene& p_scene)
	{
		bc_xbot_actor_controller::added_to_scene(p_context, p_scene);
		m_weapon_obstacle_distance = p_scene.get_global_scale() * 0.8f;
	}

	void bc_xbot_player_actor_controller::update_origin_instance(const game::bc_actor_component_update_content& p_context)
	{
		if(!get_scene() || p_context.m_is_double_update)
		{
			return;
		}

		const bool l_can_move = can_move();
		const bool l_can_look = can_look();

		if(!l_can_move)
		{
			m_forward_pressed = m_backward_pressed = m_right_pressed = m_left_pressed = false;
		}
		if (!l_can_look)
		{
			m_pointing_delta_x = 0;
		}

		if(m_weapon_obstacle_distance > 0)
		{
			const auto* l_weapon = get_weapon();
			auto l_weapon_obstacle_distance = std::numeric_limits<bcFLOAT>::max();

			if (l_weapon)
			{
				if (m_weapon_obstacle_query.is_executed())
				{
					l_weapon_obstacle_distance = m_weapon_obstacle_query.get().get_result().as_throw<bcFLOAT>();
				}

				_build_weapon_obstacle_query(p_context.m_query_manager, *l_weapon);
			}

			if (l_weapon_obstacle_distance <= m_weapon_obstacle_distance)
			{
				m_weapon_obstacle_rotation_velocity.push(p_context.m_clock.m_elapsed_second);
			}
			else
			{
				m_weapon_obstacle_rotation_velocity.release(p_context.m_clock.m_elapsed_second);
			}

			bc_xbot_player_actor_controller::set_weapon_rotation(m_weapon_obstacle_rotation_velocity.get_value());
		}

		bc_xbot_actor_controller::update(bc_xbot_input_update_context
		{
			p_context.m_clock,
			m_pointing_delta_x,
			m_forward_pressed,
			m_backward_pressed,
			m_right_pressed,
			m_left_pressed,
			m_walk_pressed || m_weapon_shoot_velocity.get_value() > 0
		});

		m_pointing_delta_x = 0; // Zero previous delta
		m_weapon_shoot_velocity.release(p_context.m_clock.m_elapsed_second);

		if(m_grenade_pressed)
		{
			m_grenade_throw_passed_time += p_context.m_clock.m_elapsed_second;
		}

		if (m_camera)
		{
			m_camera->update
			(
				get_position() + get_look_direction() * m_camera_z_offset + core::bc_vector3f(0, m_camera_y_offset, 0),
				get_position() + get_look_direction() * m_camera_look_at_offset
			);
		}
	}
	
	void bc_xbot_player_actor_controller::removed_from_scene(const game::bc_actor_component_event_context& p_context, game::bc_scene& p_scene)
	{
		bc_xbot_actor_controller::removed_from_scene(p_context, p_scene);

		if (get_replication_side() == game::bc_actor_replication_side::origin && m_camera)
		{
			m_input_system->remove_camera(m_camera);
			m_camera = nullptr;
		}
	}

	void bc_xbot_player_actor_controller::handle_event(const game::bc_actor_component_event_context& p_context)
	{
		bc_xbot_actor_controller::handle_event(p_context);
	}

	void bc_xbot_player_actor_controller::set_camera_offsets(bcFLOAT p_camera_y_offset, bcFLOAT p_camera_z_offset, bcFLOAT p_camera_look_at_offset) noexcept
	{
		if(get_replication_side() == game::bc_actor_replication_side::origin && !m_camera)
		{
			const auto* l_current_camera = m_input_system->get_camera();
			auto l_camera = core::bc_make_unique<game::bc_chasing_camera>(game::bc_chasing_camera
			(
				l_current_camera->get_screen_width(),
				l_current_camera->get_screen_height(),
				1.2f,
				l_current_camera->get_near_clip(),
				l_current_camera->get_far_clip()
			));
			m_camera = static_cast<game::bc_chasing_camera*>(m_input_system->add_camera(std::move(l_camera)));
		}

		m_camera_y_offset = p_camera_y_offset;
		m_camera_z_offset = p_camera_z_offset;
		m_camera_look_at_offset = p_camera_look_at_offset;
	}

	void bc_xbot_player_actor_controller::set_weapon_obstacle_distance(bcFLOAT p_distance) noexcept
	{
		m_weapon_obstacle_distance = p_distance;
	}

	void bc_xbot_player_actor_controller::throw_grenade(game::bc_actor& p_grenade) noexcept
	{
		auto* l_scene = get_scene();
		const auto* l_grenade_mediate_component = p_grenade.get_component<game::bc_mediate_component>();
		const auto l_grenade_throw_power = std::max(0.3f, std::min(1.0f, m_grenade_throw_passed_time / m_grenade_throw_time)) * m_grenade_throw_force * get_bound_box_max_side_length();
		const auto l_throw_direction = core::bc_vector3f::normalize(get_look_direction() + core::bc_vector3f(0, 0.2f, 0)) * l_grenade_throw_power;
		const auto* l_threw_grenade_name = std::strcmp(l_grenade_mediate_component->get_entity_name(), m_grenade_name) == 0 ? m_threw_grenade_name : m_threw_smoke_grenade_name;

		if (m_network_system->get_network_type() != game::bc_network_type::not_started)
		{
			m_network_system->send_message(bc_xbot_grenade_throw_network_message(get_actor(), l_threw_grenade_name, l_grenade_mediate_component->get_world_transform(), l_throw_direction));
		}
		else
		{
			auto l_threw_grenade = l_scene->create_actor(l_threw_grenade_name, l_grenade_mediate_component->get_world_transform());
			l_threw_grenade.mark_for_double_update();

			auto* l_rigid_dynamic_component = l_threw_grenade.get_component<game::bc_rigid_dynamic_component>();
			auto l_rigid_dynamic = l_rigid_dynamic_component->get_dynamic_body();

			{
				game::bc_rigid_component_lock l_lock(*l_rigid_dynamic_component);

				l_rigid_dynamic.set_rigid_body_flags(physics::bc_rigid_body_flag::ccd, true);
				l_rigid_dynamic.add_force(l_throw_direction);
			}
		}

		l_scene->remove_actor(p_grenade);
		m_grenade_throw_passed_time = 0;

		grenade_thrown(l_threw_grenade_name);
	}

	bool bc_xbot_player_actor_controller::can_look() noexcept
	{
		return true;
	}

	bool bc_xbot_player_actor_controller::can_move() noexcept
	{
		return true;
	}

	bool bc_xbot_player_actor_controller::can_throw_grenade(core::bc_string_view p_entity)  noexcept
	{
		return true;
	}

	bool bc_xbot_player_actor_controller::can_attach_weapon(core::bc_string_view p_entity)  noexcept
	{
		return true;
	}

	bool bc_xbot_player_actor_controller::can_shoot_weapon()  noexcept
	{
		return true;
	}

	void bc_xbot_player_actor_controller::grenade_thrown(core::bc_string_view p_entity) noexcept
	{
	}

	void bc_xbot_player_actor_controller::weapon_attached(game::bc_actor& p_weapon) noexcept
	{
	}

	void bc_xbot_player_actor_controller::weapon_shoot(game::bc_actor& p_weapon) noexcept
	{
	}

	void bc_xbot_player_actor_controller::_on_event(core::bci_event& p_event) noexcept
	{
		if (const auto* l_pointing_event = core::bci_message::as<platform::bc_app_event_pointing>(p_event))
		{
			_on_pointing(*l_pointing_event);
			return;
		}

		if (const auto* l_key_event = core::bci_message::as<platform::bc_app_event_key>(p_event))
		{
			_on_key(*l_key_event);
			return;
		}
	}

	void bc_xbot_player_actor_controller::_on_pointing(const platform::bc_app_event_pointing& p_pointing_event) noexcept
	{
		m_pointing_delta_x = p_pointing_event.get_state().m_dx;
	}

	void bc_xbot_player_actor_controller::_on_key(const platform::bc_app_event_key& p_key_event) noexcept
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
			if (p_key_event.get_key_state() == platform::bc_key_state::pressing && m_grenade_throw_passed_time <= 0)
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
		const bool l_can_throw = can_throw_grenade(p_entity);
		if (!l_can_throw)
		{
			return;
		}

		const bool l_threw = bc_xbot_actor_controller::start_grenade_throw(p_entity);
		if (!l_threw)
		{
			return;
		}

		if (m_network_system->get_network_type() != game::bc_network_type::not_started)
		{
			m_network_system->send_message(bc_xbot_start_grenade_throw_network_message(get_actor(), p_entity));
		}
	}

	void bc_xbot_player_actor_controller::_attach_weapon(const bcCHAR* p_entity) noexcept
	{
		const bool l_can_attach = can_attach_weapon(p_entity);
		if (!l_can_attach)
		{
			return;
		}

		if (get_weapon())
		{
			return;
		}

		auto l_weapon_actor = get_scene()->create_actor(p_entity, core::bc_matrix4f::translation_matrix(get_position()));
		bc_xbot_actor_controller::attach_weapon(l_weapon_actor);

		if (m_network_system->get_network_type() != game::bc_network_type::not_started)
		{
			m_network_system->send_message(bc_xbot_weapon_attach_network_message(get_actor(), l_weapon_actor));
		}

		weapon_attached(l_weapon_actor);
	}

	void bc_xbot_player_actor_controller::_detach_weapon() noexcept
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

	void bc_xbot_player_actor_controller::_drop_weapon() noexcept
	{
		auto* l_weapon = get_weapon();
		if (!l_weapon)
		{
			return;
		}

		bc_xbot_actor_controller::drop_weapon();

		if (m_network_system->get_network_type() != game::bc_network_type::not_started)
		{
			m_network_system->send_message(bc_xbot_weapon_drop_network_message(get_actor()));
		}
	}

	void bc_xbot_player_actor_controller::_shoot_weapon() noexcept
	{
		const bool l_can_shoot = m_weapon_obstacle_rotation_velocity.get_value() <= 0 && can_shoot_weapon();
		if (!l_can_shoot)
		{
			return;
		}

		const bool l_shoot = bc_xbot_actor_controller::shoot_weapon();
		if (!l_shoot)
		{
			return;
		}

		if (m_network_system->get_network_type() != game::bc_network_type::not_started)
		{
			m_network_system->send_message(bc_xbot_weapon_shoot_network_message(get_actor()));
		}

		weapon_shoot(get_weapon()->m_actor);
		m_weapon_shoot_velocity.set_value(1);
	}

	void bc_xbot_player_actor_controller::_build_weapon_obstacle_query(core::bc_query_manager& p_query_manager, const bc_xbot_weapon& p_weapon) noexcept
	{
		auto* l_scene = get_scene();
		if (!l_scene)
		{
			return;
		}

		auto l_scene_query = game::bc_scene_query([=, &p_weapon](const game::bc_scene_query_context& p_context)
		{
			const auto& l_weapon_mediate_component = p_weapon.m_actor.get_component<game::bc_mediate_component>();

			auto* l_px_scene = &l_scene->get_px_scene();
			const auto l_ray = physics::bc_ray(l_weapon_mediate_component->get_position(), get_look_direction(), m_weapon_obstacle_distance);
			auto l_hit_buffer = physics::bc_scene_ray_query_buffer();
			bool l_hit;

			{
				physics::bc_scene_shared_lock l_lock(l_px_scene);
				l_hit = l_px_scene->raycast(l_ray, l_hit_buffer, physics::bc_hit_flag::distance, physics::bc_query_flags::statics);
			}

			if (l_hit)
			{
				const auto l_ray_hit = static_cast<game::bc_ray_hit>(l_hit_buffer.get_block());
				const auto l_hit_actor = l_ray_hit.get_actor();
				if (l_hit_actor == p_weapon.m_actor)
				{
					l_hit = false;
				}
			}

			const auto l_hit_distance = l_hit ? l_hit_buffer.get_block().get_distance() : std::numeric_limits<bcFLOAT>::max();

			return core::bc_any(l_hit_distance);
		});

		m_weapon_obstacle_query = p_query_manager.queue_query(std::move(l_scene_query));
	}
}
