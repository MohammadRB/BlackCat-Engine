// [2/23/2015 MRB]

#pragma once

#include "Core/Messaging/Event/bcEventManager.h"
#include "Core/Utility/bcInitializable.h"
#include "Core/Utility/bcValueSampler.h"
#include "Core/Utility/bcStopWatch.h"
#include "PlatformImp/Application/bcApplication.h"
#include "Game/Application/bcIRenderApplicationOutputWindow.h"
#include "Game/Application/bcEngineApplicationParameter.h"
#include "Game/bcEvent.h"
#include "Game/bcExport.h"

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

			const bcECHAR* get_app_name() const noexcept;
			
			const bci_render_application_output_window* get_output_window() const noexcept;

			bcFLOAT get_fps() const noexcept;

			bcFLOAT get_ft() const noexcept;
			
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
			platform::bc_basic_window create_basic_render_window(const bcECHAR* p_caption, bcUINT32 p_width, bcUINT32 p_height);

			/**
			* \brief Create a console window. Support for creating console window along with render window is platform specific.
			* \param p_caption
			* \return
			*/
			platform::bc_console_window create_console_window(const bcECHAR* p_caption);

		private:
			/**
			 * \brief Initialize required engine components
			 * \param p_parameters
			 */
			virtual void app_start_engine_components(const bc_engine_application_parameter& p_parameters) = 0;

			/**
			 * \brief Do required initialization for app
			 * \param p_parameters
			 */
			virtual void app_initialize(const bc_engine_application_parameter& p_parameters) = 0;

			/**
			 * \brief Load required contents
			 */
			virtual void app_load_content() = 0;

			/**
			 * \brief Update app
			 * \param p_clock
			 * \param p_is_partial_update
			 */
			virtual void app_update(const platform::bc_clock::update_param& p_clock, bool p_is_partial_update) = 0;

			/**
			 * \brief Render app
			 * \param p_clock
			 */
			virtual void app_render(const platform::bc_clock::update_param& p_clock) = 0;

			/**
			 * \brief Called when main thread is paused
			 * \param p_clock 
			 */
			virtual void app_pause_idle(const platform::bc_clock::update_param& p_clock) = 0;

			/**
			 * \brief Called when render thread is paused 
			 * \param p_clock 
			 */
			virtual void app_render_pause_idle(const platform::bc_clock::update_param& p_clock) = 0;
			
			/**
			 * \brief Called when main thread is idle before swap frame
			 */
			virtual void app_swap_frame_idle(const platform::bc_clock::update_param& p_clock) = 0;
			
			/**
			 * \brief Called when both update and render are executed and ready to start next frame
			 */
			virtual void app_swap_frame(const platform::bc_clock::update_param& p_clock) = 0;

			/**
			 * \brief Called when both update and render are executed and ready to start next frame
			 */
			virtual void app_render_swap_frame(const platform::bc_clock::update_param& p_clock) = 0;
			
			/**
			 * \brief Handle app events
			 * \param p_event
			 * \return
			 */
			virtual void app_event(core::bci_event& p_event) = 0;

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

			void _initialize(bc_engine_application_parameter& p_parameters) override;

			void _destroy() override;

			void _app_event(core::bci_event& p_event);

			const bcECHAR* m_app_name;
			core::bc_unique_ptr<platform::bc_application> m_app;
			bci_render_application_output_window* m_output_window;
			core::bc_unique_ptr<platform::bc_clock> m_clock;
			core::bc_stop_watch m_frame_watch;
			core::bc_value_sampler<platform::bc_clock::small_delta_time, 64> m_fps_sampler;
			platform::bc_clock::small_delta_time m_fps;
			bcUINT32 m_min_update_rate;
			bcINT32 m_render_rate;

			bool m_is_terminated;
			bool m_paused;
			bcINT32 m_termination_code;

			core::bc_event_listener_handle m_event_handle_window_state;
			core::bc_event_listener_handle m_event_handle_window_resize;
			core::bc_event_listener_handle m_event_handle_window_focus;
			core::bc_event_listener_handle m_event_handle_window_close;
			core::bc_event_listener_handle m_event_handle_app_exit;
			core::bc_event_listener_handle m_event_handle_app_pause;
			core::bc_event_listener_handle m_event_handle_error;
			core::bc_event_listener_handle m_event_handle_key;
			core::bc_event_listener_handle m_event_handle_pointing;
		};
	}

	extern BC_GAME_DLL game::bc_render_application* g_application;

	inline game::bc_render_application& bc_get_application()
	{
		return *g_application;
	}
}