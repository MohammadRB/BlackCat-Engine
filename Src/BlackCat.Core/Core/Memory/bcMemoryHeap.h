// [24/10/2014 MRB]

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
			using pointer_refs = std::array<void**, 4>;

			_bc_heap_memblock(bcSIZE p_size, bcSIZE p_prev_size, bool p_free);

			bcSIZE size() const noexcept;

			void size(bcSIZE val) noexcept;

			bcSIZE next_size() const noexcept;

			void next_size(bcSIZE p_val) noexcept;

			bool free() const noexcept;

			void free(bool p_val) noexcept;
			
#ifdef BC_MEMORY_DEFRAG
			pointer_refs::iterator pointers_begin() noexcept;

			pointer_refs::iterator pointers_end() noexcept;
#endif

			void lock() noexcept;

			void unlock() noexcept;

			bool try_lock() noexcept;

#ifdef BC_MEMORY_DEFRAG
			// If any pointer exist in this block or this block is not free return MemBlock of one of it's pointers 
			// else return nullptr that indicate this block is free /
			bc_memblock* get_memblock() noexcept;

			void register_new_pointer(void** p_pointer) noexcept;

			void unregister_pointer(void** p_pointer) noexcept;
#endif
		private:
			bcSIZE m_size;
			bcSIZE m_prev_size;
			bool m_free;
			platform::bc_spin_mutex m_mutex;
#ifdef BC_MEMORY_DEFRAG
			pointer_refs m_pointers;
#endif
		};

		class BC_CORE_DLL bc_memory_heap : public bci_memory_movable, public bc_initializable<bcSIZE, const bcCHAR*>
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

			~bc_memory_heap() noexcept override;

			this_type& operator =(this_type&& p_other) noexcept;

			bcSIZE capacity() const noexcept;

			bcSIZE fragmentation_count() const noexcept;

			void* alloc(bc_memblock* p_memblock) noexcept override;

			void free(void* p_pointer, bc_memblock* p_memblock) noexcept override;

			bool contain_pointer(void* p_memory) const noexcept override;

			void clear() noexcept override;

#ifdef BC_MEMORY_DEFRAG
			void register_pointer(void** p_pointer, bc_memblock* p_memblock) noexcept override;

			void unregister_pointer(void** p_pointer, bc_memblock* p_memblock) noexcept override;

			void defrag(bcINT32 p_num_defrag, defrag_callback p_defrag_callback) noexcept override;
#endif

		private:
			void _initialize(bcSIZE p_capacity, const bcCHAR* p_tag) override;

			void _destroy() noexcept override;

			bcBYTE* m_heap;
			bcSIZE m_capacity;
			bcSIZE m_remaining_free_space_limit;
			bcSIZE m_block_size;
			_bc_heap_memblock* m_last_block;
			platform::bc_atomic< bcINT32 > m_num_fragmentation;
		};

		inline _bc_heap_memblock::_bc_heap_memblock(bcSIZE p_size, bcSIZE p_prev_size, bool p_free)
			: m_size(p_size),
			m_prev_size(p_prev_size),
			m_free(p_free)
#ifdef BC_MEMORY_DEFRAG
			, m_pointers{ {nullptr, nullptr, nullptr, nullptr} }
#endif
		{
		}

		inline bcSIZE _bc_heap_memblock::size() const noexcept
		{
			return m_size;
		}

		inline void _bc_heap_memblock::size(bcSIZE val) noexcept
		{
			m_size = val;
		}

		inline bcSIZE _bc_heap_memblock::next_size() const noexcept
		{
			return m_prev_size;
		}

		inline void _bc_heap_memblock::next_size(bcSIZE p_val) noexcept
		{
			m_prev_size = p_val;
		}

		inline bool _bc_heap_memblock::free() const noexcept
		{
			return m_free;
		}

		inline void _bc_heap_memblock::free(bool p_val) noexcept
		{
			m_free = p_val;
#ifdef BC_MEMORY_DEFRAG
			if (p_val)
			{
				m_pointers = { nullptr, nullptr, nullptr, nullptr };
			}
#endif
		}

#ifdef BC_MEMORY_DEFRAG
		inline _bc_heap_memblock::pointer_refs::iterator _bc_heap_memblock::pointers_begin() noexcept
		{
			return m_pointers.begin();
		}

		inline _bc_heap_memblock::pointer_refs::iterator _bc_heap_memblock::pointers_end() noexcept
		{
			return m_pointers.end();
		}
#endif

		inline void _bc_heap_memblock::lock() noexcept
		{
			m_mutex.lock();
		}

		inline void _bc_heap_memblock::unlock() noexcept
		{
			m_mutex.unlock();
		}

		inline bool _bc_heap_memblock::try_lock() noexcept
		{
			return m_mutex.try_lock();
		}

#ifdef BC_MEMORY_DEFRAG
		// If any pointer exist in this block or this block is not free return MemBlock of one of it's pointers 
		// else return nullptr that indicate this block is free /
		inline bc_memblock* _bc_heap_memblock::get_memblock() noexcept
		{
			void* l_pointer = nullptr;

			for (void** l_ptr : m_pointers)
			{
				if (l_ptr != nullptr)
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

		inline void _bc_heap_memblock::register_new_pointer(void** p_pointer) noexcept
		{
			bool l_has_set = false;

			{
				platform::bc_spin_mutex_guard l_lock(m_mutex);

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

			BC_ASSERT(l_has_set);
		}

		inline void _bc_heap_memblock::unregister_pointer(void** p_pointer) noexcept
		{
			bool l_found = false;

			{
				platform::bc_spin_mutex_guard l_lock(m_mutex);

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

			BC_ASSERT(l_found);
		}

#endif
		inline bcSIZE bc_memory_heap::capacity() const noexcept
		{
			return m_capacity;
		}

		inline bcSIZE bc_memory_heap::fragmentation_count() const noexcept
		{
			return m_num_fragmentation.load(platform::bc_memory_order::relaxed);
		}
#endif
	}
}