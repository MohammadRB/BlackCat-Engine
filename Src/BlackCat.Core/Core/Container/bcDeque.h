// [24/1/2015 MRB]

#pragma once

#include "Core/CorePCH.h"
#include "Core/Container/bcAllocator.h"
#include "Core/Container/bcContainer.h"
#include <deque>

namespace black_cat::core
{
	template<typename T, typename TAllocator = bc_allocator<T>>
	using bc_deque = std::deque<T, TAllocator>;

	template<typename T, template<typename> typename TAllocator>
	using bc_deque_a = bc_deque<T, TAllocator<T>>;

	template<typename T, bcSIZE TAlign, template<typename, bcSIZE> class TAllocator>
	using bc_deque_aa = bc_deque<T, TAllocator<T, TAlign>>;

	template<typename T, bcSIZE TAlign>
	using bc_deque_aligned = bc_deque_aa<T, TAlign, bc_aligned_allocator>;

	template<typename T>
	using bc_deque_program = bc_deque_a<T, bc_allocator_program>;

	template<typename T, bcSIZE TAlign>
	using bc_deque_program_aligned = bc_deque_aa<T, TAlign, bc_aligned_allocator_program>;

	template<typename T>
	using bc_deque_frame = bc_deque_a<T, bc_allocator_frame>;

	template<typename T, bcSIZE TAlign>
	using bc_deque_frame_aligned = bc_deque_aa<T, TAlign, bc_aligned_allocator_frame>;
}
