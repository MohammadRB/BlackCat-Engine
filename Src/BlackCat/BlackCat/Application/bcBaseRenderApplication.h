// [03/23/2016 MRB]

#pragma once

#include "Core/bcConstant.h"
#include "Core/Memory/bcMemoryManagment.h"
#include "Core/bcEvent.h"
#include "Core/Utility/bcSingleton.h"
#include "Core/Utility/bcServiceManager.h"
#include "Core/Utility/bcLogger.h"
#include "Core/Utility/bcExpressionParameterManager.h"
#include "Core/Concurrency/bcThreadManager.h"
#include "Core/Event/bcEventManager.h"
#include "Core/File/bcContentManager.h"
#include "Core/File/bcContentStreamManager.h"
#include "Core/File/bcLazyContent.h"
#include "PlatformImp/bc_ide_logger.h"
#include "GraphicImp/Device/bcDevice.h"
#include "GraphicImp/Device/bcDevicePipeline.h"
#include "GraphicImp/Device/Command/bcDeviceCommandExecutor.h"
#include "Game/Application/bcRenderApplication.h"
#include "Game/System/Render/bcRenderPassManager.h"
#include "Game/System/Render/bcMaterialManager.h"
#include "Game/System/Script/bcScriptSystem.h"
#include "Game/System/Script/bcScriptBinding.h"
#include "Game/Object/Scene/bcActorComponentManager.h"
#include "Game/Object/Scene/bcEntityManager.h"
#include "Game/Object/Scene/Component/bcRenderComponent.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"
#include "Game/Object/Scene/Component/bcHierarchyComponent.h"
#include "Game/Object/Scene/Component/bcHeightMapComponent.h"
#include "Game/Object/Scene/Component/bcRigidStaticComponent.h"
#include "Game/Object/Scene/Component/bcRigidDynamicComponent.h"
#include "Game/Object/Scene/Component/bcNameComponent.h"
#include "Game/System/bcGameSystem.h"
#include "BlackCat/Loader/bcMeshLoader.h"
#include "BlackCat/Loader/bcTextureLoader.h"
#include "BlackCat/Loader/bcVertexShaderLoader.h"
#include "BlackCat/Loader/bcHullShaderLoader.h"
#include "BlackCat/Loader/bcDomainShaderLoader.h"
#include "BlackCat/Loader/bcGeometryShaderLoader.h"
#include "BlackCat/Loader/bcPixelShaderLoader.h"
#include "BlackCat/Loader/bcComputeShaderLoader.h"
#include "BlackCat/BlackCatPCH.h"
#include "BlackCat/Loader/bcMeshColliderLoader.h"

namespace black_cat
{
	template< class TApp >
	class bc_base_render_application :
		public game::bc_render_application,
		public core::bc_singleton< TApp(game::bc_engine_application_parameter&) >
	{
	public:
		using bc_render_application::initialize;
		using bc_render_application::destroy;

	public:
		bc_base_render_application();

		virtual ~bc_base_render_application();

		virtual void application_start_engine_components(game::bc_engine_component_parameter& p_engine_components, core::bc_service_manager& p_service_manager) = 0;

		virtual void application_initialize(const bcCHAR* p_commandline) = 0;

		virtual void application_load_content(core::bc_content_stream_manager* p_stream_manager) = 0;

		virtual void application_update(core_platform::bc_clock::update_param p_clock_update_param) = 0;

		virtual void application_render(core_platform::bc_clock::update_param p_clock_update_param) = 0;

		virtual bool application_event(core::bc_ievent& p_event) = 0;

		virtual void application_unload_content(core::bc_content_stream_manager* p_stream_manager) = 0;

		virtual void application_destroy() = 0;

		virtual void application_close_engine_components() = 0;

	protected:
		core::bc_service_manager* m_service_manager;
		game::bc_game_system* m_game_system;

	private:
		void app_start_engine_components(game::bc_engine_component_parameter& p_engine_components) override final;

		void app_initialize(const bcCHAR* p_commandline) override final;

		void app_load_content() override final;

		void app_update(core_platform::bc_clock::update_param p_clock_update_param) override final;

		void app_render(core_platform::bc_clock::update_param p_clock_update_param) override;

		bool app_event(core::bc_ievent& p_event) override final;

		void app_unload_content() override final;

		void app_destroy() override final;

		void app_close_engine_components() override final;

		void _initialize(game::bc_engine_application_parameter&) override;

		void _destroy() override;
	};

