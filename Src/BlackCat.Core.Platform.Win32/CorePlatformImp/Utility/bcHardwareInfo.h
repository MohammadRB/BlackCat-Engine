// [04/11/2014 MRB]

#pragma once

#include <new>
#include "CorePlatform/Utility/bcHardwareInfo.h"
#include "CorePlatformImp/CorePlatformImpPCH.h"

namespace black_cat
{
	namespace platform
	{
		template<>
		inline void bc_platform_hardware_info<bc_platform::win32>::get_basic_info(bc_basic_hardware_info& p_info)
		{
#ifdef __cpp_lib_hardware_interference_size
			using std::hardware_constructive_interference_size;
			using std::hardware_destructive_interference_size;
#else
			constexpr std::size_t hardware_constructive_interference_size = 64;
			constexpr std::size_t hardware_destructive_interference_size = 64;
#endif

			SYSTEM_INFO l_sys_info;
			GetSystemInfo(&l_sys_info);

			ULONGLONG l_mem_size = 0;
			GetPhysicallyInstalledSystemMemory(&l_mem_size);

			switch (l_sys_info.wProcessorArchitecture)
			{
			case PROCESSOR_ARCHITECTURE_INTEL:
				p_info.m_processor_architecture = bc_processor_architecture::x86;
				break;
			case PROCESSOR_ARCHITECTURE_AMD64:
				p_info.m_processor_architecture = bc_processor_architecture::x64;
				break;
			case PROCESSOR_ARCHITECTURE_ARM:
				p_info.m_processor_architecture = bc_processor_architecture::arm;
				break;
			case PROCESSOR_ARCHITECTURE_IA64:
			case PROCESSOR_ARCHITECTURE_UNKNOWN:
				p_info.m_processor_architecture = bc_processor_architecture::unknown;
				break;
			default: 
				p_info.m_processor_architecture = bc_processor_architecture::unknown;
				break;
			}

			p_info.m_processor_count = l_sys_info.dwNumberOfProcessors;
			p_info.m_cache_line_size = hardware_constructive_interference_size;
			p_info.m_page_size = l_sys_info.dwPageSize;
			p_info.m_memory_size = l_mem_size;
			p_info.m_app_min_address = l_sys_info.lpMinimumApplicationAddress;
			p_info.m_app_max_address = l_sys_info.lpMaximumApplicationAddress;
		};
	}
}