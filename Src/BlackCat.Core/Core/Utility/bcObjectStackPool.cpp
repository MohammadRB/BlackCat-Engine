// [01/04/2020 MRB]

#include "Core/CorePCH.h"
#include "Core/Memory/bcAlloc.h"
#include "Core/Utility/bcObjectStackPool.h"

namespace black_cat
{
	namespace core 
	{
		bc_concurrent_object_stack_pool::bc_concurrent_object_stack_pool() noexcept
			: m_size(0)
		{
		}

		bc_concurrent_object_stack_pool::bc_concurrent_object_stack_pool(bc_concurrent_object_stack_pool&& p_other) noexcept
			: m_stack_allocator(std::move(p_other.m_stack_allocator)),
			m_size(p_other.m_size.load(core_platform::bc_memory_order::relaxed))
		{	
		}

		bc_concurrent_object_stack_pool::~bc_concurrent_object_stack_pool()
		{
			if(m_stack_allocator.is_initialized())
			{
				m_stack_allocator.destroy();
			}
		}

		bc_concurrent_object_stack_pool& bc_concurrent_object_stack_pool::operator=(bc_concurrent_object_stack_pool&& p_other) noexcept
		{
			m_stack_allocator = std::move(p_other.m_stack_allocator);
			m_size.store(p_other.m_size.load(core_platform::bc_memory_order::relaxed), core_platform::bc_memory_order::relaxed);

			return *this;
		}

		void bc_concurrent_object_stack_pool::_initialize(bcSIZE p_capacity)
		{
			m_stack_allocator.initialize(p_capacity, "concurrent_object_stack_pool");
		}

		void bc_concurrent_object_stack_pool::_destroy()
		{
			BC_ASSERT(m_size.load(core_platform::bc_memory_order::relaxed) == 0);
			m_stack_allocator.destroy();
		}

		void* bc_concurrent_object_stack_pool::_alloc(bcSIZE p_size)
		{
			bc_memblock l_memblock;
			l_memblock.size(p_size);

			void* l_memory = m_stack_allocator.alloc(&l_memblock);
			if (!l_memory)
			{
				l_memory = BC_ALLOC(p_size, bc_alloc_type::unknown);
			}
			
			return l_memory;
		}

		void bc_concurrent_object_stack_pool::_free(void* p_pointer, bcSIZE p_size)
		{
			if (m_stack_allocator.contain_pointer(p_pointer))
			{
				bc_memblock l_memblock;
				l_memblock.size(p_size);
				
				m_stack_allocator.free(p_pointer, &l_memblock);
			}
			else 
			{
				BC_FREE(p_pointer);
			}
		}
	}
}