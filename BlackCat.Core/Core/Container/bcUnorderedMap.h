// [2/6/2015 MRB]

#pragma once

#include "Core/CorePCH.h"
#include "Core/Container/bcContainer.h"
#include <unordered_map>

namespace black_cat
{
	namespace core
	{
		template< typename TKey,
			typename TValue,
			class THash = std::hash< TKey >,
			class TKeyEqual = std::equal_to< TKey >,
			class TAllocator = bc_std_allocator< std::pair< const TKey, TValue > > >
		using bc_unordered_map = std::unordered_map< TKey, TValue, THash, TKeyEqual, TAllocator >;
	}
}