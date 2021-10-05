// [03/23/2016 MRB]

#pragma once

#include "Core/bcEvent.h"
#include "Core/Utility/bcValueSampler.h"
#include "Core/Utility/bcStopWatch.h"
#include "Game/Application/bcRenderApplication.h"
#include "Game/System/bcGameSystem.h"
#include "BlackCat/bcExport.h"

namespace black_cat
{
	class BC_DLL bc_render_application : public game::bc_render_application
	{
	public:
		using game::bc_render_application::initialize;
		using game::bc_render_application::destroy;

	public:
		bc_render_application();

		bc_render_application(bc_render_application&&) = delete;

		~bc_render_application() override;

		bc_render_application& operator=(bc_render_application&&) = delete;

		bcFLOAT get_fps() const noexcept;
		
	protected:		
		game::bc_game_system* m_game_system;

	private:
		virtual void application_start_engine_components(game::bc_engine_application_parameter& p_parameters) = 0;

		virtual void application_initialize(game::bc_engine_application_parameter& p_parameters) = 0;

		virtual void application_load_content(core::bc_content_stream_manager& p_stream_manager) = 0;

		virtual void application_update(core_platform::bc_clock::update_param p_clock, bool p_is_partial_update) = 0;

		virtual void application_render(core_platform::bc_clock::update_param p_clock) = 0;

		virtual void application_pause_idle(const core_platform::bc_clock::update_param& p_clock);

		virtual void application_render_pause_idle(const core_platform::bc_clock::update_param& p_clock);

		virtual void application_swap_frame_idle(const core_platform::bc_clock::update_param& p_clock);

		virtual void application_swap_frame(const core_platform::bc_clock::update_param& p_clock);

		virtual void application_render_swap_frame(const core_platform::bc_clock::update_param& p_clock);
		
		virtual bool application_event(core::bci_event& p_event) = 0;

		virtual void application_unload_content(core::bc_content_stream_manager& p_stream_manager) = 0;

		virtual void application_destroy() = 0;

		virtual void application_close_engine_components() = 0;

		void app_start_engine_components(game::bc_engine_application_parameter& p_parameters) override final;

		void app_initialize(game::bc_engine_application_parameter& p_parameters) override final;

		void app_load_content() override final;

		void app_update(const core_platform::bc_clock::update_param& p_clock, bool p_is_partial_update) override final;

		void app_render(const core_platform::bc_clock::update_param& p_clock) override final;

		void app_pause_idle(const core_platform::bc_clock::update_param& p_clock) override final;

		void app_render_pause_idle(const core_platform::bc_clock::update_param& p_clock) override final;
		
		void app_swap_frame_idle(const core_platform::bc_clock::update_param& p_clock) override final;
		
		void app_swap_frame(const core_platform::bc_clock::update_param& p_clock) override final;

		void app_render_swap_frame(const core_platform::bc_clock::update_param& p_clock) override final;
		
		bool app_event(core::bci_event& p_event) override final;

		void app_unload_content() override final;

		void app_destroy() override final;

		void app_close_engine_components() override final;
		
		core::bc_value_sampler<core_platform::bc_clock::small_delta_time, 64> m_fps_sampler;
		core_platform::bc_clock::small_delta_time m_fps;

		core::bc_stop_watch m_update_watch;
		core::bc_stop_watch m_render_watch;
		core::bc_stop_watch m_swap_watch;
	};
}