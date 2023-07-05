// [21/11/2021 MRB]

#pragma once

#include "Core/CorePCH.h"
#include "Core/Container/bcAllocator.h"
#include "Core/Container/bcContainer.h"
#include "Core/Container/bcDeque.h"
#include <stack>

namespace black_cat::core
{
	template<typename T, typename TContainer = bc_deque<T>>
	using bc_stack = std::stack<T, TContainer>;

	template<typename T, template<typename> typename TAllocator>
	using bc_stack_a = bc_stack<T, bc_deque_a<T, TAllocator>>;

	template<typename T, bcSIZE TAlign, template<typename, bcSIZE> class TAllocator>
	using bc_stack_aa = bc_stack<T, TAllocator<T, TAlign>>;

	template<typename T, bcSIZE TAlign>
	using bc_stack_aligned = bc_stack_aa<T, TAlign, bc_aligned_allocator>;

	template<typename T>
	using bc_stack_program = bc_stack_a<T, bc_allocator_program>;

	template<typename T, bcSIZE TAlign>
	using bc_stack_program_aligned = bc_stack_aa<T, TAlign, bc_aligned_allocator_program>;

	template<typename T>
	using bc_stack_frame = bc_stack_a<T, bc_allocator_frame>;

	template<typename T, bcSIZE TAlign>
	using bc_stack_frame_aligned = bc_stack_aa<T, TAlign, bc_aligned_allocator_frame>;
}
