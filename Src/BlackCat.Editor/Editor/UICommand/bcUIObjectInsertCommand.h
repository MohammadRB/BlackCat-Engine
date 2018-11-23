// [11/22/2018 MRB]

#pragma once

#include "Core/Container/bcString.h"
#include "Editor/Application/bcUICommand.h"

namespace black_cat
{
	namespace editor
	{
		class bc_ui_object_insert_command : public bc_iui_command
		{
		public:
			bc_ui_object_insert_command(QString p_entity_name,
				bcUINT16 p_screen_width,
				bcUINT16 p_screen_height,
				bcUINT16 p_point_left,
				bcUINT16 p_point_top);

			bc_ui_object_insert_command(const bc_ui_object_insert_command&);

			~bc_ui_object_insert_command();

			bc_ui_object_insert_command& operator=(const bc_ui_object_insert_command&);

			core::bc_string title() const override;

			state_ptr create_state(state_context& p_context) const override;

			bool update(update_context& p_context) override;
		protected:
			
		private:
			QString m_entity_name;
			bcUINT16 m_screen_width;
			bcUINT16 m_screen_height;
			bcUINT16 m_point_left;
			bcUINT16 m_point_top;
		};
	}
}