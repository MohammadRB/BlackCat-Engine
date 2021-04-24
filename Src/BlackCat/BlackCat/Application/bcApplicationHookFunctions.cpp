// [12/29/2020 MRB]

#include "BlackCat/BlackCatPCH.h"

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
#include "Game/Application/bcEngineApplicationParameter.h"
#include "Game/System/bcGameSystem.h"
#include "Game/System/Script/bcScriptSystem.h"
#include "Game/System/Script/bcScriptBinding.h"
#include "Game/System/Render/Material/bcMaterialManager.h"
#include "Game/System/Render/Particle/bcParticleManager.h"
#include "Game/System/Render/Decal/bcDecalManager.h"
#include "Game/System/Physics/bcPhysicsSimulationCallback.h"
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
#include "Game/Object/Animation/bcSkinnedAnimation.h"
#include "BlackCat/Application/bcApplicationHookFuncations.h"
#include "BlackCat/Loader/bcTextureLoader.h"
#include "BlackCat/Loader/bcVertexShaderLoader.h"
#include "BlackCat/Loader/bcHullShaderLoader.h"
#include "BlackCat/Loader/bcDomainShaderLoader.h"
#include "BlackCat/Loader/bcGeometryShaderLoader.h"
#include "BlackCat/Loader/bcPixelShaderLoader.h"
#include "BlackCat/Loader/bcComputeShaderLoader.h"
#include "BlackCat/Loader/bcMeshColliderLoader.h"
#include "BlackCat/Loader/bcMeshLoader.h"
#include "BlackCat/Loader/bcSkinnedAnimationLoader.h"
#include "BlackCat/Loader/bcSceneLoader.h"
#include "BlackCat/Loader/bcSceneCheckPointLoader.h"
#include "BlackCat/SampleImp/ActorController/bcFireActorController.h"
#include "BlackCat/SampleImp/ActorController/bcExplosionActorController.h"
#include "BlackCat/SampleImp/ActorController/bcXBotController.h"
#include "BlackCat/SampleImp/ActorController/bcXBotCameraController.h"
#include "BlackCat/SampleImp/Particle/bcExplosionParticle.h"
#include "BlackCat/SampleImp/Particle/bcWeaponParticle.h"

namespace black_cat
{
	void bc_start_engine_services(game::bc_engine_application_parameter& p_parameters)
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

		core::bc_register_service(core::bc_make_service<core::bc_logger>());
#ifdef BC_DEBUG
		core::bc_get_service<core::bc_logger>()->register_listener
		(
			core::bc_enum::mask_or({ core::bc_log_type::debug, core::bc_log_type::error }),
			core::bc_make_unique<platform::bc_ide_logger>(core::bc_alloc_type::program)
		);
#endif
		core::bc_register_service
		(
			core::bc_make_service< core::bc_thread_manager >
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
		core::bc_register_service(core::bc_make_service<game::bc_actor_component_manager>(*core::bc_get_service<core::bc_query_manager>(), *core::bc_get_service<game::bc_game_system>()));
		core::bc_register_service(core::bc_make_service<game::bc_entity_manager>
		(
			*core::bc_get_service<core::bc_content_stream_manager>(), 
			*core::bc_get_service<game::bc_actor_component_manager>(),
			*core::bc_get_service<game::bc_game_system>()
		));
	}

