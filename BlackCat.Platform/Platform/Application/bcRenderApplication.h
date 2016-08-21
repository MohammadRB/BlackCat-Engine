// [2/23/2015 MRB]

#pragma once

#include "Core/Utility/bcSingleton.h"
#include "Core/Concurrency/bcThreadManager.h"
#include "Core/Event/bcEventManager.h"
#include "Platform/PlatformPCH.h"
#include "Platform/bcPlatformEvents.h"
#include "Platform/Application/bcRenderWindow.h"

namespace black_cat
{
	namespace platform
	{
		struct bc_engine_components_parameters
		{
			bcSIZE m_memmng_fsa_start_size;
			bcSIZE m_memmng_fsa_count;
			bcSIZE m_memmng_fsa_allocation_count;
			bcSIZE m_memmng_program_stack_size;
			bcSIZE m_memmng_level_stack_size;
			bcSIZE m_memmng_frame_stack_size;
			bcSIZE m_memmng_super_heap_size;
			bcSIZE m_thread_manager_thread_count;
			bcSIZE m_thread_manager_reserve_thread_count;
		};

		template< core_platform::bc_platform >
		struct bc_platform_render_application_parameter_pack
		{
		public:
			bc_platform_render_application_parameter_pack(const bcECHAR* p_app_name,
				bcUINT32 p_window_width,
				bcUINT32 p_window_height,
				bc_engine_components_parameters p_engine_components,
				const bcCHAR* p_commandline)
				: m_app_name(std::move(p_app_name)),
				m_window_width(p_window_width),
				m_window_height(p_window_height),
				m_engine_components(p_engine_components),
				m_commandline(p_commandline)
			{
			}

			const bcECHAR* m_app_name;
			bcUINT32 m_window_width;
			bcUINT32 m_window_height;
			bc_engine_components_parameters m_engine_components;
			const bcCHAR* m_commandline;
		};

		using bc_render_application_parameter = bc_platform_render_application_parameter_pack< core_platform::g_current_platform >;

		template< core_platform::bc_platform >
		struct bc_platform_render_application_pack
		{

		};

		template< core_platform::bc_platform TPlatform >
		class bc_platform_render_application
		{
		public:
			using platform_pack = bc_platform_render_application_pack< TPlatform >;

		public:
			bc_platform_render_application(bc_render_application_parameter& p_parameter);

			~bc_platform_render_application();

			bc_render_window create_render_window(core::bc_estring p_caption, bcUINT32 p_width, bcUINT32 p_height);

			// Update method for things like message dispatching. For example platform must propagate application exit event 
			// in response to OS exit request
			void update(bc_render_window& p_render_window);

			void request_termination();

		protected:

		private:
			platform_pack m_pack;
		};

		template< class TApp >
		class bc_render_application : public core::bc_singleton< TApp(bc_render_application_parameter&) >
		{
		public:
			bc_render_application();

			~bc_render_application() = default;

			bc_render_window& get_render_window() const;

			core_platform::bc_clock& get_clock() const;

			bcUINT32 get_fps() const;

			// Specify an upper range for application fps or specify a value lower than 0 for unlimited fps
			void set_fps(bcUINT32 p_fps);

			bcINT32 run();

			// Broadcast application exit event and destroy render window and application
			void request_termination() const;

			// Factory method to create render window
			bc_render_window create_render_window(core::bc_estring p_caption, bcUINT32 p_width, bcUINT32 p_height);

			// Initialize required engine components(call in top of derived function)
			virtual void app_start_engine_components(bc_engine_components_parameters& p_engine_components) = 0;

			// Do required initialization for app(call in top of derived function)
			virtual void app_initialize(const bcCHAR* p_commandline) = 0;

			// Load required contents(call in top of derived function)
			virtual void app_load_content() = 0;

			// Update and render app(call in top of derived function)
			virtual void app_update(core_platform::bc_clock::update_param p_clock_update_param) = 0;

