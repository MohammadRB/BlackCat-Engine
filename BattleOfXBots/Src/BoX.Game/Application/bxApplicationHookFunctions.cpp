// [12/23/2021 MRB]

#include "Core/Messaging/Event/bcEventManager.h"
#include "Core/Messaging/Query/bcQueryManager.h"
#include "Game/System/Render/Material/bcMaterialManager.h"
#include "Game/Object/Scene/bcEntityManager.h"
#include "Game/System/Input/bcFileSystem.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Script/bcGameConsole.h"
#include "App/Loader/bcSceneCheckPointLoader.h"
#include "App/Loader/bcHeightMapLoaderDx11.h"
#include "BoX.Game/Application/bxApplicationHookFunctions.h"
#include "BoX.Game/Application/bxPlayerService.h"
#include "BoX.Game/Application/bxPlayerListService.h"
#include "BoX.Game/Application/bxSceneCheckPoint.h"
#include "BoX.Game/Game/bxPlayerActorController.h"
#include "BoX.Game/Game/bxNetworkPlayerActorController.h"
#include "BoX.Game/Game/bxPlayerSeatComponent.h"
#include "BoX.Game/Game/bxFireActorController.h"
#include "BoX.Game/Game/bxExplosionActorController.h"
#include "BoX.Game/Game/bxRocketActorController.h"
#include "BoX.Game/Game/bxGrenadeActorController.h"
#include "BoX.Game/Game/bxSmokeGrenadeActorController.h"
#include "BoX.Game/Game/bxEnvironmentLightController.h"
#include "BoX.Game/Particle/bxExplosionParticle.h"
#include "BoX.Game/Particle/bxWeaponParticle.h"
#include "BoX.Game/Particle/bxEnvironmentParticle.h"
#include "BoX.Game/Network/bxTeamSelectNetworkMessage.h"
#include "BoX.Game/Network/bxPlayerSpawnNetworkMessage.h"
#include "BoX.Game/Network/bxPlayerKilledNetworkMessage.h"
#include "BoX.Game/Network/bxGameStateNetworkMessage.h"
#include "BoX.Game/Network/bxGameResetNetworkMessage.h"
#include "BoX.Game/Network/bxGameScoresNetworkMessage.h"
#include "BoX.Game/Network/bxGameMessageNetworkMessage.h"

namespace box
{
	void bx_start_game_services(const game::bc_engine_application_parameter& p_parameters)
	{
		core::bc_register_service(core::bc_make_service<bx_player_service>(*core::bc_get_service<core::bc_event_manager>()));
		core::bc_register_service<bxi_player_list_service>(core::bc_make_service<bx_player_list_service>());
	}

	void bx_register_game_loaders(const game::bc_engine_application_parameter& p_parameters, bool p_is_editor)
	{
		if(!p_is_editor)
		{
			core::bc_register_loader<game::bc_height_map, bc_height_map_loader_dx11>("height_map", core::bc_make_loader<bc_height_map_loader_dx11>());
		}
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
			game::bc_actor_controller_register<bx_fire_actor_controller>("fire"),
			game::bc_actor_controller_register<bx_explosion_actor_controller>("explosion"),
			game::bc_actor_controller_register<bx_rocket_controller>("rocket"),
			game::bc_actor_controller_register<bx_grenade_actor_controller>("grenade"),
			game::bc_actor_controller_register<bx_smoke_grenade_actor_controller>("smoke_grenade"),
			game::bc_actor_controller_register<bx_environment_light_controller>("environment_light"),
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
			bx_game_reset_network_message,
			bx_game_message_network_message,
			bx_game_scores_network_message
		>();
	}

	void bx_bind_game_scripts(game::bc_game_system& p_game_system)
	{
	}

	void bx_register_game_particle_emitters(game::bc_game_system& p_game_system)
	{
		auto& l_particle_manager = p_game_system.get_render_system().get_particle_manager();
		core::bc_random l_random;

		l_particle_manager.register_emitter_definition("big_explosion", bx_big_explosion_particle()(l_random));
		l_particle_manager.register_emitter_definition("rifle_fire", bx_rifle_fire_particle()());
		l_particle_manager.register_emitter_definition("bullet_terrain", bx_bullet_terrain_particle()());
		l_particle_manager.register_emitter_definition("bullet_soil", bx_bullet_soil_particle()());
		l_particle_manager.register_emitter_definition("bullet_wood", bx_bullet_wood_particle()());
		l_particle_manager.register_emitter_definition("bullet_iron", bx_bullet_iron_particle()());
		l_particle_manager.register_emitter_definition("falling_leaf", bx_falling_leaf_particle()());
	}

	void bx_load_game_shaders(core::bc_content_stream_manager& p_stream_manager, game::bc_game_system& p_game_system)
	{
		auto& l_entity_manager = *core::bc_get_service<game::bc_entity_manager>();
		auto& l_material_manager = p_game_system.get_render_system().get_material_manager();
		auto& l_decal_manager = p_game_system.get_render_system().get_decal_manager();

		p_stream_manager.read_stream_file(p_game_system.get_file_system().get_content_data_path(bcL("BoXContentStream.json")).c_str());
		l_material_manager.read_material_file(p_game_system.get_file_system().get_content_data_path(bcL("BoXMaterial.json")).c_str());
		l_decal_manager.read_decal_file(p_game_system.get_file_system().get_content_data_path(bcL("BoXDecal.json")).c_str());
		l_entity_manager.read_entity_file(p_game_system.get_file_system().get_content_data_path(bcL("BoXEntityType.json")).c_str());

		p_stream_manager.load_content_stream(core::bc_alloc_type::program, "game_shaders");
	}

	void bx_load_game_resources(core::bc_content_stream_manager& p_stream_manager, game::bc_game_system& p_game_system)
	{
		p_stream_manager.load_content_stream(core::bc_alloc_type::program, "game_textures");
		p_stream_manager.load_content_stream(core::bc_alloc_type::program, "game_models");
		p_stream_manager.load_content_stream(core::bc_alloc_type::program, "game_sounds");
	}

	void bx_unload_game_resources(core::bc_content_stream_manager& p_stream_manager)
	{
		p_stream_manager.unload_content_stream("game_shaders");
		p_stream_manager.unload_content_stream("game_textures");
		p_stream_manager.unload_content_stream("game_models");
		p_stream_manager.unload_content_stream("game_sounds");
	}

	void bx_close_game_services()
	{
	}
}
