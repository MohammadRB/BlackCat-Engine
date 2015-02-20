// [1/11/2015 MRB]

#pragma once

#include "Core/CorePCH.h"
#include "Core/Container/bcTree.h"

namespace black_cat
{
	namespace core
	{
		template< class TK, typename T, class TCompare = std::less< TK >, class TAllocator = bc_allocator< std::pair<const TK, T> > >
		using bc_map = bc_tree < TK, T, 2, TCompare, TAllocator >;
	}
}