			// Handle render window resizing
			virtual void app_handle_render_window_resize(bcUINT32 p_width, bcUINT32 p_height) = 0;

			// Handle app events(call in bottom of derived function)
			virtual bool app_event(core::bc_ievent& p_event);

			// Cleanup loaeded contents(call in bottom of derived function)
			virtual void app_unload_content() = 0;

			// Do cleanup in app(call in bottom of derived function)
			virtual void app_destroy() = 0;

			// Close all used engine components(call in bottom of derived function)
			virtual void app_close_engine_components() = 0;

		protected:

		private:
			void _initialize(bc_render_application_parameter& p_platform_parameters) override;

			void _destroy() override final;

			void _calculate_fps(core_platform::bc_clock::small_delta_time p_delta);

			core::bc_unique_ptr< bc_platform_render_application< core_platform::g_current_platform > > m_app;
			core::bc_unique_ptr< bc_render_window > m_render_window;
			core::bc_unique_ptr< core_platform::bc_clock > m_clock;

			bool m_is_terminated;
			bool m_paused;
			bcINT32 m_termination_code;

			static const bcUINT32 s_num_time_delta_samples = 64;
			core_platform::bc_clock::small_delta_time m_time_delta_buffer[s_num_time_delta_samples];
			bcUINT32 m_current_time_delta_sample;
			bcUINT32 m_fps;
			bcUINT32 m_fixed_fps;

			core::bc_event_listener_handle m_event_handle_app_exit;
			core::bc_event_listener_handle m_event_handle_app_active;
			core::bc_event_listener_handle m_event_handle_window_resize;
		};

		template< class TApp >
		bc_render_application< TApp >::bc_render_application(): m_render_window(nullptr),
			m_clock(nullptr),
			m_is_terminated(false),
			m_paused(false),
			m_termination_code(0),
			m_current_time_delta_sample(0),
			m_fps(0),
			m_fixed_fps(0)
		{
		}

		template< class TApp >
		bc_render_window& bc_render_application< TApp >::get_render_window() const
		{
			return *m_render_window;
		}

		template< class TApp >
		core_platform::bc_clock& bc_render_application< TApp >::get_clock() const
		{
			return *m_clock;
		}

		template< class TApp >
		bcUINT32 bc_render_application< TApp >::get_fps() const
		{
			return m_fps;
		}

		template< class TApp >
		void bc_render_application< TApp >::set_fps(bcUINT32 p_fps)
		{
			if (p_fps > 0)
				m_fixed_fps = p_fps;
			else
				m_fixed_fps = -1;
		}

		template< class TApp >
		bcINT32 bc_render_application< TApp >::run()
		{
			try
			{
				while (!m_is_terminated)
				{
					m_app->update(get_render_window());

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
						l_elaped = m_fixed_fps != -1 ? l_fixed_elapsed : 1000.0f / 32;
#endif

					_calculate_fps(l_elaped);

					app_update(core_platform::bc_clock::update_param(l_total_elapsed, l_elaped));
				}
			}
			catch (std::exception& l_exception)
			{
				core::bc_event_manager* l_event_manager = core::bc_service_manager::get().get_service<core::bc_event_manager>();

				core::bc_app_event_error m_event(l_exception.what());
				l_event_manager->process_event(m_event);

				m_termination_code = -1;
			}

			return m_termination_code;
		}

		template< class TApp >
		void bc_render_application< TApp >::request_termination() const
		{
			m_app->request_termination();
		}

		template< class TApp >
		bc_render_window bc_render_application< TApp >::create_render_window(core::bc_estring p_caption, bcUINT32 p_width, bcUINT32 p_height)
		{
			return m_app->create_render_window(move(p_caption), p_width, p_height);
		}

