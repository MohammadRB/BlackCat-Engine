// [11/24/2020 MRB]

#pragma once

#include "PlatformImp/bcIDELogger.h"
#include "Core/Memory/bcMemoryManagment.h"
#include "Core/Concurrency/bcThreadManager.h"
#include "Core/Messaging/Event/bcEventManager.h"
#include "Core/Messaging/Query/bcQueryManager.h"
#include "Core/Content/bcContentManager.h"
#include "Core/Content/bcContentStreamManager.h"
#include "Core/Utility/bcServiceManager.h"
#include "Core/Utility/bcCounterValueManager.h"
#include "Core/Utility/bcLogger.h"
#include "Game/Application/bcEngineApplicationParameter.h"
#include "Game/System/bcGameSystem.h"
#include "Game/System/Script/bcScriptSystem.h"
#include "Game/System/Script/bcScriptBinding.h"
#include "Game/System/Render/bcMaterialManager.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/bcEntityManager.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/Object/Scene/Component/bcRenderComponent.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"
#include "Game/Object/Scene/Component/bcSimpleMeshComponent.h"
#include "Game/Object/Scene/Component/bcVegetableMeshComponent.h"
#include "Game/Object/Scene/Component/bcHierarchyComponent.h"
#include "Game/Object/Scene/Component/bcHeightMapComponent.h"
#include "Game/Object/Scene/Component/bcRigidStaticComponent.h"
#include "Game/Object/Scene/Component/bcRigidDynamicComponent.h"
#include "Game/Object/Scene/Component/bcLightComponent.h"
#include "BlackCat/Loader/bcMeshLoader.h"
#include "BlackCat/Loader/bcTextureLoader.h"
#include "BlackCat/Loader/bcVertexShaderLoader.h"
#include "BlackCat/Loader/bcHullShaderLoader.h"
#include "BlackCat/Loader/bcDomainShaderLoader.h"
#include "BlackCat/Loader/bcGeometryShaderLoader.h"
#include "BlackCat/Loader/bcPixelShaderLoader.h"
#include "BlackCat/Loader/bcComputeShaderLoader.h"
#include "BlackCat/Loader/bcMeshColliderLoader.h"
#include "BlackCat/Loader/bcSceneLoader.h"

namespace black_cat
{
	inline void bc_start_engine_services(game::bc_engine_application_parameter& p_parameters)
	{
#ifdef BC_MEMORY_ENABLE
		core::bc_memmng::startup
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
			core::bc_enum:: or ({ core::bc_log_type::debug, core::bc_log_type::error }),
			core::bc_make_unique< platform::bc_ide_logger >(core::bc_alloc_type::program)
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
		core::bc_register_service(core::bc_make_service<game::bc_actor_component_manager>());
		core::bc_register_service(core::bc_make_service<game::bc_entity_manager>(*core::bc_get_service<game::bc_actor_component_manager>()));
		core::bc_register_service(core::bc_make_service<game::bc_game_system>());
	}

	inline void bc_register_engine_loaders(game::bc_engine_application_parameter& p_parameters)
	{
		core::bc_register_loader< graphic::bc_texture2d_content, bc_texture_loader >("texture2d", core::bc_make_loader< bc_texture_loader >());
		core::bc_register_loader< graphic::bc_vertex_shader_content, bc_vertex_shader_loader >("vertex_shader", core::bc_make_loader< bc_vertex_shader_loader >());
		core::bc_register_loader< graphic::bc_hull_shader_content, bc_hull_shader_loader >("hull_shader", core::bc_make_loader< bc_hull_shader_loader >());
		core::bc_register_loader< graphic::bc_domain_shader_content, bc_domain_shader_loader >("domain_shader", core::bc_make_loader< bc_domain_shader_loader >());
		core::bc_register_loader< graphic::bc_geometry_shader_content, bc_geometry_shader_loader >("geometry_shader", core::bc_make_loader< bc_geometry_shader_loader >());
		core::bc_register_loader< graphic::bc_pixel_shader_content, bc_pixel_shader_loader >("pixel_shader", core::bc_make_loader< bc_pixel_shader_loader >());
		core::bc_register_loader< graphic::bc_compute_shader_content, bc_compute_shader_loader >("compute_shader", core::bc_make_loader< bc_compute_shader_loader >());
		core::bc_register_loader< game::bc_mesh_collider, bc_mesh_collider_loader >("mesh_collider", core::bc_make_loader< bc_mesh_collider_loader >(true));
		core::bc_register_loader< game::bc_mesh, bc_mesh_loader >("mesh", core::bc_make_loader< bc_mesh_loader >());
		core::bc_register_loader< game::bc_scene, bc_scene_loader >("scene", core::bc_make_loader< bc_scene_loader >(std::move(p_parameters.m_app_parameters.m_scene_graph_factory)));
	}

