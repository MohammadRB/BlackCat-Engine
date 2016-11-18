// [2/26/2015 MRB]

#pragma once

#include "PlatformImp/Application/bcBasicWindow.h"
#include "PlatformImp/Application/bcConsoleWindow.h"

#include "Platform/Application/bcApplication.h"
#include "PlatformImp/PlatformImpPCH.h"

namespace black_cat
{
	namespace platform
	{
		template<>
		struct bc_platform_application_parameter_pack< core_platform::bc_platform::win32 >
			: public bc_platform_application_parameter_pack< core_platform::bc_platform::unknown >
		{
		private:
			using base_type = bc_platform_application_parameter_pack< core_platform::bc_platform::unknown >;

		public:
			bc_platform_application_parameter_pack(HINSTANCE p_instance,
				const bcECHAR* p_app_name,
				const bcCHAR* p_commandline)
				: base_type(p_app_name, p_commandline),
				m_instance(p_instance)
			{
			}

			HINSTANCE m_instance;
		};

		template<>
		struct bc_platform_application_pack< core_platform::bc_platform::win32 > 
		{
			explicit bc_platform_application_pack(HINSTANCE p_instance)
				: m_instance(p_instance)
			{
			}

			HINSTANCE m_instance;
		};
	}
}