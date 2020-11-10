// [11/22/2018 MRB]

#include "BlackCat/BlackCatPCH.h"

#include "PlatformImp/bcIDELogger.h"
#include "Core/Memory/bcMemoryManagment.h"
#include "Core/Concurrency/bcThreadManager.h"
#include "Core/Event/bcEventManager.h"
#include "Core/Messaging/Query/bcQueryManager.h"
#include "Core/Content/bcContentManager.h"
#include "Core/Content/bcContentStreamManager.h"
#include "Core/Utility/bcServiceManager.h"
#include "Core/Utility/bcCounterValueManager.h"
#include "Core/Utility/bcLogger.h"
#include "Core/bcEvent.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/bcEntityManager.h"
#include "Game/System/Script/bcScriptBinding.h"
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
		m_time_delta_buffer{},
		m_current_time_delta_sample(0),
		m_fps(0)
	{
		std::memset(&m_time_delta_buffer, 0, sizeof(core_platform::bc_clock::small_delta_time) * s_num_time_delta_samples);
	}

	bc_render_application::~bc_render_application() = default;

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

		core::bc_register_service(core::bc_make_service<core::bc_logger>());
#ifdef BC_DEBUG
		core::bc_get_service<core::bc_logger>()->register_listener
		(
			core::bc_enum:: or ({ core::bc_log_type::debug, core::bc_log_type::error }),
			core::bc_make_unique< platform::bc_ide_logger >(core::bc_alloc_type::program)
		);
#endif
		core::bc_register_service(core::bc_make_service< core::bc_thread_manager >
		(
			p_parameters.m_engine_parameters.m_thread_manager_thread_count,
			p_parameters.m_engine_parameters.m_thread_manager_reserved_thread_count
		));
		core::bc_register_service(core::bc_make_service<core::bc_event_manager>());
		core::bc_register_service(core::bc_make_service<core::bc_query_manager>());
		core::bc_register_service(core::bc_make_service<core::bc_counter_value_manager>());
		core::bc_register_service(core::bc_make_service<core::bc_content_manager>());
		core::bc_register_service(core::bc_make_service<core::bc_content_stream_manager>(*core::bc_get_service<core::bc_content_manager>()));
		core::bc_register_service(core::bc_make_service<game::bc_actor_component_manager>());
		core::bc_register_service(core::bc_make_service<game::bc_entity_manager>(*core::bc_get_service<game::bc_actor_component_manager>()));
		core::bc_register_service(core::bc_make_service<game::bc_game_system>());

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

		m_game_system = core::bc_get_service<game::bc_game_system>();
		
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

	void bc_render_application::app_update(core_platform::bc_clock::update_param p_clock_update_param, bool p_is_same_frame)
	{
		m_update_watch.start();
		
		auto* l_event_manager = core::bc_get_service<core::bc_event_manager>();

		if(!p_is_same_frame)
		{
			core::bc_event_frame_update_start l_event_frame_start;
			l_event_manager->process_event(l_event_frame_start);
		}
		
		m_game_system->update_game(p_clock_update_param, p_is_same_frame);

		if (!p_is_same_frame)
		{
			core::bc_service_manager::get().update(p_clock_update_param);
		}
		
		application_update(p_clock_update_param, p_is_same_frame);

		if(!p_is_same_frame)
		{
			core::bc_event_frame_update_finish l_event_frame_finish;
			l_event_manager->process_event(l_event_frame_finish);
		}

		m_update_watch.stop();
	}

	void bc_render_application::app_render(core_platform::bc_clock::update_param p_clock_update_param)
	{
		m_render_watch.start();
		
		auto* l_event_manager = core::bc_get_service<core::bc_event_manager>();

		core::bc_event_frame_render_start l_event_frame_start;
		l_event_manager->process_event(l_event_frame_start);
		
		m_game_system->render_game(p_clock_update_param);
		application_render(p_clock_update_param);

		core::bc_event_frame_render_finish l_event_frame_finish;
		l_event_manager->process_event(l_event_frame_finish);

		m_render_watch.stop();
	}

	void bc_render_application::app_swap_frame(core_platform::bc_clock::update_param p_clock)
	{
		auto* l_event_manager = core::bc_get_service<core::bc_event_manager>();
		auto* l_counter_value_manager = core::bc_get_service<core::bc_counter_value_manager>();
		
		m_game_system->swap_frame(p_clock);

		m_update_watch.restart();
		m_render_watch.restart();
		_calculate_fps(p_clock.m_elapsed);

		const auto l_update_time = m_update_watch.average_total_elapsed();
		const auto l_render_time = m_render_watch.average_total_elapsed();

		l_counter_value_manager->add_counter("fps", core::bc_to_wstring(m_fps));
		l_counter_value_manager->add_counter("update_time", core::bc_to_wstring(l_update_time, L"%.1f"));
		l_counter_value_manager->add_counter("render_time", core::bc_to_wstring(l_render_time, L"%.1f"));

		core::bc_event_frame_swap l_event_frame_swap;
		l_event_manager->process_event(l_event_frame_swap);
	}

	bool bc_render_application::app_event(core::bc_ievent& p_event)
	{
		return application_event(p_event);
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
		const bcUINT32 l_leak_counts = core::bc_memmng::get().report_memory_leaks();
		if (l_leak_counts > 0)
		{
			bcAssert(false);
		}
#endif

		core::bc_memmng::close();
#endif
	}

	void bc_render_application::_calculate_fps(core_platform::bc_clock::small_delta_time p_elapsed)
	{
		m_time_delta_buffer[m_current_time_delta_sample] = p_elapsed;
		m_current_time_delta_sample = (m_current_time_delta_sample + 1) % s_num_time_delta_samples;

		core_platform::bc_clock::small_delta_time l_average_delta = 0;
		for (float i : m_time_delta_buffer)
		{
			l_average_delta += i;
		}
		l_average_delta /= s_num_time_delta_samples;

		m_fps = std::round(1000.0f / l_average_delta);
	}
}