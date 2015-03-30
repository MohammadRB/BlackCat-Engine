// [2/27/2015 MRB]

#pragma once

#include "PlatformImp/PlatformImpPCH.h"
#include "Platform/Application/bcRenderWindow.h"

namespace black_cat
{
	namespace platform
	{
		template<>
		struct bc_render_window_parameter_pack< core_platform::bc_platform::win32 > 
			: public bc_render_window_parameter_pack< core_platform::bc_platform::unknown >
		{
		private:
			using base_type = bc_render_window_parameter_pack< core_platform::bc_platform::unknown >;

		public:
			bc_render_window_parameter_pack(HINSTANCE p_instance, 
				core::bc_estring p_caption, 
				bcUINT32 p_width, 
				bcUINT32 p_height)
				: base_type(std::move(p_caption), p_width, p_height),
				m_instance(p_instance)
			{
			}

			HINSTANCE m_instance;
		};

		template<>
		struct bc_render_window_pack< core_platform::bc_platform::win32 >
			: public bc_render_window_pack< core_platform::bc_platform::unknown >
		{
			bc_render_window_pack(HINSTANCE p_instance, core::bc_estring p_string)
				: m_instance(p_instance),
				m_caption(std::move(p_string))
			{
			}

			HINSTANCE m_instance;
			HWND m_handle;
			core::bc_estring m_caption;
		};
	}
}