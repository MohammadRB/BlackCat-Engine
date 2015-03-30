// [03/20/2015 MRB]

#include "PlatformImp/PlatformImpPCH.h"
#include "PlatformImp/Application/bcRenderWindow.h"

namespace black_cat
{
	namespace platform
	{
		LRESULT CALLBACK _window_proc(HWND p_hwnd, UINT p_msg, WPARAM p_wparam, LPARAM p_lparam)
		{
			auto& l_event_manager = core::bc_service_manager::get().global_event_manager();

			// Handle some specific messages. Note that if we handle a message, we should return 0.
			switch (p_msg)
			{
				// WM_ACTIVATE is sent when the window is activated or deactivated.
			case WM_ACTIVATE:
				{
					if (LOWORD(p_wparam) == WA_INACTIVE)
					{
						bc_app_event_active l_pause_event(false);
						l_event_manager.process_event(l_pause_event);
					}
					else
					{
						bc_app_event_active l_resume_event(true);
						l_event_manager.process_event(l_resume_event);
					}

					return 0;
				}
			case WM_SIZE:
				{
					if (p_wparam != SIZE_MINIMIZED)
					{
						bcUINT32 l_width = LOWORD(p_lparam);
						bcUINT32 l_height = HIWORD(p_lparam);
						bc_app_event_window_resize l_resize_event(l_width, l_height);

						bc_app_event_active l_resume_event(true);

						// Because we send a pause event when window is minimized we send a resume event before resize event 
						l_event_manager.process_event(l_resume_event);
						l_event_manager.process_event(l_resize_event);
					}
					else
					{
						bc_app_event_active l_pause_event(false);
						l_event_manager.process_event(l_pause_event);
					}

					return 0;
				}
				// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
			case WM_ENTERSIZEMOVE:
				{
					bc_app_event_active l_pause_event(false);
					l_event_manager.process_event(l_pause_event);

					return 0;
				}
				// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
			case WM_EXITSIZEMOVE:
				{
					bc_app_event_active l_resume_event(true);
					l_event_manager.process_event(l_resume_event);

					return 0;
				}
				// WM_DESTROY is sent when the window is being destroyed.
			case WM_DESTROY:
				{
					PostQuitMessage(0);

					return 0;
				}
			}

			// Forward any other messages we did not handle above to the default window procedure. Note that our window 
			// procedure must return the return value of DefWindow Proc.
			return DefWindowProc(p_hwnd, p_msg, p_wparam, p_lparam);
		};

		bool _update_window_pos_size(HWND p_hwnd, bcUINT32 p_left, bcUINT32 p_top, bcUINT32 p_width, bcUINT32 p_height)
		{
			return MoveWindow(p_hwnd, p_left, p_top, p_width, p_height, true);
		}

		template< >
		BC_PLATFORMIMP_DLL_EXP bc_render_window_proxy<core_platform::bc_platform::win32>::bc_render_window_proxy(bc_render_window_parameter& p_parameters)
			: m_platform_pack(p_parameters.m_instance, std::move(p_parameters.m_caption))
		{
			// The first task to creating a window is to describe some of its characteristics by filling out a WNDCLASS structure.
			WNDCLASS l_window_class;

			l_window_class.style = CS_HREDRAW | CS_VREDRAW;
			l_window_class.lpfnWndProc = _window_proc;
			l_window_class.cbClsExtra = 0;
			l_window_class.cbWndExtra = 0;
			l_window_class.hInstance = m_platform_pack.m_instance;
			l_window_class.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
			l_window_class.hCursor = LoadCursor(nullptr, IDC_ARROW);
			l_window_class.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
			l_window_class.lpszMenuName = nullptr;
			l_window_class.lpszClassName = L"BlackCatRenderWindow";

			// Next, we register this WNDCLASS instance with Windows so that we can create a window based on it.
			if (!RegisterClass(&l_window_class))
			{
				throw bc_platform_resource_exception("Register window class failed");
			}

			// With our WNDCLASS instance registered, we can create a window with the CreateWindow function. This function
			// returns a handle to the window it creates(an HWND). If the creation failed, the handle will have the value
			// of zero.
			m_platform_pack.m_handle = CreateWindow(
				l_window_class.lpszClassName,
				m_platform_pack.m_caption.c_str(),
				WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				p_parameters.m_width,
				p_parameters.m_height,
				nullptr,
				nullptr,
				m_platform_pack.m_instance,
				nullptr);
			if (!m_platform_pack.m_handle)
			{
				throw bc_platform_resource_exception("Create window failed");
			}

			ShowWindow(m_platform_pack.m_handle, SW_SHOW);
			UpdateWindow(m_platform_pack.m_handle);
		};

