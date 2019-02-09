// [02/07/2019 MRB]

#pragma once

#include "Core/File/bcPath.h"
#include "Game/Object/Scene/bcScene.h"
#include "Editor/Application/bcUICommandService.h"
#include "Editor/UICommand/bcUISceneCommand.h"
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
			bc_form_main_menu(QWidget& p_parent, bc_ui_command_service& p_command_service);

			void set_scene(game::bc_scene* p_scene);

		private slots:
			void onNewSceneClicked() const;
			
			void onLoadSceneClicked() const;

			void onSaveSceneClicked() const;

		private:
			QString _get_executing_path() const;

			bc_ui_command_service& m_command_service;
			game::bc_scene* m_scene;
		};

		inline bc_form_main_menu::bc_form_main_menu(QWidget& p_parent, bc_ui_command_service& p_command_service)
			: QObject(&p_parent),
			m_command_service(p_command_service),
			m_scene(nullptr)
		{
			QMenu* l_file_menu = p_parent.findChild< QMenu* >("fileMenu");
			
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

		inline void bc_form_main_menu::set_scene(game::bc_scene* p_scene)
		{
			m_scene = p_scene;
		}

		inline void bc_form_main_menu::onNewSceneClicked() const
		{
			bc_ui_scene_command l_command = bc_ui_scene_command::for_new_scene();
			m_command_service.execute_command(std::move(l_command));
		}

		inline void bc_form_main_menu::onLoadSceneClicked() const
		{
			const auto l_executing_path_qt = _get_executing_path();
			auto l_file_name = QFileDialog::getOpenFileName
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

			bc_ui_scene_command l_command = bc_ui_scene_command::for_load_scene(l_file_name.toStdWString().c_str());
			m_command_service.execute_command(std::move(l_command));
		}

		inline void bc_form_main_menu::onSaveSceneClicked() const
		{
			if(!m_scene)
			{
				bc_ui_scene_command l_command = bc_ui_scene_command::for_get_scene();
				m_command_service.execute_command(std::move(l_command));
				return;
			}

			const auto l_executing_path_qt = _get_executing_path();

			if(m_scene->get_path().empty())
			{
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

				bc_ui_scene_command l_command = bc_ui_scene_command::for_save_as_scene(m_scene, l_file_name.toStdWString().c_str());
				m_command_service.execute_command(std::move(l_command));
			}
			else
			{
				bc_ui_scene_command l_command = bc_ui_scene_command::for_save_scene(m_scene);
				m_command_service.execute_command(std::move(l_command));
			}
		}

		inline QString bc_form_main_menu::_get_executing_path() const
		{
			auto l_executing_path = core::bc_path(core::bc_path::get_program_path().c_str()).get_directory();
			auto l_executing_path_qt = QString::fromWCharArray(l_executing_path.c_str(), l_executing_path.size());
			return l_executing_path_qt;
		}
	}
}