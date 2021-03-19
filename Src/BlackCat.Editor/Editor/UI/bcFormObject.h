// [11/16/2018 MRB]

#pragma once

#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Editor/Application/bcUICommandService.h"
#include <QtWidgets/QLabel>
#include <QtWidgets/QAbstractButton>

namespace black_cat
{
	namespace editor
	{
		class bc_form_object : QObject
		{
			Q_OBJECT
		
		public:
			bc_form_object(QWidget& p_parent, bc_ui_command_service& p_ui_command_service);

			void setSelectedActor(game::bc_actor p_actor);

		private slots:
			void actorDeleteClick(bool);
		
		private:
			bc_ui_command_service* m_command_service;
			
			QLabel* m_entity_name_label;
			QAbstractButton* m_entity_delete_button;

			game::bc_actor m_selected_actor;
		};
	}
}