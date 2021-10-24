// [10/09/2021 MRB]

#include "BlackCat/BlackCatPCH.h"

#include "Core/Utility/bcJsonParse.h"
#include "Core/Utility/bcLogger.h"
#include "Game/System/Input/bcInputSystem.h"
#include "Game/System/Network/bcNetworkSystem.h"
#include "BlackCat/SampleImp/ActorController/bcXBotNetworkPlayerActorController.h"
#include "BlackCat/SampleImp/ActorController/bcXBotNetworkMessage.h"

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
			const auto l_weapon = p_context.m_parameters.find("wpn");
			if(l_weapon != std::end(p_context.m_parameters))
			{
				m_initial_attached_weapon = core::bc_make_unique<core::bc_string>(std::move(l_weapon->second.as_throw<core::bc_string>()));
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
		}
	}

	void bc_xbot_network_player_actor_controller::added_to_scene(const game::bc_actor_component_event_context& p_context,game::bc_scene& p_scene)
	{
		bc_xbot_actor_controller::added_to_scene(p_context, p_scene);

		if(m_initial_attached_weapon)
		{
			auto l_weapon_actor = get_scene()->create_actor(m_initial_attached_weapon->c_str(), core::bc_matrix4f::translation_matrix(get_position()));
			bc_xbot_actor_controller::attach_weapon(l_weapon_actor);
			
			m_initial_attached_weapon.reset();
		}
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

	void bc_xbot_network_player_actor_controller::attach_weapon(const bcCHAR* p_entity) noexcept
	{
		auto* l_weapon = get_weapon();
		if (l_weapon)
		{
			bc_xbot_actor_controller::detach_weapon();
		}

		auto l_weapon_actor = get_scene()->create_actor(p_entity, core::bc_matrix4f::translation_matrix(get_position()));
		bc_xbot_actor_controller::attach_weapon(l_weapon_actor);

		if(get_network_component().get_network_type() == game::bc_network_type::server)
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
		bc_xbot_actor_controller::shoot_weapon();

		if (get_network_component().get_network_type() == game::bc_network_type::server)
		{
			m_network_system->send_message(bc_xbot_weapon_shoot_network_message(get_actor()));
		}
	}
}