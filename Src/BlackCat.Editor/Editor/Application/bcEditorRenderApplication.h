// [11/13/2016 MRB]

#pragma once

#include "Core/Event/bcEvent.h"
#include "BlackCat/Application/bcBaseRenderApplication.h"

namespace black_cat
{
	namespace editor
	{
		class bc_editor_render_app : public bc_base_render_application< bc_editor_render_app >
		{
		public:
			bc_editor_render_app();

			~bc_editor_render_app();

			void application_start_engine_components(game::bc_engine_component_parameter& p_engine_components, core::bc_service_manager& p_service_manager) override;

			void application_initialize(const bcCHAR* p_commandline) override;

			void application_load_content(core::bc_content_stream_manager* p_stream_manager) override;

			void application_update(core_platform::bc_clock::update_param p_clock_update_param) override;

			void application_render(core_platform::bc_clock::update_param p_clock_update_param) override;

			bool application_event(core::bc_ievent& p_event) override;

			void application_unload_content(core::bc_content_stream_manager* p_stream_manager) override;

			void application_destroy() override;

			void application_close_engine_components() override;
		protected:

		private:
			core::bc_event_listener_handle m_shape_throw_key_handle;
		};
	}
}