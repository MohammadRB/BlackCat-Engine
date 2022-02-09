// [11/22/2018 MRB]

#include "App/AppPCH.h"

#include "Core/Messaging/Event/bcEventManager.h"
#include "Core/Messaging/Query/bcQueryManager.h"
#include "Core/Content/bcContentManager.h"
#include "Core/Content/bcContentStreamManager.h"
#include "Core/Utility/bcServiceManager.h"
#include "Core/Utility/bcCounterValueManager.h"
#include "Core/Utility/bcLogger.h"
#include "Core/Math/bcCoordinate.h"
#include "Core/bcEvent.h"
#include "Game/System/Script/bcScriptSystem.h"
#include "GraphicImp/Device/bcDevice.h"
#include "GraphicImp/Device/bcDevicePipeline.h"
#include "App/Application/bcRenderApplication.h"
#include "App/Application/bcApplicationHookFuncations.h"
#include "Platform/bcEvent.h"

namespace black_cat
{
	bc_render_application::bc_render_application()
		: game::bc_render_application(),
		m_game_system(nullptr)
	{
	}

	bc_render_application::~bc_render_application() = default;
	
	void bc_render_application::application_pause_idle(const core_platform::bc_clock::update_param& p_clock)
	{
	}

	void bc_render_application::application_render_pause_idle(const core_platform::bc_clock::update_param& p_clock)
	{
	}

	void bc_render_application::application_swap_frame_idle(const core_platform::bc_clock::update_param& p_clock)
	{
	}

	void bc_render_application::application_swap_frame(const core_platform::bc_clock::update_param& p_clock)
	{
	}

	void bc_render_application::application_render_swap_frame(const core_platform::bc_clock::update_param& p_clock)
	{
	}

	void bc_render_application::app_start_engine_components(game::bc_engine_application_parameter& p_parameters)
	{
		bc_start_engine_services(p_parameters);
		bc_register_engine_loaders(p_parameters);

		core::bc_log(core::bc_log_type::info) << "starting engine components" << core::bc_lend;

		m_game_system = core::bc_get_service<game::bc_game_system>();
		m_game_system->initialize
		(
			game::bc_game_system_parameter
			(
				*core::bc_get_service<core::bc_query_manager>(),
				*core::bc_get_service<core::bc_event_manager>(),
				game::bc_render_application::get_output_window()
				? game::bc_render_system_parameter
				(
					*core::bc_get_service<core::bc_content_stream_manager>(),
					m_game_system->get_physics_system(),
					game::bc_render_application::get_output_window()->get_width(),
					game::bc_render_application::get_output_window()->get_height(),
					graphic::bc_format::R8G8B8A8_UNORM,
					game::bc_render_application::get_output_window()->get_device_output()
				)
				: game::bc_render_system_parameter
				(
					*core::bc_get_service<core::bc_content_stream_manager>(),
					m_game_system->get_physics_system()
				)
			)
		);

		bc_register_engine_actor_components();
		bc_register_engine_network_messages(m_game_system->get_network_system());
		bc_bind_engine_scripts(*m_game_system);
		bc_register_engine_particle_emitters(*m_game_system);
		bc_load_engine_shaders(*core::bc_get_service<core::bc_content_stream_manager>(), *m_game_system);

		application_start_engine_components(p_parameters);
	}

	void bc_render_application::app_initialize(game::bc_engine_application_parameter& p_parameters)
	{
		core::bc_log(core::bc_log_type::info) << "initializing engine" << core::bc_lend;

		application_initialize(p_parameters);
	}

	void bc_render_application::app_load_content()
	{
		core::bc_log(core::bc_log_type::info) << "loading engine resources" << core::bc_lend;

		auto* l_content_stream_manager = core::bc_get_service<core::bc_content_stream_manager>();
		
		bc_load_engine_resources(*l_content_stream_manager, *m_game_system);
		application_load_content(*l_content_stream_manager);
	}

	void bc_render_application::app_update(const core_platform::bc_clock::update_param& p_clock, bool p_is_partial_update)
	{
		m_update_watch.start();
		
		auto* l_event_manager = core::bc_get_service<core::bc_event_manager>();

		if(!p_is_partial_update)
		{
			core::bc_event_frame_update_start l_event_frame_start;
			l_event_manager->process_event(l_event_frame_start);
		}
		
		m_game_system->update_game(p_clock, p_is_partial_update);

		if (!p_is_partial_update)
		{
			core::bc_service_manager::get()->update(p_clock);
		}
		
		application_update(p_clock, p_is_partial_update);

		if(!p_is_partial_update)
		{
			core::bc_event_frame_update_finish l_event_frame_finish;
			l_event_manager->process_event(l_event_frame_finish);
		}

		m_update_watch.stop();
	}

