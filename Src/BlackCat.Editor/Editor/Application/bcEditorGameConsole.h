// [11/15/2016 MRB]

#pragma once

#include <mutex>
#include <QtCore/QObject>
#include <QtConcurrent/QtConcurrent>
#include "Game/System/Script/bcGameConsole.h"
#include "Game/System/Script/bcIGameConsoleImp.h"
#include "Editor/Widget/bcWidgetConsole.h"

namespace black_cat
{
	namespace editor
	{
		class bc_editor_game_console : public QObject, public game::bci_game_console_imp
		{
			Q_OBJECT;

		public:
			bc_editor_game_console(game::bc_game_console& p_game_console, bc_widget_console& p_widget);
			
			bc_editor_game_console(bc_editor_game_console&&) noexcept;

			~bc_editor_game_console();

			bc_editor_game_console& operator=(bc_editor_game_console&&) noexcept;

			void write_output(game::bc_console_output_type p_type, const bcECHAR* p_msg) override;

			void clear_output() override;

			void show() override;

			void hide() override;

			void update(const platform::bc_clock::update_param& p_clock_update_param) override;

			void update_ui();
			
			bool is_visible() override;
			
			/**
			 * \brief Make a widget able to run script through console.
			 * This function connect widget's executeUIScript signal to it's slot. 
			 * \param p_widget 
			 */
			void connect_widget(QWidget* p_widget);

		private slots:
			void executeScript(const QString& p_ui_script, bool p_output_console);

		signals:
			void scriptExecuted(const QString& p_string);

		private:
			std::mutex m_logs_lock;
			std::vector<std::pair<game::bc_console_output_type, QString>> m_received_logs;
			bool m_clear_logs;
			
			bc_widget_console* m_widget;
		};
	}
}