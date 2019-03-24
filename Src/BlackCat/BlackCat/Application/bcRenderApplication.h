// [03/23/2016 MRB]

#pragma once

#include "Core/bcEvent.h"
#include "Core/Utility/bcServiceManager.h"
#include "Game/Application/bcRenderApplication.h"
#include "Game/System/bcGameSystem.h"
#include "BlackCat/BlackCatPCH.h"

namespace black_cat
{
	class bc_render_application : public game::bc_render_application
	{
	public:
		using game::bc_render_application::initialize;
		using game::bc_render_application::destroy;

	public:
		bc_render_application();

		virtual ~bc_render_application();

	protected:
		game::bc_game_system* m_game_system;

	private:
		virtual void application_start_engine_components(game::bc_engine_application_parameter& p_parameters) = 0;

		virtual void application_initialize(game::bc_engine_application_parameter& p_parameters) = 0;

		virtual void application_load_content(core::bc_content_stream_manager* p_stream_manager) = 0;

		virtual void application_update(core_platform::bc_clock::update_param p_clock_update_param) = 0;

		virtual void application_render(core_platform::bc_clock::update_param p_clock_update_param) = 0;

		virtual bool application_event(core::bc_ievent& p_event) = 0;

		virtual void application_unload_content(core::bc_content_stream_manager* p_stream_manager) = 0;

		virtual void application_destroy() = 0;

		virtual void application_close_engine_components() = 0;

		void app_start_engine_components(game::bc_engine_application_parameter& p_parameters) override final;

		void app_initialize(game::bc_engine_application_parameter& p_parameters) override final;

		void app_load_content() override final;

		void app_update(core_platform::bc_clock::update_param p_clock_update_param) override final;

		void app_render(core_platform::bc_clock::update_param p_clock_update_param) override final;

		bool app_event(core::bc_ievent& p_event) override final;

		void app_unload_content() override final;

		void app_destroy() override final;

		void app_close_engine_components() override final;

		core::bc_service_manager* m_service_manager;
	};
}