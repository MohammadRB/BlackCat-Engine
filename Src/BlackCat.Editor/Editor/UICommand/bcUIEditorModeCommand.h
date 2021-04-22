// [04/22/2021 MRB]

#pragma once

#include "Editor/Application/bcUICommand.h"

namespace black_cat
{
	namespace editor
	{
		class bc_ui_editor_mode_command : public bc_iui_command
		{
		public:
			bc_ui_editor_mode_command(bool p_editor_mode) noexcept;

			bc_ui_editor_mode_command(const bc_ui_editor_mode_command&) noexcept;

			~bc_ui_editor_mode_command();

			bc_ui_editor_mode_command& operator=(const bc_ui_editor_mode_command&) noexcept;

			core::bc_string title() const override;
			
			bool update(update_context& p_context) override;
		
		private:
			bool m_editor_mode;
		};
	}
}