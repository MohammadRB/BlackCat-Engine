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
			void start(HINSTANCE p_instance, const char* p_cmd_line, game::bc_irender_application_output_window* p_output_window);

			void wait_for_initialization() const;

		protected:
			void run() override;

		private:
			HINSTANCE m_instance;
			const char* m_cmd_line;
			game::bc_irender_application_output_window* m_output_window;
			QAtomicInt m_initialized;
			QAtomicInt m_result_code;
		};
	}
}