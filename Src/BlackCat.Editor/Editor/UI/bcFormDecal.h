// [12/13/2021 MRB]

#pragma once

#include "Game/System/Render/Decal/bcDecalInstance.h"
#include "Editor/Application/bcUICommandService.h"
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QAbstractButton>

namespace black_cat
{
	namespace editor
	{
		class bc_form_decal : QObject
		{
			Q_OBJECT

		public:
			bc_form_decal(const QWidget& p_parent, bc_ui_command_service& p_ui_command_service);

			void setSelectedDecal(game::bc_decal_instance* p_decal);

		private slots:
			void decalUpdateClick(bool);

			void decalDeleteClick(bool);

		private:
			bc_ui_command_service* m_command_service;

			QLabel* m_decal_name_label;
			QLineEdit* m_decal_position_x_text;
			QLineEdit* m_decal_position_y_text;
			QLineEdit* m_decal_position_z_text;
			QLineEdit* m_decal_rotation_x_text;
			QLineEdit* m_decal_rotation_y_text;
			QLineEdit* m_decal_rotation_z_text;
			QAbstractButton* m_decal_update_button;
			QAbstractButton* m_decal_delete_button;

			game::bc_decal_instance* m_selected_decal;
		};
	}
}