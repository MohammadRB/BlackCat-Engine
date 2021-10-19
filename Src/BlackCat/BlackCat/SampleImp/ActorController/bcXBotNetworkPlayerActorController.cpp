// [10/09/2021 MRB]

#include "BlackCat/BlackCatPCH.h"

#include "Core/Utility/bcJsonParse.h"
#include "Core/Utility/bcLogger.h"
#include "BlackCat/SampleImp/ActorController/bcXBotNetworkPlayerActorController.h"

namespace black_cat
{
	bc_xbot_network_player_actor_controller::bc_xbot_network_player_actor_controller() noexcept
		: m_look_velocity(0, 1, 0.50f),
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
	}

	void bc_xbot_network_player_actor_controller::added_to_scene(const game::bc_actor_component_event_context& p_context,game::bc_scene& p_scene)
	{
		bc_xbot_actor_controller::added_to_scene(p_context, p_scene);
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
}