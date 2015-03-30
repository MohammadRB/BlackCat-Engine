// [2/23/2015 MRB]

#pragma once

#include "Core/Utility/bcSingleton.h"
#include "Platform/PlatformPCH.h"
#include "Platform/bcPlatformEvents.h"
#include "Platform/Application/bcRenderWindow.h"

namespace black_cat
{
	namespace platform
	{
		template< core_platform::bc_platform >
		struct bc_render_application_parameter_pack
		{
		public:
			bc_render_application_parameter_pack(const bcECHAR* p_app_name,
				bcUINT32 p_window_width,
				bcUINT32 p_window_height,
				const bcCHAR* p_commandline)
				: m_app_name(std::move(p_app_name)),
				m_window_width(p_window_width),
				m_window_height(p_window_height),
				m_commandline(p_commandline)
			{
			}

			const bcECHAR* m_app_name;
			bcUINT32 m_window_width;
			bcUINT32 m_window_height;
			const bcCHAR* m_commandline;
		};

		using bc_render_application_parameter = bc_render_application_parameter_pack< core_platform::g_current_platform >;

		template< core_platform::bc_platform >
		struct bc_render_application_pack
		{

		};

		template< core_platform::bc_platform TPlatform >
		class bc_render_application_proxy
		{
		public:
			using platform_pack = bc_render_application_pack< TPlatform >;

		public:
			bc_render_application_proxy(bc_render_application_parameter& p_parameter);

			~bc_render_application_proxy();

			bc_render_window create_render_window(core::bc_estring p_caption, bcUINT32 p_width, bcUINT32 p_height);

			// Update method for things like message dispatching. For example platform must propagate application exit event 
			// in response to OS exit request
			void update(bc_render_window& p_render_window);

			void request_termination();

		protected:

		private:
			platform_pack m_platform_pack;
		};

		template< class TApp >
		class bc_render_application : public core::bc_singleton< TApp(bc_render_application_parameter&) >
		{
		public:
			bc_render_application()
				: m_clock(nullptr),
				m_render_window(nullptr)
			{
			}

			~bc_render_application() = default;

			bc_render_window& render_window()
			{
				return *m_render_window;
			}

			core_platform::bc_clock& clock()
			{
				return *m_clock;
			}

			bcUINT32 fps() const
			{
				return m_fps;
			}

			// Specify an upper range for application fps or specify a value lower than 0 for unlimited fps
			void fps(bcUINT32 p_fps)
			{
				if (p_fps > 0)
					m_fixed_fps = p_fps;
				else
					m_fixed_fps = -1;
			}

			void initialize(bc_render_application_parameter& p_platform_parameters) override final
			{
				app_start_engine_components();

				m_app = core::bc_make_unique< bc_render_application_proxy< core_platform::g_current_platform > >(core::bc_alloc_type::program,
					        p_platform_parameters);
				m_render_window = core::bc_make_unique< bc_render_window >(core::bc_alloc_type::program,
					                  create_render_window(
						                  core::bc_estring(p_platform_parameters.m_app_name),
						                  p_platform_parameters.m_window_width,
						                  p_platform_parameters.m_window_height));
				m_clock = core::bc_make_unique< core_platform::bc_clock >(core::bc_alloc_type::program);

				m_is_terminated = false;
				m_paused = false;
				m_termination_code = 0;

				std::memset(&m_time_delta_buffer, 0, sizeof(core_platform::bc_clock::little_time_delta_type) * s_num_time_delta_samples);
				m_current_time_delta_sample = 0;
				m_fps = 0;
				m_fixed_fps = -1;

				core::bc_event_manager& l_event_manager = core::bc_service_manager::get().global_event_manager();

				l_event_manager.register_event_listener(
					bc_app_event_exit::event_name(),
					core::bc_event_manager::delegate_type(this, &bc_render_application::app_event));
				l_event_manager.register_event_listener(
					bc_app_event_window_resize::event_name(),
					core::bc_event_manager::delegate_type(this, &bc_render_application::app_event));
				l_event_manager.register_event_listener(
					bc_app_event_active::event_name(),
					core::bc_event_manager::delegate_type(this, &bc_render_application::app_event));

				app_initialize(p_platform_parameters.m_commandline);
				app_load_content();
			}

			void destroy() override final
			{
				app_destroy();
				app_close_engine_components();

				m_app.reset(nullptr);
				m_render_window.reset(nullptr);
				m_clock.reset(nullptr);

				core_platform::bc_thread::on_main_thread_exit();
			}

