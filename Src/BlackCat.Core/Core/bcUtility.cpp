// [14/09/2016 MRB]

#include "Core/CorePCH.h"
#include "Core/bcUtility.h"

namespace 
{
	using namespace black_cat;

	bcUINT as_uint(float p_float) {
		return *reinterpret_cast<bcUINT*>(&p_float);
	}

	bcFLOAT as_float(bcUINT p_int) {
		return *reinterpret_cast<bcFLOAT*>(&p_int);
	}
}

namespace black_cat
{
	// https://stackoverflow.com/a/60047308/2279977
	bcFLOAT bc_half_to_float(bcSHORT p_half) {
		const bcUINT e = (p_half & 0x7C00) >> 10;
		const bcUINT m = (p_half & 0x03FF) << 13;
		const bcUINT v = as_uint(static_cast<float>(m)) >> 23;
		return as_float((p_half & 0x8000) << 16 | (e != 0) * ((e + 112) << 23 | m) | ((e == 0) & (m != 0)) * ((v - 37) << 23 | ((m << (150 - v)) & 0x007FE000)));
	}

	bcSHORT bc_float_to_half(const float p_float) {
		const bcUINT b = as_uint(p_float) + 0x00001000;
		const bcUINT e = (b & 0x7F800000) >> 23;
		const bcUINT m = b & 0x007FFFFF;
		return (b & 0x80000000) >> 16 | (e > 112) * ((((e - 112) << 10) & 0x7C00) | m >> 13) | ((e < 113) & (e > 101)) * ((((0x007FF000 + m) >> (125 - e)) + 1) >> 1) | (e > 143) * 0x7FFF;
	}
}
