//  [8/29/2013 MRB]

#include "Core/CorePCH.h"
#include "CorePlatformImp/Memory/bcMemAlloc.h"
#include "Core/Memory/bcMemoryHeap.h"

namespace black_cat
{
	namespace core
	{
#ifdef BC_MEMORY_ENABLE

		_bc_heap_memblock* _get_next(_bc_heap_memblock* p_block, bcBYTE* p_heap, bcSIZE p_heap_size)
		{
			const auto l_block_size = p_block->size();
			auto* l_block = reinterpret_cast<bcBYTE*>(p_block);
			l_block += l_block_size;

			if(l_block >= p_heap + p_heap_size)
			{
				return nullptr;
			}

			return reinterpret_cast< _bc_heap_memblock* >(l_block);
		}

		_bc_heap_memblock* _get_prev(_bc_heap_memblock* p_block)
		{
			const auto l_prev_block_size = p_block->prev_size();

			if(l_prev_block_size == 0)
			{
				return nullptr;
			}

			auto* l_block = reinterpret_cast<bcBYTE*>(p_block);
			l_block -= l_prev_block_size;

			return reinterpret_cast< _bc_heap_memblock* >(l_block);
		}

		bc_memory_heap::bc_memory_heap() noexcept
			: m_heap(nullptr),
			m_heap_size(0),
			m_remaining_free_space_limit(0),
			m_block_size(0),
			m_last_block(nullptr)
		{
		}

		bc_memory_heap::bc_memory_heap(bc_memory_heap::this_type&& p_other) noexcept
			: bci_memory_movable(std::move(p_other))
		{
			operator=(std::move(p_other));
		}

		bc_memory_heap::~bc_memory_heap() noexcept
		{
			if (is_initialized())
			{
				destroy();
			}
		}

		bc_memory_heap::this_type& bc_memory_heap::operator =(bc_memory_heap::this_type&& p_other) noexcept
		{
			bci_memory_movable::operator=(std::move(p_other));
			m_heap = p_other.m_heap;
			m_heap_size = p_other.m_heap_size;
			m_remaining_free_space_limit = p_other.m_remaining_free_space_limit;
			m_block_size = p_other.m_block_size;
			m_last_block = p_other.m_last_block;
			m_num_fragmentation.store(p_other.m_num_fragmentation.load(core_platform::bc_memory_order::relaxed), core_platform::bc_memory_order::relaxed);

			p_other.m_heap = nullptr;
			p_other.m_heap_size = 0;
			p_other.m_remaining_free_space_limit = 0;
			p_other.m_block_size = 0;
			p_other.m_last_block = nullptr;
			p_other.m_num_fragmentation.store(0, core_platform::bc_memory_order::relaxed);

			return *this;
		}

		void bc_memory_heap::_initialize(bcSIZE p_size, const bcCHAR* p_tag)
		{
			m_heap_size = p_size;
			m_remaining_free_space_limit = bc_memblock::get_aligned_size(sizeof(bc_memblock), BC_MEMORY_MIN_ALIGN);
			m_block_size = bc_memblock::get_aligned_size(sizeof(_bc_heap_memblock), BC_MEMORY_MIN_ALIGN);

			// We alloc m_heap by min align defined in CorePCH because we want all of our allocations have MIN_Align /
			m_heap = static_cast<bcBYTE*>(core_platform::bc_mem_aligned_alloc(m_heap_size * sizeof(bcBYTE), BC_MEMORY_MIN_ALIGN));

			if (!m_heap)
			{
				throw std::bad_alloc();
			}

			auto* l_first_block = reinterpret_cast<_bc_heap_memblock*>(m_heap);
			new (l_first_block) _bc_heap_memblock(m_heap_size - m_block_size, 0, true);

			m_last_block = reinterpret_cast<_bc_heap_memblock*>(m_heap + m_heap_size - m_block_size);
			new (m_last_block) _bc_heap_memblock(m_block_size, l_first_block->size(), false);

			m_num_fragmentation.store(0, core_platform::bc_memory_order::relaxed);

			m_tracer.initialize(p_size);
			if (p_tag)
			{
				bci_memory::tag(p_tag);
			}

			m_tracer.accept_overhead(m_block_size);
		}

