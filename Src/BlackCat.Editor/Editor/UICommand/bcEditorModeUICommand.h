// [22/04/2021 MRB]

#pragma once

#include "Editor/Application/bcUICommand.h"

namespace black_cat
{
	namespace editor
	{
		class bc_editor_mode_ui_command : public bci_ui_command
		{
		public:
			bc_editor_mode_ui_command(bool p_editor_mode) noexcept;

			bc_editor_mode_ui_command(const bc_editor_mode_ui_command&) noexcept;

			~bc_editor_mode_ui_command() override;

			bc_editor_mode_ui_command& operator=(const bc_editor_mode_ui_command&) noexcept;

			core::bc_string title() const override;
			
			bool update(update_context& p_context) override;
		
		private:
			bool m_editor_mode;
		};
	}
}