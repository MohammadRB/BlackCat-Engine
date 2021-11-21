// [11/21/2021 MRB]

#pragma once

#include "Core/CorePCH.h"
#include "Core/Container/bcAllocator.h"
#include "Core/Container/bcContainer.h"
#include "Core/Container/bcDeque.h"
#include <stack>

namespace black_cat
{
	namespace core
	{
		template<typename T, typename TContainer = bc_deque<T>>
		using bc_stack = std::stack<T, TContainer>;

		template<typename T, template<typename> typename TAllocator>
		using bc_stack_a = bc_stack<T, bc_deque_a<T, TAllocator>>;

		template<typename T>
		using bc_stack_program = bc_stack_a<T, bc_allocator_program>;

		template<typename T>
		using bc_stack_frame = bc_stack_a<T, bc_allocator_frame>;

		/*template<typename T>
		using bc_stack_movale = bc_stack_a<T, bc_allocator_movable>;*/
	}
}