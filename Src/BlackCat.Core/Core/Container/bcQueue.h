// [11/15/2018 MRB]

#pragma once

#include "Core/Container/bcAllocator.h"
#include "Core/Container/bcDeque.h"
#include <queue>

namespace black_cat
{
	namespace core
	{
		template< typename T, typename TAllocator = bc_allocator<T> >
		using bc_queue = std::queue< T, bc_deque<T, TAllocator> >;

		template< typename T, template< typename > class TAllocator >
		using bc_queue_a = bc_queue< T, TAllocator<T> >;

		template< typename T >
		using bc_queue_program = bc_queue_a< T, bc_allocator_program >;

		template< typename T >
		using bc_queue_frame = bc_queue_a< T, bc_allocator_frame >;

		/*template< typename T >
		using bc_queue_movable = bc_queue_a< T, bc_allocator_movable >;*/
	}
}