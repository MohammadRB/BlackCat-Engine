// [11/22/2018 MRB]

#pragma once

#include "Core/Utility/bcDelegate.h"
#include "Editor/Application/bcRenderApplicationD3DWidgetOutputWindow.h"
#include <QtCore/QThread>

namespace black_cat
{
	namespace editor
	{
		class bc_editor_render_app;
		using bc_editor_render_app_factory = core::bc_delegate<bc_editor_render_app*()>;

		class bc_editor_render_app_thread : public QThread
		{
		public:
			void start(HINSTANCE p_instance, 
				const char* p_cmd_line, 
				bc_editor_render_app_factory p_render_app_factory, 
				game::bci_render_application_output_window* p_output_window);

			void wait_for_initialization() const;

			std::int32_t is_still_running() const noexcept;

			std::int32_t get_result_code() const noexcept;

		protected:
			void run() override;

		private:
			const std::int32_t s_default_result_code = std::numeric_limits<std::int32_t>::max();

			HINSTANCE m_instance = nullptr;
			const char* m_cmd_line = nullptr;
			bc_editor_render_app_factory m_render_app_factory;
			game::bci_render_application_output_window* m_output_window = nullptr;
			QAtomicInt m_initialized;
			QAtomicInt m_result_code;
		};
	}
}