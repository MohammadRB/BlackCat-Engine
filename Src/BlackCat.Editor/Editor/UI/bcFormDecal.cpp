// [12/13/2021 MRB]

#include "Editor/EditorPCH.h"

#include "Game/bcUtility.h"
#include "Editor/UI/bcFormDecal.h"
#include "Editor/UICommand/bcDecalRemoveUICommand.h"

namespace black_cat
{
	namespace editor
	{
		bc_form_decal::bc_form_decal(QWidget& p_parent, bc_ui_command_service& p_ui_command_service)
		{
			m_command_service = &p_ui_command_service;
			m_decal_name_label = p_parent.findChild<QLabel*>("decalNameLabelValue");
			m_decal_rotation_text = p_parent.findChild<QLineEdit*>("decalRotationText");
			m_decal_update_button = p_parent.findChild<QAbstractButton*>("decalUpdateButton");
			m_decal_delete_button = p_parent.findChild<QAbstractButton*>("decalDeleteButton");

			auto* l_double_validator = new QDoubleValidator(this);
			m_decal_rotation_text->setValidator(l_double_validator);

			QObject::connect(m_decal_update_button, SIGNAL(clicked(bool)), this, SLOT(decalUpdateClick(bool)));
			QObject::connect(m_decal_delete_button, SIGNAL(clicked(bool)), this, SLOT(decalDeleteClick(bool)));
		}

		void bc_form_decal::setSelectedDecal(game::bc_decal_instance* p_decal)
		{
			m_selected_decal = p_decal;

			if (m_selected_decal)
			{
				const auto l_axis_angle_rotation = bc_matrix3f_decompose_to_axis_angle(p_decal->get_local_rotation());
				const auto l_rotation = core::bc_to_degree(l_axis_angle_rotation.w);

				m_decal_name_label->setText(m_selected_decal->get_decal()->get_name().data());
				m_decal_rotation_text->setText(QString::number(l_rotation));
				m_decal_rotation_text->setEnabled(true);
				m_decal_update_button->setEnabled(true);
				m_decal_delete_button->setEnabled(true);
			}
			else
			{
				m_decal_name_label->setText(nullptr);
				m_decal_rotation_text->setText(nullptr);
				m_decal_rotation_text->setEnabled(false);
				m_decal_update_button->setEnabled(false);
				m_decal_delete_button->setEnabled(false);
			}
		}

		void bc_form_decal::decalUpdateClick(bool)
		{
			/*const core::bc_vector3f l_position
			(
				m_entity_position_x_text->text().toFloat(),
				m_entity_position_y_text->text().toFloat(),
				m_entity_position_z_text->text().toFloat()
			);
			const core::bc_vector3f l_rotation
			(
				core::bc_to_radian(m_entity_rotation_x_text->text().toFloat()),
				core::bc_to_radian(m_entity_rotation_y_text->text().toFloat()),
				core::bc_to_radian(m_entity_rotation_z_text->text().toFloat())
			);

			const core::bc_matrix3f l_rotation_mat = bc_matrix3f_rotation_zyx(l_rotation);

			auto l_transform = core::bc_matrix4f::identity();
			l_transform.set_translation(l_position);
			l_transform.set_rotation(l_rotation_mat);

			m_command_service->queue_command(bc_object_update_ui_command(m_selected_actor, l_transform));*/
		}

		void bc_form_decal::decalDeleteClick(bool)
		{
			m_command_service->queue_command(bc_decal_remove_ui_command(*m_selected_decal));
		}
	}
}