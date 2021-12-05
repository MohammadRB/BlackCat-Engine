// [11/15/2016 MRB]

#include "Editor/EditorPCH.h"
#include "Editor/Application/bcEditorGameConsole.h"

namespace black_cat
{
	namespace editor
	{
		bc_editor_game_console::bc_editor_game_console(game::bc_game_console& p_game_console, bc_widget_console& p_widget)
			: game::bci_game_console_imp(p_game_console),
			m_clear_logs(false),
			m_widget(&p_widget)
		{
		}

		bc_editor_game_console::bc_editor_game_console(bc_editor_game_console&& p_other) noexcept
			: game::bci_game_console_imp(std::move(p_other)),
			m_clear_logs(false),
			m_widget(p_other.m_widget)
		{
		}

		bc_editor_game_console::~bc_editor_game_console()
		{
		}

		bc_editor_game_console& bc_editor_game_console::operator=(bc_editor_game_console&& p_other) noexcept
		{
			game::bci_game_console_imp::operator=(std::move(p_other));
			m_widget = p_other.m_widget;
			p_other.m_widget = nullptr;

			return *this;
		}

		void bc_editor_game_console::write_output(game::bc_console_output_type p_type, const bcECHAR* p_msg)
		{
			auto l_msg = QString::fromWCharArray(p_msg);
			
			{
				std::lock_guard l_lock(m_logs_lock);
				m_received_logs.push_back(std::make_pair(p_type, std::move(l_msg)));
			}
		}

		void bc_editor_game_console::clear_output()
		{
			{
				std::lock_guard l_lock(m_logs_lock);
				m_clear_logs = true;
			}
		}

		void bc_editor_game_console::show()
		{
		}

		void bc_editor_game_console::hide()
		{
		}

		void bc_editor_game_console::update(const core_platform::bc_clock::update_param& p_clock_update_param)
		{
		}

		void bc_editor_game_console::update_ui()
		{
			decltype(m_received_logs) l_received_logs;
			
			{
				std::lock_guard l_lock(m_logs_lock);

				if (m_clear_logs)
				{
					m_widget->clear_logs();
					m_clear_logs = false;
				}

				std::swap(m_received_logs, l_received_logs);
			}

			m_widget->write_logs(l_received_logs.data(), l_received_logs.size());
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
			const auto l_wstring = p_ui_script.toStdWString();

			m_game_console->run_script(l_wstring.c_str(), p_output_console);

			emit scriptExecuted(p_ui_script);
		}
	}
}