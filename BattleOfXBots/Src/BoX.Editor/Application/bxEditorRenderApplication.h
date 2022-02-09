// [01/29/2022 MRB]

#pragma once

#include "Core/Messaging/Event/bcEvent.h"
#include "Editor/Application/bcEditorRenderApplication.h"

namespace box
{
	using namespace black_cat;

	class bx_editor_render_app : public editor::bc_editor_render_app
	{
	public:
		bx_editor_render_app();

		bx_editor_render_app(bx_editor_render_app&&) = delete;

		~bx_editor_render_app() override;

		bx_editor_render_app& operator=(bx_editor_render_app&&) = delete;

	private:
		void application_start_engine_components(game::bc_engine_application_parameter& p_parameters) override;

		void application_initialize(game::bc_engine_application_parameter& p_parameters) override;

		void application_load_content(core::bc_content_stream_manager& p_stream_manager) override;

		void application_update(const core_platform::bc_clock::update_param& p_clock_update_param, bool p_is_partial_update) override;

		void application_render(const core_platform::bc_clock::update_param& p_clock_update_param) override;

		bool application_event(core::bci_event& p_event) override;

		void application_unload_content(core::bc_content_stream_manager& p_stream_manager) override;

		void application_destroy() override;

		void application_close_engine_components() override;
	};
}