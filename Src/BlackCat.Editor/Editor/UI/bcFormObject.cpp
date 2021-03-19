// [03/19/2021 MRB]

#include "Editor/EditorPCH.h"

#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Editor/UI/bcFormObject.h"
#include "Editor/UICommand/bcObjectDeleteCommand.h"

namespace black_cat
{
	namespace editor
	{
		bc_form_object::bc_form_object(QWidget& p_parent, bc_ui_command_service& p_ui_command_service)
		{
			m_command_service = &p_ui_command_service;
			m_entity_name_label = p_parent.findChild< QLabel* >("entityNameLabelValue");
			m_entity_delete_button = p_parent.findChild< QAbstractButton* >("entityDeleteButton");

			QObject::connect(m_entity_delete_button, SIGNAL(clicked(bool)), this, SLOT(actorDeleteClick(bool)));
		}

		void bc_form_object::setSelectedActor(game::bc_actor p_actor)
		{
			m_selected_actor = p_actor;

			if (m_selected_actor.is_valid())
			{
				auto* l_mediate_component = m_selected_actor.get_component< game::bc_mediate_component >();

				m_entity_name_label->setText(l_mediate_component->get_entity_name());
				m_entity_delete_button->setEnabled(true);
			}
			else
			{
				m_entity_name_label->setText(nullptr);
				m_entity_delete_button->setEnabled(false);
			}
		}

		void bc_form_object::actorDeleteClick(bool)
		{
			m_command_service->queue_command(bc_ui_object_delete_command(m_selected_actor));
		}
	}	
}