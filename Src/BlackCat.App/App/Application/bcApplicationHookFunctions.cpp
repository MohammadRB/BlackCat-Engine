// [12/29/2020 MRB]

#include "App/AppPCH.h"

#include "PlatformImp/bcIDELogger.h"
#include "Core/Memory/bcMemoryManagment.h"
#include "Core/Concurrency/bcThreadManager.h"
#include "Core/Messaging/Event/bcEventManager.h"
#include "Core/Messaging/Query/bcQueryManager.h"
#include "Core/Content/bcContentManager.h"
#include "Core/Content/bcContentStreamManager.h"
#include "Core/Utility/bcServiceManager.h"
#include "Core/Utility/bcCounterValueManager.h"
#include "Core/bcUtility.h"
#include "GraphicImp/Shader/bcComputeShader.h"
#include "GraphicImp/Shader/bcDomainShader.h"
#include "GraphicImp/Shader/bcGeometryShader.h"
#include "GraphicImp/Shader/bcHullShader.h"
#include "GraphicImp/Shader/bcPixelShader.h"
#include "GraphicImp/Shader/bcVertexShader.h"
#include "Game/Application/bcEngineApplicationParameter.h"
#include "Game/System/Input/bcFileSystem.h"
#include "Game/System/Input/bcFileLogger.h"
#include "Game/System/Script/bcScriptSystem.h"
#include "Game/System/Script/bcScriptBinding.h"
#include "Game/System/Script/bcGameConsole.h"
#include "Game/System/Render/Material/bcMaterialManager.h"
#include "Game/System/Render/Material/bcMaterialManager.h"
#include "Game/System/Render/Decal/bcDecalManager.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Network/bcNetworkSystem.h"
#include "Game/System/Network/Message/bcStringNetworkMessage.h"
#include "Game/System/Network/Message/bcPingNetworkMessage.h"
#include "Game/System/Network/Message/bcAcknowledgeNetworkMessage.h"
#include "Game/System/Network/Message/bcClientConnectNetworkMessage.h"
#include "Game/System/Network/Message/bcActorReplicateNetworkMessage.h"
#include "Game/System/Network/Message/bcActorSyncNetworkMessage.h"
#include "Game/System/Network/Message/bcActorRemoveNetworkMessage.h"
#include "Game/System/Network/Message/bcSceneChangeNetworkMessage.h"
#include "Game/System/Network/Message/bcSceneReplicateNetworkMessage.h"
#include "Game/System/Physics/bcPhysicsSimulationCallback.h"
#include "Game/System/bcGameSystem.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/bcEntityManager.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/Object/Scene/bcSceneCheckPoint.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/Object/Scene/Component/bcRenderComponent.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"
#include "Game/Object/Scene/Component/bcSimpleMeshComponent.h"
#include "Game/Object/Scene/Component/bcVegetableMeshComponent.h"
#include "Game/Object/Scene/Component/bcSkinnedMeshComponent.h"
#include "Game/Object/Scene/Component/bcHierarchyComponent.h"
#include "Game/Object/Scene/Component/bcHeightMapComponent.h"
#include "Game/Object/Scene/Component/bcRigidStaticComponent.h"
#include "Game/Object/Scene/Component/bcRigidDynamicComponent.h"
#include "Game/Object/Scene/Component/bcRigidControllerComponent.h"
#include "Game/Object/Scene/Component/bcLightComponent.h"
#include "Game/Object/Scene/Component/bcWindComponent.h"
#include "Game/Object/Scene/Component/bcParticleEmitterComponent.h"
#include "Game/Object/Scene/Component/bcDecalComponent.h"
#include "Game/Object/Scene/Component/bcDecalResolverComponent.h"
#include "Game/Object/Scene/Component/bcWeaponComponent.h"
#include "Game/Object/Scene/Component/bcNetworkComponent.h"
#include "Game/Object/Scene/Component/bcHumanRagdollComponent.h"
#include "Game/Object/Scene/Component/bcIconComponent.h"
#include "Game/Object/Scene/Component/bcCheckPointComponent.h"
#include "Game/Object/Scene/Component/bcSoundComponent.h"
#include "Game/System/Animation/bcSkinnedAnimation.h"
#include "App/Application/bcApplicationHookFuncations.h"
#include "App/Loader/bcTextureLoader.h"
#include "App/Loader/bcVertexShaderLoader.h"
#include "App/Loader/bcHullShaderLoader.h"
#include "App/Loader/bcDomainShaderLoader.h"
#include "App/Loader/bcGeometryShaderLoader.h"
#include "App/Loader/bcPixelShaderLoader.h"
#include "App/Loader/bcComputeShaderLoader.h"
#include "App/Loader/bcSoundLoader.h"
#include "App/Loader/bcMeshColliderLoader.h"
#include "App/Loader/bcMeshLoader.h"
#include "App/Loader/bcSkinnedAnimationLoader.h"
#include "App/Loader/bcSceneLoader.h"
#include "App/Loader/bcSceneCheckPointLoader.h"
#include "App/SampleImp/XBot/bcXBotIdleActorController.h"
#include "App/SampleImp/XBot/bcXBotPlayerActorController.h"
#include "App/SampleImp/XBot/bcXBotNetworkPlayerActorController.h"
#include "App/SampleImp/XBot/bcXBotWeaponNetworkMessage.h"
#include "App/SampleImp/XBot/bcXBotGrenadeNetworkMessage.h"
#include "App/SampleImp/XBot/bcXBotRagdollNetworkMessage.h"
#include "App/SampleImp/ActorController/bcRigidDynamicNetworkActorController.h"

