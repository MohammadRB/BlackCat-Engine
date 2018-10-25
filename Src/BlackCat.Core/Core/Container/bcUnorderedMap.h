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
			class TAllocator = bc_allocator< std::pair< const TKey, TValue > > >
		using bc_unordered_map = std::unordered_map< TKey, TValue, THash, TKeyEqual, TAllocator >;

		template< typename TKey,
			typename TValue,
			class THash = std::hash< TKey >,
			class TKeyEqual = std::equal_to< TKey >,
			template< typename > class TAllocator >
		using bc_unordered_map_a = bc_unordered_map< TKey, TValue, THash, TKeyEqual, TAllocator< std::pair< const TKey, TValue > > >;

		template< typename TKey,
			typename TValue,
			class THash = std::hash< TKey >,
			class TKeyEqual = std::equal_to< TKey > >
		using bc_unordered_map_program = bc_unordered_map_a< TKey, TValue, THash, TKeyEqual, bc_allocator_program >;

		template< typename TKey,
			typename TValue,
			class THash = std::hash< TKey >,
			class TKeyEqual = std::equal_to< TKey > >
		using bc_unordered_map_level = bc_unordered_map_a< TKey, TValue, THash, TKeyEqual, bc_allocator_level >;

		template< typename TKey,
			typename TValue,
			class THash = std::hash< TKey >,
			class TKeyEqual = std::equal_to< TKey > >
		using bc_unordered_map_frame = bc_unordered_map_a< TKey, TValue, THash, TKeyEqual, bc_allocator_frame >;

		/*template< typename TKey,
			typename TValue,
			class THash = std::hash< TKey >,
			class TKeyEqual = std::equal_to< TKey > >
		using bc_unordered_map_movale = bc_unordered_map_a< TKey, TValue, THash, TKeyEqual, bc_allocator_movable >;*/
	}
}
