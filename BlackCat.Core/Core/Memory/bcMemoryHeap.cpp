//  [8/29/2013 MRB]

#include "Core/CorePCH.h"
#include "Core/Memory/bcMemoryHeap.h"

namespace black_cat
{
	namespace core
	{
#ifdef BC_MEMORY_ENABLE

		bc_memory_heap::bc_memory_heap() noexcept(true)
		{
		};

		bc_memory_heap::bc_memory_heap(bc_memory_heap::this_type&& p_other) noexcept(true) : bc_memory_movable(std::move(p_other))
		{
			_move(std::move(p_other));
		};

		bc_memory_heap::~bc_memory_heap() noexcept(true)
		{
			if (m_initialized)
				destroy();
		};

		bc_memory_heap::this_type& bc_memory_heap::operator =(bc_memory_heap::this_type&& p_other) noexcept(true)
		{
			bc_memory_movable::operator=(std::move(p_other));
			_move(std::move(p_other));

			return *this;
		};

		void bc_memory_heap::initialize(bcSIZE p_size, const bcCHAR* p_tag)
		{
			m_size = p_size;
			m_remaining_free_space_limit = sizeof(bc_memblock) + BC_MEMORY_MIN_ALIGN;

			// We alloc m_heap by min align defined in CorePCH because we want all of our allocations have MIN_Align /
			m_heap = static_cast<bcUBYTE*>(core_platform::bc_mem_aligned_alloc(m_size * sizeof(bcUBYTE), BC_MEMORY_MIN_ALIGN));

			if (!m_heap)
				throw std::bad_alloc();

			bcUINT32 l_heap_block_count = 100;
			m_block_allocator.initialize(sizeof(heap_memblock)* l_heap_block_count);
#ifdef BC_MEMORY_DEFRAG
			//m_pointer_ref_allocator.initialize(sizeof(heap_memblock::pointer_refs) * l_heap_block_count * 2);
#endif

			m_first_memblock = m_block_allocator.alloc();
			new(m_first_memblock)heap_memblock();
			m_first_memblock->data_address(reinterpret_cast<bcUINTPTR>(m_heap));
			m_first_memblock->size(m_size);
			m_first_memblock->free(true);
			m_first_memblock->next(nullptr);
			m_first_memblock->prev(nullptr);
			
			m_top.store(m_first_memblock, core_platform::bc_memory_order::relaxed);

			m_num_fragmentation = 0;

			m_tracer.initialize(p_size);
			if (p_tag) bc_memory::tag(p_tag);

			m_tracer.accept_overhead(sizeof(heap_memblock));

			m_initialized = true;
		};

		void bc_memory_heap::destroy() noexcept(true)
		{
			core_platform::bc_mem_aligned_free(m_heap);

			m_initialized = false;
		};

		void* bc_memory_heap::alloc(bc_memblock* p_memblock) noexcept(true)
		{
			void* l_return_pointer = nullptr;
			bcSIZE l_require_size = p_memblock->size();

			bcAssert(l_require_size > 0);

			heap_memblock* l_curr_block = nullptr;
			while (true)
			{
				l_curr_block = m_top.load(core_platform::bc_memory_order::relaxed);

				l_curr_block->lock(core_platform::bc_lock_operation::light);

				// if before locking m_top another thread has changed it try again
				if (m_top.load(core_platform::bc_memory_order::relaxed) != l_curr_block)
				{
					l_curr_block->unlock();
					continue;
				}
				break;
			}

			if (!l_curr_block->free() || l_curr_block->size() < l_require_size)
			{
				l_curr_block->unlock();

				// start search from first block
				l_curr_block = m_first_memblock;

				l_curr_block->lock(core_platform::bc_lock_operation::medium);

				while (l_curr_block && (!l_curr_block->free() || l_curr_block->size() < l_require_size))
				{
					heap_memblock* l_next = l_curr_block->next();

					if (l_next != nullptr)
					{
						l_next->lock(core_platform::bc_lock_operation::medium);
						l_curr_block->unlock();
					}
					else
					{
						// unlock l_curr_block before setting it to null
						l_curr_block->unlock();
					}

					l_curr_block = l_next;
				}
			}

			// no more free memory
			if (!l_curr_block)
			{
				bc_memory_heap::m_tracer.reject_alloc(l_require_size);
				return l_return_pointer;
			}

			// Check if we must divide our free block then do it /
			if (l_curr_block->size() > l_require_size && l_curr_block->size() - l_require_size >= m_remaining_free_space_limit)
			{
				heap_memblock* l_free_memblock = m_block_allocator.alloc();
				new(l_free_memblock)heap_memblock();

				l_free_memblock->data_address(l_curr_block->data_address() + l_require_size);
				l_free_memblock->size(l_curr_block->size() - l_require_size);
				l_free_memblock->next(l_curr_block->next());
				l_free_memblock->prev(l_curr_block);
				if (l_curr_block->next())
				{
					heap_memblock* l_next = l_curr_block->next();

					l_next->lock(core_platform::bc_lock_operation::light);
					l_next->prev(l_free_memblock);
					l_next->unlock();
				}
				l_free_memblock->free(true);

				l_curr_block->size(l_require_size);
				l_curr_block->next(l_free_memblock);
				l_curr_block->free(false);

				if (m_top.load(core_platform::bc_memory_order::relaxed) == l_curr_block)
					m_top.store(l_free_memblock, core_platform::bc_memory_order::relaxed);

				l_curr_block->unlock();

				p_memblock->allocators_extra(l_curr_block);
				l_return_pointer = reinterpret_cast<void*>(l_curr_block->data_address());

				m_tracer.accept_alloc(l_require_size);
				m_tracer.accept_overhead(sizeof(heap_memblock));
			}
			else
			{
				l_curr_block->free(false);

				l_curr_block->unlock();

				p_memblock->allocators_extra(l_curr_block);
				l_return_pointer = reinterpret_cast<void*>(l_curr_block->data_address());

				m_tracer.accept_alloc(l_require_size);
			}

			return l_return_pointer;
		};

