// [03/23/2016 MRB]

#pragma once

#include "Core/bcConstant.h"
#include "Core/Memory/bcMemoryManagment.h"
#include "Core/bcEvent.h"
#include "Core/Utility/bcServiceManager.h"
#include "Core/Utility/bcLogger.h"
#include "Core/Utility/bcExpressionParameterManager.h"
#include "Core/Concurrency/bcThreadManager.h"
#include "Core/Event/bcEventManager.h"
#include "Core/File/bcContentManager.h"
#include "Core/File/bcContentStreamManager.h"
#include "Core/File/bcLazyContent.h"
#include "PlatformImp/bcLogIDEDebug.h"
#include "PlatformImp/Application/bcRenderWindow.h"
#include "PlatformImp/Application/bcRenderApplication.h"
#include "GraphicImp/Device/bcDevice.h"
#include "GraphicImp/Device/bcDevicePipeline.h"
#include "GraphicImp/Device/Command/bcDeviceCommandExecuter.h"
#include "Game/System/Render/bcRenderPassManager.h"
#include "Game/Object/Scence/bcActorComponentManager.h"
#include "Game/Object/Scence/bcEntityManager.h"
#include "Game/Object/Scence/Component/bcRenderComponent.h"
#include "Game/Object/Scence/Component/bcMeshComponent.h"
#include "Game/Object/Scence/Component/bcHierarchyComponent.h"
#include "Game/Object/Scence/Component/bcHeightMapComponent.h"
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

namespace black_cat
{
	template< class TApp >
	class bc_base_render_application : public platform::bc_render_application< TApp >
	{
	private:
		using base_type = platform::bc_render_application< TApp >;

	public:
		bc_base_render_application();

		~bc_base_render_application();

		virtual void app_start_engine_components(platform::bc_engine_components_parameters& p_engine_components) override;

		virtual void app_initialize(const bcCHAR* p_commandline) override;

		virtual void app_load_content(core::bc_content_stream_manager* p_stream_manager) override;

		virtual void app_update(core_platform::bc_clock::update_param p_clock_update_param) override;

		virtual bool app_event(core::bc_ievent& p_event) override;

		virtual void app_unload_content(core::bc_content_stream_manager* p_stream_manager) override;

		virtual void app_destroy() override;

		virtual void app_close_engine_components() override;

	protected:
		core::bc_service_manager* m_service_manager;
		game::bc_game_system* m_game_system;

	private:
		core::bc_event_listener_handle m_event_error_handle;
		core::bc_event_listener_handle m_event_warning_handle;
	};

	template< class TApp >
	bc_base_render_application< TApp >::bc_base_render_application()
		: base_type(), 
		m_service_manager(nullptr),
		m_game_system(nullptr)
	{
	}

	template< class TApp >
	bc_base_render_application< TApp >::~bc_base_render_application()
	{
	}

	template< class TApp >
	void bc_base_render_application< TApp >::app_start_engine_components(platform::bc_engine_components_parameters& p_engine_components)
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
		m_service_manager->register_service< core::bc_content_manager >
			(
				core::bc_make_service< core::bc_content_manager >()
			);
		auto* l_content_stream_manager = m_service_manager->register_service< core::bc_content_stream_manager >
			(
				core::bc_make_service< core::bc_content_stream_manager >()
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
			core::bc_log_type::debug,
			core::bc_make_unique< platform::bcLogIDEDebug >(core::bc_alloc_type::program)
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
		l_content_stream_manager->register_loader< game::bc_mesh, bc_mesh_loader >
			(
				core::bc_make_loader< bc_mesh_loader >()
			);

		l_entity_manager->register_component_types
			<
				game::bc_mesh_component,
				game::bc_hierarchy_component,
				game::bc_render_component,
				game::bc_height_map_component
			>();
	}

	template< class TApp >
	void bc_base_render_application< TApp >::app_initialize(const bcCHAR* p_commandline)
	{
		auto* l_event_manager = core::bc_service_manager::get().get_service<core::bc_event_manager>();
		auto* l_content_stream_manager = m_service_manager->get_service< core::bc_content_stream_manager >();
		auto* l_entity_manager = m_service_manager->get_service< game::bc_entity_manager >();

		m_event_error_handle = l_event_manager->register_event_listener< core::bc_app_event_error >
		(
			core::bc_event_manager::delegate_type(this, &bc_base_render_application::app_event)
		);
		m_event_warning_handle = l_event_manager->register_event_listener< core::bc_app_event_warning >
		(
			core::bc_event_manager::delegate_type(this, &bc_base_render_application::app_event)
		);

		m_game_system->initialize
		(
			game::bc_game_system_parameter(game::bc_render_system_parameter
			(
				base_type::get_render_window().get_width(),
				base_type::get_render_window().get_height(),
				graphic::bc_format::R8G8B8A8_UNORM,
				base_type::get_render_window()
			))
		);

		l_content_stream_manager->read_stream_file(m_game_system->get_file_system().get_content_stream_path());
		l_entity_manager->read_entity_file(m_game_system->get_file_system().get_entity_type_path());

		l_content_stream_manager->load_content_stream(core::bc_alloc_type::program, "engine_shaders");
		l_content_stream_manager->load_content_stream(core::bc_alloc_type::program, "engine_resources");
	}

	template< class TApp >
	void bc_base_render_application< TApp >::app_load_content(core::bc_content_stream_manager* p_stream_manager)
	{
	}

	template< class TApp >
	void bc_base_render_application< TApp >::app_update(core_platform::bc_clock::update_param p_clock_update_param)
	{
	}

	template< class TApp >
	bool bc_base_render_application< TApp >::app_event(core::bc_ievent& p_event)
	{
		if (core::bc_ievent::event_is< core::bc_app_event_error >(p_event))
		{
			base_type::get_render_window().messagebox
			(
				bcL("Error"),
				bc_to_wstring(static_cast< core::bc_app_event_error& >(p_event).get_message()).c_str(),
				platform::bc_messagebox_type::error,
				platform::bc_messagebox_buttom::ok
			);
			return true;
		}

		if (core::bc_ievent::event_is< core::bc_app_event_warning >(p_event))
		{
			base_type::get_render_window().messagebox
			(
				bcL("Warning"),
				bc_to_wstring(static_cast< core::bc_app_event_warning& >(p_event).get_message()).c_str(),
				platform::bc_messagebox_type::warning,
				platform::bc_messagebox_buttom::ok
			);
			return true;
		}

		return base_type::app_event(p_event);
	}

	template< class TApp >
	void bc_base_render_application< TApp >::app_unload_content(core::bc_content_stream_manager* p_stream_manager)
	{
		auto* l_content_stream_manager = m_service_manager->get_service< core::bc_content_stream_manager >();

		l_content_stream_manager->unload_content_stream("engine_shaders");
		l_content_stream_manager->unload_content_stream("engine_resources");
	}

	template< class TApp >
	void bc_base_render_application< TApp >::app_destroy()
	{
		m_event_warning_handle.reset();
		m_event_error_handle.reset();
	}

	template< class TApp >
	void bc_base_render_application< TApp >::app_close_engine_components()
	{
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