		template< >
		BC_PLATFORMIMP_DLL_EXP bc_render_window_proxy<core_platform::bc_platform::win32>::bc_render_window_proxy(bc_render_window_proxy&& p_other) noexcept(true)
			: m_platform_pack(p_other.m_platform_pack.m_instance, std::move(p_other.m_platform_pack.m_caption))
		{
			m_platform_pack.m_handle = p_other.m_platform_pack.m_handle;
		};

		template< >
		BC_PLATFORMIMP_DLL_EXP bc_render_window_proxy<core_platform::bc_platform::win32>::~bc_render_window_proxy()
		{
		};

		template< >
		BC_PLATFORMIMP_DLL_EXP bc_render_window_proxy<core_platform::bc_platform::win32>& bc_render_window_proxy<core_platform::bc_platform::win32>::operator=(bc_render_window_proxy&& p_other) noexcept(true)
		{
			m_platform_pack.m_instance = p_other.m_platform_pack.m_instance;
			m_platform_pack.m_handle = p_other.m_platform_pack.m_handle;
			m_platform_pack.m_caption = std::move(p_other.m_platform_pack.m_caption);

			return *this;
		};

		template< >
		BC_PLATFORMIMP_DLL_EXP bcUINT32 bc_render_window_proxy<core_platform::bc_platform::win32>::width() const noexcept(true)
		{
			RECT l_rect;
			GetClientRect(m_platform_pack.m_handle, &l_rect);

			return l_rect.right - l_rect.left;
		};

		template< >
		BC_PLATFORMIMP_DLL_EXP void bc_render_window_proxy<core_platform::bc_platform::win32>::width(bcUINT32 p_width) noexcept(true)
		{
			_update_window_pos_size(m_platform_pack.m_handle, left(), top(), p_width, height());
		};

		template< >
		BC_PLATFORMIMP_DLL_EXP bcUINT32 bc_render_window_proxy<core_platform::bc_platform::win32>::height() const noexcept(true)
		{
			RECT l_rect;
			GetClientRect(m_platform_pack.m_handle, &l_rect);

			return l_rect.bottom - l_rect.top;
		};

		template< >
		BC_PLATFORMIMP_DLL_EXP void bc_render_window_proxy<core_platform::bc_platform::win32>::height(bcUINT32 p_height) noexcept(true)
		{
			_update_window_pos_size(m_platform_pack.m_handle, left(), top(), width(), p_height);
		};

		template< >
		BC_PLATFORMIMP_DLL_EXP bcUINT32 bc_render_window_proxy<core_platform::bc_platform::win32>::left() const noexcept(true)
		{
			POINT point;
			point.x = 0;
			point.y = 0;

			ClientToScreen(m_platform_pack.m_handle, &point);

			return point.x;
		};

		template< >
		BC_PLATFORMIMP_DLL_EXP void bc_render_window_proxy<core_platform::bc_platform::win32>::left(bcUINT32 p_left) noexcept(true)
		{
			_update_window_pos_size(m_platform_pack.m_handle, p_left, top(), width(), height());
		};

