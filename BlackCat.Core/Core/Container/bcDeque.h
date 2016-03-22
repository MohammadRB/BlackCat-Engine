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
		template< typename T, typename TAllocator = bc_allocator<T> >
		using bc_deque = std::deque< T, TAllocator >;

		template< typename T, template< typename > typename TAllocator >
		using bc_deque_a = bc_deque< T, TAllocator<T> >;

		template< typename T >
		using bc_deque_program = bc_deque_a< T, bc_allocator_program >;

		template< typename T >
		using bc_deque_level = bc_deque_a< T, bc_allocator_level >;

		template< typename T >
		using bc_deque_frame = bc_deque_a< T, bc_allocator_frame >;

		/*template< typename T >
		using bc_deque_movale = bc_deque_a< T, bc_allocator_movable >;*/
	}
}