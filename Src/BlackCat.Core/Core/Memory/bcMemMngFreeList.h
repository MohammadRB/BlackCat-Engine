#pragma once

#include "Core/CorePCH.h"
#include "CorePlatform/Concurrency/bcConcurrencyDef.h"
#include "CorePlatformImp/Memory/bcMemAlloc.h"
#include "CorePlatformImp/Utility/bcHardwareInfo.h"
#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "CorePlatformImp/Concurrency/bcMutex.h"

namespace black_cat
{
	namespace core
	{
#ifdef BC_MEMORY_ENABLE

		inline bcINT32 _roundup_p2(bcINT32 p_num, bcINT32 p_multiple)
		{
			return (p_num + p_multiple - 1) & ~(p_multiple - 1);
		}

		template< typename T >
		class bc_memmng_freelist
		{
		private:
			struct _freelist_chunk
			{
				void* m_pointer;
				_freelist_chunk* m_next;
			};

			struct _freelist_item
			{
				_freelist_item* m_next_free; // TODO place free pointers together
				T m_data;
			};

		public:
			bc_memmng_freelist() noexcept
				: m_alloc_size(0),
				m_first_chunk(nullptr)
			{
				m_free.store(reinterpret_cast<_freelist_item*>(nullptr));
				m_count.store(0U);
			}

			bc_memmng_freelist(bc_memmng_freelist&& p_other) noexcept
			{
				_move(std::move(p_other));
			}

			~bc_memmng_freelist()
			{
				_cleanup();
			}

			bc_memmng_freelist& operator =(bc_memmng_freelist&& p_other) noexcept
			{
				_move(std::move(p_other));

				return *this;
			}

			// Roundup allocation size to multiple size of platform page size and reserve memory
			void initialize(bcSIZE p_alloc_size = 4096)
			{
				platform::bc_basic_hardware_info l_hardware_info;
				platform::bc_hardware_info::get_basic_info(l_hardware_info);

				// Round up allocation size to multiple of platform page size, because we use paging allocation /
				m_alloc_size = _roundup_p2(p_alloc_size, l_hardware_info.m_page_size);
				
				_alloc_new_block();
			}

			// Call object construction after this function
			T* alloc()
			{
				_freelist_item* l_local_free = m_free.load(platform::bc_memory_order::seqcst);
				while (l_local_free != nullptr)
				{
					if (m_free.compare_exchange_strong(
						&l_local_free,
						l_local_free->m_next_free,
						platform::bc_memory_order::seqcst,
						platform::bc_memory_order::seqcst))
					{
						l_local_free->m_next_free = nullptr;
						T* lResult = &l_local_free->m_data;

						m_count.fetch_add(1U, platform::bc_memory_order::seqcst);

						return lResult;
					}
				}
				// If next block is nullptr, allocate a new chunk and connect current block (first block in current chunk) 
				// to changed m_free that point to last block to new chunk /
				if (l_local_free == nullptr)
				{
					platform::bc_lock_guard<platform::bc_mutex> l_guard(m_mutex);

					_freelist_item* l_free_ptr = m_free.load(platform::bc_memory_order::relaxed);

					if (l_free_ptr == nullptr)
					{
						// This function will change m_free to last block in next allocated chunk /
						_alloc_new_block();

						l_free_ptr = m_free.load(platform::bc_memory_order::relaxed);
						m_free.store(l_free_ptr->m_next_free, platform::bc_memory_order::relaxed);

						l_free_ptr->m_next_free = nullptr;
						T* lResult = &l_free_ptr->m_data;

						m_count.fetch_add(1U, platform::bc_memory_order::relaxed);

						return lResult;
					}
				}

				// try to alloc again in case of double check lock fail
				return alloc();
			}

