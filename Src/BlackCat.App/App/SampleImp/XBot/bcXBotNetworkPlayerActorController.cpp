// [10/09/2021 MRB]

#include "App/AppPCH.h"

#include "Core/Utility/bcJsonParse.h"
#include "Core/Utility/bcLogger.h"
#include "Game/System/Input/bcInputSystem.h"
#include "Game/System/Network/bcNetworkSystem.h"
#include "Game/Object/Scene/Component/bcHumanRagdollComponent.h"
#include "Game/Object/Scene/Component/bcRigidDynamicComponent.h"
#include "Game/bcJsonParse.h"
#include "Game/bcConstant.h"
#include "App/SampleImp/XBot/bcXBotNetworkPlayerActorController.h"
#include "App/SampleImp/XBot/bcXBotWeaponNetworkMessage.h"
#include "App/SampleImp/XBot/bcXBotGrenadeNetworkMessage.h"
#include "App/SampleImp/XBot/bcXBotRagdollNetworkMessage.h"

namespace black_cat
{
	bc_xbot_network_player_actor_controller::bc_xbot_network_player_actor_controller() noexcept
		: m_network_system(nullptr),
		m_look_velocity(0, 1, 0.50f),
		m_network_look_side(0),
		m_network_forward_pressed(false),
		m_network_backward_pressed(false),
		m_network_right_pressed(false),
		m_network_left_pressed(false),
		m_network_walk_pressed(false)
	{
	}

	void bc_xbot_network_player_actor_controller::start_grenade_throw(const bcCHAR* p_entity_name) noexcept
	{
		// Entity name parameter will be passed form network messages. To prevent dangling pointer we must make a copy of it  
		m_grenade_name = p_entity_name;
		bc_xbot_actor_controller::start_grenade_throw(m_grenade_name.c_str());

		if (get_network_component().get_network_type() == game::bc_network_type::server)
		{
			m_network_system->send_message(bc_xbot_start_grenade_throw_network_message(get_actor(), m_grenade_name.c_str()));
		}
	}

	void bc_xbot_network_player_actor_controller::throw_grenade(core::bc_string_view p_grenade_name,
		const core::bc_vector3f& p_position,
		const core::bc_matrix3f& p_rotation,
		const core::bc_vector3f& p_direction)
	{
		const auto l_player_id = get_network_component().get_network_client_id();
		auto l_transform = core::bc_matrix4f::identity();
		l_transform.set_translation(p_position + core::bc_vector3f::normalize(p_direction));
		l_transform.set_rotation(p_rotation);

		// grenade will be replicated by its network component
		auto l_actor = get_scene()->create_actor
		(
			p_grenade_name.data(),
			l_transform,
			core::bc_data_driven_parameter(core::bc_alloc_type::frame).add_or_update(constant::g_param_player_id, l_player_id)
		);
		l_actor.mark_for_double_update();

		auto* l_network_component = l_actor.get_component<game::bc_network_component>();
		auto* l_rigid_dynamic_component = l_actor.get_component<game::bc_rigid_dynamic_component>();

		if (!l_network_component || !l_rigid_dynamic_component)
		{
			core::bc_log(core::bc_log_type::error, bcL("grenade entity must have network and rigid dynamic component"));
			return;
		}

		auto l_rigid_body = l_rigid_dynamic_component->get_dynamic_body();

		{
			game::bc_rigid_component_lock l_lock(*l_rigid_dynamic_component);

			l_rigid_body.add_force(p_direction);
		}
	}

	void bc_xbot_network_player_actor_controller::attach_weapon(const bcCHAR* p_entity) noexcept
	{
		auto* l_weapon = get_weapon();
		if (l_weapon)
		{
			bc_xbot_actor_controller::detach_weapon();
		}

		auto l_weapon_actor = get_scene()->create_actor(p_entity, core::bc_matrix4f::translation_matrix(get_position()));
		//l_weapon_actor.mark_for_double_update();

		bc_xbot_actor_controller::attach_weapon(l_weapon_actor);

		if (get_network_component().get_network_type() == game::bc_network_type::server)
		{
			m_network_system->send_message(bc_xbot_weapon_attach_network_message(get_actor(), l_weapon_actor));
		}
	}

