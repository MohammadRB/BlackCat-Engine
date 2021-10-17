// [10/09/2021 MRB]

#include "BlackCat/BlackCatPCH.h"

#include "Core/Utility/bcJsonParse.h"
#include "BlackCat/SampleImp/ActorController/bcXBotNetworkPlayerActorController.h"

namespace black_cat
{
	void bc_xbot_network_player_actor_controller::initialize(const game::bc_actor_component_initialize_context& p_context)
	{
		bc_xbot_actor_controller::initialize(p_context);
	}

	void bc_xbot_network_player_actor_controller::load_origin_network_instance(const game::bc_actor_component_network_load_context& p_context)
	{
	}

	void bc_xbot_network_player_actor_controller::load_replicated_network_instance(const game::bc_actor_component_network_load_context& p_context)
	{
		core::bc_vector3f l_position;
		core::bc_vector4f l_look_direction;
		core::bc_vector4f l_velocity;
		core::bc_vector2f l_velocity1;

		json_parse::bc_load(p_context.m_parameters, "pos", l_position);
		json_parse::bc_load(p_context.m_parameters, "lk_dir", l_look_direction);
		json_parse::bc_load(p_context.m_parameters, "vlc", l_velocity);
		json_parse::bc_load(p_context.m_parameters, "vlc1", l_velocity1);

		m_network_position = l_position;
		m_network_look_direction = l_look_direction.xyz();
		m_network_look_side = static_cast<bcINT32>(l_look_direction.w);
		m_network_look_velocity = l_velocity1.x;
		m_network_forward_velocity = l_velocity.x;
		m_network_backward_velocity = l_velocity.y;
		m_network_right_velocity = l_velocity.z;
		m_network_left_velocity = l_velocity.w;
		m_network_walk_velocity = l_position.y;
	}

	void bc_xbot_network_player_actor_controller::write_origin_network_instance(const game::bc_actor_component_network_write_context& p_context)
	{
	}

	void bc_xbot_network_player_actor_controller::write_replicated_network_instance(const game::bc_actor_component_network_write_context& p_context)
	{
		const auto l_velocity = get_velocity();
		json_parse::bc_write(p_context.m_parameters, "pos", get_position());
		json_parse::bc_write(p_context.m_parameters, "lk_dir", core::bc_vector4f(get_look_direction(), l_velocity.m_look_side));
		json_parse::bc_write(p_context.m_parameters, "vlc", core::bc_vector4f(l_velocity.m_forward_velocity, l_velocity.m_backward_velocity, l_velocity.m_right_velocity, l_velocity.m_left_velocity));
		json_parse::bc_write(p_context.m_parameters, "vlc1", core::bc_vector2f(l_velocity.m_look_velocity, l_velocity.m_walk_velocity));
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
		bc_xbot_actor_controller::update(bc_xbot_input_update_context1
		{
			p_context.m_clock,
			m_network_position,
			m_network_look_direction,
			{
				m_network_look_side,
				m_network_look_velocity,
				m_network_forward_velocity,
				m_network_backward_velocity,
				m_network_right_velocity,
				m_network_left_velocity,
				m_network_walk_velocity
			}
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