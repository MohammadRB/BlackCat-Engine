// [11/22/2018 MRB]

#pragma once

#include "Editor/EditorPCH.h"
#include "Editor/Application/bcRenderApplicationD3DWidgetOutputWindow.h"

namespace black_cat
{
	namespace editor
	{
		class bc_editor_render_app_thread : public QThread
		{
		public:
			void start(HINSTANCE p_instance, const char* p_cmd_line, game::bci_render_application_output_window* p_output_window);

			void wait_for_initialization() const;

			std::int32_t is_still_running() const noexcept;

			std::int32_t get_result_code() const noexcept;

		protected:
			void run() override;

		private:
			const std::int32_t s_default_result_code = std::numeric_limits<std::int32_t>::max();

			HINSTANCE m_instance;
			const char* m_cmd_line;
			game::bci_render_application_output_window* m_output_window;
			QAtomicInt m_initialized;
			QAtomicInt m_result_code;
		};
	}
}