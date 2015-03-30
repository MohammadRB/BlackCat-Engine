// [11/4/2014 MRB]

#pragma once

#include "coreplatform/bchardwareinfo.h"
#include "CorePlatformImp/CorePlatformImpPCH.h"

namespace black_cat
{
	namespace core_platform
	{
		template<>
		inline void bc_hardware_info_proxy<bc_platform::win32>::get_basic_info(bc_basic_hardware_info* p_info)
		{
			SYSTEM_INFO l_sys_info;
			GetSystemInfo(&l_sys_info);
			ULONGLONG l_mem_size = 0;
			GetPhysicallyInstalledSystemMemory(&l_mem_size);

			switch (l_sys_info.wProcessorArchitecture)
			{
			case PROCESSOR_ARCHITECTURE_INTEL:
				p_info->processor_architecture = bc_processor_architecture::x86;
				break;
			case PROCESSOR_ARCHITECTURE_AMD64:
				p_info->processor_architecture = bc_processor_architecture::x64;
				break;
			case PROCESSOR_ARCHITECTURE_ARM:
				p_info->processor_architecture = bc_processor_architecture::ARM;
				break;
			case PROCESSOR_ARCHITECTURE_IA64:
			case PROCESSOR_ARCHITECTURE_UNKNOWN:
				p_info->processor_architecture = bc_processor_architecture::unknown;
				break;
			}

			p_info->proccessor_count = l_sys_info.dwNumberOfProcessors;
			p_info->page_size = l_sys_info.dwPageSize;
			p_info->memory_size = l_mem_size;
			p_info->app_min_address = l_sys_info.lpMinimumApplicationAddress;
			p_info->app_max_address = l_sys_info.lpMaximumApplicationAddress;
		};
	}
}