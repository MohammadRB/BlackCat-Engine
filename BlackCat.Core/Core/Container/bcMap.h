// [1/11/2015 MRB]

#pragma once

#include "Core/CorePCH.h"
#include "Core/Container/bcTree.h"

namespace black_cat
{
	namespace core
	{
		template< class TK, typename T, class TCompare = std::less< TK >, class TAllocator = bc_allocator< std::pair< const TK, T > > >
		using bc_map = bc_tree< TK, T, 2, TCompare, TAllocator >;

		template< class TK, typename T, class TCompare = std::less< TK >, template< typename > class TAllocator >
		using bc_map_a = bc_map< TK, T, TCompare, TAllocator< std::pair< const TK, T > > >;

		template< class TK, typename T, class TCompare = std::less< TK >, template< typename > class TAllocator >
		using bc_map_program = bc_map_a< TK, T, TCompare, bc_allocator_program >;

		template< class TK, typename T, class TCompare = std::less< TK > >
		using bc_map_level = bc_map_a< TK, T, TCompare, bc_allocator_level >;

		template< class TK, typename T, class TCompare = std::less< TK > >
		using bc_map_frame = bc_map_a< TK, T, TCompare, bc_allocator_frame >;

		template< class TK, typename T, class TCompare = std::less< TK > >
		using bc_map_movale = bc_map_a< TK, T, TCompare, bc_allocator_movable >;
	}
}