			// Call object destructor before this function
			void free(T* p_ptr)
			{
				_freelist_item* l_node = reinterpret_cast<_freelist_item*>(
					reinterpret_cast<bcUBYTE*>(p_ptr) - 
					sizeof(_freelist_item*));

				_freelist_item* l_local_free = m_free.load(platform::bc_memory_order::seqcst);
				l_node->m_next_free = l_local_free;
				while (l_local_free != nullptr)
				{
					if (m_free.compare_exchange_strong(
						&l_node->m_next_free,
						l_node,
						platform::bc_memory_order::seqcst,
						platform::bc_memory_order::seqcst))
					{
						m_count.fetch_sub(1U, platform::bc_memory_order::seqcst);
						return;
					}
					else
						l_local_free = l_node->m_next_free;
				}

				// Block any thread in alloc function, so we can free our block safely /
				if (l_local_free == nullptr)
				{
					platform::bc_lock_guard<platform::bc_mutex> l_guard(m_mutex);

					l_local_free = m_free.load(platform::bc_memory_order::relaxed);
					if (l_local_free == nullptr)
					{
						l_node->m_next_free = l_local_free;
						m_free.store(l_node, platform::bc_memory_order::relaxed);

						m_count.fetch_sub(1U, platform::bc_memory_order::relaxed);
					}
				}
			}

			// TODO can be removed
			bcUINT32 count() const noexcept
			{
				return m_count.load(platform::bc_memory_order::seqcst);
			}

			void clear()
			{
				// use lock for cache coherency
				platform::bc_lock_guard<platform::bc_mutex> l_guard(m_mutex);
				_cleanup();
				initialize(m_alloc_size);
			}

		private:
			void _move(bc_memmng_freelist&& p_other)
			{
				_freelist_item* l_free = m_free.load(platform::bc_memory_order::acquire);

				m_alloc_size = p_other.m_alloc_size;
				m_first_chunk = p_other.m_first_chunk;
				m_count.store(p_other.m_count.load(platform::bc_memory_order::relaxed), platform::bc_memory_order::relaxed);

				p_other.m_alloc_size = 0;
				p_other.m_first_chunk = nullptr;
				p_other.m_count.store(0, platform::bc_memory_order::relaxed);

				m_free.store(l_free, platform::bc_memory_order::release);
			}

			void _alloc_new_block()
			{
				const bcSIZE l_alloc_size = m_alloc_size;
				const bcSIZE l_num_alloc_per_batch = l_alloc_size / sizeof(_freelist_item);

				// Record allocated blocks to free on deconstruction /
				_freelist_chunk* l_prev_last_block = nullptr;
				_freelist_chunk* l_last_block = m_first_chunk;
				while (l_last_block)
				{
					l_prev_last_block = l_last_block;
					l_last_block = l_last_block->m_next;
				}

				l_last_block = static_cast<_freelist_chunk*>(platform::bc_mem_alloc(sizeof(_freelist_chunk)));
				if (l_last_block == nullptr)
				{
					// TODO check for allocation fail here
					BC_ASSERT(false);
				}
				l_last_block->m_pointer = nullptr;
				l_last_block->m_next = nullptr;

				if (l_prev_last_block)
				{
					l_prev_last_block->m_next = l_last_block;
				}
				else
				{
					m_first_chunk = l_last_block;
				}
				
				void* l_chunk = platform::bc_mem_page_reserve(l_alloc_size);
				l_chunk = platform::bc_mem_page_commit(l_chunk, l_alloc_size);
				l_last_block->m_pointer = l_chunk;
				if (l_last_block->m_pointer == nullptr)
				{
					// TODO check for allocation fail here
					BC_ASSERT(false);
				}

				_freelist_item* l_alloc_batch = static_cast<_freelist_item*>(l_last_block->m_pointer);

				// Because we protect this method with mutex we can load mFree to a local variable /
				_freelist_item* l_local_free = m_free.load(platform::bc_memory_order::relaxed);
				for (bcUINT32 i = 0; i < l_num_alloc_per_batch; ++i)
				{
					l_alloc_batch->m_next_free = l_local_free;
					l_local_free = l_alloc_batch;
					++l_alloc_batch;
				}
				m_free.store(l_local_free, platform::bc_memory_order::relaxed);
			}

			void _cleanup()
			{
				_freelist_chunk* l_current = m_first_chunk;
				while (l_current)
				{
					_freelist_chunk* l_temp = l_current;
					platform::bc_mem_page_release(l_temp->m_pointer);
					l_current = l_temp->m_next;
					platform::bc_mem_free(l_temp);
				}
			}

			bcSIZE m_alloc_size;
			_freelist_chunk* m_first_chunk;
			platform::bc_atomic< _freelist_item* > m_free;
			platform::bc_atomic< bcUINT32 > m_count;
			platform::bc_mutex m_mutex;
		};

#endif
	}
}