// [03/19/2021 MRB]

#pragma once

#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Editor/Application/bcUICommand.h"

namespace black_cat
{
	namespace editor
	{
		class bc_object_delete_ui_command : public bci_ui_command
		{
		public:
			bc_object_delete_ui_command(game::bc_actor p_actor) noexcept;

			bc_object_delete_ui_command(const bc_object_delete_ui_command&) noexcept;

			~bc_object_delete_ui_command() override;

			bc_object_delete_ui_command& operator=(const bc_object_delete_ui_command&) noexcept;

			core::bc_string title() const override;

			state_ptr create_state(state_context& p_context) const override;

			bool update(update_context& p_context) override;

			void update_ui(update_ui_context& p_context) override;

		private:
			game::bc_actor m_actor;
		};
	}
}