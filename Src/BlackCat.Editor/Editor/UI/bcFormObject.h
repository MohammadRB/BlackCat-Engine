// [16/11/2018 MRB]

#pragma once

#include "Core/Math/bcMatrix4f.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Editor/Application/bcUICommandService.h"
#include <QtWidgets/QtWidgets>

namespace black_cat
{
	namespace editor
	{
		class bc_form_object : QObject
		{
			Q_OBJECT
		
		public:
			bc_form_object(QWidget& p_parent, bc_ui_command_service& p_ui_command_service);

			void setSelectedActor(game::bc_actor p_actor, const core::bc_matrix4f& p_transform);

		private slots:
			void actorUpdateClick(bool);
			
			void actorDeleteClick(bool);
		
		private:
			bc_ui_command_service* m_command_service;
			
			QLabel* m_entity_name_label;
			QAbstractButton* m_entity_update_button;
			QAbstractButton* m_entity_delete_button;
			QLineEdit* m_entity_position_x_text;
			QLineEdit* m_entity_position_y_text;
			QLineEdit* m_entity_position_z_text;
			QLineEdit* m_entity_rotation_x_text;
			QLineEdit* m_entity_rotation_y_text;
			QLineEdit* m_entity_rotation_z_text;

			game::bc_actor m_selected_actor;
		};
	}
}