		void bc_memory_heap::_destroy() noexcept
		{
			clear();

			auto* l_first_block = reinterpret_cast<_bc_heap_memblock*>(m_heap);
			l_first_block->~_bc_heap_memblock();

			m_last_block->~_bc_heap_memblock();

			core_platform::bc_mem_aligned_free(m_heap);
		}

		void* bc_memory_heap::alloc(bc_memblock* p_memblock) noexcept
		{
			void* l_return_pointer = nullptr;
			const bcSIZE l_size = p_memblock->size();
			const bcSIZE l_require_size = l_size + m_block_size;

			BC_ASSERT(l_require_size > 0);

			_bc_heap_memblock* l_next = m_last_block;
			l_next->lock(core_platform::bc_lock_operation::light);

			_bc_heap_memblock* l_curr_block = _get_prev(l_next);
			l_curr_block->lock(core_platform::bc_lock_operation::light);

			while (l_curr_block && (!l_curr_block->free() || l_curr_block->size() < l_require_size))
			{
				_bc_heap_memblock* l_prev = _get_prev(l_curr_block);

				if(l_prev)
				{
					l_prev->lock(core_platform::bc_lock_operation::light);
					l_next->unlock();
				}
				else
				{
					// Unlock l_curr_block before setting it to null
					l_next->unlock();
					l_curr_block->unlock();
				}

				l_next = l_curr_block;
				l_curr_block = l_prev;
			}

			// no more free memory
			if (!l_curr_block)
			{
				m_tracer.reject_alloc(l_size);
				return l_return_pointer;
			}

			// Check if we must divide our free block then do it
			if (l_curr_block->size() - l_require_size >= m_remaining_free_space_limit)
			{
				_bc_heap_memblock* l_new_heap_block = reinterpret_cast< _bc_heap_memblock* >(reinterpret_cast<bcBYTE*>(l_curr_block) + l_require_size);
				new (l_new_heap_block) _bc_heap_memblock(l_curr_block->size() - l_require_size, l_require_size, true);

				l_curr_block->size(l_require_size);
				l_curr_block->free(false);

				l_next->prev_size(l_new_heap_block->size());

				l_next->unlock();
				l_curr_block->unlock();

				l_return_pointer = reinterpret_cast<bcBYTE*>(l_curr_block) + m_block_size;

				m_tracer.accept_alloc(l_size);
				m_tracer.accept_overhead(m_block_size);
			}
			else
			{
				l_curr_block->free(false);

				l_next->unlock();
				l_curr_block->unlock();

				l_return_pointer = reinterpret_cast<bcBYTE*>(l_curr_block) + m_block_size;

				m_tracer.accept_alloc(l_require_size);
			}

			return l_return_pointer;
		}

		void bc_memory_heap::free(void* p_pointer, bc_memblock* p_memblock) noexcept
		{
			_bc_heap_memblock* l_block = reinterpret_cast< _bc_heap_memblock* >(static_cast<bcBYTE*>(p_pointer) - m_block_size);
			_bc_heap_memblock* l_next_next;
			_bc_heap_memblock* l_next;
			_bc_heap_memblock* l_prev;
			
			bool l_next_free = false;
			bool l_prev_free = false;

			// We always have next block
			l_next = _get_next(l_block, m_heap, m_heap_size);
			
			// Try to lock next and next next block (if required) so avoid deadlock.
			// (We lock memory blocks in reverse order)
			while (true)
			{
				l_next->lock(core_platform::bc_lock_operation::light);
				l_next_next = _get_next(l_next, m_heap, m_heap_size);

				if (l_next->free() && l_next_next)
				{
					if(l_next_next->try_lock(core_platform::bc_lock_operation::light))
					{
						break;
					}
					else
					{
						l_next->unlock();
					}
				}
				else
				{
					break;
				}
			}

			l_block->lock(core_platform::bc_lock_operation::light);

			// We must get prev block after locking current block
			l_prev = _get_prev(l_block);
			if(l_prev)
			{
				l_prev->lock(core_platform::bc_lock_operation::light);
			}

			l_block->free(true);

			// Merge free block to it's next, if next block is free
			if (l_next && l_next->free())
			{
				l_next_free = true;

				l_block->size(l_block->size() + l_next->size());
				
				if(l_next_next)
				{
					l_next_next->prev_size(l_block->size());
				}
			}
			// Merge free block to it's prev, if prev block is free
			if (l_prev && l_prev->free())
			{
				l_prev_free = true;

				l_prev->size(l_prev->size() + l_block->size());
				
				if(l_next_free && l_next_next)
				{
					l_next_next->prev_size(l_prev->size());
				}
				else
				{
					l_next->prev_size(l_prev->size());
				}
			}

			if (l_next_free && l_next_next)
			{
				l_next_next->unlock();
			}
			if (l_next)
			{
				l_next->unlock();

				if (l_next_free)
				{
					l_next->~_bc_heap_memblock();
				}
			}
			l_block->unlock();
			if (l_prev)
			{
				l_prev->unlock();

				if (l_prev_free)
				{
					l_block->~_bc_heap_memblock();
				}
			}

			if (!l_prev_free && !l_next_free)
			{
				m_num_fragmentation.fetch_add(1, core_platform::bc_memory_order::relaxed);
			}
			else if(l_prev_free && l_next_free)
			{
				m_num_fragmentation.fetch_sub(1, core_platform::bc_memory_order::relaxed);
			}

			if (l_next_free)
			{
				m_tracer.accept_free_overhead(m_block_size);
			}
			if (l_prev_free)
			{
				m_tracer.accept_free_overhead(m_block_size);
			}
			m_tracer.accept_free_overhead(m_block_size);
			m_tracer.accept_free(p_memblock->size());

#ifdef BC_MEMORY_DEBUG
			std::memset(p_pointer, 0, p_memblock->size());
#endif
		}

