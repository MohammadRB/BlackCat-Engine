// [11/22/2018 MRB]

#include "BlackCat/BlackCatPCH.h"

#include "Core/Messaging/Event/bcEventManager.h"
#include "Core/Messaging/Query/bcQueryManager.h"
#include "Core/Content/bcContentManager.h"
#include "Core/Content/bcContentStreamManager.h"
#include "Core/Utility/bcServiceManager.h"
#include "Core/Utility/bcCounterValueManager.h"
#include "Core/bcEvent.h"
#include "Game/System/Script/bcScriptSystem.h"
#include "GraphicImp/Device/bcDevice.h"
#include "GraphicImp/Device/bcDevicePipeline.h"
#include "BlackCat/Application/bcRenderApplication.h"
#include "BlackCat/Application/bcApplicationHookFuncations.h"

namespace black_cat
{
	bc_render_application::bc_render_application()
		: game::bc_render_application(),
		m_game_system(nullptr),
		m_fps_sampler(0),
		m_fps(0)
	{
	}

	bc_render_application::~bc_render_application() = default;

	void bc_render_application::app_start_engine_components(game::bc_engine_application_parameter& p_parameters)
	{
		bc_start_engine_services(p_parameters);
		bc_register_engine_loaders(p_parameters);
		bc_register_engine_actor_components();
		
		m_game_system = core::bc_get_service<game::bc_game_system>();
		
		application_start_engine_components(p_parameters);
	}

	void bc_render_application::app_initialize(game::bc_engine_application_parameter& p_parameters)
	{
		m_game_system->initialize
		(
			game::bc_game_system_parameter
			(
				*core::bc_get_service<core::bc_query_manager>(),
				*core::bc_get_service<core::bc_event_manager>(),
				game::bc_render_system_parameter
				(
					game::bc_render_application::get_output_window().get_width(),
					game::bc_render_application::get_output_window().get_height(),
					graphic::bc_format::R8G8B8A8_UNORM,
					game::bc_render_application::get_output_window().get_device_output()
				)
			)
		);

		bc_bind_scripts(*m_game_system);
		bc_load_engine_resources(*m_game_system);
		bc_register_particle_emitters(*m_game_system);

		application_initialize(p_parameters);
	}

	void bc_render_application::app_load_content()
	{
		application_load_content(core::bc_get_service<core::bc_content_stream_manager>());
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
			core::bc_service_manager::get().update(p_clock);
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
		core::bc_get_service<core::bc_event_manager>()->process_event_queue(p_clock);
	}

	void bc_render_application::app_render_pause_idle(const core_platform::bc_clock::update_param& p_clock)
	{
		core::bc_get_service<core::bc_event_manager>()->process_render_event_queue(p_clock);
	}

	void bc_render_application::app_swap_frame_idle(const core_platform::bc_clock::update_param& p_clock)
	{
		m_update_watch.start();
		
		m_game_system->swap_frame_idle(p_clock);

		m_update_watch.stop();
	}

	void bc_render_application::app_swap_frame(const core_platform::bc_clock::update_param& p_clock)
	{
		auto* l_event_manager = core::bc_get_service<core::bc_event_manager>();
		auto* l_counter_value_manager = core::bc_get_service<core::bc_counter_value_manager>();

		m_update_watch.restart();
		m_render_watch.restart();
		m_fps_sampler.add_sample(p_clock.m_elapsed);
		m_fps = std::round(1000.0f / m_fps_sampler.average_value());

		l_counter_value_manager->add_counter("fps", core::bc_to_wstring(m_fps));
		l_counter_value_manager->add_counter("update_time", m_update_watch.average_total_elapsed());
		l_counter_value_manager->add_counter("render_time", m_render_watch.average_total_elapsed());

		m_swap_watch.start();
		
		m_game_system->swap_frame(p_clock);
		
		core::bc_event_frame_swap l_event_frame_swap;
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
		auto* l_content_stream_manager = core::bc_get_service< core::bc_content_stream_manager >();
		
		application_unload_content(l_content_stream_manager);
		bc_unload_engine_resources(*l_content_stream_manager);
	}

	void bc_render_application::app_destroy()
	{
		application_destroy();
	}

	void bc_render_application::app_close_engine_components()
	{
		application_close_engine_components();
		bc_close_engine_services();
	}
}