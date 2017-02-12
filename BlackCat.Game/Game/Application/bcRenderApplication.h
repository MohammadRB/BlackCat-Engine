// [2/23/2015 MRB]

#pragma once

#include "Core/Utility/bcInitializable.h"
#include "Core/Event/bcEventManager.h"
#include "PlatformImp/Application/bcApplication.h"
#include "Game/bcExport.h"
#include "Game/Application/bcIRenderApplicationOutputWindow.h"

namespace black_cat
{
	namespace game
	{
		struct bc_engine_component_parameter
		{
			bc_engine_component_parameter(bcSIZE p_memmng_fsa_start_size,
				bcSIZE p_memmng_fsa_count,
				bcSIZE p_memmng_fsa_step_size,
				bcSIZE p_memmng_fsa_allocation_count,
				bcSIZE p_memmng_program_stack_size,
				bcSIZE p_memmng_level_stack_size,
				bcSIZE p_memmng_frame_stack_size,
				bcSIZE p_memmng_super_heap_size,
				bcSIZE p_thread_manager_thread_count,
				bcSIZE p_thread_manager_reserve_thread_count)
				: m_memmng_fsa_start_size(p_memmng_fsa_start_size),
				m_memmng_fsa_count(p_memmng_fsa_count),
				m_memmng_fsa_step_size(p_memmng_fsa_step_size),
				m_memmng_fsa_allocation_count(p_memmng_fsa_allocation_count),
				m_memmng_program_stack_size(p_memmng_program_stack_size),
				m_memmng_level_stack_size(p_memmng_level_stack_size),
				m_memmng_frame_stack_size(p_memmng_frame_stack_size),
				m_memmng_super_heap_size(p_memmng_super_heap_size),
				m_thread_manager_thread_count(p_thread_manager_thread_count),
				m_thread_manager_reserve_thread_count(p_thread_manager_reserve_thread_count)
			{
			}

			bcSIZE m_memmng_fsa_start_size;
			bcSIZE m_memmng_fsa_count;
			bcSIZE m_memmng_fsa_step_size;
			bcSIZE m_memmng_fsa_allocation_count;
			bcSIZE m_memmng_program_stack_size;
			bcSIZE m_memmng_level_stack_size;
			bcSIZE m_memmng_frame_stack_size;
			bcSIZE m_memmng_super_heap_size;
			bcSIZE m_thread_manager_thread_count;
			bcSIZE m_thread_manager_reserve_thread_count;
		};

		struct bc_render_application_parameter : public platform::bc_application_parameter
		{
			bc_render_application_parameter(platform::bc_application_parameter& p_app_parameters,
				bc_irender_application_output_window* p_output_window)
				: platform::bc_application_parameter(p_app_parameters),
				m_output_window(p_output_window)
			{
			}

			bc_irender_application_output_window* m_output_window;
		};

		struct bc_engine_application_parameter
		{
			bc_engine_application_parameter(bc_engine_component_parameter& p_engine_parameters,
				bc_render_application_parameter& p_app_parameters)
				: m_engine_parameters(p_engine_parameters),
				m_app_parameters(p_app_parameters)
			{
			}

			bc_engine_component_parameter m_engine_parameters;
			bc_render_application_parameter m_app_parameters;
		};

		class BC_GAME_DLL bc_render_application : public core::bc_initializable< bc_engine_application_parameter& >
		{
		public:
			bc_render_application();

			bc_render_application(bc_render_application&&) = default;

			virtual ~bc_render_application() = default;

			bc_render_application& operator=(bc_render_application&&) = default;

			bc_irender_application_output_window& get_output_window() const;

			core_platform::bc_clock& get_clock() const;

			bcUINT32 get_fps() const;

			/**
			 * \brief Specify an upper range for application fps or use a value lower than 0 for unlimited fps.
			 * \param p_fps 
			 */
			void set_fps(bcUINT32 p_fps);

			bcINT32 run();

			/**
			 * \brief Broadcast application exit event.
			 */
			void request_termination() const;

			/**
			* \brief Create a window that rendering can be done with it.
			* \param p_caption
			* \param p_width
			* \param p_height
			* \return
			*/
			platform::bc_basic_window create_basic_render_window(core::bc_estring p_caption, bcUINT32 p_width, bcUINT32 p_height);

			/**
			* \brief Create a console window. Support for creating console window along with render window is platform specific.
			* \param p_caption
			* \return
			*/
			platform::bc_console_window create_console_window(core::bc_estring p_caption);

			/**
			 * \brief Initialize required engine components
			 * \param p_engine_components 
			 */
			virtual void app_start_engine_components(bc_engine_component_parameter& p_engine_components) = 0;

			/**
			 * \brief Do required initialization for app
			 * \param p_commandline 
			 */
			virtual void app_initialize(const bcCHAR* p_commandline) = 0;

			/**
			 * \brief Load required contents
			 */
			virtual void app_load_content() = 0;

			/**
			 * \brief Update app
			 * \param p_clock_update_param 
			 */
			virtual void app_update(core_platform::bc_clock::update_param p_clock_update_param) = 0;

			/**
			 * \brief Render app
			 * \param p_clock_update_param 
			 */
			virtual void app_render(core_platform::bc_clock::update_param p_clock_update_param) = 0;

			/**
			 * \brief Handle app events
			 * \param p_event 
			 * \return 
			 */
			virtual bool app_event(core::bc_ievent& p_event);

			/**
			 * \brief Cleanup loaeded contents
			 */
			virtual void app_unload_content() = 0;

			/**
			 * \brief Do cleanup in app
			 */
			virtual void app_destroy() = 0;

			/**
			 * \brief Close all used engine components
			 */
			virtual void app_close_engine_components() = 0;

		protected:
			void _initialize(bc_engine_application_parameter& p_platform_parameters) override;

			void _destroy() override;

		private:
			void _calculate_fps(core_platform::bc_clock::small_delta_time p_delta);

			core::bc_unique_ptr< platform::bc_application > m_app;
			core::bc_unique_ptr< bc_render_application_basic_output_window > m_default_output_window;
			bc_irender_application_output_window* m_output_window;
			core::bc_unique_ptr< core_platform::bc_clock > m_clock;

			bool m_is_terminated;
			bool m_paused;
			bcINT32 m_termination_code;

			bcUINT32 m_min_update_rate;
			bcINT32 m_render_rate;

			static const bcUINT32 s_num_time_delta_samples = 64;
			core_platform::bc_clock::small_delta_time m_time_delta_buffer[s_num_time_delta_samples];
			bcUINT32 m_current_time_delta_sample;
			bcUINT32 m_fps;

			core::bc_event_listener_handle m_event_handle_window_resizing;
			core::bc_event_listener_handle m_event_handle_window_close;
			core::bc_event_listener_handle m_event_handle_app_active;
			core::bc_event_listener_handle m_event_handle_app_exit;
			core::bc_event_listener_handle m_event_error_handle;
			core::bc_event_listener_handle m_event_warning_handle;
		};
	}
}