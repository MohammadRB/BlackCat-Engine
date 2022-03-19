// [12/10/2021 MRB]

#pragma once

#include "Editor/Application/bcUICommand.h"
#include <QtCore/QString>

namespace black_cat
{
	namespace editor
	{
		class bc_decal_paint_ui_command : public bci_ui_command
		{
		public:
			bc_decal_paint_ui_command(QString p_decal_name,
				bcUINT16 p_screen_width,
				bcUINT16 p_screen_height,
				bcUINT16 p_point_left,
				bcUINT16 p_point_top);

			bc_decal_paint_ui_command(const bc_decal_paint_ui_command&) noexcept;

			~bc_decal_paint_ui_command() override;

			bc_decal_paint_ui_command& operator=(bc_decal_paint_ui_command&) noexcept;

			core::bc_string title() const override;
			
			bool update(update_context& p_context) override;
		
		private:
			QString m_decal_name;
			bcUINT16 m_screen_width;
			bcUINT16 m_screen_height;
			bcUINT16 m_point_left;
			bcUINT16 m_point_top;
		};
	}	
}