// [03/23/2016 MRB]

#pragma once

#include "Core/bcConstant.h"
#include "Core/Memory/bcMemoryManagment.h"
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
#include "Game/System/bcGameSystem.h"
#include "BlackCat/Loader/bcMeshLoader.h"
#include "BlackCat/Loader/bcTextureLoader.h"
#include "BlackCat/Loader/bcVertexShaderLoader.h"
#include "BlackCat/Loader/bcHullShaderLoader.h"
#include "BlackCat/Loader/bcDomainShaderLoader.h"
#include "BlackCat/Loader/bcGeometryShaderLoader.h"
#include "BlackCat/Loader/bcPixelShaderLoader.h"
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

		virtual void app_load_content() override;

		virtual void app_update(core_platform::bc_clock::update_param p_clock_update_param) override;

		virtual void app_handle_render_window_resize(bcUINT32 p_width, bcUINT32 p_height) override = 0;

		virtual bool app_event(core::bc_ievent& p_event) override;

		virtual void app_unload_content() override;

		virtual void app_destroy() override;

		virtual void app_close_engine_components() override;

	protected:
		core::bc_service_manager* m_service_manager;
		game::bc_game_system* m_game_system;

	private:
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
				p_engine_components.m_memmng_fsa_allocation_count,
				p_engine_components.m_memmng_program_stack_size,
				p_engine_components.m_memmng_level_stack_size,
				p_engine_components.m_memmng_frame_stack_size,
				p_engine_components.m_memmng_super_heap_size
			);
#endif
		core::bc_service_manager::start_up();
		m_service_manager = &core::bc_service_manager::get();
		
		m_service_manager->register_service< core::bc_thread_manager >
			(
				core::bc_make_service< core::bc_thread_manager >
				(
					p_engine_components.m_thread_manager_thread_count,
					p_engine_components.m_thread_manager_reserve_thread_count
				)
			);
		m_service_manager->register_service< core::bc_event_manager >
			(
				core::bc_make_service< core::bc_event_manager >()
			);
		auto* l_logger_manager = m_service_manager->register_service< core::bc_logger >
			(
				core::bc_make_service< core::bc_logger >()
			);
		m_game_system = m_service_manager->register_service< game::bc_game_system >
			(
				core::bc_make_service< game::bc_game_system >()
			);
		m_service_manager->register_service< core::bc_content_manager >
			(
				core::bc_make_service< core::bc_content_manager >()
			);
		auto* l_content_stream_manager = m_service_manager->register_service< core::bc_content_stream_manager >
			(
				core::bc_make_service< core::bc_content_stream_manager >(m_game_system->get_file_system().get_content_stream_path())
			);
		auto* l_expression_parameter_manager = m_service_manager->register_service< core::bc_expression_parameter_manager >
			(
				core::bc_make_service< core::bc_expression_parameter_manager >()
			);

		l_expression_parameter_manager->register_resolver
			(
				core::g_param_rsv_device,
				core::bc_expr_param_func_pack_void([this]()
					{
						return core::bc_parameter_pack(&this->m_game_system->get_render_system().get_device());
					})
			);
		l_expression_parameter_manager->register_resolver
			(
				core::g_param_rsv_mesh,
				core::bc_expr_param_func_pack_string([this](const core::bc_string& p_mesh_name)
					{
						return core::bc_parameter_pack(core::bc_lazy_content(p_mesh_name));
					})
			);

		m_service_manager->register_service< game::bc_actor_component_manager >
			(
				core::bc_make_service< game::bc_actor_component_manager >()
			);
		auto* l_entity_manager = m_service_manager->register_service< game::bc_entity_manager >
			(
				core::bc_make_service< game::bc_entity_manager >(m_game_system->get_file_system().get_entity_type_path())
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
		l_content_stream_manager->register_loader< game::bc_mesh, bc_mesh_loader >
			(
				core::bc_make_loader< bc_mesh_loader >()
			);

		l_entity_manager->register_component_types
			<
				game::bc_mesh_component,
				game::bc_hierarchy_component,
				game::bc_render_component
			>();

#ifdef BC_DEBUG
		l_logger_manager->register_listener
			(
				core::bc_log_type::debug,
				core::bc_make_unique< platform::bcLogIDEDebug >(core::bc_alloc_type::program)
			);
#endif
	}

	template< class TApp >
	void bc_base_render_application< TApp >::app_initialize(const bcCHAR* p_commandline)
	{
		auto* l_content_stream_manager = m_service_manager->get_service< core::bc_content_stream_manager >();

		m_game_system->get_render_system().initialize(game::bc_render_system_parameter
			(
				base_type::get_render_window().get_width(),
				base_type::get_render_window().get_height(),
				graphic::bc_format::R8G8B8A8_UNORM,
				base_type::get_render_window()
			));

		l_content_stream_manager->load_content_stream(core::bc_alloc_type::program, "engine_shaders");
	}

	template< class TApp >
	void bc_base_render_application< TApp >::app_load_content()
	{
	}

	template< class TApp >
	void bc_base_render_application< TApp >::app_update(core_platform::bc_clock::update_param p_clock_update_param)
	{
#ifdef BC_MEMORY_ENABLE
		core::bc_memmng::get().end_of_frame();
#endif
	}

	template< class TApp >
	bool bc_base_render_application< TApp >::app_event(core::bc_ievent& p_event)
	{
		return base_type::app_event(p_event);
	}

	template< class TApp >
	void bc_base_render_application< TApp >::app_unload_content()
	{
	}

	template< class TApp >
	void bc_base_render_application< TApp >::app_destroy()
	{
		//m_game_system->get_render_system().destroy();
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
			/*auto l_message = core::bc_to_estring_frame(l_leak_counts) + bcL(" Memory leak detected");
			base_type::get_render_window().messagebox
				(
					L"Warning",
					l_message,
					platform::bc_messagebox_type::warning,
					platform::bc_messagebox_buttom::ok
				);
			m_service_manager->get_service< core::bc_logger >()->log
				(
					core::bc_log_type::info,
					l_message
				);*/
		}
#endif

		core::bc_memmng::close();
#endif
	}
}