namespace black_cat
{
	void bc_start_engine_services(const game::bc_engine_application_parameter& p_parameters)
	{
#ifdef BC_MEMORY_ENABLE
		core::bc_memory_manager::startup
		(
			p_parameters.m_engine_parameters.m_thread_manager_thread_count + p_parameters.m_engine_parameters.m_thread_manager_reserved_thread_count,
			p_parameters.m_engine_parameters.m_memmng_fsa_start_size,
			p_parameters.m_engine_parameters.m_memmng_fsa_count,
			p_parameters.m_engine_parameters.m_memmng_fsa_step_size,
			p_parameters.m_engine_parameters.m_memmng_fsa_allocation_count,
			p_parameters.m_engine_parameters.m_memmng_program_stack_size,
			p_parameters.m_engine_parameters.m_memmng_frame_stack_size,
			p_parameters.m_engine_parameters.m_memmng_super_heap_size
		);
#endif
		core::bc_service_manager::start_up();

		auto* l_logger = core::bc_register_service(core::bc_make_service<core::bc_logger>());
#ifdef BC_DEBUG
		l_logger->register_listener(core::bc_log_type::all, core::bc_make_unique<platform::bc_ide_logger>(core::bc_alloc_type::program));
#endif
		core::bc_register_service
		(
			core::bc_make_service<core::bc_thread_manager>
			(
				p_parameters.m_engine_parameters.m_thread_manager_thread_count,
				p_parameters.m_engine_parameters.m_thread_manager_reserved_thread_count
			)
		);
		core::bc_register_service(core::bc_make_service<core::bc_event_manager>());
		core::bc_register_service(core::bc_make_service<core::bc_query_manager>());
		core::bc_register_service(core::bc_make_service<core::bc_counter_value_manager>());
		core::bc_register_service(core::bc_make_service<core::bc_content_manager>());
		core::bc_register_service(core::bc_make_service<core::bc_content_stream_manager>(*core::bc_get_service<core::bc_content_manager>()));
		core::bc_register_service(core::bc_make_service<game::bc_game_system>());
		core::bc_register_service(core::bc_make_service<game::bc_actor_component_manager>
		(
			*core::bc_get_service<core::bc_query_manager>(), 
			*core::bc_get_service<game::bc_game_system>())
		);
		core::bc_register_service(core::bc_make_service<game::bc_entity_manager>
		(
			*core::bc_get_service<core::bc_content_stream_manager>(), 
			*core::bc_get_service<game::bc_actor_component_manager>(),
			*core::bc_get_service<game::bc_game_system>()
		));
	}

