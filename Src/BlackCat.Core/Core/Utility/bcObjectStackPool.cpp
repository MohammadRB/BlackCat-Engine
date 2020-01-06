// [01/04/2020 MRB]

#include "Core/CorePCH.h"
#include "Core/Memory/bcAlloc.h"
#include "Core/Utility/bcObjectStackPool.h"

namespace black_cat
{
	namespace core 
	{
		bc_concurrent_object_stack_pool::bc_concurrent_object_stack_pool() noexcept = default;

		bc_concurrent_object_stack_pool::bc_concurrent_object_stack_pool(bc_concurrent_object_stack_pool&& p_other) noexcept = default;

		bc_concurrent_object_stack_pool::~bc_concurrent_object_stack_pool() = default;

		bc_concurrent_object_stack_pool& bc_concurrent_object_stack_pool::operator=(bc_concurrent_object_stack_pool&& p_other) noexcept = default;

		bcSIZE bc_concurrent_object_stack_pool::capacity() const noexcept
		{
			return m_stack_allocator.size();
		}

		void black_cat::core::bc_concurrent_object_stack_pool::_initialize(bcSIZE p_max_num_thread, bcSIZE p_capacity)
		{
			m_stack_allocator.initialize(p_max_num_thread, p_capacity, "bc_concurrent_object_stack_pool");
		}

		void black_cat::core::bc_concurrent_object_stack_pool::_destroy()
		{
			m_stack_allocator.destroy();
		}

		void* bc_concurrent_object_stack_pool::_alloc(bcSIZE p_size)
		{
			bc_memblock l_memblock;
			l_memblock.size(p_size);

			void* l_memory = m_stack_allocator.alloc(&l_memblock);
			if (!l_memory)
			{
				l_memory = bcAlloc(p_size, bc_alloc_type::unknown);
			}
			
			return l_memory;
		}

		void bc_concurrent_object_stack_pool::_free(void* p_pointer, bcSIZE p_size)
		{
			bc_memblock l_memblock;
			l_memblock.size(p_size);

			if (m_stack_allocator.contain_pointer(p_pointer))
			{
				m_stack_allocator.free(p_pointer, &l_memblock);
			}
			else 
			{
				bcFree(p_pointer);
			}
		}
	}
}