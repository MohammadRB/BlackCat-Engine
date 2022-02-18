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
		void application_start_engine_components(const bc_application_start_context& p_context) override;

		void application_initialize(const bc_application_initialize_context& p_context) override;

		void application_load_content(const bc_application_load_context& p_context) override;

		void application_update(const bc_application_update_context& p_context) override;

		void application_render(const bc_application_render_context& p_context) override;

		void application_event(core::bci_event& p_event) override;

		void application_unload_content(const bc_application_load_context& p_context) override;

		void application_destroy() override;

		void application_close_engine_components() override;
	};
}