		void bc_memory_heap::free(void* p_pointer, bc_memblock* p_memblock) noexcept(true)
		{
			heap_memblock* l_heapblock = reinterpret_cast<heap_memblock*>(p_memblock->allocators_extra());

			bool l_next_free = false;
			bool l_prev_free = false;

			heap_memblock* l_top = nullptr;
			heap_memblock* l_next = nullptr;
			heap_memblock* l_prev = nullptr;
#ifdef BC_MEMORY_DEFRAG
			//const heap_memblock::pointer_refs* l_heapblock_ptrs = nullptr;
#endif

			// try to lock prev and current block so that avoid deadlock
			while (true)
			{
				l_heapblock->lock(core_platform::bc_lock_operation::light);
				l_prev = l_heapblock->prev();

				if (l_prev != nullptr)
				{
					if (l_prev->try_lock(core_platform::bc_lock_operation::light))
						break;
					else
						l_heapblock->unlock();
				}
				else
					break;
			}


#ifdef BC_MEMORY_DEFRAG
			//l_heapblock_ptrs = l_heapblock->pointers();
#endif
			l_next = l_heapblock->next();

			// we need next block in merging prev or next
			if (l_next != nullptr && (l_prev != nullptr || l_next != nullptr))
				l_next->lock(core_platform::bc_lock_operation::light);

			p_memblock->free(true);
			l_heapblock->free(true);

			// Merge free block to it's next, if next block is free
			if (l_next && l_next->free() == true)
			{
				l_next_free = true;

				l_heapblock->size(l_heapblock->size() + l_next->size());
				if (l_next->next())
				{
					l_next->next()->lock(core_platform::bc_lock_operation::light);
					l_next->next()->prev(l_heapblock);
					l_next->next()->unlock();
				}
				l_heapblock->next(l_next->next());

				// Check if next block is mTop, change mTop to point to current block /
				if ((l_top = m_top.load(core_platform::bc_memory_order::relaxed)) == l_next)
				{
					m_top.store(l_heapblock, core_platform::bc_memory_order::relaxed);
					l_top = l_heapblock;
				}
			}
			// Merge free block to it's prev, if prev block is free
			if (l_prev && l_prev->free() == true)
			{
				l_prev_free = true;

				l_prev->size(l_prev->size() + l_heapblock->size());
				l_prev->next(l_heapblock->next());
				if (l_heapblock->next())
					l_heapblock->next()->prev(l_prev);
			}

			if (l_prev)
				l_prev->unlock();
			l_heapblock->unlock();
			if (l_next)
				l_next->unlock();

			if (!l_prev_free && !l_next_free)
				++m_num_fragmentation;
			else if ((l_prev_free && l_next_free) || 
					(l_prev_free && l_heapblock == (l_top != nullptr ? l_top : m_top.load(core_platform::bc_memory_order::relaxed))) ||
					(l_prev_free && l_prev == m_first_memblock && l_next == nullptr))
				--m_num_fragmentation;

#ifdef BC_MEMORY_DEFRAG
			//// Free allocated pointers in this block /
			//while (l_heapblock_ptrs)
			//{
			//	heap_memblock::pointer_refs* l_next = l_heapblock_ptrs->m_next;
			//	l_heapblock_ptrs->~pointer_refs();
			//	m_pointer_ref_allocator.free(l_heapblock_ptrs);
			//	l_heapblock_ptrs = l_next;

			//	m_tracer.accept_free_overhead(sizeof(heap_memblock::pointer_refs));
			//}
#endif

			if (l_next_free)
			{
				// clear next free block
				l_next->~heap_memblock();
				m_block_allocator.free(l_next);
				m_tracer.accept_free_overhead(sizeof(heap_memblock));
			}
			if (l_prev_free)
			{
				// clear prev free block
				l_heapblock->~heap_memblock();
				m_block_allocator.free(l_heapblock);
				m_tracer.accept_free_overhead(sizeof(heap_memblock));
			}
			m_tracer.accept_free(p_memblock->size());

#ifdef BC_MEMORY_DEBUG
			std::memset(reinterpret_cast<void*>(reinterpret_cast<bcUINTPTR>(p_pointer)-p_memblock->offset()), 0, p_memblock->size());
#endif
		};