	void bc_xbot_network_player_actor_controller::detach_weapon() noexcept
	{
		auto* l_weapon = get_weapon();
		if (!l_weapon)
		{
			return;
		}

		bc_xbot_actor_controller::detach_weapon();

		if (get_network_component().get_network_type() == game::bc_network_type::server)
		{
			m_network_system->send_message(bc_xbot_weapon_detach_network_message(get_actor()));
		}
	}

	void bc_xbot_network_player_actor_controller::shoot_weapon() noexcept
	{
		const auto l_player_id = get_network_client_id();
		bc_xbot_actor_controller::shoot_weapon(l_player_id);

		if (get_network_component().get_network_type() == game::bc_network_type::server)
		{
			m_network_system->send_message(bc_xbot_weapon_shoot_network_message(get_actor()));
		}
	}

	void bc_xbot_network_player_actor_controller::enable_ragdoll(core::bc_string_view p_body_part_force, const core::bc_vector3f& p_force) noexcept
	{
		bc_xbot_actor_controller::enable_ragdoll(p_body_part_force, p_force);

		if (get_network_component().get_network_type() == game::bc_network_type::server)
		{
			m_network_system->send_message(bc_xbot_ragdoll_activation_network_message(get_actor(), core::bc_string(p_body_part_force), p_force));
		}
	}

	void bc_xbot_network_player_actor_controller::initialize(const game::bc_actor_component_initialize_context& p_context)
	{
		bc_xbot_actor_controller::initialize(p_context);
		m_network_system = &p_context.m_game_system.get_network_system();
	}

	void bc_xbot_network_player_actor_controller::load_origin_network_instance(const game::bc_actor_component_network_load_context& p_context)
	{
	}

	void bc_xbot_network_player_actor_controller::load_replicated_network_instance(const game::bc_actor_component_network_load_context& p_context)
	{
		json_parse::bc_load(p_context.m_parameters, "pos", m_network_position);
		json_parse::bc_load(p_context.m_parameters, "lk_dir", m_network_look_direction);
		m_network_look_side = p_context.m_parameters.find("lk_sd")->second.as_throw<bcINT32>();
		const auto* l_keys = &p_context.m_parameters.find("keys")->second.as_throw<core::bc_vector<core::bc_any>>();

		m_network_forward_pressed = (*l_keys)[0].as_throw<bool>();
		m_network_backward_pressed = (*l_keys)[1].as_throw<bool>();
		m_network_right_pressed = (*l_keys)[2].as_throw<bool>();
		m_network_left_pressed = (*l_keys)[3].as_throw<bool>();
		m_network_walk_pressed = (*l_keys)[4].as_throw<bool>();
		
		get_network_component().add_extrapolating_value("lk_dir", m_network_look_direction);

		if(p_context.m_is_replication_load)
		{
			m_network_initial_data = core::bc_make_unique<_bc_xbot_network_initial_data>();

			const auto l_weapon_param = p_context.m_parameters.find("wpn");
			if(l_weapon_param != std::end(p_context.m_parameters))
			{
				m_network_initial_data->m_network_weapon_name = std::move(l_weapon_param->second.as_throw<core::bc_string>());
			}

			const auto l_ragdoll_param = p_context.m_parameters.find("rag");
			if(l_ragdoll_param != std::end(p_context.m_parameters))
			{
				m_network_initial_data->m_network_ragdoll_enabled = true;
				json_parse::bc_load(p_context.m_parameters, "rag", m_network_initial_data->m_ragdoll_transforms);
			}
		}
	}

	void bc_xbot_network_player_actor_controller::write_origin_network_instance(const game::bc_actor_component_network_write_context& p_context)
	{
	}

