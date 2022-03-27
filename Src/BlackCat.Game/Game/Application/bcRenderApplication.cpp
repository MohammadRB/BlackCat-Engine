// [2/23/2015 MRB]

#include "Game/GamePCH.h"

#include "CorePlatformImp/Concurrency/bcThread.h"
#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "Core/Utility/bcLogger.h"
#include "Game/Application/bcRenderApplication.h"
#include "Game/bcEvent.h"
#include "Game/System/Input/bcGlobalConfig.h"

namespace black_cat
{
	game::bc_render_application* g_application = nullptr;

	namespace game
	{
		struct bc_render_loop_state
		{
			enum class signal : bcUBYTE
			{
				ready = 0,
				render = 1,
				swap = 2,
				pause = 3
			};
			
			using render_func = void (bc_render_application::*)(const platform::bc_clock::update_param& p_clock);
			using pause_func = void (bc_render_application::*)(const platform::bc_clock::update_param& p_clock);
			using swap_func = void (bc_render_application::*)(const platform::bc_clock::update_param& p_clock);
			
			bc_render_loop_state(bc_render_application* p_app, 
				render_func p_render_function,
				pause_func p_pause_function,
				swap_func p_swap_function)
				: m_app(p_app),
				m_render_function(p_render_function),
				m_pause_function(p_pause_function),
				m_swap_function(p_swap_function),
				m_terminate(false),
				m_signal(signal::ready),
				m_clock(0, 0, 0)
			{
			}

			bc_render_application* m_app;
			render_func m_render_function;
			pause_func m_pause_function;
			swap_func m_swap_function;
			platform::bc_atomic<bool> m_terminate;
			platform::bc_atomic<signal> m_signal;

			platform::bc_clock::update_param m_clock;
		};
		
		bc_render_application::bc_render_application()
			: m_app_name(nullptr),
			m_app(nullptr),
			m_output_window(nullptr),
			m_clock(nullptr),
			m_fps_sampler(0),
			m_fps(0),
			m_min_update_rate(0),
			m_render_rate(0),
			m_is_terminated(false),
			m_paused(false),
			m_termination_code(0)
		{
			g_application = this;
		}

		const bcECHAR* bc_render_application::get_app_name() const noexcept
		{
			return m_app_name;
		}

		const bci_render_application_output_window* bc_render_application::get_output_window() const noexcept
		{
			return m_output_window;
		}

		bcFLOAT bc_render_application::get_ft() const noexcept
		{
			return m_frame_watch.average_total_elapsed();
		}

		bcFLOAT bc_render_application::get_fps() const noexcept
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
			constexpr auto l_pause_sleep_ms = 50U;
			bc_render_loop_state l_render_thread_state
			(
				this, 
				&bc_render_application::app_render, 
				&bc_render_application::app_render_pause_idle,
				&bc_render_application::app_render_swap_frame
			);
			platform::bc_thread l_render_thread
			(
				[&](bc_render_loop_state* p_state)
				{
					core::bc_log(core::bc_log_type::info) << "render loop started" << core::bc_lend;
					
					while (!p_state->m_terminate.load(platform::bc_memory_order::relaxed))
					{
						const auto l_signal = p_state->m_signal.load(platform::bc_memory_order::acquire);
						if (l_signal == bc_render_loop_state::signal::ready)
						{
							platform::bc_thread::current_thread_yield();
							continue;
						}

						if(l_signal == bc_render_loop_state::signal::render)
						{
							(p_state->m_app->*p_state->m_render_function)(p_state->m_clock);
						}
						else if(l_signal == bc_render_loop_state::signal::pause)
						{
							(p_state->m_app->*p_state->m_pause_function)(p_state->m_clock);
							platform::bc_thread::current_thread_sleep_for(std::chrono::milliseconds(l_pause_sleep_ms));
							continue;
						}

						p_state->m_signal.store(bc_render_loop_state::signal::ready, platform::bc_memory_order::release);
						
						while (p_state->m_signal.load(platform::bc_memory_order::acquire) != bc_render_loop_state::signal::swap)
						{
							if(p_state->m_terminate.load(platform::bc_memory_order::relaxed))
							{
								break;
							}
							
							platform::bc_thread::current_thread_yield();
						}
						
						(p_state->m_app->*p_state->m_swap_function)(p_state->m_clock);
						
						p_state->m_signal.store(bc_render_loop_state::signal::ready, platform::bc_memory_order::release);
					}
				},
				&l_render_thread_state
			);
			l_render_thread.set_name(bcL("BC_RENDER_WORKER"));
			l_render_thread.set_priority(platform::bc_thread_priority::highest);
			