		bool bc_memory_heap::contain_pointer(void* p_memory) const noexcept(true)
		{
			return (p_memory >= m_heap && p_memory < m_heap + m_size) ? true : false;
		};

		/*bool bc_memory_heap::is_empty() const
		{
			heap_memblock* l_first_memblock = m_first_memblock;
			l_first_memblock->lock(core_platform::bc_lock_operation::light);

			while (l_first_memblock)
			{
				if (!l_first_memblock->free())
				{
					l_first_memblock->unlock();
					return false;
				}

				l_first_memblock->unlock();

				l_first_memblock = l_first_memblock->next();

				if (l_first_memblock)
					l_first_memblock->lock(core_platform::bc_lock_operation::light);
			}

			return true;
		};*/

		void bc_memory_heap::clear() noexcept(true)
		{
//			// Free all blocks and theirs pointer references /
//			heap_memblock* l_first_memblock = m_first_memblock;
//			while (l_first_memblock)
//			{
//				heap_memblock* l_next = m_first_memblock->next();
//
//#ifdef BC_MEMORY_DEFRAG
//				// Free allocated pointers to this block /
//				const heap_memblock::pointer_refs* l_heap_blockptrs = l_first_memblock->pointers();
//				while (l_heap_blockptrs)
//				{
//					heap_memblock::pointer_refs* l_next_ptr = l_heap_blockptrs->m_next;
//					l_heap_blockptrs->~pointer_refs();
//					m_pointer_ref_allocator.free(l_heap_blockptrs);
//					l_heap_blockptrs = l_next_ptr;
//				}
//#endif
//
//				l_first_memblock->~heap_memblock();
//				m_block_allocator.free(l_first_memblock);
//				l_first_memblock = l_next;
//			}

			m_block_allocator.clear();
#ifdef BC_MEMORY_DEFRAG
			//m_pointer_ref_allocator.clear();
#endif

			m_first_memblock = m_block_allocator.alloc();
			new(m_first_memblock)heap_memblock();

			m_first_memblock->data_address(reinterpret_cast<bcUINTPTR>(m_heap));

			m_first_memblock->size(m_size);
			m_first_memblock->next(nullptr);
			m_first_memblock->prev(nullptr);
			m_first_memblock->free(true);

#ifdef BC_MEMORY_DEBUG
			std::memset(m_heap, 0, m_size);
#endif 

			m_top.store(m_first_memblock, core_platform::bc_memory_order::relaxed);

			m_tracer.accept_clear();
			m_tracer.accept_overhead(sizeof(heap_memblock));
		};
		
#ifdef BC_MEMORY_DEFRAG
		void bc_memory_heap::register_pointer(void** p_pointer) noexcept(true)
		{
			heap_memblock* l_mem_block = reinterpret_cast<heap_memblock*>(
				bc_memblock::retrieve_mem_block(*p_pointer)->allocators_extra());

			/*heap_memblock::pointer_refs* l_pointer = m_pointer_ref_allocator.alloc();
			new(l_pointer)heap_memblock::pointer_refs();*/

			l_mem_block->register_new_pointer(p_pointer);

			//m_tracer.accept_overhead(sizeof(heap_memblock::pointer_refs));
		};

