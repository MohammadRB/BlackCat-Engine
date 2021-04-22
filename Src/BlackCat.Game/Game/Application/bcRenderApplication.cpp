// [2/23/2015 MRB]

#include "Game/GamePCH.h"

#include "CorePlatformImp/Concurrency/bcThread.h"
#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "Core/Utility/bcLogger.h"
#include "Core/bcEvent.h"
#include "Platform/bcEvent.h"
#include "Game/Application/bcRenderApplication.h"

namespace black_cat
{
	namespace game
	{
		struct bc_render_loop_state
		{
			explicit bc_render_loop_state(void (bc_render_application::* p_render_function)(core_platform::bc_clock::update_param))
				: m_terminate(false),
				m_signal(false),
				m_render_function(p_render_function),
				m_render_application(nullptr),
				m_clock(0, 0, 0)
			{
			}

			core_platform::bc_atomic< bool > m_terminate;
			core_platform::bc_atomic< bool > m_signal;
			void(bc_render_application::*m_render_function)(core_platform::bc_clock::update_param);

			bc_render_application* m_render_application;
			core_platform::bc_clock::update_param m_clock;
		};
		
		bc_render_application::bc_render_application()
			: m_app(nullptr),
			m_default_output_window(nullptr),
			m_output_window(nullptr),
			m_clock(nullptr),
			m_is_terminated(false),
			m_paused(false),
			m_termination_code(0),
			m_min_update_rate(0),
			m_render_rate(0)
		{
		}

		bci_render_application_output_window& bc_render_application::get_output_window() const
		{
			return *m_output_window;
		}