	inline void bc_register_engine_actor_components()
	{
		game::bc_register_component_types
		(
			game::bc_component_register< game::bc_mediate_component >("mediate"),
			game::bc_component_register< game::bc_simple_mesh_component >("simple_mesh"),
			game::bc_component_register< game::bc_vegetable_mesh_component >("vegetable_mesh"),
			game::bc_component_register< game::bc_hierarchy_component >("hierarchy"),
			game::bc_component_register< game::bc_rigid_static_component >("rigid_static"),
			game::bc_component_register< game::bc_rigid_dynamic_component >("rigid_dynamic"),
			game::bc_component_register< game::bc_height_map_component >("height_map"),
			game::bc_component_register< game::bc_light_component >("light")
		);
		game::bc_register_abstract_component_types
		(
			game::bc_abstract_component_register< game::bc_mesh_component, game::bc_simple_mesh_component, game::bc_vegetable_mesh_component >(),
			game::bc_abstract_component_register< game::bc_render_component, game::bc_mesh_component, game::bc_height_map_component >(),
			game::bc_abstract_component_register< game::bc_rigid_body_component, game::bc_rigid_static_component, game::bc_rigid_dynamic_component >()
		);
	}

	inline void bc_bind_scripts(game::bc_game_system& p_game_system)
	{
		auto& l_script_system = p_game_system.get_script_system();
		auto l_script_binder = l_script_system.get_script_binder();

		l_script_binder.bind<game::bc_game_console>(game::bc_script_context::ui, p_game_system.get_console());

		l_script_system.set_script_binder(std::move(l_script_binder));
	}
	
	inline void bc_load_engine_resources(game::bc_game_system& p_game_system)
	{
		auto* l_content_stream_manager = core::bc_get_service< core::bc_content_stream_manager >();
		auto* l_entity_manager = core::bc_get_service< game::bc_entity_manager >();
		auto& l_material_manager = p_game_system.get_render_system().get_material_manager();

		l_content_stream_manager->read_stream_file(p_game_system.get_file_system().get_content_data_path(bcL("ContentStream.json")).c_str());
		l_entity_manager->read_entity_file(p_game_system.get_file_system().get_content_data_path(bcL("EntityType.json")).c_str());
		l_material_manager.read_material_file(p_game_system.get_file_system().get_content_data_path(bcL("Material.json")).c_str());

		l_content_stream_manager->load_content_stream(core::bc_alloc_type::program, "engine_resources");
		l_content_stream_manager->load_content_stream(core::bc_alloc_type::program, "engine_shaders");
		l_content_stream_manager->load_content_stream(core::bc_alloc_type::program, "deferred_rendering_shaders");
		l_content_stream_manager->load_content_stream(core::bc_alloc_type::program, "particle_shaders");
	}
	
	inline void bc_unload_engine_resources(core::bc_content_stream_manager& p_stream_manager)
	{
		p_stream_manager.unload_content_stream("particle_shaders");
		p_stream_manager.unload_content_stream("deferred_rendering_shaders");
		p_stream_manager.unload_content_stream("engine_shaders");
		p_stream_manager.unload_content_stream("engine_resources");
	}

	inline void bc_close_engine_services()
	{
		core::bc_service_manager::close();
#ifdef BC_MEMORY_ENABLE
#ifdef BC_MEMORY_LEAK_DETECTION
		const bcUINT32 l_leak_counts = core::bc_memmng::get().report_memory_leaks();
		if (l_leak_counts > 0)
		{
			bcAssert(false);
		}
#endif

		core::bc_memmng::close();
#endif
	}
}