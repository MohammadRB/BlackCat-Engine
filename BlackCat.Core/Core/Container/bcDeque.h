// [1/24/2015 MRB]

#pragma once

#include "Core/CorePCH.h"
#include "Core/Container/bcAllocator.h"
#include "Core/Container/bcContainer.h"
#include <deque>

namespace black_cat
{
	namespace core
	{
		template< typename T, typename TAllocator = bc_std_allocator<T> >
		using bc_deque = std::deque< T, TAllocator >;
	}
}