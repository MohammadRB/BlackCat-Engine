// [12/15/2021 MRB]

#pragma once

#include "Core/Math/bcMatrix3f.h"
#include "Game/System/Render/Decal/bcDecalInstance.h"
#include "Editor/Application/bcUICommand.h"

namespace black_cat
{
	namespace editor
	{
		class bc_decal_update_ui_command : public bci_ui_command
		{
		public:
			bc_decal_update_ui_command(game::bc_decal_instance& p_decal, const core::bc_matrix3f& p_rotation);

			bc_decal_update_ui_command(const bc_decal_update_ui_command&) noexcept;

			~bc_decal_update_ui_command() override;

			bc_decal_update_ui_command& operator=(bc_decal_update_ui_command&) noexcept;

			core::bc_string title() const override;

			bool update(update_context& p_context) override;

		private:
			game::bc_decal_instance* m_decal;
			core::bc_matrix3f m_rotation;
		};
	}
}