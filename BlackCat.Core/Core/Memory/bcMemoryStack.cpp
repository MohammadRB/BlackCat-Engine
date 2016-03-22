//  [8/24/2013 MRB]

#include "Core/CorePCH.h"
#include "Core/Memory/bcMemoryStack.h"

namespace black_cat
{
	namespace core
	{
#ifdef BC_MEMORY_ENABLE

		bc_memory_stack::bc_memory_stack() noexcept(true)
		{
		};

		bc_memory_stack::bc_memory_stack(bc_memory_stack::this_type&& p_other) noexcept(true) : bc_memory(std::move(p_other))
		{
			_move(std::move(p_other));
		};

		bc_memory_stack::~bc_memory_stack() noexcept(true)
		{
			if (m_initialized)
				_destroy();
		};

		bc_memory_stack::this_type& bc_memory_stack::operator =(bc_memory_stack::this_type&& p_other) noexcept(true)
		{
			bc_memory::operator=(std::move(p_other));
			_move(std::move(p_other));

			return *this;
		}

		void bc_memory_stack::_initialize(bcUINT32 p_size, const bcCHAR* p_tag)
		{
			m_size = p_size;

			// We alloc mHeap by min align defined in CorePCH because we want all of our allocations have MIN_Align /
			m_heap = static_cast<bcUBYTE*>(core_platform::bc_mem_aligned_alloc(m_size * sizeof(bcUBYTE), BC_MEMORY_MIN_ALIGN));

			if (!m_heap)
				throw std::bad_alloc();

			m_top.store(m_heap, core_platform::bc_memory_order::seqcst);

			m_tracer.initialize(m_size);
			if (p_tag) bc_memory::tag(p_tag);
		};

		void bc_memory_stack::_destroy() noexcept(true)
		{
			core_platform::bc_mem_aligned_free(m_heap);
		};

		void* bc_memory_stack::push(bc_memblock* p_mem_block) noexcept(true)
		{
			void* l_result = nullptr;
			bcSIZE l_size = p_mem_block->size();

			bcAssert(l_size > 0);

			bcUBYTE* l_local_top = m_top.load(core_platform::bc_memory_order::seqcst);
			while (true)
			{
				bc_memblock* l_last_mem_block = nullptr;
				bcSIZE l_byte_free = 0;
				if (l_local_top != m_heap)
				{
					l_last_mem_block = bc_memblock::retrieve_mem_block(l_local_top);
					l_byte_free = 
						(reinterpret_cast<bcSIZE>(m_heap)+m_size) -
						(reinterpret_cast<bcSIZE>(l_local_top)-l_last_mem_block->offset() + l_last_mem_block->size());
				}
				else
				{
					l_byte_free = m_size;
				}
				
				if (l_size <= l_byte_free)
				{
					bcUINT32 l_offset;
					bcUBYTE* l_new_top;
					bcUBYTE* l_allocation_top;

					// m_top must point to last allocation space plus it's offset /
					if (l_local_top != m_heap)
					{
						l_allocation_top = l_local_top - l_last_mem_block->offset() + l_last_mem_block->size();
					}
					else
					{
						l_allocation_top = l_local_top;
					}

					l_offset = bc_memblock::get_required_offset_for_data(l_allocation_top, p_mem_block->alignment());
					l_new_top = l_allocation_top + l_offset;

					if (m_top.compare_exchange_strong(&l_local_top,
						l_new_top,
						core_platform::bc_memory_order::seqcst,
						core_platform::bc_memory_order::seqcst))
					{
						l_result = l_allocation_top;

						// MemBlock::mExtra must point to previous allocated data so we can extract it's MemBlock in 
						// pop function in order to free it if that block was free /
						p_mem_block->allocators_extra(l_local_top);

						m_tracer.accept_alloc(l_size);

						return l_result;
					}
					else
						continue;
				}
				else // No more free memory /
				{
					m_tracer.reject_alloc(l_size);

					return l_result;
				}
			}
		};
		
