// [03/20/2015 MRB]

#include "PlatformImp/PlatformImpPCH.h"

#include "CorePlatform/bcException.h"
#include "CorePlatformImp/bcUtility.h"
#include "Core/Utility/bcServiceManager.h"
#include "Core/Messaging/Event/bcEventManager.h"
#include "PlatformImp/bcExport.h"
#include "PlatformImp/Application/bcBasicWindow.h"
#include "Platform/bcEvent.h"

namespace black_cat
{
	namespace platform
	{
		void _get_window_size(HWND p_hwnd, bcUINT32& p_width, bcUINT32& p_height)
		{
			RECT l_rect;
			if (GetWindowRect(p_hwnd, &l_rect))
			{
				p_width = l_rect.right - l_rect.left;
				p_height = l_rect.bottom - l_rect.top;
			}
		}
		
		LRESULT CALLBACK _window_proc(HWND p_hwnd, UINT p_msg, WPARAM p_wparam, LPARAM p_lparam)
		{
			auto* l_event_manager = core::bc_get_service<core::bc_event_manager>();

			// Handle some specific messages. Note that if we handle a message, we should return 0.
			switch (p_msg)
			{
				// WM_ACTIVATE is sent when the window is activated or deactivated.
			case WM_ACTIVATE:
				{
					if (LOWORD(p_wparam) == WA_INACTIVE)
					{
						bc_app_event_window_focus l_focus_event(reinterpret_cast<bc_window_id>(p_hwnd), false);
						l_event_manager->process_event(l_focus_event);
					}
					else
					{
						bc_app_event_window_focus l_focus_event(reinterpret_cast<bc_window_id>(p_hwnd), true);
						l_event_manager->process_event(l_focus_event);
					}

					break;
				}
			case WM_SIZE:
				{
					bcUINT32 l_width = LOWORD(p_lparam);
					bcUINT32 l_height = HIWORD(p_lparam);
					auto l_state = bc_app_event_window_state::state::restored;

					if (p_wparam == SIZE_MINIMIZED)
					{
						l_state = bc_app_event_window_state::state::minimized;
					}
					else if (p_wparam == SIZE_MAXIMIZED)
					{
						l_state = bc_app_event_window_state::state::maximized;
					}
					else if (p_wparam == SIZE_RESTORED)
					{
						l_state = bc_app_event_window_state::state::restored;
					}

					bc_app_event_window_state l_resize_event(reinterpret_cast<bc_window_id>(p_hwnd), l_state);
					l_event_manager->process_event(l_resize_event);

					break;
				}
				// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
			case WM_ENTERSIZEMOVE:
				{
					bcUINT32 l_width, l_height;
					_get_window_size(p_hwnd, l_width, l_height);
				
					bc_app_event_window_resize l_start_event(reinterpret_cast<bc_window_id>(p_hwnd), l_width, l_height, true);
					l_event_manager->process_event(l_start_event);

					break;
				}
				// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
			case WM_EXITSIZEMOVE:
				{
					bcUINT32 l_width, l_height;
					_get_window_size(p_hwnd, l_width, l_height);
				
					bc_app_event_window_resize l_end_event(reinterpret_cast<bc_window_id>(p_hwnd), l_width, l_height, false);
					l_event_manager->process_event(l_end_event);

					break;
				}
				// WM_DESTROY is sent when the window is being destroyed.
			case WM_DESTROY:
				{
					bc_app_event_window_close l_close_event(reinterpret_cast<bc_window_id>(p_hwnd));
					l_event_manager->process_event(l_close_event);

					break;
				}
			default:
				// Forward any other messages we did not handle above to the default window procedure. Note that our window 
				// procedure must return the return value of DefWindow Proc.
				return DefWindowProc(p_hwnd, p_msg, p_wparam, p_lparam);
			}

			return 0;
		}

		bool _update_window_pos_size(HWND p_hwnd, bcUINT32 p_left, bcUINT32 p_top, bcUINT32 p_width, bcUINT32 p_height)
		{
			return MoveWindow(p_hwnd, p_left, p_top, p_width, p_height, true);
		}