	void bc_register_engine_loaders(const game::bc_engine_application_parameter& p_parameters)
	{
		core::bc_register_loader<graphic::bc_texture2d_content, bc_texture_loader>("texture2d", core::bc_make_loader<bc_texture_loader>());
		core::bc_register_loader<graphic::bc_vertex_shader_content, bc_vertex_shader_loader>("vertex_shader", core::bc_make_loader<bc_vertex_shader_loader>());
		core::bc_register_loader<graphic::bc_hull_shader_content, bc_hull_shader_loader>("hull_shader", core::bc_make_loader<bc_hull_shader_loader>());
		core::bc_register_loader<graphic::bc_domain_shader_content, bc_domain_shader_loader>("domain_shader", core::bc_make_loader<bc_domain_shader_loader>());
		core::bc_register_loader<graphic::bc_geometry_shader_content, bc_geometry_shader_loader>("geometry_shader", core::bc_make_loader<bc_geometry_shader_loader>());
		core::bc_register_loader<graphic::bc_pixel_shader_content, bc_pixel_shader_loader>("pixel_shader", core::bc_make_loader<bc_pixel_shader_loader>());
		core::bc_register_loader<graphic::bc_compute_shader_content, bc_compute_shader_loader>("compute_shader", core::bc_make_loader<bc_compute_shader_loader>());
		core::bc_register_loader<sound::bc_sound_content, bc_sound_loader>("sound", core::bc_make_loader<bc_sound_loader>());
		core::bc_register_loader<game::bc_mesh_collider, bc_mesh_collider_loader>("mesh_collider", core::bc_make_loader<bc_mesh_collider_loader>());
		core::bc_register_loader<game::bc_mesh, bc_mesh_loader>("mesh", core::bc_make_loader<bc_mesh_loader>());
		core::bc_register_loader<game::bc_skinned_animation, bc_skinned_animation_loader>("animation", core::bc_make_loader<bc_skinned_animation_loader>());
		core::bc_register_loader<game::bc_scene, bc_scene_loader>
		(
			"scene",
			core::bc_make_loader<bc_scene_loader>
			(
				bc_scene_loader
				(
					[]()
					{
						return std::move
						(
							physics::bc_scene_builder()
							.enable_locking()
							.enable_ccd()
							.use_simulation_callback
							(
								core::bc_make_unique<game::bc_physics_simulation_callback>
								(
									game::bc_physics_simulation_callback(core::bc_get_service<game::bc_game_system>()->get_physics_system())
								)
							)
						);
					},
					p_parameters.m_app_parameters.m_scene_graph_factory
				)
			)
		);
		core::bc_register_loader<game::bc_scene_checkpoint, bc_scene_checkpoint_loader<>>("scene_check_point", core::bc_make_loader<bc_scene_checkpoint_loader<>>());
	}

	void bc_register_engine_actor_components()
	{
		game::bc_register_component_types
		(
			game::bc_component_register<game::bc_mediate_component>("mediate"),
			game::bc_component_register<game::bc_simple_mesh_component>("simple_mesh"),
			game::bc_component_register<game::bc_vegetable_mesh_component>("vegetable_mesh"),
			game::bc_component_register<game::bc_skinned_mesh_component>("skinned_mesh"),
			game::bc_component_register<game::bc_hierarchy_component>("hierarchy"),
			game::bc_component_register<game::bc_rigid_static_component>("rigid_static"),
			game::bc_component_register<game::bc_rigid_dynamic_component>("rigid_dynamic"),
			game::bc_component_register<game::bc_rigid_controller_component>("rigid_controller"),
			game::bc_component_register<game::bc_height_map_component>("height_map"),
			game::bc_component_register<game::bc_light_component>("light"),
			game::bc_component_register<game::bc_wind_component>("wind"),
			game::bc_component_register<game::bc_particle_emitter_component>("particle_emitter"),
			game::bc_component_register<game::bc_decal_component>("decal"),
			game::bc_component_register<game::bc_weapon_component>("weapon"),
			game::bc_component_register<game::bc_network_component>("network"),
			game::bc_component_register<game::bc_human_ragdoll_component>("human_ragdoll"),
			game::bc_component_register<game::bc_icon_component>("icon"),
			game::bc_component_register<game::bc_checkpoint_component>("checkpoint"),
			game::bc_component_register<game::bc_sound_component>("sound")
		);
		game::bc_register_abstract_component_types
		(
			game::bc_abstract_component_register<game::bc_mesh_component, game::bc_simple_mesh_component, game::bc_vegetable_mesh_component, game::bc_skinned_mesh_component>(),
			game::bc_abstract_component_register<game::bc_render_component, game::bc_mesh_component, game::bc_height_map_component>(),
			game::bc_abstract_component_register<game::bc_rigid_body_component, game::bc_rigid_static_component, game::bc_rigid_dynamic_component, game::bc_rigid_controller_component>(),
			game::bc_abstract_component_register<game::bc_decal_resolver_component, game::bc_mesh_component, game::bc_height_map_component>()
		);
		game::bc_register_actor_controller_types
		(
			game::bc_actor_controller_register<bc_xbot_idle_actor_controller>("xbot_idle"),
			game::bc_actor_controller_register<bc_xbot_player_actor_controller>("xbot_player"),
			game::bc_actor_controller_register<bc_xbot_network_player_actor_controller>("xbot_network_player"),
			game::bc_actor_controller_register<bc_rigid_dynamic_network_actor_controller>("rigid_dynamic_network")
		);
	}