	void bc_xbot_network_player_actor_controller::write_replicated_network_instance(const game::bc_actor_component_network_write_context& p_context)
	{
		json_parse::bc_write(p_context.m_parameters, "pos", m_network_position);
		json_parse::bc_write(p_context.m_parameters, "lk_dir", m_network_look_direction);
		p_context.m_parameters.add("lk_sd", core::bc_any(m_network_look_side));
		p_context.m_parameters.add
		(
			"keys",
			core::bc_any(core::bc_vector<core::bc_any>
			({
				core::bc_any(m_network_forward_pressed),
				core::bc_any(m_network_backward_pressed),
				core::bc_any(m_network_right_pressed),
				core::bc_any(m_network_left_pressed),
				core::bc_any(m_network_walk_pressed),
			}))
		);

		if (p_context.m_is_replication_write)
		{
			const auto* l_weapon = get_weapon();
			if (l_weapon)
			{
				const auto* l_mediate_component = l_weapon->m_actor.get_component<game::bc_mediate_component>();
				core::bc_string l_weapon_entity_name = l_mediate_component->get_entity_name();
				p_context.m_parameters.add("wpn", core::bc_any(l_weapon_entity_name));
			}

			if (get_ragdoll_enabled())
			{
				const auto* l_ragdoll_component = get_actor().get_component<game::bc_human_ragdoll_component>();
				json_parse::bc_write(p_context.m_parameters, "rag", l_ragdoll_component->get_body_px_transforms());
			}
		}
	}

	void bc_xbot_network_player_actor_controller::added_to_scene(const game::bc_actor_component_event_context& p_context,game::bc_scene& p_scene)
	{
		bc_xbot_actor_controller::added_to_scene(p_context, p_scene);

		if(!m_network_initial_data->m_network_weapon_name.empty())
		{
			auto l_weapon_actor = get_scene()->create_actor(m_network_initial_data->m_network_weapon_name.c_str(), core::bc_matrix4f::translation_matrix(get_position()));
			bc_xbot_actor_controller::attach_weapon(l_weapon_actor);
		}

		if(m_network_initial_data->m_network_ragdoll_enabled)
		{
			auto l_actor = get_actor();
			auto* l_ragdoll_component = l_actor.get_component<game::bc_human_ragdoll_component>();

			bc_xbot_actor_controller::enable_ragdoll();
			l_ragdoll_component->set_body_px_transforms(m_network_initial_data->m_ragdoll_transforms);
		}

		m_network_initial_data.reset();
	}

	void bc_xbot_network_player_actor_controller::update_origin_instance(const game::bc_actor_component_update_content& p_context)
	{
	}

	void bc_xbot_network_player_actor_controller::update_replicated_instance(const game::bc_actor_component_update_content& p_context)
	{
		const auto l_extrapolated_look_direction = get_network_component().get_extrapolated_value("lk_dir", p_context.m_clock).second;
		if(l_extrapolated_look_direction.magnitude() > 0.f)
		{
			m_look_velocity.push(p_context.m_clock.m_elapsed_second);
		}
		else
		{
			m_look_velocity.release(p_context.m_clock.m_elapsed_second);
		}

		bc_xbot_actor_controller::update(bc_xbot_input_update_context1
		{
			p_context.m_clock,
			m_network_position,
			m_network_look_direction,
			m_network_look_side,
			m_look_velocity.get_value(),
			m_network_forward_pressed,
			m_network_backward_pressed,
			m_network_right_pressed,
			m_network_left_pressed,
			m_network_walk_pressed,
		});
	}

	void bc_xbot_network_player_actor_controller::removed_from_scene(const game::bc_actor_component_event_context& p_context, game::bc_scene& p_scene)
	{
		bc_xbot_actor_controller::removed_from_scene(p_context, p_scene);
	}

	void bc_xbot_network_player_actor_controller::handle_event(const game::bc_actor_component_event_context& p_context)
	{
		bc_xbot_actor_controller::handle_event(p_context);
	}

	void bc_xbot_network_player_actor_controller::throw_grenade(game::bc_actor& p_grenade) noexcept
	{
		// Grenade will be thrown by a separate network message

		auto* l_scene = get_scene();
		l_scene->remove_actor(p_grenade);
	}
}