		void bc_memory_stack::pop(void* p_pointer, bc_memblock* p_mem_block) noexcept(true)
		{
			bcUINTPTR l_free_position = 0;
			bcSIZE l_free_size = 0;

			bool l_back_trace = false;

			bcUBYTE* l_new_top;
			bcUBYTE* l_local_top;
			while (true)
			{
				l_local_top = m_top.load(core_platform::bc_memory_order::seqcst);

				if (l_local_top == m_heap) // it is free
				{
					bcAssert(false);
					return;
				}

				bc_memblock* l_top_mem_block = bc_memblock::retrieve_mem_block(l_local_top);
				l_new_top = static_cast<bcUBYTE*>(l_top_mem_block->allocators_extra());
				bool l_is_freeing_top = p_pointer == l_local_top;
				bcUBYTE* l_pointer_copy = static_cast<bcUBYTE*>(p_pointer);

				// If we're not freeing the top of the stack mark block as free /
				if (!l_is_freeing_top)
				{
					p_mem_block->free(true, core_platform::bc_memory_order::seqcst);

					// After we mark this block as free, check again if block has become top block because
					// before we mark block as free it's possible duo to poping by other threads this block
					// become top block
					l_back_trace = m_top.compare_exchange_strong(&l_pointer_copy,
						l_new_top,
						core_platform::bc_memory_order::seqcst,
						core_platform::bc_memory_order::seqcst);

					if (l_back_trace)
					{
						m_tracer.accept_free(p_mem_block->size());
#ifdef BC_MEMORY_DEBUG
						l_free_position = reinterpret_cast<bcUINTPTR>(l_pointer_copy) - p_mem_block->offset();
						l_free_size = p_mem_block->size();
#endif

					}
					break;
				}
				else
				{
					if (m_top.compare_exchange_strong(&l_local_top,
						l_new_top,
						core_platform::bc_memory_order::seqcst,
						core_platform::bc_memory_order::seqcst))
					{
						m_tracer.accept_free(p_mem_block->size());

#ifdef BC_MEMORY_DEBUG
						l_free_position = reinterpret_cast<bcUINTPTR>(l_local_top) - p_mem_block->offset();
						l_free_size = p_mem_block->size();
#endif

						l_back_trace = true;
						break;
					}
					else
						continue;
				}
			}

			// If Previous blocks are free, decrease mTop pointer /
			if (l_back_trace)
			{
				// MemBlock->mExtra point to previous allocated data so we can extract it's MemBlock to check if it was free / 
				bcUBYTE* l_prev_top = static_cast<bcUBYTE*>(p_mem_block->allocators_extra());
				if (l_prev_top != m_heap)
				{
					bc_memblock* l_prev_allocation_block = bc_memblock::retrieve_mem_block(l_prev_top);
					
					bcUBYTE* l_back_trace_new_top = nullptr;
					bool l_is_prev_block_free = l_prev_allocation_block->free(core_platform::bc_memory_order::seqcst);
					while (l_is_prev_block_free)
					{
						l_back_trace_new_top = static_cast<bcUBYTE*>(l_prev_allocation_block->allocators_extra());

						if (!m_top.compare_exchange_strong(&l_new_top,
							l_back_trace_new_top,
							core_platform::bc_memory_order::seqcst,
							core_platform::bc_memory_order::seqcst))
							break;

						m_tracer.accept_free(l_prev_allocation_block->size());

#ifdef BC_MEMORY_DEBUG
						l_free_position = reinterpret_cast<bcUINTPTR>(l_new_top) - l_prev_allocation_block->offset();
						l_free_size += l_prev_allocation_block->size();
#endif

						l_new_top = l_back_trace_new_top;
						if (l_new_top == m_heap)
							break;
						bc_memblock* l_prev_prev_allocation_block = bc_memblock::retrieve_mem_block(l_prev_allocation_block->allocators_extra());
						l_prev_allocation_block = l_prev_prev_allocation_block;
						l_is_prev_block_free = l_prev_allocation_block->free(core_platform::bc_memory_order::seqcst);
					}
				}
			}

#ifdef BC_MEMORY_DEBUG
			std::memset(reinterpret_cast<void*>(l_free_position), 0, l_free_size);
#endif
		};

		void* bc_memory_stack::alloc(bc_memblock* pMemBlock) noexcept(true)
		{
			return push(pMemBlock);
		};

		void bc_memory_stack::free(void* pPointer, bc_memblock* pMemBlock) noexcept(true)
		{
			pop(pPointer, pMemBlock);
		};

		bool bc_memory_stack::contain_pointer(void* pPointer) const noexcept(true)
		{
			return (pPointer >= m_heap && pPointer < m_heap + m_size) ? true : false;
		};

		void bc_memory_stack::clear() noexcept(true)
		{
			m_top.store(m_heap, core_platform::bc_memory_order::seqcst);

			m_tracer.accept_clear();
		};

#endif
	}
}