// [10/24/2014 MRB]

#pragma once

#include "Core/CorePCH.h"
#include "Core/bcExport.h"
#include "Core/Utility/bcInitializable.h"
#include "Core/Memory/bcMemBlock.h"
#include "Core/Memory/bcMemory.h"
#include "Core/Memory/bcMemoryTracer.h"
#include "Core/Memory/bcMemMngFreeList.h"
#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "CorePlatformImp/Concurrency/bcMutex.h"

namespace black_cat
{
	namespace core
	{
#ifdef BC_MEMORY_ENABLE

		struct _bc_heap_memblock
		{
		public:
#ifdef BC_MEMORY_DEFRAG
			using pointer_refs = std::array< void**, 4 >;
#endif

		public:
			_bc_heap_memblock(bcSIZE p_size, bcSIZE p_prev_size, bool p_free)
				: m_size(p_size),
				m_prev_size(p_prev_size),
				m_free(p_free)
#ifdef BC_MEMORY_DEFRAG
				, m_pointers{{nullptr, nullptr, nullptr, nullptr}}
#endif
			{
			}

			/*_bc_heap_memblock(const _bc_heap_memblock& p_other) noexcept
				: m_size(p_other.m_size),
				m_prev_size(p_other.m_prev_size),
				m_free(p_other.m_free),
				m_mutex(),
				m_pointers(p_other.m_pointers)
			{
			}

			_bc_heap_memblock& operator=(const _bc_heap_memblock& p_other) noexcept
			{
				m_size = p_other.m_size;
				m_prev_size = p_other.m_prev_size;
				m_free = p_other.m_free;
				m_pointers = p_other.m_pointers;

				return *this;
			}*/

			bcSIZE size() const noexcept
			{
				return m_size;
			}

			void size(bcSIZE val) noexcept
			{
				m_size = val;
			}

			bcSIZE prev_size() const noexcept
			{
				return m_prev_size;
			}

			void prev_size(bcSIZE val) noexcept
			{
				m_prev_size = val;
			}

			bool free() const noexcept
			{
				return m_free;
			}

			void free(bool val) noexcept
			{
				m_free = val;
			}
			
#ifdef BC_MEMORY_DEFRAG
			pointer_refs::iterator pointers_begin() noexcept
			{
				return m_pointers.begin();
			}

			pointer_refs::iterator pointers_end() noexcept
			{
				return m_pointers.end();
			}
#endif

			void lock(core_platform::bc_lock_operation p_operation) noexcept
			{
				m_mutex.lock(p_operation);
			}

			void unlock() noexcept
			{
				m_mutex.unlock();
			}

			bool try_lock(core_platform::bc_lock_operation p_operation) noexcept
			{
				return m_mutex.try_lock(p_operation);
			}

#ifdef BC_MEMORY_DEFRAG
			// If any pointer exist in this block or this block is not free return MemBlock of one of it's pointers 
			// else return nullptr that indicate this block is free /
			bc_memblock* get_memblock() noexcept
			{
				void* l_pointer = nullptr;

				for (void** l_ptr : m_pointers)
				{
					if (*l_ptr != nullptr)
					{
						l_pointer = *l_ptr;
						break;
					}
				}

				if (l_pointer)
				{
					return bc_memblock::retrieve_mem_block(l_pointer);
				}
					
				return nullptr;
			}

			void register_new_pointer(void** p_pointer) noexcept
			{
				bool l_has_set = false;

				core_platform::bc_lock_guard< core_platform::bc_hybrid_mutex > l_guard(m_mutex);
				{
					for (void**& l_ptr : m_pointers)
					{
						if (l_ptr == nullptr)
						{
							l_ptr = p_pointer;
							l_has_set = true;
							break;
						}
					}
				}

				bcAssert(l_has_set);
			}

			void unregister_pointer(void** p_pointer) noexcept
			{
				bool l_found = false;

				core_platform::bc_lock_guard< core_platform::bc_hybrid_mutex > l_guard(m_mutex);
				{
					for (void**& l_ptr : m_pointers)
					{
						if (l_ptr && l_ptr == p_pointer)
						{
							l_ptr = nullptr;
							l_found = true;
							break;
						}
					}
				}

				bcAssert(l_found);
			}
#endif
		protected:

		private:
			static const bcINT32 m_free_mask = 1;
			static const bcINT32 m_address_mask = ~m_free_mask;

			bcSIZE m_size;
			bcSIZE m_prev_size;
			bool m_free;
			core_platform::bc_hybrid_mutex m_mutex;
#ifdef BC_MEMORY_DEFRAG
			pointer_refs m_pointers;
#endif
		};

		class BC_CORE_DLL bc_memory_heap : public bc_memory_movable, public bc_initializable<bcSIZE, const bcCHAR*>
		{
		public:
			using this_type = bc_memory_heap;
#ifdef BC_MEMORY_DEFRAG
			using is_movabe_type = std::true_type;
#else
			using is_movabe_type = std::false_type;
#endif

		public:
			bc_memory_heap() noexcept;

			bc_memory_heap(this_type&& p_other) noexcept;

			~bc_memory_heap() noexcept;

			this_type& operator =(this_type&& p_other) noexcept;

			bcSIZE size() const noexcept
			{
				return m_heap_size;
			}

			bcSIZE fragmentation_count() const noexcept
			{
				return m_num_fragmentation.load(core_platform::bc_memory_order::relaxed);
			}

			void* alloc(bc_memblock* p_memblock) noexcept override;

			void free(void* p_pointer, bc_memblock* p_memblock) noexcept override;

			bool contain_pointer(void* p_memory) const noexcept override;

			void clear() noexcept override;

#ifdef BC_MEMORY_DEFRAG
			void register_pointer(void** p_pointer, bc_memblock* p_memblock) noexcept override;

			void unregister_pointer(void** p_pointer, bc_memblock* p_memblock) noexcept override;

			void defragment(bcINT32 p_num_defrag, defrag_callback p_defrag_callback) noexcept override;
#endif

		protected:

		private:
			void _initialize(bcSIZE p_size, const bcCHAR* p_tag) override;

			void _destroy() noexcept(true) override;

			void _move(this_type&& p_other)
			{
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
			}

			bcBYTE* m_heap;
			bcSIZE m_heap_size;
			bcSIZE m_remaining_free_space_limit;
			bcSIZE m_block_size;
			_bc_heap_memblock* m_last_block;
			core_platform::bc_atomic< bcINT32 > m_num_fragmentation;
		};

#endif
	}
}