		core_platform::bc_clock& bc_render_application::get_clock() const
		{
			return *m_clock;
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
			bc_render_loop_state l_render_thread_state(&bc_render_application::app_render);
			core_platform::bc_thread l_render_thread
			(
				[&](bc_render_loop_state* p_state)
				{
					while (!p_state->m_terminate.load(core_platform::bc_memory_order::relaxed))
					{
						while (!p_state->m_signal.load(core_platform::bc_memory_order::acquire))
						{
							core_platform::bc_thread::current_thread_yield();
						}

						(p_state->m_render_application->*p_state->m_render_function)(p_state->m_clock);

						p_state->m_signal.store(false, core_platform::bc_memory_order::release);
					}
				},
				&l_render_thread_state
			);
			l_render_thread.set_name(bcL("BC_RENDER_WORKER"));
			l_render_thread.set_priority(core_platform::bc_thread_priority::highest);
			
			try
			{
				const core_platform::bc_clock::small_delta_time l_min_update_elapsed = 1000.0f / m_min_update_rate;
				core_platform::bc_clock::small_delta_time l_sleep_time = 0;
				core_platform::bc_clock::small_delta_time l_elapsed = 0;
				core_platform::bc_clock::small_delta_time l_local_elapsed = 0;
				core_platform::bc_clock::big_delta_time l_total_elapsed = 0;
				
				while (!m_is_terminated)
				{
					m_output_window->update();
					m_app->update();

					if (m_paused)
					{
						core_platform::bc_thread::current_thread_sleep_for(std::chrono::milliseconds(100));
						core::bc_get_service< core::bc_event_manager >()->process_event_queue // Let events be processed
						(
							core_platform::bc_clock::update_param(l_total_elapsed, l_elapsed)
						);
						continue;
					}

					m_clock->update();

					l_elapsed = m_clock->get_elapsed();
					l_total_elapsed = m_clock->get_total_elapsed();

#ifdef BC_DEBUG
					if (l_elapsed > 1000.0f)
					{
						l_elapsed = l_min_update_elapsed;
					}
#endif

					l_render_thread_state.m_render_application = this;
					l_render_thread_state.m_clock = core_platform::bc_clock::update_param(l_total_elapsed, l_elapsed);
					l_render_thread_state.m_signal.store(true, core_platform::bc_memory_order::release);

					l_local_elapsed += l_elapsed;
					const auto l_update_call_count = std::max(static_cast<bcUINT32>(std::floor(l_local_elapsed / l_min_update_elapsed)), 1U);
					auto l_update_call_counter = l_update_call_count;

					while (l_update_call_counter > 0)
					{
						app_update(core_platform::bc_clock::update_param(l_total_elapsed, l_elapsed, std::min(l_elapsed, l_min_update_elapsed)), l_update_call_counter > 1);
						--l_update_call_counter;
					}

					l_local_elapsed = std::max(l_local_elapsed - l_min_update_elapsed * l_update_call_count, static_cast<core_platform::bc_clock::small_delta_time>(0));
					
					while (l_render_thread_state.m_signal.load(core_platform::bc_memory_order::acquire))
					{
						app_swap_frame_idle(core_platform::bc_clock::update_param(l_total_elapsed, l_elapsed));
					}

					app_swap_frame(core_platform::bc_clock::update_param(l_total_elapsed, l_elapsed));

#ifdef BC_MEMORY_ENABLE
					core::bc_memory_manager::get().end_of_frame();
#endif
					
					if (m_render_rate != -1) // Fixed render rate
					{
						const core_platform::bc_clock::small_delta_time l_render_rate_fixed_elapsed = 1000.0f / m_render_rate;

						if(l_elapsed - l_sleep_time < l_render_rate_fixed_elapsed)
						{
							l_sleep_time = l_render_rate_fixed_elapsed - (l_elapsed - l_sleep_time);
							core_platform::bc_thread::current_thread_sleep_for
							(
								std::chrono::milliseconds(static_cast<std::chrono::milliseconds::rep>(l_sleep_time))
							);
						}
					}
				}
			}
			catch (std::exception& l_exception)
			{
				core::bc_app_event_error l_event(l_exception.what());
				core::bc_get_service< core::bc_event_manager >()->process_event(l_event);

				m_termination_code = -1;

#ifdef BC_MEMORY_ENABLE
				core::bc_memory_manager::get().end_of_frame();
#endif
			}

			l_render_thread_state.m_signal.store(true, core_platform::bc_memory_order::relaxed);
			l_render_thread_state.m_terminate.store(true, core_platform::bc_memory_order::relaxed);
			l_render_thread.join();
			
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

			auto* l_event_manager = core::bc_get_service< core::bc_event_manager >();
			m_event_handle_window_state = l_event_manager->register_event_listener< platform::bc_app_event_window_state >
			(
				core::bc_event_manager::delegate_type(*this, &bc_render_application::_app_event)
			);
			m_event_handle_window_resize = l_event_manager->register_event_listener<platform::bc_app_event_window_resize >
			(
				core::bc_event_manager::delegate_type(*this, &bc_render_application::_app_event)
			);
			m_event_handle_window_focus = l_event_manager->register_event_listener< platform::bc_app_event_window_focus >
			(
				core::bc_event_manager::delegate_type(*this, &bc_render_application::_app_event)
			);
			m_event_handle_window_close = l_event_manager->register_event_listener< platform::bc_app_event_window_close >
			(
				core::bc_event_manager::delegate_type(*this, &bc_render_application::_app_event)
			);
			m_event_handle_app_active = l_event_manager->register_event_listener< platform::bc_app_event_active >
			(
				core::bc_event_manager::delegate_type(*this, &bc_render_application::_app_event)
			);
			m_event_handle_app_exit = l_event_manager->register_event_listener< platform::bc_app_event_exit >
			(
				core::bc_event_manager::delegate_type(*this, &bc_render_application::_app_event)
			);
			m_event_handle_error = l_event_manager->register_event_listener< core::bc_app_event_error >
			(
				core::bc_event_manager::delegate_type(*this, &bc_render_application::_app_event)
			);
			m_event_handle_debug = l_event_manager->register_event_listener< core::bc_app_event_debug >
			(
				core::bc_event_manager::delegate_type(*this, &bc_render_application::_app_event)
			);
			m_event_handle_key = l_event_manager->register_event_listener< platform::bc_app_event_key >
			(
				core::bc_event_manager::delegate_type(*this, &bc_render_application::_app_event)
			);
			m_event_handle_pointing = l_event_manager->register_event_listener<platform::bc_app_event_pointing>
			(
				core::bc_event_manager::delegate_type(*this, &bc_render_application::_app_event)
			);

			app_initialize(p_parameters);
			app_load_content();
		}

