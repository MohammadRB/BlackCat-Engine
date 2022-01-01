// [01/01/2022 MRB]

#include "BoX/Game/bxNetworkPlayerActorController.h"

namespace box
{
	void bx_network_player_actor_controller::load_origin_network_instance(const game::bc_actor_component_network_load_context& p_context)
	{
		bc_xbot_network_player_actor_controller::load_origin_network_instance(p_context);
	}

	void bx_network_player_actor_controller::load_replicated_network_instance(const game::bc_actor_component_network_load_context& p_context)
	{
		bc_xbot_network_player_actor_controller::load_replicated_network_instance(p_context);
	}

	void bx_network_player_actor_controller::write_origin_network_instance(const game::bc_actor_component_network_write_context& p_context)
	{
		bc_xbot_network_player_actor_controller::write_origin_network_instance(p_context);
	}

	void bx_network_player_actor_controller::write_replicated_network_instance(const game::bc_actor_component_network_write_context& p_context)
	{
		bc_xbot_network_player_actor_controller::write_replicated_network_instance(p_context);
	}

	void bx_network_player_actor_controller::added_to_scene(const game::bc_actor_component_event_context& p_context, game::bc_scene& p_scene)
	{
		bc_xbot_network_player_actor_controller::added_to_scene(p_context, p_scene);
	}

	void bx_network_player_actor_controller::update_origin_instance(const game::bc_actor_component_update_content& p_context)
	{
		bc_xbot_network_player_actor_controller::update_origin_instance(p_context);
	}

	void bx_network_player_actor_controller::update_replicated_instance(const game::bc_actor_component_update_content& p_context)
	{
		bc_xbot_network_player_actor_controller::update_replicated_instance(p_context);
	}

	void bx_network_player_actor_controller::removed_from_scene(const game::bc_actor_component_event_context& p_context, game::bc_scene& p_scene)
	{
		bc_xbot_network_player_actor_controller::removed_from_scene(p_context, p_scene);
	}

	void bx_network_player_actor_controller::handle_event(const game::bc_actor_component_event_context& p_context)
	{
		bc_xbot_network_player_actor_controller::handle_event(p_context);
	}
}