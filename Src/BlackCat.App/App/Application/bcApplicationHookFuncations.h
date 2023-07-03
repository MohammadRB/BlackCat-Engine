// [24/11/2020 MRB]

#pragma once

namespace black_cat
{
	namespace core
	{
		class bc_content_stream_manager;
	}
	
	namespace game
	{
		struct bc_engine_application_parameter;
		class bc_game_system;
	}
}

namespace black_cat
{
	void bc_start_engine_services(const game::bc_engine_application_parameter& p_parameters);

	void bc_register_engine_loaders(const game::bc_engine_application_parameter& p_parameters);

	void bc_register_engine_actor_components();

	void bc_register_engine_network_messages(game::bc_network_system& p_network_system);
	
	void bc_bind_engine_scripts(game::bc_game_system& p_game_system);

	void bc_register_engine_particle_emitters(game::bc_game_system& p_game_system);

	void bc_load_engine_shaders(core::bc_content_stream_manager& p_stream_manager, game::bc_game_system& p_game_system);
	
	void bc_load_engine_resources(core::bc_content_stream_manager& p_stream_manager, game::bc_game_system& p_game_system);
	
	void bc_unload_engine_resources(core::bc_content_stream_manager& p_stream_manager);

	void bc_close_engine_services();
}