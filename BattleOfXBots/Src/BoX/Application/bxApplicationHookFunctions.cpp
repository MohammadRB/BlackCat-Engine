// [12/23/2021 MRB]

#include "Game/Object/Scene/bcEntityManager.h"
#include "BoX/Application/bxApplicationHookFunctions.h"
#include "BoX/Game/bxPlayerController.h"

namespace box
{
	void bc_start_game_services(game::bc_engine_application_parameter& p_parameters)
	{
	}

	void bc_register_game_loaders(game::bc_engine_application_parameter& p_parameters)
	{
	}

	void bc_register_game_actor_components()
	{
	}

	void bc_register_game_network_messages(game::bc_network_system& p_network_system)
	{
	}

	void bc_bind_game_scripts(game::bc_game_system& p_game_system)
	{
	}

	void bc_register_game_particle_emitters(game::bc_game_system& p_game_system)
	{
	}

	void bc_load_game_shaders(core::bc_content_stream_manager& p_stream_manager, game::bc_game_system& p_game_system)
	{
	}

	void bc_load_game_resources(core::bc_content_stream_manager& p_stream_manager, game::bc_game_system& p_game_system)
	{
	}

	void bc_unload_game_resources(core::bc_content_stream_manager& p_stream_manager)
	{
	}

	void bc_close_game_services()
	{
	}
}