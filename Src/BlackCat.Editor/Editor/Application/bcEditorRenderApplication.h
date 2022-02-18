// [11/13/2016 MRB]

#pragma once

#include "Core/Messaging/Event/bcEvent.h"
#include "App/Application/bcRenderApplication.h"

namespace black_cat
{
	namespace editor
	{
		class bc_editor_render_app : public bc_render_application
		{
		public:
			bc_editor_render_app();

			bc_editor_render_app(bc_editor_render_app&&) = delete;

			~bc_editor_render_app() override;

			bc_editor_render_app& operator=(bc_editor_render_app&&) = delete;
		
		protected:
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
}