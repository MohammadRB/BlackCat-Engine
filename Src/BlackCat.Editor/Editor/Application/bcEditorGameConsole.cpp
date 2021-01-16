// [11/15/2016 MRB]

#include "Editor/EditorPCH.h"
#include "Editor/Application/bcEditorGameConsole.h"

namespace black_cat
{
	namespace editor
	{
		bc_editor_game_console::bc_editor_game_console(game::bc_game_console& p_game_console, bc_widget_console& p_widget)
			: game::bci_game_console_imp(p_game_console)
		{
			qRegisterMetaType<game::bc_console_output_type>("game::bc_console_output_type");
			
			QObject::connect
			(
				this,
				SIGNAL(logRecieved(game::bc_console_output_type, const QString&)),
				&p_widget,
				SLOT(onLog(game::bc_console_output_type, const QString&)),
				Qt::QueuedConnection
			);
			QObject::connect(this, SIGNAL(clearRecievied()), &p_widget, SLOT(onClear()), Qt::QueuedConnection);
		}

		bc_editor_game_console::bc_editor_game_console(bc_editor_game_console&& p_other) noexcept
			: game::bci_game_console_imp(std::move(p_other))
		{
		}

		bc_editor_game_console::~bc_editor_game_console()
		{
		}

		bc_editor_game_console& bc_editor_game_console::operator=(bc_editor_game_console&& p_other) noexcept
		{
			game::bci_game_console_imp::operator=(std::move(p_other));

			return *this;
		}

		void bc_editor_game_console::write_output(game::bc_console_output_type p_type, const core::bc_estring& p_msg)
		{
			emit logRecieved(p_type, QString::fromWCharArray(p_msg.c_str()));
		}

		void bc_editor_game_console::clear_output()
		{
			emit clearRecievied();
		}

		void bc_editor_game_console::show()
		{
		}

		void bc_editor_game_console::hide()
		{
		}

		void bc_editor_game_console::update(core_platform::bc_clock::update_param p_clock_update_param)
		{
		}

		bool bc_editor_game_console::is_visible()
		{
			return true;
		}

		void bc_editor_game_console::connect_widget(QWidget* p_widget)
		{
			QObject::connect(p_widget, SIGNAL(executeScript(const QString&, bool)), this, SLOT(executeScript(const QString&, bool)));
		}

		void bc_editor_game_console::executeScript(const QString& p_ui_script, bool p_output_console)
		{
			auto l_wstring = p_ui_script.toStdWString();

			m_game_console.run_script(l_wstring.c_str(), p_output_console);

			emit scriptExecuted(p_ui_script);
		}
	}
}