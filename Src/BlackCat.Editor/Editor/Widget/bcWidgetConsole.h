// [11/14/2016 MRB]

#pragma once

#include "Game/System/Script/bcGameConsole.h"
#include "Editor/EditorPCH.h"
#include "Editor/QtAwesome/QtAwesome.h"

namespace black_cat
{
	namespace editor
	{
		class bc_widget_console : public QWidget
		{
		public:
			Q_OBJECT
			Q_DISABLE_COPY(bc_widget_console)

		public:
			explicit bc_widget_console(QtAwesome* p_awesome, QWidget* p_parent = nullptr);

			~bc_widget_console();

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
			void executeUIScript(const QString& p_string);

		private:
			void _setup_ui();

			QtAwesome* m_awesome;

			QColor m_info_color;
			QColor m_debug_color;
			QColor m_error_color;
			QColor m_script_color;

			QVBoxLayout* m_main_layout;
			QHBoxLayout* m_toolbar_layout;
			QFrame* m_toolbar;
			QPushButton* m_toolbar_info;
			QPushButton* m_toolbar_debug;
			QPushButton* m_toolbar_error;
			QPushButton* m_toolbar_script;
			QPushButton* m_toolbar_clear;
			QListWidget* m_console_list;
			QLineEdit* m_console_input;
		};
	}
}