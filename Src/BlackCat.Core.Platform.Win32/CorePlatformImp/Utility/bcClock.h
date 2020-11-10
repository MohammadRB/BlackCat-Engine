// [2/13/2015 MRB]

#pragma once

#include "CorePlatformImp/CorePlatformImpPCH.h"
#include "CorePlatform/Utility/bcClock.h"

namespace black_cat
{
	namespace core_platform
	{
		template<>
		struct bc_platform_clock_pack<bc_platform::win32>
		{
			using big_clock = bcUINT64;
			using small_clock = bcUINT32;
			using big_delta_time = bcDOUBLE;
			using small_delta_time = bcFLOAT;
		};

		template< >
		inline bc_platform_clock<bc_platform::win32>::big_clock bc_platform_clock<bc_platform::win32>::query_clock_per_millisecond()
		{
			big_clock l_clock_per_millisecond;

			QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&l_clock_per_millisecond));
			l_clock_per_millisecond /= 1000;

			return l_clock_per_millisecond;
		}

		template< >
		inline bc_platform_clock<bc_platform::win32>::big_clock bc_platform_clock<bc_platform::win32>::query_elapsed_clocks()
		{
			big_clock l_clocks;

			QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&l_clocks));

			return l_clocks;
		}
	}
}