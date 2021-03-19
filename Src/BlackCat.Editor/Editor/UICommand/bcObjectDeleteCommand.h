// [03/19/2021 MRB]

#pragma once

#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Editor/Application/bcUICommand.h"

namespace black_cat
{
	namespace editor
	{
		class bc_ui_object_delete_command : public bc_iui_command
		{
		public:
			bc_ui_object_delete_command(game::bc_actor p_selected_actor);

			bc_ui_object_delete_command(const bc_ui_object_delete_command&);

			virtual ~bc_ui_object_delete_command();

			bc_ui_object_delete_command& operator=(const bc_ui_object_delete_command&);

			core::bc_string title() const override;

			state_ptr create_state(state_context& p_context) const override;

			bool update(update_context& p_context) override;

			void update_ui(update_ui_context& p_context) override;

		private:
			game::bc_actor m_selected_actor;
		};
	}
}