		void bc_memory_heap::unregister_pointer(void** p_pointer) noexcept(true)
		{
			heap_memblock* l_memblock = reinterpret_cast<heap_memblock*>(
				bc_memblock::retrieve_mem_block(*p_pointer)->allocators_extra());

			l_memblock->unregister_pointer(p_pointer);
			/*heap_memblock::pointer_refs* l_pointer = lMemBlock->unregister_pointer(p_pointer);
			l_pointer->~pointer_refs();*/

			//m_pointer_ref_allocator.free(l_pointer);

			//m_tracer.accept_free_overhead(sizeof(heap_memblock::pointer_refs));
		};

		void bc_memory_heap::defragment(bcINT32 p_num_defrag) noexcept(true)
		{
			// TODO check for memory consistency problem
			heap_memblock* l_first_block = m_first_memblock;
			heap_memblock* l_prev_freeblock = nullptr;
			heap_memblock* l_freeblock = nullptr;
			heap_memblock* l_usedblock = nullptr;
			heap_memblock* l_last_usedblock = nullptr;
			heap_memblock* l_alignedblock = nullptr;
			bcUINT32 l_num_movedblocks = 0;

			while (--p_num_defrag >= 0)
			{
				// Find first free block /
				while (l_first_block->next() && !l_first_block->free())
				{
					//lPrevFreeBlock = lFirstBlock;
					l_first_block = l_first_block->next();
				}

				// If we have found a free block /
				if (l_first_block->free())
				{
					l_freeblock = l_first_block;
					l_prev_freeblock = l_first_block->prev();
				}
				else
					return;

				// After we have moved a aligned block we return to this point for checking next blocks /
			RETURN_FROM_Aligned_BLOCK_DEFRAG:

				l_usedblock = l_freeblock->next();
				if (!l_usedblock)
					return;

				l_last_usedblock = l_usedblock;
				++l_num_movedblocks;
				// We must move aligned block individually /
				while (l_last_usedblock->next() &&
					!l_last_usedblock->next()->free() &&
					l_last_usedblock->next()->get_memblock()->alignment() == BC_MEMORY_MIN_ALIGN)
				{
					l_last_usedblock = l_last_usedblock->next();
					++l_num_movedblocks;
				}

				bcUINTPTR l_num_byte_move = (l_last_usedblock->data_address() + l_last_usedblock->size()) - l_usedblock->data_address();

				std::memmove(reinterpret_cast<void*>(l_freeblock->data_address()),
					reinterpret_cast<void*>(l_usedblock->data_address()),
					l_num_byte_move);

				// Correct MemBlock's mDataAddress /
				bcUINTPTR l_move_offset = l_usedblock->data_address() - l_freeblock->data_address();
				heap_memblock* l_used_blocks = l_usedblock;
				for (; l_num_movedblocks > 0; --l_num_movedblocks, l_used_blocks = l_used_blocks->next())
				{
					l_used_blocks->data_address(l_used_blocks->data_address() - l_move_offset);
					/*const heap_memblock::pointer_refs* l_block_pointers = l_used_blocks->pointers();
					while (l_block_pointers)
					{
						*l_block_pointers->m_pointer_reference = reinterpret_cast<void*>(
							reinterpret_cast<bcINTPTR>(*l_block_pointers->m_pointer_reference) - l_move_offset);
						l_block_pointers = l_block_pointers->m_next;
					}*/

					heap_memblock::pointer_refs::iterator l_current_pointer = l_used_blocks->pointer_refs_begin();
					heap_memblock::pointer_refs::iterator l_pointers_end = l_used_blocks->pointer_refs_end();

					while (l_current_pointer != l_pointers_end)
					{
						if (**l_current_pointer != nullptr)
							**l_current_pointer = reinterpret_cast< void* >(
								                      reinterpret_cast< bcUINTPTR >(**l_current_pointer) - l_move_offset);

						++l_current_pointer;
					}
				}
				l_freeblock->data_address(l_freeblock->data_address() + l_num_byte_move);

				// Correct MemBlock Pointers /
				if (l_prev_freeblock)
					l_prev_freeblock->next(l_usedblock);
				l_usedblock->prev(l_prev_freeblock);
				//if(lLastUsedBlock)
				l_freeblock->next(l_last_usedblock->next());
				if (l_last_usedblock->next())
					l_last_usedblock->next()->prev(l_freeblock);
				//if(lLastUsedBlock)
				l_last_usedblock->next(l_freeblock);
				l_freeblock->prev(l_last_usedblock);

				// If Loop was ended for an aligned block, so we move it or theirs /
				while (l_freeblock->next() &&
					!l_freeblock->next()->free() &&
					l_freeblock->next()->get_memblock()->alignment() > BC_MEMORY_MIN_ALIGN)
				{
					l_alignedblock = l_freeblock->next();

					// Retrieve MemBlock for this block, so we can store it again /
					bc_memblock l_aligned_memblock = *l_alignedblock->get_memblock();

					/*bcUINT32 lAlignment = lAlignedBlock->mAlignment;
					bcUINT32 lMask = lAlignment - 1;
					bcUINT32 lMisalignment = (lFreeBlock->mDataAddress & lMask);
					bcUINT32 lAdjustment = lAlignment - lMisalignment;*/
					bcSIZE l_adjustment = bc_memblock::get_required_offset_for_data(
						reinterpret_cast<void*>(l_freeblock->data_address()), l_aligned_memblock.alignment());
					bcUINTPTR l_aligend_move_src = l_alignedblock->data_address() + l_aligned_memblock.offset();
					bcUINTPTR l_aligned_move_dest = l_freeblock->data_address() + l_adjustment;
					bcUINTPTR l_aligned_move_offset = l_aligend_move_src - l_aligned_move_dest;

					std::memmove(reinterpret_cast<void*>(l_aligned_move_dest),
						reinterpret_cast<void*>(l_aligend_move_src),
						l_alignedblock->size() - l_aligned_memblock.offset());

					/*const heap_memblock::pointer_refs* l_block_pointers = l_alignedblock->pointers();
					while (l_block_pointers)
					{
						*l_block_pointers->m_pointer_reference = reinterpret_cast<void*>(
							reinterpret_cast<bcUINTPTR>(*l_block_pointers->m_pointer_reference) - l_aligned_move_offset);
						l_block_pointers = l_block_pointers->m_next;
					}*/
					heap_memblock::pointer_refs::iterator l_current_pointer = l_alignedblock->pointer_refs_begin();
					heap_memblock::pointer_refs::iterator l_pointers_end = l_alignedblock->pointer_refs_end();

					while (l_current_pointer != l_pointers_end)
					{
						if (**l_current_pointer != nullptr)
							**l_current_pointer = reinterpret_cast< void* >(
								                      reinterpret_cast< bcUINTPTR >(**l_current_pointer) - l_aligned_move_offset);

						++l_current_pointer;
					}

					// Store MemBlock for moved block /
					bc_memblock::store_mem_block(reinterpret_cast<void*>(l_aligned_move_dest), &l_aligned_memblock);

					// Correct MemBlock's mDataAddress /
					l_alignedblock->data_address(l_freeblock->data_address());
					l_freeblock->data_address(l_freeblock->data_address() + l_alignedblock->size());

					//// Store lAdjustment in first byte before aligned address
					//*((bcUBYTE*)lAlignedBlock->mDataAddress - 1) = lAdjustment;

					// Correct MemBlock Pointers /
					l_last_usedblock->next(l_alignedblock);
					l_alignedblock->prev(l_last_usedblock);
					l_freeblock->next(l_alignedblock->next());
					if (l_alignedblock->next())
						l_alignedblock->next()->prev(l_freeblock);
					l_alignedblock->next(l_freeblock);
					l_freeblock->prev(l_alignedblock);

					l_last_usedblock = l_alignedblock;
				}

				// Correct lPrevFreeBlock to point to lLastUsedBlock for next loop /
				l_prev_freeblock = l_last_usedblock;

				// Correct mFirstMemBlock /
				if (m_first_memblock == l_freeblock)
					//mFirstMemBlock.mOffsetAddress = (bcUINT32)lUsedBlock - *mFirstMemBlock.mBaseAddress;
					m_first_memblock = l_usedblock;

				// Check if next blocks are still used, swap them with correct free block
				if (l_freeblock->next() && !l_freeblock->next()->free())
					goto RETURN_FROM_Aligned_BLOCK_DEFRAG;

				// If we haven't reach to end of heap, merge two free MemBlock and delete lFreeBlock block /
				if (l_freeblock->next() && l_freeblock->next()->free())
				{
					// Correct lFirstBlock for next defragment loops /
					l_first_block = l_freeblock->next();

					l_prev_freeblock->next(l_freeblock->next());
					l_freeblock->next()->prev(l_prev_freeblock);
					l_freeblock->next()->size(l_freeblock->next()->size() + l_freeblock->size());
					l_freeblock->next()->data_address(l_freeblock->data_address());

					l_freeblock->~heap_memblock();
					m_block_allocator.free(l_freeblock);
				}
				// If lFreeBlock->mNext point to nullptr, it is last block in the heap, so mTop most point to it /
				else if (!l_freeblock->next())
				{
					m_top.store(l_freeblock, core_platform::bc_memory_order::relaxed);
				}

				--m_num_fragmentation;
			}
		};
#endif

#endif
	}
}