			bcINT32 run()
			{
				try
				{
					while (!m_is_terminated)
					{
						m_app->update(render_window());

						if (m_paused)
						{
							core_platform::bc_thread::current_thread_sleep_for(100000000);
							continue;
						}

						m_clock->update();

						core_platform::bc_clock::little_time_delta_type l_elaped = m_clock->elapsed();
						
						// Fixed FPS
						core_platform::bc_clock::little_time_delta_type l_fixed_elapsed = 1000.0f / m_fixed_fps;
						if (m_fixed_fps != -1 && l_elaped < l_fixed_elapsed)
						{
							core_platform::bc_clock::little_time_delta_type l_diff = l_fixed_elapsed - l_elaped;
							core_platform::bc_thread::current_thread_sleep_for(l_diff * 1000000);

							l_elaped = l_fixed_elapsed;
						}
#ifdef BC_DEBUG
						if (l_elaped > 1000.0f)
							l_elaped = m_fixed_fps != -1 ? l_fixed_elapsed : 1000.0f / 32;
#endif

						_calculate_fps(l_elaped);

						app_update(l_elaped);

#ifdef BC_MEMORY_ENABLE
						core::bc_memmng::get().end_of_frame();
#endif
					}
				}
				catch (std::exception& l_exception)
				{
					core::bc_event_manager& l_event_manager = core::bc_service_manager::get().global_event_manager();

					core::bc_app_event_error m_event(l_exception.what());
					l_event_manager.process_event(m_event);

					m_termination_code = -1;
				}

				return m_termination_code;
			}

			// Broadcast application exit event and destroy render window and application
			void request_termination()
			{
				m_app->request_termination();
			}

			// Factory method to create render window
			bc_render_window create_render_window(core::bc_estring p_caption, bcUINT32 p_width, bcUINT32 p_height)
			{
				return m_app->create_render_window(std::move(p_caption), p_width, p_height);
			}

			// Initialize required engine components
			virtual void app_start_engine_components() = 0;

			// Do required initialization for app
			virtual void app_initialize(const bcCHAR* p_commandline) = 0;

			// Load required contents
			virtual void app_load_content() = 0;

			// Update and render app
			virtual void app_update(core_platform::bc_clock::little_time_delta_type p_elapsed) = 0;

			// Handle render window resizing
			virtual void app_handle_render_window_resize(bcUINT32 p_width, bcUINT32 p_height) = 0;

			// Handle app events
			virtual bool app_event(core::bc_ievent& p_event)
			{
				core::bc_event_hash l_hash = p_event.get_event_hash();

				if (l_hash == core::bc_ievent::get_hash(bc_app_event_window_resize::event_name()))
				{
					bc_app_event_window_resize& l_resize_event = static_cast<bc_app_event_window_resize&>(p_event);
					app_handle_render_window_resize(l_resize_event.width(), l_resize_event.height());

					return true;
				}

				if (l_hash == core::bc_ievent::get_hash(bc_app_event_active::event_name()))
				{
					bc_app_event_active& l_active_event = static_cast<bc_app_event_active&>(p_event);
					m_paused = !l_active_event.activate();

					if (m_paused)
						m_clock->pause();
					else
						m_clock->resume();

					return true;
				}

				if (l_hash == core::bc_ievent::get_hash(bc_app_event_exit::event_name()))
				{
					bc_app_event_exit& l_exit_event = static_cast<bc_app_event_exit&>(p_event);

					m_is_terminated = true;
					m_termination_code = l_exit_event.exit_code();

					return true;
				}

				return false;
			}

			// Do cleanup in app
			virtual void app_destroy() = 0;

			// Close all used engine components
			virtual void app_close_engine_components() = 0;

		protected:

		private:
			void _calculate_fps(core_platform::bc_clock::little_time_delta_type p_delta)
			{
				m_time_delta_buffer[m_current_time_delta_sample] = p_delta;
				m_current_time_delta_sample = (m_current_time_delta_sample + 1) % s_num_time_delta_samples;

				core_platform::bc_clock::little_time_delta_type l_average_delta = 0;
				for (UINT i = 0; i < s_num_time_delta_samples; ++i)
					l_average_delta += m_time_delta_buffer[i];
				l_average_delta /= s_num_time_delta_samples;

				m_fps = 1000.0f / l_average_delta;
			}

			core::bc_unique_ptr< bc_render_application_proxy< core_platform::g_current_platform > > m_app;
			core::bc_unique_ptr< bc_render_window > m_render_window;
			core::bc_unique_ptr< core_platform::bc_clock > m_clock;