	void bc_register_engine_network_messages(game::bc_network_system& p_network_system)
	{
		p_network_system.register_messages
		<
			game::bc_string_network_message,
			game::bc_ping_network_message,
			game::bc_acknowledge_network_message,
			game::bc_client_connect_network_message,
			game::bc_client_disconnect_network_message,
			game::bc_actor_replicate_network_message,
			game::bc_actor_sync_network_message,
			game::bc_actor_remove_network_message,
			game::bc_scene_change_network_message,
			game::bc_scene_replicate_network_message,
			bc_xbot_weapon_attach_network_message,
			bc_xbot_weapon_detach_network_message,
			bc_xbot_weapon_shoot_network_message,
			bc_xbot_start_grenade_throw_network_message,
			bc_xbot_grenade_throw_network_message,
			bc_xbot_ragdoll_activation_network_message
		>();
	}
	
	void bc_bind_engine_scripts(game::bc_game_system& p_game_system)
	{
		auto& l_script_system = p_game_system.get_script_system();
		auto l_script_binder = l_script_system.get_script_binder();

		l_script_binder.bind(game::bc_script_context::app, p_game_system.get_console());

		l_script_system.set_script_binder(std::move(l_script_binder));
	}

	void bc_register_engine_particle_emitters(game::bc_game_system& p_game_system)
	{
	}

	void bc_load_engine_shaders(core::bc_content_stream_manager& p_stream_manager, game::bc_game_system& p_game_system)
	{
		p_stream_manager.read_stream_file(p_game_system.get_file_system().get_content_data_path(bcL("ContentStream.json")).c_str());

		p_stream_manager.load_content_stream(core::bc_alloc_type::program, "engine_shaders");
		p_stream_manager.load_content_stream(core::bc_alloc_type::program, "deferred_rendering_shaders");
		p_stream_manager.load_content_stream(core::bc_alloc_type::program, "particle_shaders");
		p_stream_manager.load_content_stream(core::bc_alloc_type::program, "post_process_shaders");
	}

	void bc_load_engine_resources(core::bc_content_stream_manager& p_stream_manager, game::bc_game_system& p_game_system)
	{
		auto& l_entity_manager = *core::bc_get_service<game::bc_entity_manager>();
		auto& l_material_manager = p_game_system.get_render_system().get_material_manager();
		auto& l_decal_manager = p_game_system.get_render_system().get_decal_manager();

		l_entity_manager.read_entity_file(p_game_system.get_file_system().get_content_data_path(bcL("EntityType.json")).c_str());
		l_material_manager.read_material_file(p_game_system.get_file_system().get_content_data_path(bcL("Material.json")).c_str());
		l_decal_manager.read_decal_file(p_game_system.get_file_system().get_content_data_path(bcL("Decal.json")).c_str());

		p_stream_manager.load_content_stream(core::bc_alloc_type::program, "engine_assets");
	}

	void bc_unload_engine_resources(core::bc_content_stream_manager& p_stream_manager)
	{
		p_stream_manager.unload_content_stream("engine_assets");
		p_stream_manager.unload_content_stream("post_process_shaders");
		p_stream_manager.unload_content_stream("particle_shaders");
		p_stream_manager.unload_content_stream("deferred_rendering_shaders");
		p_stream_manager.unload_content_stream("engine_shaders");
	}

	void bc_close_engine_services()
	{
		core::bc_service_manager::close();
		
#ifdef BC_MEMORY_ENABLE
#ifdef BC_MEMORY_LEAK_DETECTION
		const bcUINT32 l_leak_counts = core::bc_memory_manager::get().report_memory_leaks();
		if (l_leak_counts > 0)
		{
			BC_ASSERT(false);
		}
#endif
		core::bc_memory_manager::close();
#endif
	}
}
