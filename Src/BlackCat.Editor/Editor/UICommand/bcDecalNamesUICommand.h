// [12/10/2021 MRB]

#pragma once

#include "Editor/Application/bcUICommand.h"
#include "QtCore/QVector.h"
#include "QtCore/QString.h"

namespace black_cat
{
	namespace editor
	{
		class bc_decal_names_ui_command : public bci_ui_command
		{
		public:
			bc_decal_names_ui_command();

			bc_decal_names_ui_command(const bc_decal_names_ui_command&);

			~bc_decal_names_ui_command();

			bc_decal_names_ui_command& operator=(const bc_decal_names_ui_command&);

			core::bc_string title() const override;

			state_ptr create_state(state_context& p_context) const override;

			bool update(update_context& p_context) override;

			void update_ui(update_ui_context& p_context) override;

		private:
			QVector<QString> m_entity_list;
		};
	}
}