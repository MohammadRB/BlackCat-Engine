// [14/11/2016 MRB]

#pragma once

#include "Game/System/Script/bcGameConsole.h"
#include "Editor/QtAwesome/QtAwesome.h"
#include <QtWidgets/QtWidgets>

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

			void write_logs(std::pair<game::bc_console_output_type, QString>* p_logs, bcUINT32 p_log_count);

			void clear_logs();
		
		private slots:
			void _returnPressed();

			void _infoToggled(bool p_toggle);

			void _debugToggled(bool p_toggle);
			
			void _warningToggled(bool p_toggle);

			void _errorToggled(bool p_toggle);

			void _scriptToggled(bool p_toggle);

			void _clearClicked();

		signals:
			void executeScript(const QString& p_string, bool p_output_console);

		private:
			void _setup_ui();

			bcUINT32 m_max_row_count;
			QtAwesome* m_awesome;

			QColor m_info_color;
			QColor m_debug_color;
			QColor m_warning_color;
			QColor m_error_color;
			QColor m_script_color;

			QVBoxLayout* m_main_layout;
			QHBoxLayout* m_toolbar_layout;
			QFrame* m_toolbar;
			QPushButton* m_toolbar_info;
			QPushButton* m_toolbar_debug;
			QPushButton* m_toolbar_warning;
			QPushButton* m_toolbar_error;
			QPushButton* m_toolbar_script;
			QPushButton* m_toolbar_clear;
			QListWidget* m_console_list;
			QLineEdit* m_console_input;
		};
	}
}