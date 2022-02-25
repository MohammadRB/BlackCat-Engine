// [12/23/2021 MRB]

#include "Core/Messaging/Event/bcEventManager.h"
#include "Game/Object/Scene/bcEntityManager.h"
#include "App/Loader/bcSceneCheckPointLoader.h"
#include "App/Loader/bcHeightMapLoaderDx11.h"
#include "BoX.Game/Application/bxApplicationHookFunctions.h"
#include "BoX.Game/Application/bxPlayerService.h"
#include "BoX.Game/Application/bxSceneCheckPoint.h"
#include "BoX.Game/Game/bxPlayerActorController.h"
#include "BoX.Game/Game/bxNetworkPlayerActorController.h"
#include "BoX.Game/Game/bxPlayerSeatComponent.h"
#include "BoX.Game/Network/bxTeamSelectNetworkMessage.h"
#include "BoX.Game/Network/bxPlayerSpawnNetworkMessage.h"
#include "BoX.Game/Network/bxPlayerKilledNetworkMessage.h"
#include "BoX.Game/Network/bxGameStateNetworkMessage.h"
#include "BoX.Game/Network/bxGameResetNetworkMessage.h"

namespace box
{
	void bx_start_game_services(const game::bc_engine_application_parameter& p_parameters)
	{
		core::bc_register_service(core::bc_make_service<bx_player_service>(*core::bc_get_service<core::bc_event_manager>()));
	}

	void bx_register_game_loaders(const game::bc_engine_application_parameter& p_parameters)
	{
		core::bc_register_loader<game::bc_height_map, bc_height_map_loader_dx11>("height_map", core::bc_make_loader<bc_height_map_loader_dx11>());
		core::bc_register_loader<bx_scene_checkpoint, bc_scene_checkpoint_loader<bx_scene_checkpoint>>("scene_check_point", core::bc_make_loader<bc_scene_checkpoint_loader<bx_scene_checkpoint>>());
	}

	void bx_register_game_actor_components()
	{
		game::bc_register_component_types
		(
			game::bc_component_register<bx_player_seat_component>("box_player_seat")
		);
		game::bc_register_actor_controller_types
		(
			game::bc_actor_controller_register<bx_player_actor_controller>("box_player"),
			game::bc_actor_controller_register<bx_network_player_actor_controller>("box_network_player")
		);
	}

	void bx_register_game_network_messages(game::bc_network_system& p_network_system)
	{
		p_network_system.register_messages
		<
			bx_team_select_network_message,
			bx_player_spawn_network_message,
			bx_player_killed_network_message,
			bx_game_state_network_message,
			bx_game_reset_network_message
		>();
	}

	void bx_bind_game_scripts(game::bc_game_system& p_game_system)
	{
	}

	void bx_register_game_particle_emitters(game::bc_game_system& p_game_system)
	{
	}

	void bx_load_game_shaders(core::bc_content_stream_manager& p_stream_manager, game::bc_game_system& p_game_system)
	{
	}

	void bx_load_game_resources(core::bc_content_stream_manager& p_stream_manager, game::bc_game_system& p_game_system)
	{
	}

	void bx_unload_game_resources(core::bc_content_stream_manager& p_stream_manager)
	{
	}

	void bx_close_game_services()
	{
	}
}