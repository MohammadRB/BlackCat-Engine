// [12/22/2021 MRB]

#pragma once

#include "Core/Content/bcContentStreamManager.h"
#include "Game/Application/bcEngineApplicationParameter.h"
#include "Game/System/Network/bcNetworkSystem.h"
#include "Game/System/bcGameSystem.h"

namespace box
{
	using namespace black_cat;

	void bx_start_game_services(game::bc_engine_application_parameter& p_parameters);

	void bx_register_game_loaders(game::bc_engine_application_parameter& p_parameters);

	void bx_register_game_actor_components();

	void bx_register_game_network_messages(game::bc_network_system& p_network_system);

	void bx_bind_game_scripts(game::bc_game_system& p_game_system);

	void bx_register_game_particle_emitters(game::bc_game_system& p_game_system);

	void bx_load_game_shaders(core::bc_content_stream_manager& p_stream_manager, game::bc_game_system& p_game_system);

	void bx_load_game_resources(core::bc_content_stream_manager& p_stream_manager, game::bc_game_system& p_game_system);

	void bx_unload_game_resources(core::bc_content_stream_manager& p_stream_manager);

	void bx_close_game_services();
}