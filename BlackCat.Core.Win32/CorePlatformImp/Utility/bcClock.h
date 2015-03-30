// [2/13/2015 MRB]

#pragma once

#include "CorePlatformImp/CorePlatformImpPCH.h"
#include "CorePlatform/Utility/bcClock.h"

namespace black_cat
{
	namespace core_platform
	{
		template<>
		struct bc_clock_pack<bc_platform::win32>
		{
			using large_clock_type = bcUINT64;
			using little_clock_type = bcUINT32;
			using large_time_delta_type = bcDOUBLE64;
			using little_time_delta_type = bcFLOAT32;
		};

		template< >
		inline bc_clock_proxy<bc_platform::win32>::large_clock_type bc_clock_proxy<bc_platform::win32>::_query_clock_per_millisecond()
		{
			large_clock_type l_clock_per_millisecond;

			QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&l_clock_per_millisecond));
			l_clock_per_millisecond /= 1000;

			return l_clock_per_millisecond;
		}

		template< >
		inline bc_clock_proxy<bc_platform::win32>::large_clock_type bc_clock_proxy<bc_platform::win32>::_query_elapsed_clocks()
		{
			large_clock_type l_clocks;

			QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&l_clocks));

			return l_clocks;
		};
	}
}