// [11/1/2015 MRB]

#pragma once

#include "Core/CorePCH.h"

namespace black_cat::core
{
	template<class T, bcSIZE TCount>
	using bc_array = std::array<T, TCount>;
}