		bool bc_memory_heap::contain_pointer(void* p_memory) const noexcept
		{
			return (p_memory >= m_heap && p_memory < m_heap + m_heap_size) ? true : false;
		}

		void bc_memory_heap::clear() noexcept
		{
			_bc_heap_memblock* l_curr_block = _get_prev(m_last_block);
			
			while(l_curr_block)
			{
				l_curr_block->lock(core_platform::bc_lock_operation::light);
				
				_bc_heap_memblock* l_prev = _get_prev(l_curr_block);

				l_curr_block->unlock();

				l_curr_block->~_bc_heap_memblock();

				l_curr_block = l_prev;
			}

#ifdef BC_MEMORY_DEBUG
			std::memset(m_heap, 0, m_heap_size - m_block_size);
#endif 

			auto* l_first_block = reinterpret_cast<_bc_heap_memblock*>(m_heap);
			new (l_first_block) _bc_heap_memblock(m_heap_size - m_block_size, 0, true);

			m_num_fragmentation.store(0, core_platform::bc_memory_order::relaxed);

			m_tracer.accept_clear();
			m_tracer.accept_overhead(m_block_size);
		};

#ifdef BC_MEMORY_DEFRAG
		void bc_memory_heap::register_pointer(void** p_pointer, bc_memblock* p_memblock) noexcept
		{
			_bc_heap_memblock* l_mem_block = reinterpret_cast< _bc_heap_memblock* >
			(
				reinterpret_cast< bcUINTPTR >(*p_pointer) - p_memblock->offset() - m_block_size
			);

			l_mem_block->register_new_pointer(p_pointer);
		}

		void bc_memory_heap::unregister_pointer(void** p_pointer, bc_memblock* p_memblock) noexcept
		{
			_bc_heap_memblock* l_mem_block = reinterpret_cast< _bc_heap_memblock* >
			(
				reinterpret_cast< bcUINTPTR >(*p_pointer) - p_memblock->offset() - m_block_size
			);

			l_mem_block->unregister_pointer(p_pointer);
		}

