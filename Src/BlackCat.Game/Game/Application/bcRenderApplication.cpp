// [2/23/2015 MRB]

#include "Game/GamePCH.h"

#include "CorePlatformImp/Concurrency/bcThread.h"
#include "Core/Utility/bcLogger.h"
#include "Core/bcEvent.h"
#include "Platform/bcEvent.h"
#include "Game/Application/bcRenderApplication.h"

namespace black_cat
{
	namespace game
	{
		bc_render_application::bc_render_application()
			: m_app(nullptr),
			m_default_output_window(nullptr),
			m_output_window(nullptr),
			m_clock(nullptr),
			m_is_terminated(false),
			m_paused(false),
			m_termination_code(0),
			m_min_update_rate(0),
			m_render_rate(0),
			m_time_delta_buffer{},
			m_current_time_delta_sample(0),
			m_fps(0)
		{
		}

		bc_irender_application_output_window& bc_render_application::get_output_window() const
		{
			return *m_output_window;
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
			{
				m_render_rate = p_fps;
			}
			else
			{
				m_render_rate = -1;
			}
		}

		bcINT32 bc_render_application::run()
		{
			auto* const l_event_manager = core::bc_get_service< core::bc_event_manager >();
			const core_platform::bc_clock::small_delta_time l_min_update_elapsed = 1000.0f / m_min_update_rate;
			core_platform::bc_clock::small_delta_time l_local_elapsed = 0;
			
			try
			{
				while (!m_is_terminated)
				{
					m_output_window->update();
					m_app->update();

					if (m_paused)
					{
						core_platform::bc_thread::current_thread_sleep_for(std::chrono::milliseconds(100));
						continue;
					}

					m_clock->update();

					auto l_elapsed = m_clock->get_elapsed();
					auto l_total_elapsed = m_clock->get_total_elapsed();

#ifdef BC_DEBUG
					if (l_elapsed > 1000.0f)
					{
						l_elapsed = l_min_update_elapsed;
					}
#endif

					l_local_elapsed += l_elapsed;
					while (l_local_elapsed >= l_min_update_elapsed)
					{
						core::bc_event_frame_update_start l_event_frame_start;
						l_event_manager->process_event(l_event_frame_start);

						app_update(core_platform::bc_clock::update_param(l_total_elapsed, l_min_update_elapsed));
						l_local_elapsed -= l_min_update_elapsed;

						core::bc_event_frame_update_finish l_event_frame_finish;
						l_event_manager->process_event(l_event_frame_finish);
					}

					core::bc_event_frame_render_start l_event_frame_start;
					l_event_manager->process_event(l_event_frame_start);

					app_render(core_platform::bc_clock::update_param(l_total_elapsed, l_elapsed));

					core::bc_event_frame_render_finish l_event_frame_finish;
					l_event_manager->process_event(l_event_frame_finish);

					if (m_render_rate != -1) // Fixed render rate
					{
						const core_platform::bc_clock::small_delta_time l_render_elapsing = 1000.0f / m_render_rate;

						if(l_elapsed < l_render_elapsing)
						{
							const auto l_diff = static_cast<std::chrono::milliseconds::rep>(l_render_elapsing - l_elapsed);
							core_platform::bc_thread::current_thread_sleep_for(std::chrono::milliseconds(l_diff));

							l_elapsed = l_render_elapsing;
						}
					}

					_calculate_fps(l_elapsed);

#ifdef BC_MEMORY_ENABLE
					core::bc_memmng::get().end_of_frame();
#endif
				}
			}
			catch (std::exception& l_exception)
			{
				core::bc_app_event_error m_event(l_exception.what());
				l_event_manager->process_event(m_event);

				m_termination_code = -1;

#ifdef BC_MEMORY_ENABLE
				core::bc_memmng::get().end_of_frame();
#endif
			}

			return m_termination_code;
		}

		void bc_render_application::request_termination() const
		{
			m_app->request_termination();
		}

		platform::bc_basic_window bc_render_application::create_basic_render_window(core::bc_estring p_caption, bcUINT32 p_width, bcUINT32 p_height)
		{
			return m_app->create_basic_window(std::move(p_caption), p_width, p_height);
		}

		platform::bc_console_window bc_render_application::create_console_window(core::bc_estring p_caption)
		{
			return m_app->create_console_window(std::move(p_caption));
		}

