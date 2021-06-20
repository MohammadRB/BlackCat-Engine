// [10/24/2016 MRB]

#include "PlatformImp/PlatformImpPCH.h"
#include "Core/Messaging/Event/bcEventManager.h"
#include "Platform/bcEvent.h"
#include "PlatformImp/bcExport.h"
#include "PlatformImp/Application/bcConsoleWindow.h"
#include "CorePlatformImp/bcUtility.h"

namespace black_cat
{
	namespace platform
	{
		bool WINAPI _console_handler(DWORD p_event)
		{
			if(p_event == CTRL_CLOSE_EVENT)
			{
				auto* l_event_manager = core::bc_get_service<core::bc_event_manager>();
				bc_app_event_window_close l_event(reinterpret_cast<bc_console_window::id>(GetConsoleWindow()));

				l_event_manager->process_event(l_event);

				return true;
			}

			return false;
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_console_window<core_platform::g_api_win32>::bc_platform_console_window(bc_console_window_parameter& p_parameter)
		{
			m_pack.m_hwnd = GetConsoleWindow();
			if (m_pack.m_hwnd == nullptr)
			{
				core_platform::win_call(static_cast<bool>(AllocConsole()));
				m_pack.m_hwnd = GetConsoleWindow();
			}

			// Disable close button because closing console causes whole process to exit
			disable_close(true);
			
			m_pack.m_std_out = GetStdHandle(STD_OUTPUT_HANDLE);
			std::freopen("CONIN$", "r", stdin);
			std::freopen("CONOUT$", "w", stdout);
			std::freopen("CONOUT$", "w", stderr);
			
			core_platform::win_call(SetConsoleCtrlHandler
			(
				reinterpret_cast<PHANDLER_ROUTINE>(_console_handler),
				true
			) == TRUE);

			set_caption(p_parameter.m_caption.c_str());
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_console_window<core_platform::g_api_win32>::bc_platform_console_window(bc_platform_console_window&& p_other) noexcept
			: bc_platform_window(std::move(p_other)),
			m_pack(std::move(p_other.m_pack))
		{
			p_other.m_pack.m_hwnd = nullptr;
			p_other.m_pack.m_std_out = nullptr;
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_console_window<core_platform::g_api_win32>::~bc_platform_console_window()
		{
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_console_window<core_platform::g_api_win32>& bc_platform_console_window<core_platform::g_api_win32>::operator=(bc_platform_console_window&& p_other) noexcept
		{
			bc_platform_window::operator=(std::move(p_other));
			m_pack = std::move(p_other.m_pack);

			p_other.m_pack.m_hwnd = nullptr;
			p_other.m_pack.m_std_out = nullptr;

			return *this;
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_console_window<core_platform::g_api_win32>::id bc_platform_console_window<core_platform::g_api_win32>::get_id() const
		{
			return reinterpret_cast<id>(m_pack.m_hwnd);
		}

		template<>
		BC_PLATFORMIMP_DLL
		const bcECHAR* bc_platform_console_window<core_platform::g_api_win32>::get_caption() const
		{
			return m_pack.m_caption.c_str();
		}

		template<>
		BC_PLATFORMIMP_DLL
		void bc_platform_console_window<core_platform::g_api_win32>::set_caption(const bcECHAR* p_caption)
		{
			m_pack.m_caption = p_caption;
			core_platform::win_call(SetConsoleTitle(m_pack.m_caption.c_str()) != 0);
		}

		template<>
		BC_PLATFORMIMP_DLL
		void bc_platform_console_window<core_platform::g_api_win32>::disable_close(bool p_disable)
		{
			const HMENU l_console_menu = GetSystemMenu(m_pack.m_hwnd, FALSE);

			if(p_disable)
			{
				EnableMenuItem(l_console_menu, SC_CLOSE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED) == TRUE;
				//core_platform::win_call(DeleteMenu(l_console_menu, SC_CLOSE, MF_BYCOMMAND) == TRUE);
			}
			else
			{
				EnableMenuItem(l_console_menu, SC_CLOSE, MF_BYCOMMAND | MF_ENABLED) == TRUE;
			}
		}

		template<>
		BC_PLATFORMIMP_DLL
		void bc_platform_console_window<core_platform::g_api_win32>::set_text_color(bc_console_window_text_color p_color)
		{
			bcINT l_color = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;;

			switch (p_color)
			{
			case bc_console_window_text_color::white:
				l_color = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
				break;
			case bc_console_window_text_color::red:
				l_color = FOREGROUND_INTENSITY | FOREGROUND_RED;
				break;
			case bc_console_window_text_color::green:
				l_color = FOREGROUND_INTENSITY | FOREGROUND_GREEN;
				break;
			case bc_console_window_text_color::blue:
				l_color = FOREGROUND_INTENSITY | FOREGROUND_BLUE;
				break;
			case bc_console_window_text_color::aqua:
				l_color = FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE;
				break;
			case bc_console_window_text_color::purple:
				l_color = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE;
				break;
			case bc_console_window_text_color::yellow:
				l_color = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN;
				break;
			default:
				break;
			}

			SetConsoleTextAttribute(m_pack.m_std_out, l_color);
		}

		template<>
		BC_PLATFORMIMP_DLL
		void bc_platform_console_window<core_platform::g_api_win32>::clear()
		{
			system("cls");
		}

		template<>
		BC_PLATFORMIMP_DLL
		void bc_platform_console_window<core_platform::g_api_win32>::update()
		{
		}

		template<>
		BC_PLATFORMIMP_DLL
		void bc_platform_console_window<core_platform::g_api_win32>::close()
		{
			core_platform::win_call(SetConsoleCtrlHandler
			(
				reinterpret_cast<PHANDLER_ROUTINE>(_console_handler),
				false
			) == TRUE);

			core_platform::win_call(PostMessage(m_pack.m_hwnd, WM_CLOSE, 0, 0) == TRUE);
			core_platform::win_call(static_cast<bool>(FreeConsole()));
			m_pack.m_hwnd = nullptr;
			m_pack.m_std_out = nullptr;
		}
	}
}