	void bc_render_application::app_render(const core_platform::bc_clock::update_param& p_clock)
	{
		m_render_watch.start();
		
		auto* l_event_manager = core::bc_get_service<core::bc_event_manager>();

		core::bc_event_frame_render_start l_event_frame_start;
		l_event_manager->process_event(l_event_frame_start);
		
		m_game_system->render_game(p_clock);
		application_render(p_clock);

		core::bc_event_frame_render_finish l_event_frame_finish;
		l_event_manager->process_event(l_event_frame_finish);

		m_render_watch.stop();
	}

	void bc_render_application::app_pause_idle(const core_platform::bc_clock::update_param& p_clock)
	{
		platform::bc_app_event_pause_state l_pause_event(platform::bc_app_event_pause_state::state::paused);
		core::bc_get_service<core::bc_event_manager>()->process_event(l_pause_event);
		core::bc_get_service<core::bc_event_manager>()->process_event_queue(p_clock);

		application_pause_idle(p_clock);
	}

	void bc_render_application::app_render_pause_idle(const core_platform::bc_clock::update_param& p_clock)
	{
		core::bc_get_service<core::bc_event_manager>()->process_render_event_queue(p_clock);
		application_render_pause_idle(p_clock);
	}

	void bc_render_application::app_swap_frame_idle(const core_platform::bc_clock::update_param& p_clock)
	{
		m_game_system->swap_frame_idle(p_clock);
		application_swap_frame_idle(p_clock);
	}

	void bc_render_application::app_swap_frame(const core_platform::bc_clock::update_param& p_clock)
	{
		auto* l_event_manager = core::bc_get_service<core::bc_event_manager>();
		auto* l_counter_value_manager = core::bc_get_service<core::bc_counter_value_manager>();

		m_update_watch.restart();
		m_render_watch.restart();

		l_counter_value_manager->add_counter("fps", get_fps());
		l_counter_value_manager->add_counter("frame_time", get_ft());
		l_counter_value_manager->add_counter("update_time", m_update_watch.average_total_elapsed());
		l_counter_value_manager->add_counter("render_time", m_render_watch.average_total_elapsed());

		m_swap_watch.start();
		
		m_game_system->swap_frame(p_clock);
		application_swap_frame(p_clock);
		
		core::bc_event_frame_swap l_event_frame_swap(get_fps());
		l_event_manager->process_event(l_event_frame_swap);

		m_swap_watch.stop();
		m_swap_watch.restart();

		l_counter_value_manager->add_counter("swap_time", m_swap_watch.average_total_elapsed());
	}

	void bc_render_application::app_render_swap_frame(const core_platform::bc_clock::update_param& p_clock)
	{
		m_game_system->render_swap_frame(p_clock);
	}

	bool bc_render_application::app_event(core::bci_event& p_event)
	{
		return application_event(p_event);
	}

	void bc_render_application::app_unload_content()
	{
		core::bc_log(core::bc_log_type::info) << "unloading engine resources" << core::bc_lend;
		
		auto* l_content_stream_manager = core::bc_get_service<core::bc_content_stream_manager>();
		
		application_unload_content(*l_content_stream_manager);
		bc_unload_engine_resources(*l_content_stream_manager);
	}

	void bc_render_application::app_destroy()
	{
		core::bc_log(core::bc_log_type::info) << "destroying engine" << core::bc_lend;
		
		application_destroy();
		m_game_system->destroy();
	}

	void bc_render_application::app_close_engine_components()
	{
		const auto& l_memory_manager = core::bc_memory_manager::get();

		core::bc_log(core::bc_log_type::info) << "closing engine components" << core::bc_lend;
		core::bc_log(core::bc_log_type::info)
			<< "memory report: "
			<< " total size: " << l_memory_manager.get_total_size()
			<< " max used size: " << l_memory_manager.get_max_used_size()
			<< core::bc_only_file
			<< core::bc_lend;

		application_close_engine_components();
		bc_close_engine_services();
	}
}