		template<>
		BC_PLATFORMIMP_DLL 
		bc_platform_basic_window<core_platform::bc_platform::win32>::bc_platform_basic_window(const bc_basic_window_parameter& p_parameters)
		{
			m_pack.m_instance = p_parameters.m_instance;

			// The first task to creating a window is to describe some of its characteristics by filling out a WNDCLASS structure.
			WNDCLASS l_window_class;

			l_window_class.style = CS_HREDRAW | CS_VREDRAW;
			l_window_class.lpfnWndProc = _window_proc;
			l_window_class.cbClsExtra = 0;
			l_window_class.cbWndExtra = 0;
			l_window_class.hInstance = m_pack.m_instance;
			l_window_class.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
			l_window_class.hCursor = LoadCursor(nullptr, IDC_ARROW);
			l_window_class.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
			l_window_class.lpszMenuName = nullptr;
			l_window_class.lpszClassName = L"BlackCatRenderWindow";

			// Next, we register this WNDCLASS instance with Windows so that we can create a window based on it.
			core_platform::win_call(RegisterClass(&l_window_class) != 0);

			// With our WNDCLASS instance registered, we can create a window with the CreateWindow function. This function
			// returns a handle to the window it creates(an HWND). If the creation failed, the handle will have the value
			// of zero.
			core_platform::win_call((m_pack.m_handle = CreateWindow
			(
				l_window_class.lpszClassName,
				p_parameters.m_caption,
				WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				p_parameters.m_width,
				p_parameters.m_height,
				nullptr,
				nullptr,
				m_pack.m_instance,
				nullptr
			)) != nullptr);

			ShowWindow(m_pack.m_handle, SW_SHOW);
			UpdateWindow(m_pack.m_handle);
		};

		template<>
		BC_PLATFORMIMP_DLL 
		bc_platform_basic_window<core_platform::bc_platform::win32>::bc_platform_basic_window(bc_platform_basic_window&& p_other) noexcept
		{
			operator=(std::move(p_other));
		}

		template<>
		BC_PLATFORMIMP_DLL 
		bc_platform_basic_window<core_platform::bc_platform::win32>::~bc_platform_basic_window()
		{
		}

