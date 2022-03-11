// [03/01/2022 MRB]

#include "Core/CorePCH.h"
#include "Core/Memory/bcMemoryStack1.h"

namespace black_cat
{
	namespace core
	{
#ifdef BC_MEMORY_ENABLE

		bc_memory_stack1::bc_memory_stack1() noexcept
			: m_capacity(0),
			m_heap(nullptr),
			m_top(nullptr)
		{
		}

		bc_memory_stack1::bc_memory_stack1(bc_memory_stack1&& p_other) noexcept
			: m_capacity(p_other.m_capacity),
			m_heap(p_other.m_heap),
			m_top(p_other.m_top)
		{
			p_other.m_capacity = 0;
			p_other.m_heap = nullptr;
			p_other.m_top = nullptr;
		}

		bc_memory_stack1::~bc_memory_stack1()
		{
			if(is_initialized())
			{
				destroy();
			}
		}

		bc_memory_stack1& bc_memory_stack1::operator=(bc_memory_stack1&& p_other) noexcept
		{
			m_capacity = p_other.m_capacity;
			m_heap = p_other.m_heap;
			m_top = p_other.m_top;
			p_other.m_capacity = 0;
			p_other.m_heap = nullptr;
			p_other.m_top = nullptr;

			return *this;
		}

		void* bc_memory_stack1::alloc(bc_memblock* p_mem_block) noexcept
		{
			const auto l_size = p_mem_block->size();
			const auto l_required_size = l_size + sizeof(_bc_memory_stack_block1);
			bcUBYTE* l_result;

			{
				platform::bc_lock_guard l_lock(m_mutex);

				auto* l_new_top = m_top + l_required_size;
				if (l_new_top > m_heap + m_capacity)
				{
					return nullptr;
				}

				l_result = m_top;
				m_top = l_new_top;

				auto* l_block_pointer = l_result + l_size;
				auto* l_block = new (l_block_pointer) _bc_memory_stack_block1(l_required_size);
			}

			m_tracer.accept_alloc(l_size);
			m_tracer.accept_overhead(sizeof(_bc_memory_stack_block1));

			return l_result;
		}

		void bc_memory_stack1::free(void* p_pointer, bc_memblock* p_mem_block) noexcept
		{
			const auto l_size = p_mem_block->size();
			const auto l_required_size = l_size + sizeof(_bc_memory_stack_block1);
			bool l_freed = false;

			{
				platform::bc_lock_guard l_lock(m_mutex);

				auto* l_new_top = m_top - l_required_size;
				const bool l_is_freeing_top = l_new_top == p_pointer;

				if (l_is_freeing_top)
				{
					m_top = l_new_top;
					l_freed = true;

					_reclaim_free_blocks();
				}
				else
				{
					auto* l_block_pointer = static_cast<bcUBYTE*>(p_pointer) + l_size;
					auto* l_block = reinterpret_cast<_bc_memory_stack_block1*>(l_block_pointer);
					l_block->set_free();
				}
			}

			if (l_freed)
			{
				m_tracer.accept_free(l_size);
				m_tracer.accept_free_overhead(sizeof(_bc_memory_stack_block1));
			}
		}

		void bc_memory_stack1::clear() noexcept
		{
			m_top = m_heap;
			m_tracer.accept_clear();
		}

		void bc_memory_stack1::_initialize(bcSIZE p_capacity, const bcCHAR* p_tag)
		{
			m_capacity = p_capacity;
			m_heap = static_cast<bcUBYTE*>(std::malloc(m_capacity));
			m_top = m_heap;

			bci_memory::tag(p_tag);
			m_tracer.initialize(m_capacity);
		}

		void bc_memory_stack1::_destroy()
		{
			std::free(m_heap);

			m_capacity = 0;
			m_heap = nullptr;
			m_top = nullptr;
		}

		void bc_memory_stack1::_reclaim_free_blocks()
		{
			while (true)
			{
				if (m_top == m_heap)
				{
					break;
				}

				auto* l_block_pointer = m_top - sizeof(_bc_memory_stack_block1);
				auto* l_block = reinterpret_cast<_bc_memory_stack_block1*>(l_block_pointer);

				if (!l_block->get_free())
				{
					break;
				}

				auto* l_new_top = m_top - l_block->get_size();
				m_top = l_new_top;

				m_tracer.accept_free(l_block->get_size() - sizeof(_bc_memory_stack_block1));
				m_tracer.accept_free_overhead(sizeof(_bc_memory_stack_block1));
			}
		}

#endif
	}
}