			try
			{
				const platform::bc_clock::small_time l_min_update_elapsed = 1000.0f / static_cast<bcFLOAT>(m_min_update_rate);
				platform::bc_clock::big_time l_total_elapsed = 0;
				platform::bc_clock::small_time l_elapsed = 0;
				platform::bc_clock::small_time l_average_elapsed = 0;
				platform::bc_clock::small_time l_local_elapsed = 0;
				platform::bc_clock::small_time l_sleep_time_error = 0;
				core::bc_stop_watch l_sleep_watch;
				core::bc_stop_watch l_sleep_watch1;
				
				core::bc_log(core::bc_log_type::info) << "update loop started" << core::bc_lend;
				
				while (!m_is_terminated)
				{
					m_frame_watch.start();
					m_clock->update();
					
					l_total_elapsed = m_clock->get_total_elapsed();
					l_elapsed = m_clock->get_elapsed();
					l_average_elapsed = m_fps_sampler.average_value();
					
					if (l_elapsed > 1000.0f)
					{
						l_elapsed = l_min_update_elapsed;
					}

					const auto l_clock = platform::bc_clock::update_param(l_total_elapsed, l_elapsed, l_average_elapsed);

					m_app->update();
					if(m_output_window)
					{
						m_output_window->update();
					}

					if (m_paused)
					{
						l_render_thread_state.m_clock = l_clock;
						l_render_thread_state.m_signal.store(bc_render_loop_state::signal::pause, platform::bc_memory_order::release);

						app_pause_idle(l_clock);

						platform::bc_thread::current_thread_sleep_for(std::chrono::milliseconds(l_pause_sleep_ms));
						continue;
					}

					l_render_thread_state.m_clock = l_clock;
					l_render_thread_state.m_signal.store(bc_render_loop_state::signal::render, platform::bc_memory_order::release);

					l_local_elapsed += l_elapsed;
					const auto l_update_call_count = std::max(static_cast<bcUINT32>(std::floor(l_local_elapsed / l_min_update_elapsed)), 1U);
					auto l_update_call_counter = l_update_call_count;

					while (l_update_call_counter > 0)
					{
						app_update(platform::bc_clock::update_param(l_total_elapsed, l_elapsed, l_average_elapsed, std::min(l_elapsed, l_min_update_elapsed)), l_update_call_counter > 1);
						--l_update_call_counter;
					}

					l_local_elapsed = std::max(l_local_elapsed - l_min_update_elapsed * l_update_call_count, static_cast<platform::bc_clock::small_time>(0));
					
					while (l_render_thread_state.m_signal.load(platform::bc_memory_order::acquire) != bc_render_loop_state::signal::ready)
					{
						app_swap_frame_idle(l_clock);
					}

					l_render_thread_state.m_signal.store(bc_render_loop_state::signal::swap, platform::bc_memory_order::release);

					app_swap_frame(l_clock);

					while (l_render_thread_state.m_signal.load(platform::bc_memory_order::acquire) != bc_render_loop_state::signal::ready)
					{
						platform::bc_thread::current_thread_yield();
					}

#ifdef BC_MEMORY_ENABLE
					core::bc_memory_manager::get().end_of_frame();
#endif

					m_frame_watch.stop();
					const auto l_frame_elapsed = m_frame_watch.restart();
					m_fps_sampler.add_sample(l_elapsed);
					m_fps = 1000.0f / m_fps_sampler.average_value();

					if (m_render_rate != -1) // Fixed render rate
					{
						const platform::bc_clock::small_time l_render_rate_fixed_elapsed = 1000.0f / static_cast<bcFLOAT>(m_render_rate);

						const auto l_sleep_time = (l_render_rate_fixed_elapsed - l_frame_elapsed) - l_sleep_time_error;
						if (l_sleep_time <= 0)
						{
							// If sleep error become a large number once, it will zero sleep time forever
							l_sleep_time_error = 0;
							continue;
						}
						
						l_sleep_watch.start();
						l_sleep_watch1.start();
							
						while(true)
						{
							platform::bc_thread::current_thread_yield_switch();
							l_sleep_watch.stop();

							const auto l_sleep_elapsed = l_sleep_watch.total_elapsed();
							if(l_sleep_elapsed >= l_sleep_time)
							{
								break;
							}

							l_sleep_watch.start();
						}
						
						l_sleep_watch1.stop();
						
						const auto l_sleep_elapsed = l_sleep_watch.restart();
						const auto l_sleep_total_elapsed = l_sleep_watch1.restart();
						l_sleep_time_error = l_sleep_total_elapsed - l_sleep_elapsed;
					}
				}
			}
			catch (std::exception& l_exception)
			{
				auto l_event = core::bc_app_event_error(l_exception.what());
				core::bc_get_service<core::bc_event_manager>()->process_event(l_event);

				m_termination_code = -1;
			}