		template<>
		BC_PLATFORMIMP_DLL 
		bc_platform_basic_window<core_platform::bc_platform::win32>& bc_platform_basic_window<core_platform::bc_platform::win32>::operator=(bc_platform_basic_window&& p_other) noexcept
		{
			bc_platform_window::operator=(std::move(p_other));
			m_pack.m_instance = p_other.m_pack.m_instance;
			m_pack.m_handle = p_other.m_pack.m_handle;

			p_other.m_pack.m_handle = nullptr;

			return *this;
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_basic_window<core_platform::bc_platform::win32>::id bc_platform_basic_window<core_platform::bc_platform::win32>::get_id() const noexcept
		{
			return reinterpret_cast<id>(m_pack.m_handle);
		}

		template<>
		BC_PLATFORMIMP_DLL 
		bcUINT32 bc_platform_basic_window<core_platform::bc_platform::win32>::get_width() const noexcept
		{
			RECT l_rect;
			GetClientRect(m_pack.m_handle, &l_rect);

			return l_rect.right - l_rect.left;
		}

		template<>
		BC_PLATFORMIMP_DLL 
		void bc_platform_basic_window<core_platform::bc_platform::win32>::set_width(bcUINT32 p_width) noexcept
		{
			_update_window_pos_size(m_pack.m_handle, get_left(), get_top(), p_width, get_height());
		}

		template<>
		BC_PLATFORMIMP_DLL 
		bcUINT32 bc_platform_basic_window<core_platform::bc_platform::win32>::get_height() const noexcept
		{
			RECT l_rect;
			GetClientRect(m_pack.m_handle, &l_rect);

			return l_rect.bottom - l_rect.top;
		}

		template<>
		BC_PLATFORMIMP_DLL 
		void bc_platform_basic_window<core_platform::bc_platform::win32>::set_height(bcUINT32 p_height) noexcept
		{
			_update_window_pos_size(m_pack.m_handle, get_left(), get_top(), get_width(), p_height);
		}

		template<>
		BC_PLATFORMIMP_DLL 
		bcUINT32 bc_platform_basic_window<core_platform::bc_platform::win32>::get_left() const noexcept
		{
			POINT point;
			point.x = 0;
			point.y = 0;

			ClientToScreen(m_pack.m_handle, &point);

			return point.x;
		}

		template<>
		BC_PLATFORMIMP_DLL 
		void bc_platform_basic_window<core_platform::bc_platform::win32>::set_left(bcUINT32 p_left) noexcept
		{
			_update_window_pos_size(m_pack.m_handle, p_left, get_top(), get_width(), get_height());
		}

		template<>
		BC_PLATFORMIMP_DLL 
		bcUINT32 bc_platform_basic_window<core_platform::bc_platform::win32>::get_top() const noexcept
		{
			POINT point;
			point.x = 0;
			point.y = 0;

			ClientToScreen(m_pack.m_handle, &point);

			return point.y;
		}

		template<>
		BC_PLATFORMIMP_DLL 
		void bc_platform_basic_window<core_platform::bc_platform::win32>::set_top(bcUINT32 p_top) noexcept
		{
			_update_window_pos_size(m_pack.m_handle, get_left(), p_top, get_width(), get_height());
		}

		template<>
		BC_PLATFORMIMP_DLL 
		void bc_platform_basic_window<core_platform::bc_platform::win32>::set_size(bcUINT32 p_width, bcUINT32 p_height) noexcept
		{
			_update_window_pos_size(m_pack.m_handle, get_left(), get_top(), p_width, p_height);
		}

		template<>
		BC_PLATFORMIMP_DLL 
		void bc_platform_basic_window<core_platform::bc_platform::win32>::set_position(bcUINT32 p_left, bcUINT32 p_top) noexcept
		{
			_update_window_pos_size(m_pack.m_handle, p_left, p_top, get_width(), get_height());
		}

		template<>
		BC_PLATFORMIMP_DLL 
		core::bc_estring bc_platform_basic_window<core_platform::bc_platform::win32>::get_caption() const
		{
			const auto l_title_length = GetWindowTextLength(m_pack.m_handle);
			core::bc_estring l_str(l_title_length, bcL('#'));
			
			GetWindowText(m_pack.m_handle, &l_str[0], l_str.size());

			return l_str;
		}

		template<>
		BC_PLATFORMIMP_DLL 
		void bc_platform_basic_window<core_platform::bc_platform::win32>::set_caption(const bcECHAR* p_caption)
		{
			SetWindowText(m_pack.m_handle, p_caption);
		}

		template<>
		BC_PLATFORMIMP_DLL 
		bool bc_platform_basic_window<core_platform::bc_platform::win32>::is_minimized() const noexcept
		{
			return IsIconic(m_pack.m_handle);
		}

		template<>
		BC_PLATFORMIMP_DLL 
		void bc_platform_basic_window<core_platform::bc_platform::win32>::show(bool p_visible)
		{
			bcINT32 l_show = p_visible ? SW_SHOW : SW_HIDE;

			ShowWindow(m_pack.m_handle, l_show);
		}

		template<>
		BC_PLATFORMIMP_DLL 
		void bc_platform_basic_window<core_platform::bc_platform::win32>::close() noexcept
		{
			DestroyWindow(m_pack.m_handle);
		}

		template<>
		BC_PLATFORMIMP_DLL
		void bc_platform_basic_window<core_platform::bc_platform::win32>::update()
		{
			MSG l_msg;

			// Get window messages and dispatch them to window's procedure
			while (PeekMessage(&l_msg, m_pack.m_handle, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&l_msg);
				DispatchMessage(&l_msg);
			}
		}

		template<>
		BC_PLATFORMIMP_DLL 
		bc_messagebox_value bc_platform_basic_window<core_platform::bc_platform::win32>::messagebox(const bcECHAR* p_caption, 
			const bcECHAR* p_text, 
			bc_messagebox_type p_type, 
			bc_messagebox_buttom p_button)
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

			bcINT32 l_value = MessageBox(m_pack.m_handle, p_text, p_caption, l_type | l_button);

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
		}

		template<>
		BC_PLATFORMIMP_DLL 
		bc_messagebox_value bc_platform_basic_window<core_platform::bc_platform::win32>::messagebox(core::bc_estring_frame p_caption, 
			core::bc_estring_frame p_text, 
			bc_messagebox_type p_type, 
			bc_messagebox_buttom p_button)
		{
			return messagebox(p_caption.c_str(), p_text.c_str(), p_type, p_button);
		}
	}
}