		void bc_memory_heap::defragment(bcINT32 p_num_defrag, defrag_callback p_defrag_callback) noexcept
		{
			_bc_heap_memblock* l_last_used_block;
			_bc_heap_memblock* l_first_used_block;
			_bc_heap_memblock* l_free_block;
			_bc_heap_memblock* l_current_block = _get_prev(m_last_block);

			while (--p_num_defrag >= 0)
			{
				while (l_current_block && l_current_block->free())
				{
					l_current_block = _get_prev(l_current_block);
				}

				if(!l_current_block)
				{
					break;
				}

				l_last_used_block = l_current_block;

				while (l_current_block && !l_current_block->free())
				{
					l_current_block = _get_prev(l_current_block);
				}

				if (!l_current_block)
				{
					break;
				}

				l_first_used_block = _get_next(l_current_block, m_heap, m_heap_size);
				l_free_block = l_current_block;

				while (l_first_used_block <= l_last_used_block)
				{
					auto* l_next_used_block = _get_next(l_first_used_block, m_heap, m_heap_size);
					
					bc_memblock l_block_memblock = *l_first_used_block->get_memblock();
					
					auto* l_dest = reinterpret_cast<bcBYTE*>(l_free_block) + m_block_size;
					auto* l_src = reinterpret_cast<bcBYTE*>(l_first_used_block) + m_block_size;
					auto l_dest_offset = bc_memblock::get_required_offset_for_data(l_dest, l_block_memblock.alignment());
					auto l_src_offset = l_block_memblock.offset();
					auto l_byte_to_move = l_block_memblock.size() - l_block_memblock.offset();
					bcUINTPTR l_move_offset = (l_src + l_src_offset) - (l_dest + l_dest_offset);

					auto l_free_block_size = l_free_block->size();
					auto l_free_block_prev_size = l_free_block->prev_size();
					auto l_used_block_size = l_first_used_block->size();
					auto* l_free_block_new_pos = reinterpret_cast< _bc_heap_memblock* >(reinterpret_cast<bcBYTE*>(l_free_block) + l_used_block_size);
					auto* l_used_block_new_pos = l_free_block;
					_bc_heap_memblock::pointer_refs l_used_block_ptrs;

					std::copy(l_first_used_block->pointers_begin(), l_first_used_block->pointers_end(), l_used_block_ptrs.begin());

					l_free_block->~_bc_heap_memblock();
					l_first_used_block->~_bc_heap_memblock();
					bc_memblock::free_mem_block(l_src + l_src_offset);
					
					std::memmove(l_dest + l_dest_offset, l_src + l_src_offset, l_byte_to_move);

					bc_memblock::store_mem_block(l_dest + l_dest_offset, &l_block_memblock);
					new(l_free_block_new_pos) _bc_heap_memblock(l_free_block_size, l_used_block_size, true);
					new(l_used_block_new_pos) _bc_heap_memblock(l_used_block_size, l_free_block_prev_size, false);

					_bc_heap_memblock::pointer_refs::iterator l_used_block_pointers_begin = l_used_block_ptrs.begin();
					_bc_heap_memblock::pointer_refs::iterator l_new_used_block_pointers_begin = l_used_block_new_pos->pointers_begin();
					_bc_heap_memblock::pointer_refs::iterator l_new_used_block_pointers_end = l_used_block_new_pos->pointers_end();

					while (l_new_used_block_pointers_begin != l_new_used_block_pointers_end)
					{
						*l_new_used_block_pointers_begin = *l_used_block_pointers_begin;

						if (*l_new_used_block_pointers_begin != nullptr)
						{
							**l_new_used_block_pointers_begin = reinterpret_cast< void* >(reinterpret_cast< bcUINTPTR >(**l_used_block_pointers_begin) - l_move_offset);
						}

						++l_used_block_pointers_begin;
						++l_new_used_block_pointers_begin;
					}

					if(l_first_used_block == l_last_used_block)
					{
						auto* l_free_block_next = _get_next(l_free_block_new_pos, m_heap, m_heap_size);

						if (l_free_block_next->free())
						{
							l_free_block_new_pos->size(l_free_block_new_pos->size() + l_free_block_next->size());
							
							l_free_block_next->~_bc_heap_memblock();
							
							l_free_block_next = _get_next(l_free_block_new_pos, m_heap, m_heap_size);
							l_free_block_next->prev_size(l_free_block_new_pos->size());
							
							m_tracer.accept_free_overhead(m_block_size);
						}
						else
						{
							l_free_block_next->prev_size(l_free_block_size);
						}

						// Let next iteration continue from last moved block
						l_current_block = l_free_block_new_pos;
					}

					if(p_defrag_callback != nullptr)
					{
						p_defrag_callback(l_src, l_dest);
					}
					
					l_first_used_block = l_next_used_block;
					l_free_block = l_free_block_new_pos;
				}

				m_num_fragmentation.fetch_sub(1, core_platform::bc_memory_order::relaxed);
			}
		}
#endif

#endif
	}
}