			l_render_thread_state.m_terminate.store(true, platform::bc_memory_order::relaxed);
			l_render_thread.join();
			
			return m_termination_code;
		}

		void bc_render_application::request_termination() const
		{
			m_app->request_termination();
		}

		platform::bc_basic_window bc_render_application::create_basic_render_window(const bcECHAR* p_caption, bcUINT32 p_width, bcUINT32 p_height)
		{
			return m_app->create_basic_window(p_caption, p_width, p_height);
		}

		platform::bc_console_window bc_render_application::create_console_window(const bcECHAR* p_caption)
		{
			return m_app->create_console_window(p_caption);
		}

		void bc_render_application::_initialize(bc_engine_application_parameter& p_parameters)
		{
			try
			{
				m_app_name = p_parameters.m_app_parameters.m_app_name;
				m_output_window = p_parameters.m_app_parameters.m_output_window_factory(); // Make output window available before starting game components

				app_start_engine_components(p_parameters);

				m_app = core::bc_make_unique<platform::bc_application>(core::bc_alloc_type::program, p_parameters.m_app_parameters);
				m_clock = core::bc_make_unique<platform::bc_clock>(core::bc_alloc_type::program);

				m_min_update_rate = 60;
				m_render_rate = m_min_update_rate;

				m_is_terminated = false;
				m_paused = false;
				m_termination_code = 0;

				auto* l_event_manager = core::bc_get_service<core::bc_event_manager>();
				m_event_handle_window_state = l_event_manager->register_event_listener<platform::bc_app_event_window_state>
				(
					core::bc_event_manager::delegate_type(*this, &bc_render_application::_app_event)
				);
				m_event_handle_window_resize = l_event_manager->register_event_listener<platform::bc_app_event_window_resize>
				(
					core::bc_event_manager::delegate_type(*this, &bc_render_application::_app_event)
				);
				m_event_handle_window_focus = l_event_manager->register_event_listener<platform::bc_app_event_window_focus>
				(
					core::bc_event_manager::delegate_type(*this, &bc_render_application::_app_event)
				);
				m_event_handle_window_close = l_event_manager->register_event_listener<platform::bc_app_event_window_close>
				(
					core::bc_event_manager::delegate_type(*this, &bc_render_application::_app_event)
				);
				m_event_handle_app_pause = l_event_manager->register_event_listener<platform::bc_app_event_pause_state>
				(
					core::bc_event_manager::delegate_type(*this, &bc_render_application::_app_event)
				);
				m_event_handle_app_exit = l_event_manager->register_event_listener<platform::bc_app_event_exit>
				(
					core::bc_event_manager::delegate_type(*this, &bc_render_application::_app_event)
				);
				m_event_handle_error = l_event_manager->register_event_listener<core::bc_app_event_error>
				(
					core::bc_event_manager::delegate_type(*this, &bc_render_application::_app_event)
				);
				m_event_handle_key = l_event_manager->register_event_listener<platform::bc_app_event_key>
				(
					core::bc_event_manager::delegate_type(*this, &bc_render_application::_app_event)
				);
				m_event_handle_pointing = l_event_manager->register_event_listener<platform::bc_app_event_pointing>
				(
					core::bc_event_manager::delegate_type(*this, &bc_render_application::_app_event)
				);
				m_event_handle_config_changed = l_event_manager->register_event_listener<bc_event_global_config_changed>
				(
					core::bc_event_manager::delegate_type(*this, &bc_render_application::_app_event)
				);

				app_initialize(p_parameters);
				app_load_content();
			}
			catch (std::exception& l_exception)
			{
				if(auto* l_event_manager = core::bc_get_service<core::bc_event_manager>())
				{
					auto l_event = core::bc_app_event_error(l_exception.what());
					if(!l_event_manager->process_event(l_event))
					{
						_app_event(l_event);
					}
				}
				throw;
			}
		}

