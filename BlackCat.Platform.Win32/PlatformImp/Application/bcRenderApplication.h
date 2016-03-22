// [2/26/2015 MRB]

#pragma once

#include "Platform/Application/bcRenderApplication.h"
#include "PlatformImp/PlatformImpPCH.h"
#include "PlatformImp/Application/bcRenderWindow.h"

namespace black_cat
{
	namespace platform
	{
		template<>
		struct bc_platform_render_application_parameter_pack< core_platform::bc_platform::win32 >
			: public bc_platform_render_application_parameter_pack< core_platform::bc_platform::unknown >
		{
		private:
			using base_type = bc_platform_render_application_parameter_pack< core_platform::bc_platform::unknown >;

		public:
			bc_platform_render_application_parameter_pack(HINSTANCE p_instance,
				const bcECHAR* p_app_name,
				bcUINT32 p_window_width,
				bcUINT32 p_window_height,
				bc_engine_components_parameters p_engine_components,
				const bcCHAR* p_commandline)
				: base_type(p_app_name, p_window_width, p_window_height, p_engine_components, p_commandline),
				m_instance(p_instance)
			{
			}

			HINSTANCE m_instance;
		};

		template<>
		struct bc_platform_render_application_pack< core_platform::bc_platform::win32 > 
			: public bc_platform_render_application_pack< core_platform::bc_platform::unknown >
		{
			explicit bc_platform_render_application_pack(HINSTANCE p_instance)
				: m_instance(p_instance)
			{
			}

			HINSTANCE m_instance;
		};

		//template< >
		//inline bc_render_application_proxy<core_platform::bc_platform::win32>::bc_render_application_proxy(bc_render_application_parameter& p_parameter)
		//	: m_pack(p_parameter.m_instance)
		//{
		//};

		//template< >
		//inline bc_render_application_proxy<core_platform::bc_platform::win32>::~bc_render_application_proxy()
		//{
		//};

		//template< >
		//inline bc_render_window bc_render_application_proxy< core_platform::bc_platform::win32 >::create_render_window(core::bc_estring p_caption,
		//	bcUINT32 p_width,
		//	bcUINT32 p_height)
		//{
		//	bc_render_window_parameter l_parameters(m_pack.m_instance, std::move(p_caption), p_width, p_height);
		//	bc_render_window l_render_window(l_parameters);

		//	return std::move(l_render_window);
		//};

		//template< >
		//inline void bc_render_application_proxy< core_platform::bc_platform::win32 >::update(bc_render_window& p_render_window)
		//{
		//	MSG l_msg;

		//	// Get render window messages and dispatch them to window's procedure
		//	auto& l_win32_render_window = p_render_window.get_platform_pack();
		//	while (PeekMessage(&l_msg, l_win32_render_window.m_handle, 0, 0, PM_REMOVE))
		//	{
		//		TranslateMessage(&l_msg);
		//		DispatchMessage(&l_msg);
		//	}

		//	// Get thread messages that doesn't belong to render window
		//	while (PeekMessage(&l_msg, nullptr, 0, 0, PM_REMOVE))
		//	{
		//		TranslateMessage(&l_msg);

		//		if (l_msg.message == WM_QUIT)
		//		{
		//			bc_app_event_exit l_exit_event(l_msg.wParam);

		//			core::bc_service_manager::get().global_event_manager().process_event(l_exit_event);
		//		}
		//	}
		//};

		//template< >
		//inline void bc_render_application_proxy<core_platform::bc_platform::win32>::request_termination()
		//{
		//	PostQuitMessage(0);
		//};
	}
}

/*
template<>
class bc_render_application_provider< core_platform::bc_platform::win32 >
: private bc_render_application_provider< core_platform::bc_platform::unknown >
{
public:
bc_render_application_provider(bc_render_application_parameter_pack& p_platform_parameters)
: m_instance(p_platform_parameters.m_instance)
{
}

~bc_render_application_provider() = default;

HINSTANCE instance() const noexcept(true)
{
return m_instance;
}

void update(bc_render_window& p_render_window)
{
MSG l_msg;

// Get render window messages and dispatch them to window's procedure
auto& l_win32_render_window = static_cast<bc_render_window_provider<core_platform::bc_platform::win32>&>(p_render_window);
while (PeekMessage(&l_msg, l_win32_render_window.handle(), 0, 0, PM_REMOVE))
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
bc_event_app_exit l_exit_event(l_msg.wParam);

core::bc_service_manager::get().global_event_manager().process_event(l_exit_event);
}
}
}

void request_termination()
{
PostQuitMessage(0);
}

bc_render_window create_render_window(core::bc_estring p_caption, bcUINT32 p_width, bcUINT32 p_height)
{
bc_render_window_parameter_pack l_parameters(m_instance, std::move(p_caption), p_width, p_height);
bc_render_window l_render_window(l_parameters);

return std::move(l_render_window);
}

protected:

private:
HINSTANCE m_instance;
};
*/