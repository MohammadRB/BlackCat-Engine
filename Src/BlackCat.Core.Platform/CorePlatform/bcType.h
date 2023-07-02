// [10/10/2014 MRB]

#pragma once

#include <cstdint>
#include "CorePlatform/CorePlatformPCH.h"

namespace black_cat
{
	using bcBYTE = char;
	using bcUBYTE = unsigned char;
	using bcCHAR = char;
	using bcUCHAR = unsigned char;
	using bcWCHAR = wchar_t;
#ifdef BC_UNICODE
	using bcECHAR = bcWCHAR;
#define  bcL(str) L##str
#else
	using bcECHAR = bcCHAR;
#define bcL(str) str
#endif
	using bcINT8 = int8_t;
	using bcUINT8 = uint8_t;
	using bcINT16 = int16_t;
	using bcUINT16 = uint16_t;
	using bcINT32 = int32_t;
	using bcUINT32 = uint32_t;
	using bcINT = int;
	using bcUINT = unsigned int;
	using bcINT64 = int64_t;
	using bcUINT64 = uint64_t;
	using bcFLOAT = float;
	using bcDOUBLE = double;
	using bcSIZE = size_t;
	using bcPTRDIFF = ptrdiff_t;
	using bcINTPTR = intptr_t;
	using bcUINTPTR = uintptr_t;
	using bcLPCSTR = const char*;
	using bcLPCWSTR = const wchar_t*;
#ifdef BC_UNICODE
	using bcLPCESTR = bcLPCSTR;
#else
	using bcLPCESTR = bcLPCWSTR;
#endif

	inline bcSIZE operator "" _uz(unsigned long long int p_value)
	{
		return p_value;
	}
}