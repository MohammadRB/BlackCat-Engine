// [24/03/2015 MRB]

#include "PlatformImp/PlatformImpPCH.h"

#include "Platform/bcEvent.h"
#include "PlatformImp/bcExport.h"
#include "PlatformImp/Application/bcBasicWindow.h"
#include "PlatformImp/Application/bcApplication.h"

namespace black_cat::platform
{
	template<>
	BC_PLATFORMIMP_DLL
	bc_platform_application<bc_platform::win32>::bc_platform_application(bc_application_parameter& p_parameter)
		: m_pack(p_parameter.m_instance)
	{
		 SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);
	}

	template<>
	BC_PLATFORMIMP_DLL
	bc_platform_application<bc_platform::win32>::bc_platform_application(bc_platform_application&& p_other) noexcept
		: m_pack(p_other.m_pack)
	{
		p_other.m_pack.m_instance = nullptr;
	}

	template<>
	BC_PLATFORMIMP_DLL
	bc_platform_application<bc_platform::win32>::~bc_platform_application()
	{
	}

	template<>
	BC_PLATFORMIMP_DLL
	bc_platform_application<bc_platform::win32>& bc_platform_application<bc_platform::win32>::operator=(bc_platform_application&& p_other) noexcept
	{
		m_pack.m_instance = p_other.m_pack.m_instance;

		p_other.m_pack.m_instance = nullptr;

		return *this;
	}

	template<>
	BC_PLATFORMIMP_DLL
	bc_basic_window bc_platform_application<bc_platform::win32>::create_basic_window(const bcECHAR* p_caption, bcUINT32 p_width, bcUINT32 p_height)
	{
		const bc_basic_window::parameter l_parameters(m_pack.m_instance, p_caption, p_width, p_height);
		bc_basic_window l_render_window(l_parameters);

		return l_render_window;
	}

	template<>
	BC_PLATFORMIMP_DLL
	bc_console_window bc_platform_application<bc_platform::win32>::create_console_window(const bcECHAR* p_caption)
	{
		const bc_console_window::parameter l_parameters(p_caption);
		bc_console_window l_console_window(l_parameters);

		return l_console_window;
	}

	template<>
	BC_PLATFORMIMP_DLL
	void bc_platform_application<bc_platform::win32>::update()
	{
		auto& l_event_manager = *core::bc_get_service<core::bc_event_manager>();
		MSG l_msg;

		// Get thread messages that doesn't belong to any window by passing -1 to HWND argument
		while (PeekMessage(&l_msg, reinterpret_cast<HWND>(-1), 0, 0, PM_REMOVE))
		{
			TranslateMessage(&l_msg);

			if (l_msg.message == WM_QUIT)
			{
				bc_app_event_exit l_exit_event(l_msg.wParam);
				l_event_manager.process_event(l_exit_event);
			}
		}
	}

	template<>
	BC_PLATFORMIMP_DLL
	void bc_platform_application<bc_platform::win32>::request_termination()
	{
		PostQuitMessage(0);
	}
		
	template<>
	BC_PLATFORMIMP_DLL
	bc_messagebox_value bc_platform_application<bc_platform::win32>::show_messagebox(bc_basic_window::id p_window,
		core::bc_estring_view p_caption,
		core::bc_estring_view p_text,
		bc_messagebox_type p_type,
		bc_messagebox_button p_button)
	{
		auto l_result = bc_messagebox_value::ok;
		bcUINT32 l_type = 0;
		bcUINT32 l_button = 0;

		switch (p_type)
		{
		case bc_messagebox_type::none:
			l_type = 0;
			break;
		case bc_messagebox_type::error:
			l_type = MB_ICONERROR;
			break;
		case bc_messagebox_type::warning:
			l_type = MB_ICONWARNING;
			break;
		case bc_messagebox_type::information:
			l_type = MB_ICONINFORMATION;
			break;
		}

		switch (p_button)
		{
		case bc_messagebox_button::ok:
			l_button = MB_OK;
			break;
		case bc_messagebox_button::ok_cancel:
			l_button = MB_OKCANCEL;
			break;
		case bc_messagebox_button::retry_cancel:
			l_button = MB_RETRYCANCEL;
			break;
		case bc_messagebox_button::yes_no:
			l_button = MB_YESNO;
			break;
		case bc_messagebox_button::yes_no_cancel:
			l_button = MB_YESNOCANCEL;
			break;
		}

		switch (bcINT32 l_value = MessageBox(reinterpret_cast<HWND>(p_window), p_text.data(), p_caption.data(), l_type | l_button))
		{
		case IDOK:
			l_result = bc_messagebox_value::ok;
			break;
		case IDCANCEL:
			l_result = bc_messagebox_value::cancel;
			break;
		case IDRETRY:
			l_result = bc_messagebox_value::retry;
			break;
		case IDYES:
			l_result = bc_messagebox_value::yes;
			break;
		case IDNO:
			l_result = bc_messagebox_value::no;
			break;
		}

		return l_result;
	}
}