	void bc_register_engine_loaders(game::bc_engine_application_parameter& p_parameters)
	{
		core::bc_register_loader<graphic::bc_texture2d_content, bc_texture_loader>("texture2d", core::bc_make_loader<bc_texture_loader>());
		core::bc_register_loader<graphic::bc_vertex_shader_content, bc_vertex_shader_loader>("vertex_shader", core::bc_make_loader<bc_vertex_shader_loader>());
		core::bc_register_loader<graphic::bc_hull_shader_content, bc_hull_shader_loader>("hull_shader", core::bc_make_loader<bc_hull_shader_loader>());
		core::bc_register_loader<graphic::bc_domain_shader_content, bc_domain_shader_loader>("domain_shader", core::bc_make_loader<bc_domain_shader_loader>());
		core::bc_register_loader<graphic::bc_geometry_shader_content, bc_geometry_shader_loader>("geometry_shader", core::bc_make_loader<bc_geometry_shader_loader>());
		core::bc_register_loader<graphic::bc_pixel_shader_content, bc_pixel_shader_loader>("pixel_shader", core::bc_make_loader<bc_pixel_shader_loader>());
		core::bc_register_loader<graphic::bc_compute_shader_content, bc_compute_shader_loader>("compute_shader", core::bc_make_loader<bc_compute_shader_loader>());
		core::bc_register_loader<game::bc_mesh_collider, bc_mesh_collider_loader>("mesh_collider", core::bc_make_loader<bc_mesh_collider_loader>());
		core::bc_register_loader<game::bc_mesh, bc_mesh_loader>("mesh", core::bc_make_loader< bc_mesh_loader >());
		core::bc_register_loader<game::bc_skinned_animation, bc_skinned_animation_loader>("animation", core::bc_make_loader<bc_skinned_animation_loader>());
		core::bc_register_loader<game::bc_scene, bc_scene_loader>
		(
			"scene",
			core::bc_make_loader<bc_scene_loader>
			(
				[]()
				{
					return std::move
					(
						physics::bc_scene_builder()
						.enable_locking()
						.enable_ccd()
						.use_simulation_callback(core::bc_make_unique<game::bc_physics_simulation_callback>())
					);
				},
				std::move(p_parameters.m_app_parameters.m_scene_graph_factory)
			)
		);
		core::bc_register_loader<game::bc_scene_check_point, bc_scene_check_point_loader>("scene_check_point", core::bc_make_loader<bc_scene_check_point_loader>());
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
			game::bc_component_register<game::bc_weapon_component>("weapon")
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
			game::bc_actor_controller_register<bc_fire_actor_controller>("fire"),
			game::bc_actor_controller_register<bc_explosion_actor_controller>("explosion"),
			game::bc_actor_controller_register<bc_xbot_controller>("xbot"),
			game::bc_actor_controller_register<bc_xbot_camera_controller>("xbot_player")
		);
	}

	void bc_bind_scripts(game::bc_game_system& p_game_system)
	{
		auto& l_script_system = p_game_system.get_script_system();
		auto l_script_binder = l_script_system.get_script_binder();

		l_script_binder.bind<game::bc_game_console>(game::bc_script_context::ui, p_game_system.get_console());

		l_script_system.set_script_binder(std::move(l_script_binder));
	}

	void bc_load_engine_resources(game::bc_game_system& p_game_system)
	{
		auto* l_content_stream_manager = core::bc_get_service<core::bc_content_stream_manager>();
		auto& l_material_manager = p_game_system.get_render_system().get_material_manager();
		auto& l_decal_manager = p_game_system.get_render_system().get_decal_manager();
		auto* l_entity_manager = core::bc_get_service<game::bc_entity_manager>();

		l_content_stream_manager->read_stream_file(p_game_system.get_file_system().get_content_data_path(bcL("ContentStream.json")).c_str());
		l_material_manager.read_material_file(p_game_system.get_file_system().get_content_data_path(bcL("Material.json")).c_str());
		l_decal_manager.read_decal_file(p_game_system.get_file_system().get_content_data_path(bcL("Decal.json")).c_str());
		l_entity_manager->read_entity_file(p_game_system.get_file_system().get_content_data_path(bcL("EntityType.json")).c_str());

		l_content_stream_manager->load_content_stream(core::bc_alloc_type::program, "engine_shaders");
		l_content_stream_manager->load_content_stream(core::bc_alloc_type::program, "deferred_rendering_shaders");
		l_content_stream_manager->load_content_stream(core::bc_alloc_type::program, "particle_shaders");
		l_content_stream_manager->load_content_stream(core::bc_alloc_type::program, "engine_assets");
	}

	void bc_register_particle_emitters(game::bc_game_system& p_game_system)
	{
		auto& l_particle_manager = p_game_system.get_render_system().get_particle_manager();
		core::bc_random l_random;

		l_particle_manager.register_emitter_definition("big_explosion", bc_big_explosion_particle()(l_random));
		l_particle_manager.register_emitter_definition("rifle_fire", bc_rifle_fire_particle()());
		l_particle_manager.register_emitter_definition("bullet_terrain", bc_bullet_terrain_particle()(l_random));
		l_particle_manager.register_emitter_definition("bullet_soil", bc_bullet_soil_particle()());
		l_particle_manager.register_emitter_definition("bullet_iron", bc_bullet_iron_particle()());
	}

	void bc_unload_engine_resources(core::bc_content_stream_manager& p_stream_manager)
	{
		p_stream_manager.unload_content_stream("engine_assets");
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