	template< class TApp >
	bc_base_render_application< TApp >::bc_base_render_application()
		: bc_render_application(),
		m_service_manager(nullptr),
		m_game_system(nullptr)
	{
	}

	template< class TApp >
	bc_base_render_application< TApp >::~bc_base_render_application()
	{
	}

	template< class TApp >
	void bc_base_render_application< TApp >::app_start_engine_components(game::bc_engine_component_parameter& p_engine_components)
	{
#ifdef BC_MEMORY_ENABLE
		core::bc_memmng::startup
		(
			p_engine_components.m_memmng_fsa_start_size,
			p_engine_components.m_memmng_fsa_count,
			p_engine_components.m_memmng_fsa_step_size,
			p_engine_components.m_memmng_fsa_allocation_count,
			p_engine_components.m_memmng_program_stack_size,
			p_engine_components.m_memmng_level_stack_size,
			p_engine_components.m_memmng_frame_stack_size,
			p_engine_components.m_memmng_super_heap_size
		);
#endif
		core::bc_service_manager::start_up();
		m_service_manager = &core::bc_service_manager::get();

		auto* l_logger_manager = m_service_manager->register_service< core::bc_logger >
		(
			core::bc_make_service< core::bc_logger >()
		);
		m_service_manager->register_service< core::bc_event_manager >
		(
			core::bc_make_service< core::bc_event_manager >()
		);
		m_service_manager->register_service< core::bc_thread_manager >
		(
			core::bc_make_service< core::bc_thread_manager >
			(
				p_engine_components.m_thread_manager_thread_count,
				p_engine_components.m_thread_manager_reserve_thread_count
			)
		);
		auto* l_expression_parameter_manager = m_service_manager->register_service< core::bc_expression_parameter_manager >
		(
			core::bc_make_service< core::bc_expression_parameter_manager >()
		);
		auto* l_content_manager = m_service_manager->register_service< core::bc_content_manager >
		(
			core::bc_make_service< core::bc_content_manager >()
		);
		auto* l_content_stream_manager = m_service_manager->register_service< core::bc_content_stream_manager >
		(
			core::bc_make_service< core::bc_content_stream_manager >(*l_content_manager)
		);
		m_service_manager->register_service< game::bc_actor_component_manager >
		(
			core::bc_make_service< game::bc_actor_component_manager >()
		);
		auto* l_entity_manager = m_service_manager->register_service< game::bc_entity_manager >
		(
			core::bc_make_service< game::bc_entity_manager >()
		);
		m_game_system = m_service_manager->register_service< game::bc_game_system >
		(
			core::bc_make_service< game::bc_game_system >()
		);

#ifdef BC_DEBUG
		l_logger_manager->register_listener
		(
			core::bc_enum::or({core::bc_log_type::debug, core::bc_log_type::error }),
			core::bc_make_unique< platform::bc_ide_logger >(core::bc_alloc_type::program)
		);
#endif

		l_expression_parameter_manager->register_resolver
		(
			core::g_param_rsv_mesh,
			core::bc_expr_param_func_pack_string([this](const core::bc_string& p_mesh_name)
				{
					return core::bc_parameter_pack(core::bc_lazy_content(p_mesh_name));
				})
		);
		l_expression_parameter_manager->register_resolver
		(
			core::g_param_rsv_heightmap,
			core::bc_expr_param_func_pack_string([this](const core::bc_string& p_heightmap_name)
				{
					return core::bc_parameter_pack(core::bc_lazy_content(p_heightmap_name));
				})
		);

		l_content_stream_manager->register_loader< graphic::bc_texture2d_content, bc_texture_loader >
		(
			core::bc_make_loader< bc_texture_loader >()
		);
		l_content_stream_manager->register_loader< graphic::bc_vertex_shader_content, bc_vertex_shader_loader >
		(
			core::bc_make_loader< bc_vertex_shader_loader >()
		);
		l_content_stream_manager->register_loader< graphic::bc_hull_shader_content, bc_hull_shader_loader >
		(
			core::bc_make_loader< bc_hull_shader_loader >()
		);
		l_content_stream_manager->register_loader< graphic::bc_domain_shader_content, bc_domain_shader_loader >
		(
			core::bc_make_loader< bc_domain_shader_loader >()
		);
		l_content_stream_manager->register_loader< graphic::bc_geometry_shader_content, bc_geometry_shader_loader >
		(
			core::bc_make_loader< bc_geometry_shader_loader >()
		);
		l_content_stream_manager->register_loader< graphic::bc_pixel_shader_content, bc_pixel_shader_loader >
		(
			core::bc_make_loader< bc_pixel_shader_loader >()
		);
		l_content_stream_manager->register_loader< graphic::bc_compute_shader_content, bc_compute_shader_loader >
		(
			core::bc_make_loader< bc_compute_shader_loader >()
		);
		l_content_stream_manager->register_loader< game::bc_mesh_collider, bc_mesh_collider_loader >
		(
			core::bc_make_loader< bc_mesh_collider_loader >(true)
		);
		l_content_stream_manager->register_loader< game::bc_mesh, bc_mesh_loader >
		(
			core::bc_make_loader< bc_mesh_loader >()
		);

		l_entity_manager->register_component_types
		<
			game::bc_name_component,
			game::bc_mesh_component,
			game::bc_hierarchy_component,
			game::bc_rigid_static_component,
			game::bc_rigid_dynamic_component,
			game::bc_render_component,
			game::bc_height_map_component
		>();
		l_entity_manager->register_abstract_component_types
		<
			game::bc_abstract_component<game::bc_rigid_body_component, game::bc_rigid_static_component, game::bc_rigid_dynamic_component>
		>();

		application_start_engine_components(p_engine_components, *m_service_manager);
	}

