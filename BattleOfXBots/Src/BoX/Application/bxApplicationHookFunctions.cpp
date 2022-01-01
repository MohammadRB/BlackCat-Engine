// [12/23/2021 MRB]

#include "Game/Object/Scene/bcEntityManager.h"
#include "App/Loader/bcHeightMapLoaderDx11.h"
#include "BoX/Application/bxApplicationHookFunctions.h"
#include "BoX/Application/bxPlayerService.h"
#include "BoX/Game/bxPlayerActorController.h"
#include "BoX/Game/bxNetworkPlayerActorController.h"

namespace box
{
	void bx_start_game_services(game::bc_engine_application_parameter& p_parameters)
	{
		core::bc_register_service(core::bc_make_service<bx_player_service>());
	}

	void bx_register_game_loaders(game::bc_engine_application_parameter& p_parameters)
	{
		core::bc_register_loader<game::bc_height_map, bc_height_map_loader_dx11>
		(
			"height_map", core::bc_make_loader<bc_height_map_loader_dx11>()
		);
	}

	void bx_register_game_actor_components()
	{
		game::bc_register_actor_controller_types
		(
			game::bc_actor_controller_register<bx_player_actor_controller>("box_player"),
			game::bc_actor_controller_register<bx_player_actor_controller>("box_network_player")
		);
	}

	void bx_register_game_network_messages(game::bc_network_system& p_network_system)
	{
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