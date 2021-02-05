// [11/08/2018 MRB]

#pragma once

#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Editor/Application/bcUICommand.h"

namespace black_cat
{
	namespace editor
	{
		class bc_ui_object_select_command : public bc_iui_command
		{
		public:
			bc_ui_object_select_command(bcUINT16 p_screen_width,
				bcUINT16 p_screen_height,
				bcUINT16 p_point_left,
				bcUINT16 p_point_top);

			bc_ui_object_select_command(const bc_ui_object_select_command&);

			virtual ~bc_ui_object_select_command();

			bc_ui_object_select_command& operator=(const bc_ui_object_select_command&);

			core::bc_string title() const override;

			state_ptr create_state(state_context& p_context) const override;

			bool update(update_context& p_context) override;

			void update_ui(update_ui_context& p_context) override;

		private:
			bcUINT16 m_screen_width;
			bcUINT16 m_screen_height;
			bcUINT16 m_point_left;
			bcUINT16 m_point_top;

			game::bc_actor_index m_selected_actor_index;
			const bcCHAR* m_selected_actor_entity_name;
		};
	}
}