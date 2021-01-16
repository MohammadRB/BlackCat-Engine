 // [10/24/2016 MRB]

#include "Game/GamePCH.h"
#include "Core/Container/bcString.h"
#include "Core/Concurrency/bcConcurrency.h"
#include "Core/Utility/bcEnumOperand.h"
#include "Core/Utility/bcLogger.h"
#include "Platform/bcEvent.h"
#include "Game/System/Script/bcDefaultGameConsole.h"
#include <string>
#include <iostream>

namespace black_cat
{
	namespace game
	{
		bc_default_game_console::bc_default_game_console(bc_render_application& p_application, bc_game_console& p_game_console) noexcept
			: bci_game_console_imp(p_game_console),
			m_application(p_application)
		{
		}

		bc_default_game_console::bc_default_game_console(bc_default_game_console&& p_other) noexcept
			: bci_game_console_imp(std::move(p_other)),
			m_application(p_other.m_application),
			m_console(std::move(p_other.m_console))
		{
		}

		bc_default_game_console::~bc_default_game_console()
		{
			if(is_visible())
			{
				m_input_spin_task.interrupt_executor_thread();
				m_console->close();
				m_console.reset();
			}
		}

		bc_default_game_console& bc_default_game_console::operator=(bc_default_game_console&& p_other) noexcept
		{
			m_console = std::move(p_other.m_console);

			return *this;
		}

		void bc_default_game_console::write_output(bc_console_output_type p_type, const core::bc_estring& p_msg)
		{
			core::bc_estring l_message;
			platform::bc_console_window_text_color l_color;

			switch (p_type)
			{
			case bc_console_output_type::info: 
				l_message = bcL("Info: ");
				l_color = platform::bc_console_window_text_color::aqua;
				break;
			case bc_console_output_type::debug: 
				l_message = bcL("Debug: ");
				l_color = platform::bc_console_window_text_color::purple;
				break;
			case bc_console_output_type::error: 
				l_message = bcL("Error: ");
				l_color = platform::bc_console_window_text_color::red;
				break;
			case bc_console_output_type::script: 
				l_message = bcL("Script: ");
				l_color = platform::bc_console_window_text_color::yellow;
				break;
			default: 
				l_color = platform::bc_console_window_text_color::white;
				break;
			}

			l_message.append(p_msg);

			if (is_visible())
			{
				core_platform::bc_lock_guard< core_platform::bc_mutex > l_input_guard(m_input_mutex);
				
				auto l_input_line_size = m_input_line.size();

				if(l_input_line_size > 0)
				{
					{
						core_platform::bc_lock_guard< core_platform::bc_mutex > l_console_guard(m_console_mutex);
						
						// Clear current line if any content is entered by user
						std::cout << '\r';
						for(bcINT32 i = 0; i < l_input_line_size; ++i)
						{
							std::cout << ' ';
						}
						std::cout << '\r';

						// Write log message
						m_console->set_text_color(l_color);
#ifdef BC_UNICODE
						std::wcout << l_message << std::endl;
#else
						std::cout << l_message << std::endl;
#endif
						m_console->set_text_color(platform::bc_console_window_text_color::white);

						// Write last line content again
						std::cout << m_input_line.c_str();
					}
				}
				else
				{
					{
						core_platform::bc_lock_guard< core_platform::bc_mutex > l_console_guard(m_console_mutex);
						
						m_console->set_text_color(l_color);
#ifdef BC_UNICODE
						std::wcout << l_message << std::endl;
#else
						std::cout << l_message << std::endl;
#endif
						m_console->set_text_color(platform::bc_console_window_text_color::white);
					}
				}
			}
		}

		void bc_default_game_console::clear_output()
		{
			if(is_visible())
			{
				core_platform::bc_lock_guard< core_platform::bc_mutex > l_guard(m_console_mutex);
				
				m_console->clear();
			}
		}

		void bc_default_game_console::show()
		{
			if(is_visible())
			{
				return;
			}

			m_console.reset(m_application.create_console_window(bcL("BlackCat Console")));
			m_input_spin_task = core::bc_concurrency::start_task
			(
				core::bc_delegate< void() >(*this, &bc_default_game_console::_input_spin),
				core::bc_enum::or({ core::bc_task_creation_option::policy_none, core::bc_task_creation_option::lifetime_exceed_frame })
			);
		}

		void bc_default_game_console::hide()
		{
			/*if(!is_visible())
			{
				return;
			}

			m_input_spin_task.interrupt_executor_thread();
			m_console->close();
			m_console.reset();*/
		}

		bool bc_default_game_console::is_visible()
		{
			return m_console.is_set();
		}

		void bc_default_game_console::update(core_platform::bc_clock::update_param p_clock_update_param)
		{
			if(is_visible())
			{
				m_console->update();
			}
		}

		void bc_default_game_console::_input_spin()
		{
			bcCHAR l_char;
			core::bc_wstring l_wline;

			while (true)
			{
				std::cin.get(l_char);

				core::bc_concurrency::check_for_interruption();

				{
					core_platform::bc_lock_guard<core_platform::bc_mutex> l_guard(m_input_mutex);

					if (l_char != '\n')
					{
						m_input_line.push_back(l_char);
					}
					else
					{
						l_wline = core::bc_to_exclusive_wstring(m_input_line);
						m_input_line.clear();
					}
				}

				if(!l_wline.empty())
				{
					m_game_console.run_script(l_wline.c_str(), true);
					
					l_wline.clear();
				}
			}
		}
	}
}