		template< class TApp >
		bool bc_render_application< TApp >::app_event(core::bc_ievent& p_event)
		{
			core::bc_event_hash l_hash = p_event.get_event_hash();

			if (core::bc_ievent::event_is< bc_app_event_window_resize >(p_event))
			{
				bc_app_event_window_resize& l_resize_event = static_cast< bc_app_event_window_resize& >(p_event);
				app_handle_render_window_resize(l_resize_event.width(), l_resize_event.height());

				return true;
			}

			if (core::bc_ievent::event_is< bc_app_event_active >(p_event))
			{
				bc_app_event_active& l_active_event = static_cast< bc_app_event_active& >(p_event);
				m_paused = !l_active_event.activate();

				if (m_paused)
					m_clock->pause();
				else
					m_clock->resume();

				return true;
			}

			if (core::bc_ievent::event_is< bc_app_event_exit >(p_event))
			{
				bc_app_event_exit& l_exit_event = static_cast< bc_app_event_exit& >(p_event);

				m_is_terminated = true;
				m_termination_code = l_exit_event.exit_code();

				return true;
			}

			return false;
		}

		template< class TApp >
		void bc_render_application< TApp >::_initialize(bc_render_application_parameter& p_platform_parameters)
		{
			app_start_engine_components(p_platform_parameters.m_engine_components);

			m_app = core::bc_make_unique< bc_platform_render_application< core_platform::g_current_platform > >
				(
					core::bc_alloc_type::program,
					p_platform_parameters
				);
			m_render_window = core::bc_make_unique< bc_render_window >
				(
					core::bc_alloc_type::program,
					create_render_window(
						core::bc_estring(p_platform_parameters.m_app_name),
						p_platform_parameters.m_window_width,
						p_platform_parameters.m_window_height)
				);
			m_clock = core::bc_make_unique< core_platform::bc_clock >(core::bc_alloc_type::program);

			m_is_terminated = false;
			m_paused = false;
			m_termination_code = 0;

			memset(&m_time_delta_buffer, 0, sizeof(core_platform::bc_clock::small_delta_time) * s_num_time_delta_samples);
			m_current_time_delta_sample = 0;
			m_fps = 0;
			m_fixed_fps = -1;

			core::bc_event_manager* l_event_manager = core::bc_service_manager::get().get_service< core::bc_event_manager >();

			m_event_handle_app_exit = l_event_manager->register_event_listener<bc_app_event_exit>
				(
					core::bc_event_manager::delegate_type(this, &bc_render_application::app_event)
				);
			m_event_handle_window_resize = l_event_manager->register_event_listener<bc_app_event_window_resize>
				(
					core::bc_event_manager::delegate_type(this, &bc_render_application::app_event)
				);
			m_event_handle_app_active = l_event_manager->register_event_listener<bc_app_event_active>
				(
					core::bc_event_manager::delegate_type(this, &bc_render_application::app_event)
				);

			app_initialize(p_platform_parameters.m_commandline);

			app_load_content();
		}

		template< class TApp >
		void bc_render_application< TApp >::_destroy()
		{
			app_unload_content();

			app_destroy();

			m_event_handle_app_exit.reset();
			m_event_handle_window_resize.reset();
			m_event_handle_app_active.reset();

			m_app.reset(nullptr);
			m_render_window.reset(nullptr);
			m_clock.reset(nullptr);

			app_close_engine_components();

			core_platform::bc_thread::on_main_thread_exit();
		}

		template< class TApp >
		void bc_render_application< TApp >::_calculate_fps(core_platform::bc_clock::small_delta_time p_delta)
		{
			m_time_delta_buffer[m_current_time_delta_sample] = p_delta;
			m_current_time_delta_sample = (m_current_time_delta_sample + 1) % s_num_time_delta_samples;

			core_platform::bc_clock::small_delta_time l_average_delta = 0;
			for (UINT i = 0; i < s_num_time_delta_samples; ++i)
				l_average_delta += m_time_delta_buffer[i];
			l_average_delta /= s_num_time_delta_samples;

			m_fps = round(1000.0f / l_average_delta);
		}
	}
}