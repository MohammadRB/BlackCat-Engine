// [11/14/2016 MRB]

#pragma once

#include "Game/System/Script/bcGameConsole.h"
#include "Editor/EditorPCH.h"
#include "Editor/QtAwesome/QtAwesome.h"

namespace black_cat
{
	namespace editor
	{
		class bc_console_widget : public QWidget
		{
		public:
			Q_OBJECT

		public:
			explicit bc_console_widget(QtAwesome* p_awesome, QWidget* p_parent = nullptr);

			~bc_console_widget();

		protected:

		private slots:
			void onLog(game::bc_console_output_type p_log_type, const QString& p_log);
			
			void onClear();

			void _returnPressed();

			void _infoToggled(bool p_toggle);

			void _debugToggled(bool p_toggle);

			void _errorToggled(bool p_toggle);

			void _scriptToggled(bool p_toggle);

			void _clearClicked();

		signals:
			void returnPressed(const QString& p_string);

			void uiScripted(const QString& p_string);

		private:
			void _setup_ui();

			QtAwesome* m_awesome;

			QColor m_info_color;
			QColor m_debug_color;
			QColor m_error_color;
			QColor m_script_color;

			std::unique_ptr< QVBoxLayout > m_main_layout;
			std::unique_ptr< QHBoxLayout > m_toolbar_layout;
			std::unique_ptr< QFrame > m_toolbar;
			std::unique_ptr< QPushButton > m_toolbar_info;
			std::unique_ptr< QPushButton > m_toolbar_debug;
			std::unique_ptr< QPushButton > m_toolbar_error;
			std::unique_ptr< QPushButton > m_toolbar_script;
			std::unique_ptr< QPushButton > m_toolbar_clear;
			std::unique_ptr< QListWidget > m_console_list;
			std::unique_ptr< QLineEdit > m_console_input;
		};
	}
}