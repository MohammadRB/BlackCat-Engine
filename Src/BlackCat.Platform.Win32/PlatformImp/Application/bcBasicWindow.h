// [2/27/2015 MRB]

#pragma once

#include "PlatformImp/Application/bcWindow.h"

#include "Core/Container/bcString.h"
#include "PlatformImp/PlatformImpPCH.h"
#include "Platform/Application/bcBasicWindow.h"

namespace black_cat
{
	namespace platform
	{
		template<>
		struct bc_platform_basic_window_parameter_pack<bc_platform::win32> : bc_platform_basic_window_parameter_pack<bc_platform::unknown>
		{
		public:
			bc_platform_basic_window_parameter_pack(HINSTANCE p_instance, 
				const bcECHAR* p_caption,
				bcUINT32 p_width, 
				bcUINT32 p_height)
				: bc_platform_basic_window_parameter_pack<bc_platform::unknown>(p_caption, p_width, p_height),
				m_instance(p_instance)
			{
			}

			HINSTANCE m_instance;
		};

		template<>
		struct bc_platform_basic_window_pack<bc_platform::win32> : bc_platform_window_pack<bc_platform::win32>
		{
			HINSTANCE m_instance;
			HWND m_handle;
		};
	}
}