	template< class TApp >
	void bc_base_render_application< TApp >::app_initialize(const bcCHAR* p_commandline)
	{
		auto* l_content_stream_manager = m_service_manager->get_service< core::bc_content_stream_manager >();
		auto* l_entity_manager = m_service_manager->get_service< game::bc_entity_manager >();
		auto& l_script_system = m_game_system->get_script_system();
		auto l_script_binder = l_script_system.get_script_binder();

		m_game_system->initialize
		(
			game::bc_game_system_parameter(game::bc_render_system_parameter
			(
				bc_render_application::get_output_window().get_width(),
				bc_render_application::get_output_window().get_height(),
				graphic::bc_format::R8G8B8A8_UNORM,
				bc_render_application::get_output_window().get_device_output()
			))
		);

		l_content_stream_manager->read_stream_file(m_game_system->get_file_system().get_content_data_path(bcL("ContentStream.json")).c_str());
		l_entity_manager->read_entity_file(m_game_system->get_file_system().get_content_data_path(bcL("EntityType.json")).c_str());
		m_game_system->get_render_system().get_material_manager().read_material_file(m_game_system->get_file_system().get_content_data_path(bcL("Material.json")).c_str());

		l_content_stream_manager->load_content_stream(core::bc_alloc_type::program, "engine_shaders");
		l_content_stream_manager->load_content_stream(core::bc_alloc_type::program, "engine_resources");

		l_script_binder.bind<game::bc_game_console>(game::bc_script_context::ui, m_game_system->get_console());

		l_script_system.set_script_binder(std::move(l_script_binder));

		application_initialize(p_commandline);
	}

	template< class TApp >
	void bc_base_render_application< TApp >::app_load_content()
	{
		application_load_content(m_service_manager->get_service< core::bc_content_stream_manager >());
	}

	template< class TApp >
	void bc_base_render_application< TApp >::app_update(core_platform::bc_clock::update_param p_clock_update_param)
	{
		application_update(p_clock_update_param);
	}

	template< class TApp >
	void bc_base_render_application<TApp>::app_render(core_platform::bc_clock::update_param p_clock_update_param)
	{
		application_render(p_clock_update_param);
	}

	template< class TApp >
	bool bc_base_render_application< TApp >::app_event(core::bc_ievent& p_event)
	{
		bool l_result = false;

		l_result = l_result || bc_render_application::app_event(p_event);
		l_result = l_result || application_event(p_event);

		return l_result;
	}

	template< class TApp >
	void bc_base_render_application< TApp >::app_unload_content()
	{
		auto* l_content_stream_manager = m_service_manager->get_service< core::bc_content_stream_manager >();

		application_unload_content(l_content_stream_manager);

		l_content_stream_manager->unload_content_stream("engine_shaders");
		l_content_stream_manager->unload_content_stream("engine_resources");
	}

	template< class TApp >
	void bc_base_render_application< TApp >::app_destroy()
	{
		application_destroy();
	}

	template< class TApp >
	void bc_base_render_application< TApp >::app_close_engine_components()
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

	template< class TApp >
	void bc_base_render_application<TApp>::_initialize(game::bc_engine_application_parameter& p_parameter)
	{
		bc_render_application::_initialize(p_parameter);
	}

	template< class TApp >
	void bc_base_render_application<TApp>::_destroy()
	{
		bc_render_application::_destroy();
	}
}