			bool m_is_terminated;
			bool m_paused;
			bcINT32 m_termination_code;

			static const bcUINT32 s_num_time_delta_samples = 64;
			core_platform::bc_clock::little_time_delta_type m_time_delta_buffer[s_num_time_delta_samples];
			bcUINT32 m_current_time_delta_sample;
			bcUINT32 m_fps;
			bcUINT32 m_fixed_fps;
		};
	}
}

//		template< core_platform::bc_platform >
//		struct bc_render_application_parameter_pack_provider
//		{
//		public:
//			bc_render_application_parameter_pack_provider(core::bc_estring p_app_name,
//				bcUINT32 p_window_width,
//				bcUINT32 p_window_height,
//				const bcCHAR* p_commandline)
//				: m_app_name(std::move(p_app_name)),
//				m_window_width(p_window_width),
//				m_window_height(p_window_height),
//				m_commandline(p_commandline)
//			{
//			}
//
//			core::bc_estring m_app_name;
//			bcUINT32 m_window_width;
//			bcUINT32 m_window_height;
//			const bcCHAR* m_commandline;
//		};
//
//		using bc_render_application_parameter_pack = bc_render_application_parameter_pack_provider< core_platform::g_current_platform >;
//
//		template< core_platform::bc_platform >
//		class bc_render_application_provider : private core_platform::bc_no_copy_move
//		{
//		public:
//
//		protected:
//
//		private:
//		};
//
//		template< class TApp, class TProvider >
//		class bc_render_application_proxy : public core::bc_singleton< TApp(bc_render_application_parameter_pack&) >
//		{
//		public:
//			using provider_type = TProvider;
//
//		public:
//			bc_render_application_proxy()
//				: m_provider(nullptr),
//				m_render_window(nullptr)
//			{
//			}
//
//			virtual ~bc_render_application_proxy() = default;
//
//			bc_render_window& render_window()
//			{
//				return *m_render_window;
//			}
//
//			core_platform::bc_clock& clock()
//			{
//				return *m_clock;
//			}
//
//			bcUINT32 fps() const
//			{
//				return m_fps;
//			}
//
//			// Specify an upper range for application fps or specify a value lower than 0 for unlimited fps
//			void fps(bcUINT32 p_fps)
//			{
//				if (p_fps > 0)
//				{
//					m_is_fixed_fps = true;
//					m_clock->fixed_step(1000.0f / p_fps);
//				}
//				else
//				{
//					m_is_fixed_fps = false;
//					m_clock->fixed_step(-1);
//				}
//			}
//
//			void initialize(bc_render_application_parameter_pack& p_platform_parameters) override final
//			{
//				app_start_engine_components();
//
//				m_clock = core::bc_make_unique< core_platform::bc_clock >(core::bc_alloc_type::program);
//				m_provider = core::bc_make_unique< provider_type >(core::bc_alloc_type::program, p_platform_parameters);
//				m_render_window = core::bc_make_unique< bc_render_window >(core::bc_alloc_type::program,
//					create_render_window(p_platform_parameters.m_app_name,
//					p_platform_parameters.m_window_width,
//					p_platform_parameters.m_window_height));
//
//				m_is_terminated = false;
//				m_termination_code = 0;
//
//				std::memset(&m_time_delta_buffer, 0, sizeof(core_platform::bc_clock::little_time_delta_type) * s_num_time_delta_samples);
//				m_current_time_delta_sample = 0;
//				m_fps = 0;
//				m_is_fixed_fps = false;
//
//				core::bc_event_manager& l_event_manager = core::bc_service_manager::get().global_event_manager();
//
//				l_event_manager.register_event_listener(
//					bc_event_app_exit::event_name(),
//					core::bc_event_manager::delegate_type(this, &bc_render_application_proxy::app_event));
//				l_event_manager.register_event_listener(
//					bc_event_window_resize::event_name(),
//					core::bc_event_manager::delegate_type(this, &bc_render_application_proxy::app_event));
//
//				app_initialize(p_platform_parameters.m_commandline);
//				app_load_content();
//			}
//
//			void destroy() override final
//			{
//				app_destroy();
//				app_close_engine_components();
//
//				m_render_window.reset(nullptr);
//				m_provider.reset(nullptr);
//				m_clock.reset(nullptr);
//			}
//
//			bcINT32 run()
//			{
//				try
//				{
//					while (!m_is_terminated)
//					{
//						// Update provider for thinks like message dispatching. provider must propagate application exit event in 
//						// response to OS exit request
//						m_provider->update(*m_render_window);
//
//						if (m_render_window->is_minimized())
//							continue;
//
//						m_clock->update();
//						core_platform::bc_clock::little_time_delta_type l_elaped = m_clock->elapsed();
//
//						_calculate_fps(l_elaped);
//
//						// Fixed FPS
//						if (m_is_fixed_fps && l_elaped < 1000.0f / m_fps)
//							continue;
//
//						app_update(l_elaped);
//
//#ifdef BC_MEMORY_ENABLE
//						core::bc_memmng::get().end_of_frame();
//#endif
//					}
//				}
//				catch (std::exception& l_exception)
//				{
//					core::bc_event_manager& l_event_manager = core::bc_service_manager::get().global_event_manager();
//
//					core::bc_event_error m_event(l_exception.what());
//					l_event_manager.process_event(m_event);
//
//					m_termination_code = -1;
//				}
//
//				return m_termination_code;
//			}
//
//			// Broadcast application exit event and destroy render window and application
//			void request_termination()
//			{
//				m_provider->request_termination();
//			}
//
//			// Factory method to create render window
//			bc_render_window create_render_window(core::bc_estring p_caption, bcUINT32 p_width, bcUINT32 p_height)
//			{
//				return m_provider->create_render_window(std::move(p_caption), p_width, p_height);
//			}
//
//			// Initialize required engine components
//			virtual void app_start_engine_components() = 0;
//
//			// Do required initialization for app
//			virtual void app_initialize(const bcCHAR* p_commandline) = 0;
//
//			// Load required contents
//			virtual void app_load_content() = 0;
//
//			// Update and render app
//			virtual void app_update(core_platform::bc_clock::little_time_delta_type p_elapsed) = 0;
//
//			// Handle render window resizing
//			virtual void app_handle_render_window_resize(bcUINT32 p_width, bcUINT32 p_height) = 0;
//
//			// Handle app events
//			virtual bool app_event(core::bc_ievent& p_event)
//			{
//				core::bc_event_hash l_hash = p_event.get_event_hash();
//
//				if (l_hash == core::bc_ievent::get_hash(bc_event_window_resize::event_name()))
//				{
//					bc_event_window_resize& l_resize_event = static_cast<bc_event_window_resize&>(p_event);
//					app_handle_render_window_resize(l_resize_event.width(), l_resize_event.height());
//
//					return true;
//				}
//
//				if (l_hash == core::bc_ievent::get_hash(bc_event_app_exit::event_name()))
//				{
//					bc_event_app_exit& l_exit_event = static_cast<bc_event_app_exit&>(p_event);
//
//					m_is_terminated = true;
//					m_termination_code = l_exit_event.exit_code();
//
//					return true;
//				}
//
//				return false;
//			}
//
//			// Do cleanup in app
//			virtual void app_destroy() = 0;
//
//			// Close all used engine components
//			virtual void app_close_engine_components() = 0;
//
//			explicit operator provider_type&()
//			{
//				return m_provider;
//			}
//
//		protected:
//
//		private:
//			void _calculate_fps(core_platform::bc_clock::little_time_delta_type p_delta)
//			{
//				m_time_delta_buffer[m_current_time_delta_sample] = p_delta;
//				m_current_time_delta_sample = (m_current_time_delta_sample + 1) % s_num_time_delta_samples;
//
//				core_platform::bc_clock::little_time_delta_type l_average_delta = 0;
//				for (UINT i = 0; i < s_num_time_delta_samples; ++i)
//					l_average_delta += m_time_delta_buffer[i];
//				l_average_delta /= s_num_time_delta_samples;
//
//				m_fps = static_cast<UINT>(std::floor((1.0f / l_average_delta) + 0.5f));
//			}
//
//			core::bc_unique_ptr< core_platform::bc_clock > m_clock;
//			core::bc_unique_ptr< provider_type > m_provider;
//			core::bc_unique_ptr< bc_render_window > m_render_window;
//
//			bool m_is_terminated;
//			bcINT32 m_termination_code;
//
//			static const bcUINT32 s_num_time_delta_samples = 64;
//			core_platform::bc_clock::little_time_delta_type m_time_delta_buffer[s_num_time_delta_samples];
//			bcUINT32 m_current_time_delta_sample;
//			bcUINT32 m_fps;
//			bool m_is_fixed_fps;
//		};
//
//		template< class TApp >
//		using bc_render_application = bc_render_application_proxy< TApp, bc_render_application_provider< core_platform::g_current_platform > >;