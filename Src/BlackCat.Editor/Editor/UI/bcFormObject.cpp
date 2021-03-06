// [03/19/2021 MRB]

#include "Editor/EditorPCH.h"

#include "Core/Math/bcVector3f.h"
#include "Core/Math/bcMatrix3f.h"
#include "Core/Math/bcMatrix4f.h"
#include "Core/Math/bcCoordinate.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/bcUtility.h"
#include "Editor/UI/bcFormObject.h"
#include "Editor/UICommand/bcObjectUpdateCommand.h"
#include "Editor/UICommand/bcObjectDeleteCommand.h"

namespace black_cat
{
	namespace editor
	{
		bc_form_object::bc_form_object(QWidget& p_parent, bc_ui_command_service& p_ui_command_service)
		{
			m_command_service = &p_ui_command_service;
			m_entity_name_label = p_parent.findChild<QLabel*>("entityNameLabelValue");
			m_entity_update_button = p_parent.findChild<QAbstractButton*>("entityUpdateButton");
			m_entity_delete_button = p_parent.findChild<QAbstractButton*>("entityDeleteButton");
			m_entity_position_x_text = p_parent.findChild<QLineEdit*>("entityPositionXText");
			m_entity_position_y_text = p_parent.findChild<QLineEdit*>("entityPositionYText");
			m_entity_position_z_text = p_parent.findChild<QLineEdit*>("entityPositionZText");
			m_entity_up_x_text = p_parent.findChild<QLineEdit*>("entityUpXText");
			m_entity_up_y_text = p_parent.findChild<QLineEdit*>("entityUpYText");
			m_entity_up_z_text = p_parent.findChild<QLineEdit*>("entityUpZText");
			m_entity_up_r_text = p_parent.findChild<QLineEdit*>("entityUpRotationText");

			QDoubleValidator* l_double_validator = new QDoubleValidator(this);
			m_entity_position_x_text->setValidator(l_double_validator);
			m_entity_position_y_text->setValidator(l_double_validator);
			m_entity_position_z_text->setValidator(l_double_validator);
			m_entity_up_x_text->setValidator(l_double_validator);
			m_entity_up_y_text->setValidator(l_double_validator);
			m_entity_up_z_text->setValidator(l_double_validator);
			m_entity_up_r_text->setValidator(l_double_validator);
			
			QObject::connect(m_entity_update_button, SIGNAL(clicked(bool)), this, SLOT(actorUpdateClick(bool)));
			QObject::connect(m_entity_delete_button, SIGNAL(clicked(bool)), this, SLOT(actorDeleteClick(bool)));
		}

		void bc_form_object::setSelectedActor(game::bc_actor p_actor, const core::bc_matrix4f& p_transform)
		{
			m_selected_actor = p_actor;

			if (m_selected_actor.is_valid())
			{
				auto* l_mediate_component = m_selected_actor.get_component<game::bc_mediate_component>();
				const auto l_position = p_transform.get_translation();
				const auto l_euler_rotation = bc_matrix4f_decompose_to_euler_rotation(p_transform);

				m_entity_name_label->setText(l_mediate_component->get_entity_name());
				m_entity_update_button->setEnabled(true);
				m_entity_delete_button->setEnabled(true);
				m_entity_position_x_text->setText(QString::number(l_position.x));
				m_entity_position_x_text->setEnabled(true);
				m_entity_position_y_text->setText(QString::number(l_position.y));
				m_entity_position_y_text->setEnabled(true);
				m_entity_position_z_text->setText(QString::number(l_position.z));
				m_entity_position_z_text->setEnabled(true);
				m_entity_up_x_text->setText(QString::number(l_euler_rotation.x));
				m_entity_up_x_text->setEnabled(true);
				m_entity_up_y_text->setText(QString::number(l_euler_rotation.y));
				m_entity_up_y_text->setEnabled(true);
				m_entity_up_z_text->setText(QString::number(l_euler_rotation.z));
				m_entity_up_z_text->setEnabled(true);
				m_entity_up_r_text->setText(QString::number(l_euler_rotation.w));
				m_entity_up_r_text->setEnabled(true);
			}
			else
			{
				m_entity_name_label->setText(nullptr);
				m_entity_update_button->setEnabled(false);
				m_entity_delete_button->setEnabled(false);
				m_entity_position_x_text->setText(nullptr);
				m_entity_position_x_text->setEnabled(false);
				m_entity_position_y_text->setText(nullptr);
				m_entity_position_y_text->setEnabled(false);
				m_entity_position_z_text->setText(nullptr);
				m_entity_position_z_text->setEnabled(false);
				m_entity_up_x_text->setText(nullptr);
				m_entity_up_x_text->setEnabled(false);
				m_entity_up_y_text->setText(nullptr);
				m_entity_up_y_text->setEnabled(false);
				m_entity_up_z_text->setText(nullptr);
				m_entity_up_z_text->setEnabled(false);
				m_entity_up_r_text->setText(nullptr);
				m_entity_up_r_text->setEnabled(false);
			}
		}

		void bc_form_object::actorUpdateClick(bool)
		{
			const core::bc_vector3f l_position
			(
				m_entity_position_x_text->text().toFloat(),
				m_entity_position_y_text->text().toFloat(),
				m_entity_position_z_text->text().toFloat()
			);
			const core::bc_vector3f l_up = core::bc_vector3f::normalize
			({
				m_entity_up_x_text->text().toFloat(),
				m_entity_up_y_text->text().toFloat(),
				m_entity_up_z_text->text().toFloat()
			});
			const float l_up_rotation = m_entity_up_r_text->text().toFloat();

			const core::bc_matrix3f l_rotation = bc_matrix3f_rotation_euler(l_up, core::bc_to_radian(l_up_rotation));
			
			auto l_transform = core::bc_matrix4f::identity();
			l_transform.set_translation(l_position);
			l_transform.set_rotation(l_rotation);
			
			m_command_service->queue_command(bc_ui_object_update_command(m_selected_actor, l_transform));
		}

		void bc_form_object::actorDeleteClick(bool)
		{
			m_command_service->queue_command(bc_ui_object_delete_command(m_selected_actor));
		}
	}	
}