		void bc_render_application::_initialize(bc_engine_application_parameter& p_parameters)
		{
			app_start_engine_components(p_parameters);
			m_app = core::bc_make_unique< platform::bc_application >(core::bc_alloc_type::program, p_parameters.m_app_parameters);

			if (p_parameters.m_app_parameters.m_output_window == nullptr)
			{
				m_default_output_window = core::bc_make_unique< bc_render_application_basic_output_window >(create_basic_render_window
				(
					core::bc_to_estring(p_parameters.m_app_parameters.m_app_name),
					800,
					450
				));
				p_parameters.m_app_parameters.m_output_window = m_default_output_window.get();
			}

			m_output_window = p_parameters.m_app_parameters.m_output_window;
			m_clock = core::bc_make_unique< core_platform::bc_clock >(core::bc_alloc_type::program);

			m_is_terminated = false;
			m_paused = false;
			m_termination_code = 0;

			m_min_update_rate = 60;
			m_render_rate = m_min_update_rate;

			memset(&m_time_delta_buffer, 0, sizeof(core_platform::bc_clock::small_delta_time) * s_num_time_delta_samples);
			m_current_time_delta_sample = 0;
			m_fps = 0;

			auto* l_event_manager = core::bc_get_service< core::bc_event_manager >();
			m_event_handle_window_resizing = l_event_manager->register_event_listener< platform::bc_app_event_window_resizing >
			(
				core::bc_event_manager::delegate_type(this, &bc_render_application::_app_event)
			);
			m_event_handle_window_close = l_event_manager->register_event_listener< platform::bc_app_event_window_close >
			(
				core::bc_event_manager::delegate_type(this, &bc_render_application::_app_event)
			);
			m_event_handle_app_active = l_event_manager->register_event_listener< platform::bc_app_event_active >
			(
				core::bc_event_manager::delegate_type(this, &bc_render_application::_app_event)
			);
			m_event_handle_app_exit = l_event_manager->register_event_listener< platform::bc_app_event_exit >
			(
				core::bc_event_manager::delegate_type(this, &bc_render_application::_app_event)
			);
			m_event_handle_error = l_event_manager->register_event_listener< core::bc_app_event_error >
			(
				core::bc_event_manager::delegate_type(this, &bc_render_application::_app_event)
			);
			m_event_handle_debug = l_event_manager->register_event_listener< core::bc_app_event_debug >
			(
				core::bc_event_manager::delegate_type(this, &bc_render_application::_app_event)
			);
			m_event_handle_key = l_event_manager->register_event_listener< platform::bc_app_event_key >
			(
				core::bc_event_manager::delegate_type(this, &bc_render_application::_app_event)
			);
			m_event_handle_pointing = l_event_manager->register_event_listener<platform::bc_app_event_pointing>
			(
				core::bc_event_manager::delegate_type(this, &bc_render_application::_app_event)
			);

			app_initialize(p_parameters);
			app_load_content();
		}

		void bc_render_application::_destroy()
		{
			app_unload_content();
			app_destroy();

			m_event_handle_window_resizing.reset();
			m_event_handle_window_close.reset();
			m_event_handle_app_active.reset();
			m_event_handle_app_exit.reset();
			m_event_handle_debug.reset();
			m_event_handle_error.reset();
			m_event_handle_key.reset();
			m_event_handle_pointing.reset();

			m_clock.reset(nullptr);
			m_output_window->close();
			m_default_output_window.reset(nullptr);
			m_app.reset(nullptr);

			app_close_engine_components();

			core_platform::bc_thread::on_main_thread_exit();
		}

		bool bc_render_application::_app_event(core::bc_ievent& p_event)
		{
			if (core::bc_ievent::event_is< platform::bc_app_event_window_resizing >(p_event))
			{
				auto& l_resizing_event = static_cast<platform::bc_app_event_window_resizing&>(p_event);

				if (l_resizing_event.get_window_id() == m_output_window->get_id())
				{
					if (l_resizing_event.start_resizing())
					{
						platform::bc_app_event_active l_active_event(false);
						core::bc_get_service< core::bc_event_manager >()->process_event(l_active_event);
					}
					else
					{
						platform::bc_app_event_active l_active_event(true);
						core::bc_get_service< core::bc_event_manager >()->process_event(l_active_event);
					}
				}

				return true;
			}

			if (core::bc_ievent::event_is< platform::bc_app_event_active >(p_event))
			{
				platform::bc_app_event_active& l_active_event = static_cast<platform::bc_app_event_active&>(p_event);
				m_paused = !l_active_event.active();

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

			if (core::bc_ievent::event_is< platform::bc_app_event_window_close >(p_event))
			{
				platform::bc_app_event_window_close& l_close_event = static_cast<platform::bc_app_event_window_close&>(p_event);

				if (l_close_event.get_window_id() == m_output_window->get_id())
				{
					platform::bc_app_event_exit l_exit_event(0);
					core::bc_get_service< core::bc_event_manager >()->process_event(l_exit_event);
				}

				return true;
			}

			if (core::bc_ievent::event_is< platform::bc_app_event_exit >(p_event))
			{
				platform::bc_app_event_exit& l_exit_event = static_cast<platform::bc_app_event_exit&>(p_event);

				m_is_terminated = true;
				m_termination_code = l_exit_event.exit_code();

				return true;
			}

			if (core::bc_ievent::event_is< core::bc_app_event_error >(p_event))
			{
				core::bc_get_service< core::bc_logger >()->log
				(
					core::bc_log_type::error,
					core::bc_to_estring_frame(static_cast<core::bc_app_event_error&>(p_event).get_message()).c_str()
				);

				return true;
			}

			if (core::bc_ievent::event_is< core::bc_app_event_debug >(p_event))
			{
				core::bc_get_service< core::bc_logger >()->log
				(
					core::bc_log_type::debug,
					core::bc_to_estring_frame(static_cast<core::bc_app_event_debug&>(p_event).get_message()).c_str()
				);

				return true;
			}

			return app_event(p_event);
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