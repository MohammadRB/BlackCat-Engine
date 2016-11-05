// [2/23/2015 MRB]

#include "Game/GamePCH.h"
#include "Game/bcRenderApplication.h"

namespace black_cat
{
	namespace game
	{
		bc_render_application::bc_render_application()
			: m_render_window(nullptr),
			m_clock(nullptr),
			m_is_terminated(false),
			m_paused(false),
			m_termination_code(0),
			m_current_time_delta_sample(0),
			m_fps(0),
			m_fixed_fps(-1)
		{
		}

		platform::bc_basic_window& bc_render_application::get_render_window() const
		{
			return *m_render_window;
		}

		core_platform::bc_clock& bc_render_application::get_clock() const
		{
			return *m_clock;
		}

		bcUINT32 bc_render_application::get_fps() const
		{
			return m_fps;
		}

		void bc_render_application::set_fps(bcUINT32 p_fps)
		{
			if (p_fps > 0)
				m_fixed_fps = p_fps;
			else
				m_fixed_fps = -1;
		}

		bcINT32 bc_render_application::run()
		{
			try
			{
				while (!m_is_terminated)
				{
					m_render_window->update();
					m_app->update();

					if (m_paused)
					{
						core_platform::bc_thread::current_thread_sleep_for(100000000);
						continue;
					}

					m_clock->update();

					core_platform::bc_clock::small_delta_time l_elaped = m_clock->get_elapsed();
					core_platform::bc_clock::big_delta_time l_total_elapsed = m_clock->get_total_elapsed();

					// Fixed FPS
					core_platform::bc_clock::small_delta_time l_fixed_elapsed = 1000.0f / m_fixed_fps;
					if (m_fixed_fps != -1 && l_elaped < l_fixed_elapsed)
					{
						auto l_diff = l_fixed_elapsed - l_elaped;
						core_platform::bc_thread::current_thread_sleep_for(l_diff * 1000000);

						l_elaped = l_fixed_elapsed;
					}
#ifdef BC_DEBUG
					if (l_elaped > 1000.0f)
					{
						l_elaped = m_fixed_fps != -1 ? l_fixed_elapsed : 1000.0f / 32;
					}
#endif

					_calculate_fps(l_elaped);

					app_update(core_platform::bc_clock::update_param(l_total_elapsed, l_elaped));

#ifdef BC_MEMORY_ENABLE
					core::bc_memmng::get().end_of_frame();
#endif
				}
			}
			catch (std::exception& l_exception)
			{
				core::bc_event_manager* l_event_manager = core::bc_service_manager::get().get_service< core::bc_event_manager >();

				core::bc_app_event_error m_event(l_exception.what());
				l_event_manager->process_event(m_event);

				m_termination_code = -1;
			}

			return m_termination_code;
		}

		void bc_render_application::request_termination() const
		{
			m_app->request_termination();
		}

		platform::bc_basic_window bc_render_application::create_render_window(core::bc_estring p_caption, bcUINT32 p_width, bcUINT32 p_height)
		{
			return m_app->create_basic_window(std::move(p_caption), p_width, p_height);
		}

		platform::bc_console_window bc_render_application::create_console_window(core::bc_estring p_caption)
		{
			return m_app->create_console_window(std::move(p_caption));
		}

		bool bc_render_application::app_event(core::bc_ievent& p_event)
		{
			if (core::bc_ievent::event_is< platform::bc_app_event_active >(p_event))
			{
				platform::bc_app_event_active& l_active_event = static_cast< platform::bc_app_event_active& >(p_event);
				m_paused = !l_active_event.activate();

				if (m_paused)
				{
					m_clock->pause();
				}
				else
				{
					m_clock->resume();
				}

				return true;
			}

			if (core::bc_ievent::event_is< platform::bc_app_event_exit >(p_event))
			{
				platform::bc_app_event_exit& l_exit_event = static_cast< platform::bc_app_event_exit& >(p_event);

				m_is_terminated = true;
				m_termination_code = l_exit_event.exit_code();

				return true;
			}

			return false;
		}

		void bc_render_application::_initialize(bc_engine_application_parameter& p_parameters)
		{
			app_start_engine_components(p_parameters.m_engine_parameters);

			auto& l_service_manager = core::bc_service_manager::get();
			auto* l_event_manager = l_service_manager.get_service< core::bc_event_manager >();

			m_app = core::bc_make_unique< platform::bc_application >
			(
				core::bc_alloc_type::program,
				p_parameters.m_app_parameters
			);
			m_render_window = core::bc_make_unique< platform::bc_basic_window >
			(
				core::bc_alloc_type::program,
				create_render_window
				(
					core::bc_estring(p_parameters.m_app_parameters.m_app_name),
					p_parameters.m_app_parameters.m_window_width,
					p_parameters.m_app_parameters.m_window_height
				)
			);
			m_clock = core::bc_make_unique< core_platform::bc_clock >(core::bc_alloc_type::program);

			m_is_terminated = false;
			m_paused = false;
			m_termination_code = 0;

			memset(&m_time_delta_buffer, 0, sizeof(core_platform::bc_clock::small_delta_time) * s_num_time_delta_samples);
			m_current_time_delta_sample = 0;
			m_fps = 0;
			m_fixed_fps = -1;

			m_event_handle_app_exit = l_event_manager->register_event_listener< platform::bc_app_event_exit >
			(
				core::bc_event_manager::delegate_type(this, &bc_render_application::app_event)
			);
			m_event_handle_app_active = l_event_manager->register_event_listener< platform::bc_app_event_active >
			(
				core::bc_event_manager::delegate_type(this, &bc_render_application::app_event)
			);

			app_initialize(p_parameters.m_app_parameters.m_commandline);
			app_load_content();
		}

		void bc_render_application::_destroy()
		{
			app_unload_content();
			app_destroy();

			m_event_handle_app_exit.reset();
			m_event_handle_app_active.reset();

			m_clock.reset(nullptr);
			m_render_window->close();
			m_render_window.reset(nullptr);
			m_app.reset(nullptr);

			app_close_engine_components();

			core_platform::bc_thread::on_main_thread_exit();
		}

		void bc_render_application::_calculate_fps(core_platform::bc_clock::small_delta_time p_delta)
		{
			m_time_delta_buffer[m_current_time_delta_sample] = p_delta;
			m_current_time_delta_sample = (m_current_time_delta_sample + 1) % s_num_time_delta_samples;

			core_platform::bc_clock::small_delta_time l_average_delta = 0;
			for (UINT i = 0; i < s_num_time_delta_samples; ++i)
			{
				l_average_delta += m_time_delta_buffer[i];
			}
			l_average_delta /= s_num_time_delta_samples;

			m_fps = round(1000.0f / l_average_delta);
		}
	}
}