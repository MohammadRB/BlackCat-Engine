// [02/07/2019 MRB]

#pragma once

#include "Core/File/bcPath.h"
#include "Game/Object/Scene/bcScene.h"
#include "Editor/Application/bcUICommandService.h"
#include "Editor/UICommand/bcSceneUICommand.h"
#include <QtWidgets/QWidget>
#include <QtWidgets/QMenu>
#include <QtWidgets/QAction>
#include <QtWidgets/QFileDialog>

namespace black_cat
{
	namespace editor
	{
		class bc_form_main_menu : public QObject
		{
			Q_OBJECT;

		public:
			bc_form_main_menu(QWidget& p_menu_bar, bc_ui_command_service& p_command_service);

		private slots:
			void onNewSceneClicked() const;
			
			void onLoadSceneClicked() const;

			void onSaveSceneClicked() const;

		private:
			QString _get_executing_path() const;

			bc_ui_command_service& m_command_service;
		};

		inline bc_form_main_menu::bc_form_main_menu(QWidget& p_menu_bar, bc_ui_command_service& p_command_service)
			: QObject(),
			m_command_service(p_command_service)
		{
			QMenu* l_file_menu = p_menu_bar.findChild< QMenu* >("fileMenu");
			
			for (QAction* l_action : l_file_menu->actions())
			{
				if(l_action->objectName() == "newSceneMenu")
				{
					QObject::connect(l_action, SIGNAL(triggered(bool)), this, SLOT(onNewSceneClicked()));
				}
				else if (l_action->objectName() == "loadSceneMenu")
				{
					QObject::connect(l_action, SIGNAL(triggered(bool)), this, SLOT(onLoadSceneClicked()));
				}
				else if (l_action->objectName() == "saveSceneMenu")
				{
					QObject::connect(l_action, SIGNAL(triggered(bool)), this, SLOT(onSaveSceneClicked()));
				}
			}
		}

		inline void bc_form_main_menu::onNewSceneClicked() const
		{
			bc_scene_ui_command l_command = bc_scene_ui_command::for_new_scene();
			m_command_service.queue_command(std::move(l_command));
		}

		inline void bc_form_main_menu::onLoadSceneClicked() const
		{
			const auto l_executing_path_qt = _get_executing_path();
			const auto l_file_name = QFileDialog::getOpenFileName
			(
				static_cast<QWidget*>(QObject::parent()), 
				"Open Scene File", 
				l_executing_path_qt, 
				"JSON File (*.json)"
			);

			if(l_file_name.isEmpty())
			{
				return;
			}

			bc_scene_ui_command l_command = bc_scene_ui_command::for_load_scene(l_file_name.toStdWString().c_str());
			m_command_service.queue_command(std::move(l_command));
		}

		inline void bc_form_main_menu::onSaveSceneClicked() const
		{
			auto l_task = m_command_service.queue_command(bc_scene_ui_command::for_get_scene());
			auto* l_scene = *l_task.get().as<game::bc_scene*>();
			if(!l_scene)
			{
				return;
			}

			/*if(l_scene->get_path().empty())
			{
				const auto l_executing_path_qt = _get_executing_path();
				auto l_file_name = QFileDialog::getSaveFileName
				(
					static_cast<QWidget*>(QObject::parent()),
					"Save Scene File",
					l_executing_path_qt,
					"JSON File (*.json)"
				);
				if (l_file_name.isEmpty())
				{
					return;
				}

				bc_ui_scene_command l_command = bc_ui_scene_command::for_save_as_scene(l_scene, l_file_name.toStdWString().c_str());
				m_command_service.queue_command(std::move(l_command));
			}
			else*/
			{
				bc_scene_ui_command l_command = bc_scene_ui_command::for_save_scene(l_scene);
				m_command_service.queue_command(std::move(l_command));
			}
		}

		inline QString bc_form_main_menu::_get_executing_path() const
		{
			const auto l_executing_path = core::bc_path(core::bc_path::get_program_path().c_str()).set_filename(bcL("Content")).get_string();
			auto l_executing_path_qt = QString::fromWCharArray(l_executing_path.c_str(), l_executing_path.size());
			return l_executing_path_qt;
		}
	}
}