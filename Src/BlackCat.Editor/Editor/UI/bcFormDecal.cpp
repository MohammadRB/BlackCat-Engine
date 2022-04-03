// [12/13/2021 MRB]

#include "Editor/EditorPCH.h"

#include "Game/bcUtility.h"
#include "Editor/UI/bcFormDecal.h"
#include "Editor/UICommand/bcDecalUpdateUICommand.h"
#include "Editor/UICommand/bcDecalRemoveUICommand.h"
#include <QtWidgets/QtWidgets>

namespace black_cat
{
	namespace editor
	{
		bc_form_decal::bc_form_decal(const QWidget& p_parent, bc_ui_command_service& p_ui_command_service)
			: m_command_service(&p_ui_command_service),
			  m_selected_decal(nullptr)
		{
			m_decal_name_label = p_parent.findChild<QLabel*>("decalNameLabelValue");
			m_decal_position_x_text = p_parent.findChild<QLineEdit*>("decalPositionXText");
			m_decal_position_y_text = p_parent.findChild<QLineEdit*>("decalPositionYText");
			m_decal_position_z_text = p_parent.findChild<QLineEdit*>("decalPositionZText");
			m_decal_rotation_x_text = p_parent.findChild<QLineEdit*>("decalRotationXText");
			m_decal_rotation_y_text = p_parent.findChild<QLineEdit*>("decalRotationYText");
			m_decal_rotation_z_text = p_parent.findChild<QLineEdit*>("decalRotationZText");
			m_decal_update_button = p_parent.findChild<QAbstractButton*>("decalUpdateButton");
			m_decal_delete_button = p_parent.findChild<QAbstractButton*>("decalDeleteButton");

			const auto* l_double_validator = new QDoubleValidator(this);
			m_decal_rotation_x_text->setValidator(l_double_validator);
			m_decal_rotation_y_text->setValidator(l_double_validator);
			m_decal_rotation_z_text->setValidator(l_double_validator);

			QObject::connect(m_decal_update_button, SIGNAL(clicked(bool)), this, SLOT(decalUpdateClick(bool)));
			QObject::connect(m_decal_delete_button, SIGNAL(clicked(bool)), this, SLOT(decalDeleteClick(bool)));
		}

		void bc_form_decal::setSelectedDecal(game::bc_decal_instance* p_decal)
		{
			m_selected_decal = p_decal;

			if (m_selected_decal)
			{
				const auto& l_position = p_decal->get_local_position();
				const auto l_euler_rotation = bc_matrix3f_decompose_to_angles_zyx(p_decal->get_local_rotation());
				
				m_decal_name_label->setText(m_selected_decal->get_decal()->get_name().data());
				m_decal_position_x_text->setText(QString::number(l_position.x));
				m_decal_position_x_text->setEnabled(true);
				m_decal_position_y_text->setText(QString::number(l_position.y));
				m_decal_position_y_text->setEnabled(true);
				m_decal_position_z_text->setText(QString::number(l_position.z));
				m_decal_position_z_text->setEnabled(true);
				m_decal_rotation_x_text->setText(QString::number(core::bc_to_degree(l_euler_rotation.x)));
				m_decal_rotation_x_text->setEnabled(true);
				m_decal_rotation_y_text->setText(QString::number(core::bc_to_degree(l_euler_rotation.y)));
				m_decal_rotation_y_text->setEnabled(true);
				m_decal_rotation_z_text->setText(QString::number(core::bc_to_degree(l_euler_rotation.z)));
				m_decal_rotation_z_text->setEnabled(true);
				m_decal_update_button->setEnabled(true);
				m_decal_delete_button->setEnabled(true);
			}
			else
			{
				m_decal_name_label->setText(nullptr);
				m_decal_position_x_text->setText(nullptr);
				m_decal_position_x_text->setEnabled(false);
				m_decal_position_y_text->setText(nullptr);
				m_decal_position_y_text->setEnabled(false);
				m_decal_position_z_text->setText(nullptr);
				m_decal_position_z_text->setEnabled(false);
				m_decal_rotation_x_text->setText(nullptr);
				m_decal_rotation_x_text->setEnabled(false);
				m_decal_rotation_y_text->setText(nullptr);
				m_decal_rotation_y_text->setEnabled(false);
				m_decal_rotation_z_text->setText(nullptr);
				m_decal_rotation_z_text->setEnabled(false);
				m_decal_update_button->setEnabled(false);
				m_decal_delete_button->setEnabled(false);
			}
		}

		void bc_form_decal::decalUpdateClick(bool)
		{
			const auto l_position = core::bc_vector3f
			(
				m_decal_position_x_text->text().toFloat(),
				m_decal_position_y_text->text().toFloat(),
				m_decal_position_z_text->text().toFloat()
			);
			const auto l_rotation = bc_matrix3f_rotation_zyx
			(
				core::bc_vector3f
				(
					core::bc_to_radian(m_decal_rotation_x_text->text().toFloat()),
					core::bc_to_radian(m_decal_rotation_y_text->text().toFloat()),
					core::bc_to_radian(m_decal_rotation_z_text->text().toFloat())
				)
			);
			m_command_service->queue_command(bc_decal_update_ui_command(*m_selected_decal, l_position, l_rotation));
		}

		void bc_form_decal::decalDeleteClick(bool)
		{
			m_command_service->queue_command(bc_decal_remove_ui_command(*m_selected_decal));
		}
	}
}