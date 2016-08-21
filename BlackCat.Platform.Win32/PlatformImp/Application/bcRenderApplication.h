// [2/26/2015 MRB]

#pragma once

#include "PlatformImp/Application/bcRenderWindow.h"

#include "Platform/Application/bcRenderApplication.h"
#include "PlatformImp/PlatformImpPCH.h"

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
	}
}