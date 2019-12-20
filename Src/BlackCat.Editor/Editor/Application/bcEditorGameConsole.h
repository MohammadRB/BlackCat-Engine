// [11/15/2016 MRB]

#pragma once

#include "Game/System/Script/bcGameConsole.h"
#include "Game/System/Script/bcIGameConsoleImp.h"
#include "Editor/Widget/bcWidgetConsole.h"

#include <QtCore/QObject>
#include <QtConcurrent/QtConcurrent>

namespace black_cat
{
	namespace editor
	{
		class bc_editor_game_console : public QObject, public game::bc_igame_console_imp
		{
			Q_OBJECT;

		public:
			bc_editor_game_console(game::bc_game_console& p_game_console, bc_widget_console* p_widget);
			
			bc_editor_game_console(bc_editor_game_console&&) noexcept;

			~bc_editor_game_console();

			bc_editor_game_console& operator=(bc_editor_game_console&&) noexcept;

			void write_output(game::bc_console_output_type p_type, const core::bc_estring& p_msg) override;

			void clear_output() override;

			void show() override;

			void hide() override;

			void update(core_platform::bc_clock::update_param p_clock_update_param) override;

			bool is_visible() override;

			/**
			 * \brief Make a widget able to run script through console.
			 * This function connect widget executeUIScript signal to it's slot. 
			 * \param p_widget 
			 */
			void connect_widget(QWidget* p_widget);

		protected:

		private slots:
			void executeScript(const QString& p_script);
			
			void executeUIScript(const QString& p_ui_script);

		signals:
			void logRecieved(game::bc_console_output_type p_log_type, const QString& p_log);

			void clearRecievied();

			void scriptExecuted(const QString& p_string);

		private:
		};
	}
}