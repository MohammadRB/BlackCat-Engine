// [11/13/2016 MRB]

#pragma once

#include "Core/Messaging/Event/bcEvent.h"
#include "BlackCat/Application/bcRenderApplication.h"

namespace black_cat
{
	namespace editor
	{
		class bc_editor_render_app : public bc_render_application
		{
		public:
			bc_editor_render_app();

			~bc_editor_render_app();
		protected:

		private:
			void application_start_engine_components(game::bc_engine_application_parameter& p_parameters) override;

			void application_initialize(game::bc_engine_application_parameter& p_parameters) override;

			void application_load_content(core::bc_content_stream_manager* p_stream_manager) override;

			void application_update(core_platform::bc_clock::update_param p_clock_update_param, bool p_is_partial_update) override;

			void application_render(core_platform::bc_clock::update_param p_clock_update_param) override;

			bool application_event(core::bc_ievent& p_event) override;

			void application_unload_content(core::bc_content_stream_manager* p_stream_manager) override;

			void application_destroy() override;

			void application_close_engine_components() override;

			bcSIZE m_shape_throw_counter = 0;
		};
	}
}