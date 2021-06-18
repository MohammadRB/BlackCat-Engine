// [2/23/2015 MRB]

#pragma once

#include "Core/Utility/bcInitializable.h"
#include "Core/Messaging/Event/bcEventManager.h"
#include "PlatformImp/Application/bcApplication.h"
#include "Game/bcExport.h"
#include "Game/Application/bcIRenderApplicationOutputWindow.h"
#include "Game/Application/bcEngineApplicationParameter.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_render_application : public core::bc_initializable<bc_engine_application_parameter&>
		{
		public:
			bc_render_application();

			bc_render_application(bc_render_application&&) = delete;

			virtual ~bc_render_application() = default;

			bc_render_application& operator=(bc_render_application&&) = delete;

			const bci_render_application_output_window* get_output_window() const;

			const core_platform::bc_clock& get_clock() const;

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

		private:
			/**
			 * \brief Initialize required engine components
			 * \param p_parameters
			 */
			virtual void app_start_engine_components(bc_engine_application_parameter& p_parameters) = 0;

			/**
			 * \brief Do required initialization for app
			 * \param p_parameters
			 */
			virtual void app_initialize(bc_engine_application_parameter& p_parameters) = 0;

			/**
			 * \brief Load required contents
			 */
			virtual void app_load_content() = 0;

			/**
			 * \brief Update app
			 * \param p_clock
			 * \param p_is_partial_update
			 */
			virtual void app_update(const core_platform::bc_clock::update_param& p_clock, bool p_is_partial_update) = 0;

			/**
			 * \brief Render app
			 * \param p_clock
			 */
			virtual void app_render(const core_platform::bc_clock::update_param& p_clock) = 0;

			/**
			 * \brief Called when main thread is paused
			 * \param p_clock 
			 */
			virtual void app_pause_idle(const core_platform::bc_clock::update_param& p_clock) = 0;

			/**
			 * \brief Called when render thread is paused 
			 * \param p_clock 
			 */
			virtual void app_render_pause_idle(const core_platform::bc_clock::update_param& p_clock) = 0;
			
			/**
			 * \brief Called when main thread is idle before swap frame
			 */
			virtual void app_swap_frame_idle(const core_platform::bc_clock::update_param& p_clock) = 0;
			
			/**
			 * \brief Called when both update and render are executed and ready to start next frame
			 */
			virtual void app_swap_frame(const core_platform::bc_clock::update_param& p_clock) = 0;

			/**
			 * \brief Called when both update and render are executed and ready to start next frame
			 */
			virtual void app_render_swap_frame(const core_platform::bc_clock::update_param& p_clock) = 0;
			
			/**
			 * \brief Handle app events
			 * \param p_event
			 * \return
			 */
			virtual bool app_event(core::bci_event& p_event) = 0;

			/**
			 * \brief Cleanup loaded contents
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

			void _initialize(bc_engine_application_parameter& p_platform_parameters) override;

			void _destroy() override;

			bool _app_event(core::bci_event& p_event);

			core::bc_unique_ptr<platform::bc_application> m_app;
			bci_render_application_output_window* m_output_window;
			core::bc_unique_ptr<core_platform::bc_clock> m_clock;
			bcUINT32 m_min_update_rate;
			bcINT32 m_render_rate;

			bool m_is_terminated;
			bool m_paused;
			bcINT32 m_termination_code;

			core::bc_event_listener_handle m_event_handle_window_state;
			core::bc_event_listener_handle m_event_handle_window_resize;
			core::bc_event_listener_handle m_event_handle_window_focus;
			core::bc_event_listener_handle m_event_handle_window_close;
			core::bc_event_listener_handle m_event_handle_app_active;
			core::bc_event_listener_handle m_event_handle_app_exit;
			core::bc_event_listener_handle m_event_handle_error;
			core::bc_event_listener_handle m_event_handle_debug;
			core::bc_event_listener_handle m_event_handle_key;
			core::bc_event_listener_handle m_event_handle_pointing;
		};

		inline const bci_render_application_output_window* bc_render_application::get_output_window() const
		{
			return m_output_window;
		}

		inline const core_platform::bc_clock& bc_render_application::get_clock() const
		{
			return *m_clock;
		}
	}
}