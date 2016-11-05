 // [10/22/2016 MRB]

#include "Game/GamePCH.h"
#include "Core/Event/bcEventManager.h"
#include "Platform/bcEvent.h"
#include "Game/System/bcGameSystem.h"
#include "Game/System/Script/bcGameConsole.h"

namespace black_cat
{
	namespace game
	{
		bc_game_console::bc_game_console(bc_script_system& p_script_system)
			: m_script_system(p_script_system),
			m_imp(),
			m_log_types({true, true, true, true}),
			m_logs()
		{
			core::bc_get_service< core::bc_logger >()->register_listener
			(
				core::bc_enum::or({core::bc_log_type::info, core::bc_log_type::debug, core::bc_log_type::error}),
				this
			);
			m_key_event_handle = core::bc_get_service< core::bc_event_manager >()->register_event_listener
			(
				platform::bc_app_event_key::event_name(),
				core::bc_event_manager::delegate_type(this, &bc_game_console::_on_key)
			);
		}

		bc_game_console::bc_game_console(bc_game_console&& p_other) noexcept
			: m_script_system(p_other.m_script_system),
			m_imp(std::move(p_other.m_imp)),
			m_log_types(p_other.m_log_types),
			m_logs(std::move(p_other.m_logs)),
			m_script(std::move(p_other.m_script))
		{
		}

		bc_game_console::~bc_game_console()
		{
		}

		bc_game_console& bc_game_console::operator=(bc_game_console&& p_other) noexcept
		{
			m_imp = std::move(p_other.m_imp);
			m_log_types = p_other.m_log_types;
			m_logs = std::move(p_other.m_logs);
			m_script = std::move(p_other.m_script);

			return *this;
		}

		void bc_game_console::set_implementation(core::bc_unique_ptr<bc_igame_console_imp>&& p_imp)
		{
			m_imp = std::move(p_imp);
		}

		void bc_game_console::run_script(const bcWCHAR* p_script)
		{
			{
				core_platform::bc_lock_guard< core_platform::bc_mutex > l_guard(m_script_mutex);

				m_script.assign(p_script);
			}
		}

		void bc_game_console::disable_output(bc_console_output_type p_output) noexcept
		{
			m_log_types[static_cast<bcSIZE>(p_output)] = false;

			if(m_imp)
			{
				m_imp->clear_output();
				_write_logs();
			}
		}

		void bc_game_console::enable_output(bc_console_output_type p_output) noexcept
		{
			m_log_types[static_cast<bcSIZE>(p_output)] = false;

			if (m_imp)
			{
				m_imp->clear_output();
				_write_logs();
			}
		}

		void bc_game_console::clear_output()
		{
			if (m_imp)
			{
				m_imp->clear_output();
			}
			m_logs.clear();
		}

		void bc_game_console::update(core_platform::bc_clock::update_param p_clock_update_param)
		{
			if (m_imp)
			{
				m_imp->update(p_clock_update_param);
			}

			platform::bc_script_variable l_result;
			core::bc_estring_frame l_result_string;

			{
				core_platform::bc_lock_guard< core_platform::bc_mutex > l_guard(m_script_mutex);

				if (!m_script.empty())
				{
					try
					{
						l_result = m_script_system.run_script(bc_script_context::ui, m_script.c_str());
					}
					catch (const bc_script_compile_exception& p_compile_exception)
					{
						l_result_string = core::bc_to_estring_frame(p_compile_exception.what());
					}
					catch (const bc_script_execute_exception& p_execute_exception)
					{
						l_result_string = core::bc_to_estring_frame(p_execute_exception.what());
					}

					m_script.clear();
				}
			}

			if(l_result.is_valid())
			{
				switch (l_result.get_type())
				{
				case platform::bc_script_value_type::function:
					l_result_string = bcL("function");
					break;
				case platform::bc_script_value_type::null:
					l_result_string = bcL("null");
					break;
				case platform::bc_script_value_type::undefined:
					l_result_string = bcL("undefined");
					break;
				default:
					{
						auto l_result_stringify = m_script_system.stringify(l_result);
						l_result_string = core::bc_to_estring_frame(l_result_stringify);

						break;
					}
				}
			}

			if(!l_result_string.empty())
			{
				on_log(static_cast< core::bc_log_type >(bc_console_output_type::script), l_result_string.c_str());
			}
		}

		void bc_game_console::on_log(core::bc_log_type p_type, const bcECHAR* p_log)
		{
			auto l_type = static_cast<bc_console_output_type>(p_type);
			auto l_log = core::bc_estring(p_log);

			{
				core_platform::bc_lock_guard< core_platform::bc_mutex > l_guard(m_logs_mutex);

				m_logs.push_back(std::make_pair(l_type, std::move(l_log)));
			}

			if (m_imp)
			{
				m_imp->write_output(l_type, m_logs.rbegin()->second);
			}
		}

		bool bc_game_console::_on_key(core::bc_ievent& p_event)
		{
			bcAssert(core::bc_ievent::event_is< platform::bc_app_event_key >(p_event), "");

			if(!m_imp)
			{
				return false;
			}

			auto l_key_event = static_cast< platform::bc_app_event_key& >(p_event);

			if (l_key_event.get_key() == platform::bc_key::kb_grave &&
				l_key_event.get_key_state() == platform::bc_key_state::pressing)
			{
				if(m_imp->is_visibile())
				{
					m_imp->hide();
				}
				else
				{
					m_imp->show();
					core::bc_get_service< core::bc_logger >()->log(core::bc_log_type::info, bcL("Console is now running"));
				}
			}

			return true;
		}

		void bc_game_console::_write_logs()
		{
			{
				core_platform::bc_lock_guard< core_platform::bc_mutex > l_guard(m_logs_mutex);

				for (auto& l_log : m_logs)
				{
					bool l_log_enabled = m_log_types[static_cast<bcSIZE>(l_log.first)];

					if (l_log_enabled)
					{
						m_imp->write_output(l_log.first, l_log.second);
					}
				}
			}
		}
	}
}