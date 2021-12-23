// [12/13/2021 MRB]

#pragma once

#include "Game/System/Render/Decal/bcDecalInstance.h"
#include "Editor/Application/bcUICommand.h"

namespace black_cat
{
	namespace editor
	{
		class bc_decal_select_ui_command : public bci_ui_command
		{
		public:
			bc_decal_select_ui_command(bcUINT16 p_screen_width,
				bcUINT16 p_screen_height,
				bcUINT16 p_point_left,
				bcUINT16 p_point_top);

			bc_decal_select_ui_command(const bc_decal_select_ui_command&);

			~bc_decal_select_ui_command() override;

			bc_decal_select_ui_command& operator=(const bc_decal_select_ui_command&);

			core::bc_string title() const override;

			state_ptr create_state(state_context& p_context) const override;

			bool update(update_context& p_context) override;

			void update_ui(update_ui_context& p_context) override;

		private:
			bcUINT16 m_screen_width;
			bcUINT16 m_screen_height;
			bcUINT16 m_point_left;
			bcUINT16 m_point_top;

			game::bc_decal_instance* m_selected_decal;
		};
	}
}