// [03/24/2015 MRB]

#include "PlatformImp/PlatformImpPCH.h"
#include "PlatformImp/Application/bcRenderWindow.h"
#include "PlatformImp/Application/bcRenderApplication.h"

namespace black_cat
{
	namespace platform
	{
		template< >
		BC_PLATFORMIMP_DLL_EXP bc_platform_render_application<core_platform::bc_platform::win32>::bc_platform_render_application(bc_render_application_parameter& p_parameter)
			: m_pack(p_parameter.m_instance)
		{
		};
		
		template< >
		BC_PLATFORMIMP_DLL_EXP bc_platform_render_application<core_platform::bc_platform::win32>::~bc_platform_render_application()
		{
		};

		template< >
		BC_PLATFORMIMP_DLL_EXP bc_render_window bc_platform_render_application< core_platform::bc_platform::win32 >::create_render_window(core::bc_estring p_caption,
			bcUINT32 p_width,
			bcUINT32 p_height)
		{
			bc_render_window_parameter l_parameters(m_pack.m_instance, std::move(p_caption), p_width, p_height);
			bc_render_window l_render_window(l_parameters);

			return std::move(l_render_window);
		};

		template< >
		BC_PLATFORMIMP_DLL_EXP void bc_platform_render_application< core_platform::bc_platform::win32 >::update(bc_render_window& p_render_window)
		{
			MSG l_msg;

			// Get render window messages and dispatch them to window's procedure
			auto& l_win32_render_window = p_render_window.get_platform_pack();
			while (PeekMessage(&l_msg, l_win32_render_window.m_handle, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&l_msg);
				DispatchMessage(&l_msg);
			}

			// Get thread messages that doesn't belong to render window
			while (PeekMessage(&l_msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&l_msg);

				if (l_msg.message == WM_QUIT)
				{
					bc_app_event_exit l_exit_event(l_msg.wParam);

					core::bc_service_manager::get().get_service<core::bc_event_manager>()->process_event(l_exit_event);
				}
			}
		};

		template< >
		BC_PLATFORMIMP_DLL_EXP void bc_platform_render_application<core_platform::bc_platform::win32>::request_termination()
		{
			PostQuitMessage(0);
		};
	}
}