		void bc_render_application::_destroy()
		{
			app_unload_content();
			app_destroy();

			m_event_handle_window_state.reset();
			m_event_handle_window_resize.reset();
			m_event_handle_window_focus.reset();
			m_event_handle_window_close.reset();
			m_event_handle_app_pause.reset();
			m_event_handle_app_exit.reset();
			m_event_handle_error.reset();
			m_event_handle_key.reset();
			m_event_handle_pointing.reset();
			m_event_handle_config_changed.reset();

			m_clock.reset(nullptr);
			m_output_window->close();
			m_app.reset(nullptr);

			app_close_engine_components();
		}

		void bc_render_application::_app_event(core::bci_event& p_event)
		{
			app_event(p_event);

			auto* l_event_manager = core::bc_get_service<core::bc_event_manager>();

			if(const auto* l_window_state_event = core::bci_message::as<platform::bc_app_event_window_state>(p_event))
			{
				if (l_window_state_event->get_window_id() == m_output_window->get_id())
				{
					if(l_window_state_event->get_state() == platform::bc_app_event_window_state::state::minimized)
					{
						platform::bc_app_event_pause_state l_active_event(platform::bc_app_event_pause_state::state::pause_request);
						l_event_manager->process_event(l_active_event);
					}
					else if(l_window_state_event->get_state() == platform::bc_app_event_window_state::state::restored)
					{
						platform::bc_app_event_pause_state l_active_event(platform::bc_app_event_pause_state::state::resume_request);
						l_event_manager->process_event(l_active_event);
					}
				}
				return;
			}

			if (const auto* l_window_focus_event = core::bci_message::as<platform::bc_app_event_window_focus>(p_event))
			{
				/*if(l_window_focus_event->get_focus())
				{
					platform::bc_app_event_pause_state l_active_event(platform::bc_app_event_pause_state::state::resume_request);
					l_event_manager->process_event(l_active_event);
				}
				else
				{
					platform::bc_app_event_pause_state l_active_event(platform::bc_app_event_pause_state::state::pause_request);
					l_event_manager->process_event(l_active_event);
				}*/
				return;
			}

			if (const auto* l_close_event = core::bci_message::as<platform::bc_app_event_window_close>(p_event))
			{
				if (m_output_window && m_output_window->get_id() == l_close_event->get_window_id())
				{
					platform::bc_app_event_exit l_exit_event(0);
					l_event_manager->process_event(l_exit_event);
				}
				return;
			}

			if (const auto* l_pause_event = core::bci_message::as<platform::bc_app_event_pause_state>(p_event))
			{
				if (l_pause_event->get_state() != platform::bc_app_event_pause_state::state::paused)
				{
					m_paused = l_pause_event->get_state() == platform::bc_app_event_pause_state::state::pause_request;
					if (m_paused)
					{
						m_clock->pause();
					}
					else
					{
						m_clock->resume();
					}
				}
				return;
			}

			if (const auto* l_exit_event = core::bci_message::as<platform::bc_app_event_exit>(p_event))
			{
				m_is_terminated = true;
				m_termination_code = l_exit_event->exit_code();
				return;
			}

			if (const auto* l_error_event = core::bci_message::as<core::bc_app_event_error>(p_event))
			{
				core::bc_log(core::bc_log_type::error) << l_error_event->get_message() << core::bc_lend;
				return;
			}

			if (const auto* l_config_event = core::bci_message::as<bc_event_global_config_changed>(p_event))
			{
				const auto l_log_types_array = l_config_event->get_config().get_log_types();
				auto l_log_types = core::bc_enum::none<core::bc_log_type>();

				for(const auto& l_log_type : l_log_types_array)
				{
					if(l_log_type == "info")
					{
						l_log_types = core::bc_enum::set(l_log_types, core::bc_log_type::info, true);
					}
					else if (l_log_type == "debug")
					{
						l_log_types = core::bc_enum::set(l_log_types, core::bc_log_type::debug, true);
					}
					else if (l_log_type == "warning")
					{
						l_log_types = core::bc_enum::set(l_log_types, core::bc_log_type::warning, true);
					}
					else if (l_log_type == "error")
					{
						l_log_types = core::bc_enum::set(l_log_types, core::bc_log_type::error, true);
					}
				}

				core::bc_get_service<core::bc_logger>()->set_enabled_log_types(l_log_types);

				return;
			}
		}
	}
}
