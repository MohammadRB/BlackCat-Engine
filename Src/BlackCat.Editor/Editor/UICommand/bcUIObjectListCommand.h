// [11/22/2018 MRB]

#pragma once

#include "Editor/Application/bcUICommand.h"
#include "QtCore/QVector.h"
#include "QtCore/QString.h"

namespace black_cat
{
	namespace editor
	{
		class bc_ui_object_list_command : public bc_iui_command
		{
		public:
			bc_ui_object_list_command();

			bc_ui_object_list_command(const bc_ui_object_list_command&);

			~bc_ui_object_list_command();

			bc_ui_object_list_command& operator=(const bc_ui_object_list_command&);

			core::bc_string title() const override;

			state_ptr create_state(state_context& p_context) const override;

			bool update(update_context& p_context) override;
			
			void update_ui(update_ui_context& p_context) override;

		private:
			QVector<QString> m_entity_list;
		};
	}
}