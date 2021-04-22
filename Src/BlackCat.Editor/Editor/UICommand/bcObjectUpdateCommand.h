// [04/22/2021 MRB]

#pragma once

#include "Core/Math/bcMatrix4f.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Editor/Application/bcUICommand.h"

namespace black_cat
{
	namespace editor
	{
		class bc_ui_object_update_command : public bc_iui_command
		{
		public:
			bc_ui_object_update_command(game::bc_actor p_actor, const core::bc_matrix4f& p_transform) noexcept;

			bc_ui_object_update_command(const bc_ui_object_update_command&) noexcept;

			~bc_ui_object_update_command() override;

			bc_ui_object_update_command& operator=(const bc_ui_object_update_command&) noexcept;

			core::bc_string title() const override;

			state_ptr create_state(state_context& p_context) const override;

			bool update(update_context& p_context) override;

		private:
			game::bc_actor m_actor;
			core::bc_matrix4f m_transform;
		};
	}
}