// [23/03/2016 MRB]

#pragma once

#include "Core/bcEvent.h"
#include "Core/Utility/bcStopWatch.h"
#include "Core/Messaging/Query/bcQueryManager.h"
#include "Game/Application/bcRenderApplication.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/bcGameSystem.h"
#include "App/bcExport.h"

namespace black_cat
{
	using bc_application_start_context = game::bc_engine_application_parameter;
	using bc_application_initialize_context = game::bc_engine_application_parameter;

	struct bc_application_load_context
	{
		core::bc_content_stream_manager& m_stream_manager;
	};

	struct bc_application_update_context
	{
		bool m_is_partial_update;
		const platform::bc_clock::update_param& m_clock;
		core::bc_query_manager& m_query_manager;
		game::bc_game_system& m_game_system;
	};

	struct bc_application_render_context
	{
		const platform::bc_clock::update_param& m_clock;
		core::bc_query_manager& m_query_manager;
		game::bc_render_system& m_render_system;
	};

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
		
	protected:
		core::bc_query_manager* m_query_manager;
		game::bc_game_system* m_game_system;

	private:
		virtual void application_start_engine_components(const bc_application_start_context& p_context) = 0;

		virtual void application_initialize(const bc_application_initialize_context& p_context) = 0;

		virtual void application_load_content(const bc_application_load_context& p_context) = 0;

		virtual void application_update(const bc_application_update_context& p_context) = 0;

		virtual void application_render(const bc_application_render_context& p_context) = 0;

		virtual void application_pause_idle(const platform::bc_clock::update_param& p_clock);

		virtual void application_render_pause_idle(const platform::bc_clock::update_param& p_clock);

		virtual void application_swap_frame_idle(const platform::bc_clock::update_param& p_clock);

		virtual void application_swap_frame(const platform::bc_clock::update_param& p_clock);

		virtual void application_render_swap_frame(const platform::bc_clock::update_param& p_clock);
		
		virtual void application_event(core::bci_event& p_event) = 0;

		virtual void application_unload_content(const bc_application_load_context& p_context) = 0;

		virtual void application_destroy() = 0;

		virtual void application_close_engine_components() = 0;

		void app_start_engine_components(const game::bc_engine_application_parameter& p_parameters) override final;

		void app_initialize(const game::bc_engine_application_parameter& p_parameters) override final;

		void app_load_content() override final;

		void app_update(const platform::bc_clock::update_param& p_clock, bool p_is_partial_update) override final;

		void app_render(const platform::bc_clock::update_param& p_clock) override final;

		void app_pause_idle(const platform::bc_clock::update_param& p_clock) override final;

		void app_render_pause_idle(const platform::bc_clock::update_param& p_clock) override final;
		
		void app_swap_frame_idle(const platform::bc_clock::update_param& p_clock) override final;
		
		void app_swap_frame(const platform::bc_clock::update_param& p_clock) override final;

		void app_render_swap_frame(const platform::bc_clock::update_param& p_clock) override final;
		
		void app_event(core::bci_event& p_event) override final;

		void app_unload_content() override final;

		void app_destroy() override final;

		void app_close_engine_components() override final;
		
		core::bc_stop_watch m_update_watch;
		core::bc_stop_watch m_render_watch;
		core::bc_stop_watch m_swap_watch;
	};
}