		void bc_render_application::_destroy()
		{
			app_unload_content();
			app_destroy();

			m_event_handle_window_state.reset();
			m_event_handle_window_resize.reset();
			m_event_handle_window_focus.reset();
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
		}

		bool bc_render_application::_app_event(core::bci_event& p_event)
		{
			auto* l_event_manager = core::bc_get_service<core::bc_event_manager>();
			
			if(core::bci_message::is<platform::bc_app_event_window_state>(p_event))
			{
				auto& l_resize_event = static_cast<platform::bc_app_event_window_state&>(p_event);
				if (l_resize_event.get_window_id() == m_output_window->get_id())
				{
					if(l_resize_event.get_state() == platform::bc_app_event_window_state::state::minimized)
					{
						platform::bc_app_event_active l_active_event(false);
						l_event_manager->process_event(l_active_event);
					}
					else if(l_resize_event.get_state() == platform::bc_app_event_window_state::state::restored)
					{
						platform::bc_app_event_active l_active_event(true);
						l_event_manager->process_event(l_active_event);
					}
				}
			}
			
			if (core::bci_message::is<platform::bc_app_event_window_resize>(p_event))
			{
				auto& l_resizing_event = static_cast<platform::bc_app_event_window_resize&>(p_event);

				if (l_resizing_event.get_window_id() == m_output_window->get_id())
				{
					if (l_resizing_event.start_resizing())
					{
						platform::bc_app_event_active l_active_event(false);
						l_event_manager->process_event(l_active_event);
					}
					else
					{
						platform::bc_app_event_active l_active_event(true);
						l_event_manager->process_event(l_active_event);
					}
				}
			}

			if (core::bci_message::is<platform::bc_app_event_window_focus>(p_event))
			{
				auto& l_focus_event = static_cast<platform::bc_app_event_window_focus&>(p_event);
				if(l_focus_event.get_focus())
				{
					platform::bc_app_event_active l_active_event(true);
					l_event_manager->process_event(l_active_event);
				}
				else
				{
					platform::bc_app_event_active l_active_event(false);
					l_event_manager->process_event(l_active_event);
				}
			}
			
			if (core::bci_message::is<platform::bc_app_event_active>(p_event))
			{
				auto& l_active_event = static_cast<platform::bc_app_event_active&>(p_event);
				m_paused = !l_active_event.active();

				if (m_paused)
				{
					m_clock->pause();
				}
				else
				{
					m_clock->resume();
				}
			}

			if (core::bci_message::is<platform::bc_app_event_window_close>(p_event))
			{
				auto& l_close_event = static_cast<platform::bc_app_event_window_close&>(p_event);

				if (l_close_event.get_window_id() == m_output_window->get_id())
				{
					platform::bc_app_event_exit l_exit_event(0);
					l_event_manager->process_event(l_exit_event);
				}
			}

			if (core::bci_message::is<platform::bc_app_event_exit>(p_event))
			{
				auto& l_exit_event = static_cast<platform::bc_app_event_exit&>(p_event);

				m_is_terminated = true;
				m_termination_code = l_exit_event.exit_code();
			}

			if (core::bci_message::is<core::bc_app_event_error>(p_event))
			{
				core::bc_get_service< core::bc_logger >()->log
				(
					core::bc_log_type::error,
					core::bc_to_estring_frame(static_cast<core::bc_app_event_error&>(p_event).get_message()).c_str()
				);
			}

			if (core::bci_message::is<core::bc_app_event_debug>(p_event))
			{
				core::bc_get_service<core::bc_logger>()->log
				(
					core::bc_log_type::debug,
					core::bc_to_estring_frame(static_cast<core::bc_app_event_debug&>(p_event).get_message()).c_str()
				);
			}

			return app_event(p_event);
		}
	}
}