// [11/04/2014 MRB]

#pragma once

#include "CorePlatform/CorePlatformPCH.h"
#include "CorePlatform/bcType.h"
#include "CorePlatform/bcPlatform.h"

namespace black_cat
{
	namespace platform
	{
		enum class bc_processor_architecture
		{
			x86,
			x64,
			arm,
			unknown
		};

		struct bc_basic_hardware_info
		{
		public:
			bc_processor_architecture m_processor_architecture;
			bcSIZE m_processor_count;
			bcSIZE m_cache_line_size;
			bcSIZE m_page_size;
			bcUINT64 m_memory_size;
			void* m_app_min_address;
			void* m_app_max_address;
		};

		template<bc_platform TPlatform>
		class bc_platform_hardware_info
		{
		public:
			static void get_basic_info(bc_basic_hardware_info& p_info);
		};

		using bc_hardware_info = bc_platform_hardware_info<g_current_platform>;
	}
}