// [11/22/2018 MRB]

#include "BlackCat/BlackCatPCH.h"

#include "PlatformImp/bcIDELogger.h"
#include "Core/Memory/bcMemoryManagment.h"
#include "Core/Utility/bcLogger.h"
#include "Core/Concurrency/bcThreadManager.h"
#include "Core/Event/bcEventManager.h"
#include "Core/Content/bcContentManager.h"
#include "Core/Content/bcContentStreamManager.h"
#include "Core/Content/bcLazyContent.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/bcEntityManager.h"
#include "Game/System/Script/bcScriptBinding.h"
#include "Game/System/Render/Pass/bcRenderPassManager.h"
#include "Game/System/Render/bcMaterialManager.h"
#include "Game/System/Script/bcScriptSystem.h"
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
#include "GraphicImp/Device/bcDevice.h"
#include "GraphicImp/Device/bcDevicePipeline.h"
#include "GraphicImp/Device/Command/bcDeviceCommandExecutor.h"
#include "BlackCat/Application/bcRenderApplication.h"
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
	bc_render_application::bc_render_application()
		: game::bc_render_application(),
		m_game_system(nullptr),
		m_service_manager(nullptr)
	{
	}

	bc_render_application::~bc_render_application()
	{
	}

	void bc_render_application::app_start_engine_components(game::bc_engine_application_parameter& p_parameters)
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
		m_service_manager = &core::bc_service_manager::get();

		core::bc_register_service(core::bc_make_service<core::bc_logger>());
		core::bc_register_service(core::bc_make_service<core::bc_event_manager>());
		core::bc_register_service(core::bc_make_service<core::bc_thread_manager>
		(
			p_parameters.m_engine_parameters.m_thread_manager_thread_count,
			p_parameters.m_engine_parameters.m_thread_manager_reserved_thread_count
		));
		core::bc_register_service(core::bc_make_service<core::bc_content_manager>());
		core::bc_register_service(core::bc_make_service<core::bc_content_stream_manager>(*core::bc_get_service<core::bc_content_manager>()));
		core::bc_register_service(core::bc_make_service<game::bc_game_system>());
		core::bc_register_service(core::bc_make_service<game::bc_actor_component_manager>());
		core::bc_register_service(core::bc_make_service<game::bc_entity_manager>(*core::bc_get_service<game::bc_actor_component_manager>()));

		auto* l_log_manager = core::bc_get_service<core::bc_logger>();
		auto* l_content_stream_manager = core::bc_get_service<core::bc_content_stream_manager>();
		auto* l_entity_manager = core::bc_get_service<game::bc_entity_manager>();
		m_game_system = core::bc_get_service<game::bc_game_system>();

#ifdef BC_DEBUG
		l_log_manager->register_listener
		(
			core::bc_enum:: or ({ core::bc_log_type::debug, core::bc_log_type::error }),
			core::bc_make_unique< platform::bc_ide_logger >(core::bc_alloc_type::program)
		);
