// [03/24/2015 MRB]

#include "PlatformImp/PlatformImpPCH.h"

#include "Platform/bcEvent.h"
#include "PlatformImp/bcExport.h"
#include "PlatformImp/Application/bcBasicWindow.h"
#include "PlatformImp/Application/bcApplication.h"

namespace black_cat
{
	namespace platform
	{
		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_application<core_platform::bc_platform::win32>::bc_platform_application(bc_application_parameter& p_parameter)
			: m_pack(p_parameter.m_instance)
		{
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_application<core_platform::bc_platform::win32>::bc_platform_application(bc_platform_application&& p_other) noexcept
			: m_pack(p_other.m_pack)
		{
			p_other.m_pack.m_instance = nullptr;
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_application<core_platform::bc_platform::win32>::~bc_platform_application()
		{
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_application<core_platform::bc_platform::win32>& bc_platform_application<core_platform::bc_platform::win32>::operator=(bc_platform_application&& p_other) noexcept
		{
			m_pack.m_instance = p_other.m_pack.m_instance;

			p_other.m_pack.m_instance = nullptr;

			return *this;
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_basic_window bc_platform_application<core_platform::bc_platform::win32>::create_basic_window(core::bc_estring p_caption, bcUINT32 p_width, bcUINT32 p_height)
		{
			bc_basic_window::parameter l_parameters(m_pack.m_instance, std::move(p_caption), p_width, p_height);
			bc_basic_window l_render_window(l_parameters);

			return l_render_window;
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_console_window bc_platform_application<core_platform::bc_platform::win32>::create_console_window(core::bc_estring p_caption)
		{
			bc_console_window::parameter l_parameters(std::move(p_caption));
			bc_console_window l_console_window(l_parameters);

			return l_console_window;
		}

		template<>
		BC_PLATFORMIMP_DLL
		void bc_platform_application<core_platform::bc_platform::win32>::update()
		{
			MSG l_msg;

			// Get thread messages that doesn't belong to any window
			while (PeekMessage(&l_msg, reinterpret_cast<HWND>(-1), 0, 0, PM_REMOVE))
			{
				TranslateMessage(&l_msg);

				if (l_msg.message == WM_QUIT)
				{
					bc_app_event_exit l_exit_event(l_msg.wParam);
					core::bc_get_service<core::bc_event_manager>()->process_event(l_exit_event);
				}
			}
		}

		template<>
		BC_PLATFORMIMP_DLL
		void bc_platform_application<core_platform::bc_platform::win32>::request_termination()
		{
			PostQuitMessage(0);
		}
	}
}