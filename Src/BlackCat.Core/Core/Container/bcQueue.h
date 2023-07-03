// [15/11/2018 MRB]

#pragma once

#include "Core/Container/bcAllocator.h"
#include "Core/Container/bcDeque.h"
#include <queue>

namespace black_cat
{
	namespace core
	{
		template<typename T, typename TAllocator = bc_allocator<T>>
		using bc_queue = std::queue<T, bc_deque<T, TAllocator>>;

		template<typename T, template<typename> class TAllocator>
		using bc_queue_a = bc_queue<T, TAllocator<T>>;

		template<typename T, bcSIZE TAlign, template<typename, bcSIZE> class TAllocator>
		using bc_queue_aa = bc_queue<T, TAllocator<T, TAlign>>;

		template<typename T, bcSIZE TAlign>
		using bc_queue_aligned = bc_queue_aa<T, TAlign, bc_aligned_allocator>;

		template<typename T>
		using bc_queue_program = bc_queue_a<T, bc_allocator_program>;

		template<typename T, bcSIZE TAlign>
		using bc_queue_program_aligned = bc_queue_aa<T, TAlign, bc_aligned_allocator_program>;

		template<typename T>
		using bc_queue_frame = bc_queue_a<T, bc_allocator_frame>;

		template<typename T, bcSIZE TAlign>
		using bc_queue_frame_aligned = bc_queue_aa<T, TAlign, bc_aligned_allocator_frame>;
	}
}