#endif

		core::bc_register_loader< graphic::bc_texture2d_content, bc_texture_loader >(core::bc_make_loader< bc_texture_loader >());
		core::bc_register_loader< graphic::bc_vertex_shader_content, bc_vertex_shader_loader >(core::bc_make_loader< bc_vertex_shader_loader >());
		core::bc_register_loader< graphic::bc_hull_shader_content, bc_hull_shader_loader >(core::bc_make_loader< bc_hull_shader_loader >());
		core::bc_register_loader< graphic::bc_domain_shader_content, bc_domain_shader_loader >(core::bc_make_loader< bc_domain_shader_loader >());
		core::bc_register_loader< graphic::bc_geometry_shader_content, bc_geometry_shader_loader >(core::bc_make_loader< bc_geometry_shader_loader >());
		core::bc_register_loader< graphic::bc_pixel_shader_content, bc_pixel_shader_loader >(core::bc_make_loader< bc_pixel_shader_loader >());
		core::bc_register_loader< graphic::bc_compute_shader_content, bc_compute_shader_loader >(core::bc_make_loader< bc_compute_shader_loader >());
		core::bc_register_loader< game::bc_mesh_collider, bc_mesh_collider_loader >(core::bc_make_loader< bc_mesh_collider_loader >(true));
		core::bc_register_loader< game::bc_mesh, bc_mesh_loader >(core::bc_make_loader< bc_mesh_loader >());
		core::bc_register_loader< game::bc_scene, bc_scene_loader >(core::bc_make_loader< bc_scene_loader >(std::move(p_parameters.m_app_parameters.m_scene_graph_factory)));

		l_entity_manager->register_component_types
		<
			game::bc_mediate_component,
			game::bc_simple_mesh_component,
			game::bc_vegetable_mesh_component,
			game::bc_hierarchy_component,
			game::bc_rigid_static_component,
			game::bc_rigid_dynamic_component,
			game::bc_height_map_component,
			game::bc_light_component
		>();
		l_entity_manager->register_abstract_component_types
		<
			game::bc_abstract_component_register< game::bc_mesh_component, game::bc_simple_mesh_component, game::bc_vegetable_mesh_component >,
			game::bc_abstract_component_register< game::bc_render_component, game::bc_mesh_component, game::bc_height_map_component >,
			game::bc_abstract_component_register< game::bc_rigid_body_component, game::bc_rigid_static_component, game::bc_rigid_dynamic_component >
		>();

		application_start_engine_components(p_parameters);
	}

	void bc_render_application::app_initialize(game::bc_engine_application_parameter& p_parameters)
	{
		m_game_system->initialize
		(
			game::bc_game_system_parameter
			(
				game::bc_render_system_parameter
				(
					game::bc_render_application::get_output_window().get_width(),
					game::bc_render_application::get_output_window().get_height(),
					graphic::bc_format::R8G8B8A8_UNORM,
					game::bc_render_application::get_output_window().get_device_output()
				)
			)
		);

		auto* l_content_stream_manager = core::bc_get_service< core::bc_content_stream_manager >();
		auto* l_entity_manager = core::bc_get_service< game::bc_entity_manager >();
		auto& l_material_manager = m_game_system->get_render_system().get_material_manager();
		auto& l_script_system = m_game_system->get_script_system();
		auto l_script_binder = l_script_system.get_script_binder();

		l_content_stream_manager->read_stream_file(m_game_system->get_file_system().get_content_data_path(bcL("ContentStream.json")).c_str());
		l_entity_manager->read_entity_file(m_game_system->get_file_system().get_content_data_path(bcL("EntityType.json")).c_str());
		l_material_manager.read_material_file(m_game_system->get_file_system().get_content_data_path(bcL("Material.json")).c_str());

		l_content_stream_manager->load_content_stream(core::bc_alloc_type::program, "engine_resources");
		l_content_stream_manager->load_content_stream(core::bc_alloc_type::program, "engine_shaders");
		l_content_stream_manager->load_content_stream(core::bc_alloc_type::program, "deferred_rendering_shaders");

		l_script_binder.bind<game::bc_game_console>(game::bc_script_context::ui, m_game_system->get_console());

		l_script_system.set_script_binder(std::move(l_script_binder));

		application_initialize(p_parameters);
	}

	void bc_render_application::app_load_content()
	{
		application_load_content(core::bc_get_service< core::bc_content_stream_manager >());
	}

	void bc_render_application::app_update(core_platform::bc_clock::update_param p_clock_update_param)
	{
		m_service_manager->update(p_clock_update_param);
		application_update(p_clock_update_param);
	}

	void bc_render_application::app_render(core_platform::bc_clock::update_param p_clock_update_param)
	{
		application_render(p_clock_update_param);
	}

	bool bc_render_application::app_event(core::bc_ievent& p_event)
	{
		const bool l_result = application_event(p_event);

		return l_result;
	}

	void bc_render_application::app_unload_content()
	{
		auto* l_content_stream_manager = core::bc_get_service< core::bc_content_stream_manager >();

		application_unload_content(l_content_stream_manager);
		
		l_content_stream_manager->unload_content_stream("deferred_rendering_shaders");
		l_content_stream_manager->unload_content_stream("engine_shaders");
		l_content_stream_manager->unload_content_stream("engine_resources");
	}

	void bc_render_application::app_destroy()
	{
		application_destroy();
	}

	void bc_render_application::app_close_engine_components()
	{
		application_close_engine_components();

		core::bc_service_manager::close();
#ifdef BC_MEMORY_ENABLE
#ifdef BC_MEMORY_LEAK_DETECTION
		bcUINT32 l_leak_counts = core::bc_memmng::get().report_memory_leaks();
		if (l_leak_counts > 0)
		{
			bcAssert(false);
		}
#endif

		core::bc_memmng::close();
#endif
	}
}