		template< >
		BC_PLATFORMIMP_DLL_EXP bcUINT32 bc_render_window_proxy<core_platform::bc_platform::win32>::top() const noexcept(true)
		{
			POINT point;
			point.x = 0;
			point.y = 0;

			ClientToScreen(m_platform_pack.m_handle, &point);

			return point.y;
		};

		template< >
		BC_PLATFORMIMP_DLL_EXP void bc_render_window_proxy<core_platform::bc_platform::win32>::top(bcUINT32 p_top) noexcept(true)
		{
			_update_window_pos_size(m_platform_pack.m_handle, left(), p_top, width(), height());
		};

		template< >
		BC_PLATFORMIMP_DLL_EXP void bc_render_window_proxy<core_platform::bc_platform::win32>::set_size(bcUINT32 p_width, bcUINT32 p_height) noexcept(true)
		{
			_update_window_pos_size(m_platform_pack.m_handle, left(), top(), p_width, p_height);
		};

		template< >
		BC_PLATFORMIMP_DLL_EXP void bc_render_window_proxy<core_platform::bc_platform::win32>::set_position(bcUINT32 p_left, bcUINT32 p_top) noexcept(true)
		{
			_update_window_pos_size(m_platform_pack.m_handle, p_left, p_top, width(), height());
		};

		template< >
		BC_PLATFORMIMP_DLL_EXP core::bc_estring bc_render_window_proxy<core_platform::bc_platform::win32>::caption() const
		{
			return m_platform_pack.m_caption;
		};

		template< >
		BC_PLATFORMIMP_DLL_EXP void bc_render_window_proxy<core_platform::bc_platform::win32>::caption(core::bc_estring& p_caption)
		{
			m_platform_pack.m_caption = p_caption;

			SetWindowText(m_platform_pack.m_handle, m_platform_pack.m_caption.c_str());
		};

		template< >
		BC_PLATFORMIMP_DLL_EXP void bc_render_window_proxy<core_platform::bc_platform::win32>::caption(core::bc_estring&& p_caption)
		{
			m_platform_pack.m_caption = std::move(p_caption);

			SetWindowText(m_platform_pack.m_handle, m_platform_pack.m_caption.c_str());
		};

		template< >
		BC_PLATFORMIMP_DLL_EXP bool bc_render_window_proxy<core_platform::bc_platform::win32>::is_minimized() const noexcept(true)
		{
			return IsIconic(m_platform_pack.m_handle);
		};

		template< >
		BC_PLATFORMIMP_DLL_EXP void bc_render_window_proxy<core_platform::bc_platform::win32>::show(bool p_visible)
		{
			bcINT32 l_show = p_visible ? SW_SHOW : SW_HIDE;

			ShowWindow(m_platform_pack.m_handle, l_show);
		};

		template< >
		BC_PLATFORMIMP_DLL_EXP void bc_render_window_proxy<core_platform::bc_platform::win32>::close() noexcept(true)
		{
			DestroyWindow(m_platform_pack.m_handle);
		};

		template< >
		BC_PLATFORMIMP_DLL_EXP bc_messagebox_value bc_render_window_proxy<core_platform::bc_platform::win32>::messagebox(core::bc_estring p_caption, core::bc_estring p_text, bc_messagebox_type p_type, bc_messagebox_buttom p_button)
		{
			bc_messagebox_value l_result = bc_messagebox_value::ok;
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
			case bc_messagebox_buttom::ok:
				l_button = MB_OK;
				break;
			case bc_messagebox_buttom::ok_cancel:
				l_button = MB_OKCANCEL;
				break;
			case bc_messagebox_buttom::retry_cancel:
				l_button = MB_RETRYCANCEL;
				break;
			case bc_messagebox_buttom::yes_no:
				l_button = MB_YESNO;
				break;
			case bc_messagebox_buttom::yes_no_cancel:
				l_button = MB_YESNOCANCEL;
				break;
			}

			bcINT32 l_value = MessageBox(m_platform_pack.m_handle, p_text.c_str(), p_caption.c_str(), l_type | l_button);

			switch (l_value)
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
		};
	}
}