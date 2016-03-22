// [10/24/2014 MRB]

#pragma once

#include "Core/CorePCH.h"
#include "Core/bcCoreUtility.h"
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

		class BC_COREDLL_EXP bc_memory_heap : public bc_memory_movable, public bc_initializable<bcSIZE, const bcCHAR*>
		{
		public:
			using this_type = bc_memory_heap;
#ifdef BC_MEMORY_DEFRAG
			using is_movabe_type = std::true_type;
#else
			using is_movabe_type = std::false_type;
#endif

		public:
			bc_memory_heap() noexcept(true);

			bc_memory_heap(this_type&& p_other) noexcept(true);

			~bc_memory_heap();

			this_type& operator =(this_type&& p_other) noexcept(true);

			bcInline bcSIZE size() { return m_size; };

			bcInline bcSIZE fragmentation_count() { return m_num_fragmentation; };

			void* alloc(bc_memblock* p_memblock) noexcept(true) override;

			void free(void* p_pointer, bc_memblock* p_memblock) noexcept(true) override;

			bool contain_pointer(void* p_memory) const noexcept(true) override;

			void clear() noexcept(true) override;

#ifdef BC_MEMORY_DEFRAG
			void register_pointer(void** p_pointer) noexcept(true) override;

			void unregister_pointer(void** p_pointer) noexcept(true) override;

			void defragment(bcINT32 p_num_defrag) noexcept(true) override;
#endif

		protected:

		private:
			void _initialize(bcSIZE p_size, const bcCHAR* p_tag) override;

			void _destroy() noexcept(true) override;

			struct heap_memblock
			{
			public:
#ifdef BC_MEMORY_DEFRAG
				using pointer_refs = std::array<void**, 4>;
				/*struct pointer_refs
				{
					const void** m_pointer_reference;
					pointer_refs* m_next;
				};*/
#endif

			public:
				heap_memblock()
					: m_data_address(0),
					m_size(0),
					m_next(nullptr),
					m_prev(nullptr),
#ifdef BC_MEMORY_DEFRAG
					/*m_pointers(nullptr)*/
					m_pointers{ { nullptr, nullptr, nullptr, nullptr } }
#endif
				{
					m_pointers.fill(nullptr);
					free(true);
				}

				bcUINTPTR data_address() const { return m_data_address & m_address_mask; }
				void data_address(bcUINTPTR val)
				{ 
					bcAssert((val & m_free_mask) == 0);
					m_data_address = free() ? val | m_free_mask : val & m_address_mask; 
				}

				bcSIZE size() const { return m_size; }
				void size(bcSIZE val) { m_size = val; }

				bool free() const { return m_data_address & m_free_mask; }
				void free(bool val) 
				{ 
					m_data_address = val ? m_data_address | m_free_mask : m_data_address & m_address_mask;
				}

				heap_memblock* next() const { return m_next; }
				void next(heap_memblock* val) { m_next = val; }

				heap_memblock* prev() const { return m_prev; }
				void prev(heap_memblock* val) { m_prev = val; }

#ifdef BC_MEMORY_DEFRAG
				/*const pointer_refs* pointers() const
				{
					return m_pointers;
				}*/
				pointer_refs::iterator pointer_refs_begin()
				{
					return m_pointers.begin();
				}

				pointer_refs::iterator pointer_refs_end()
				{
					return m_pointers.end();
				}
#endif

				void lock(core_platform::bc_lock_operation p_operation) { m_mutex.lock(p_operation); }

				void unlock() { m_mutex.unlock(); }

				bool try_lock(core_platform::bc_lock_operation p_operation) { return m_mutex.try_lock(p_operation); }

#ifdef BC_MEMORY_DEFRAG
				// If any pointer exist in this block or this block is not free return MemBlock of one of it's pointers 
				// else return nullptr that indicate this block is free /
				bcInline bc_memblock* get_memblock()
				{
					void** l_pointer = nullptr;

					for (void** l_ptr : m_pointers)
					{
						if (*l_ptr != nullptr)
						{
							*l_pointer = *l_ptr;
							break;
						}
					}

					/*if (m_pointers)
						return bc_memblock::retrieve_mem_block(*(m_pointers->m_pointer_reference));*/
					if (l_pointer)
						return bc_memblock::retrieve_mem_block(*l_pointer);
					return nullptr;
				}

				bcInline void register_new_pointer(/*pointer_refs* p_pointer_ref, */void** p_pointer)
				{
					//p_pointer_ref->m_pointer_reference = p_pointer;

					core_platform::bc_lock_guard< core_platform::bc_hybrid_mutex > l_guard(m_mutex, 
						core_platform::bc_lock_operation::light);

					bool l_has_set = false;
					for (void** l_ptr : m_pointers)
					{
						if (l_ptr == nullptr)
						{
							l_ptr = p_pointer;
							l_has_set = true;
							break;
						}
					}

					bcAssert(l_has_set);

					/*if(m_pointers)
					{
						p_pointer_ref->m_next = m_pointers->m_next;
						m_pointers = p_pointer_ref;
					}
					else
						m_pointers = p_pointer_ref;*/
				}

				bcInline void unregister_pointer(void** p_pointer)
				{
					core_platform::bc_lock_guard< core_platform::bc_hybrid_mutex > l_guard(m_mutex,
						core_platform::bc_lock_operation::light);

					bool l_found = false;
					for (void** l_ptr : m_pointers)
					{
						if (l_ptr && *l_ptr == *p_pointer)
						{
							l_found = true;
							l_ptr = nullptr;
							break;
						}
					}

					bcAssert(l_found);

					/*pointer_refs* l_prev_pointer_ref = nullptr;
					pointer_refs* l_pointer_ref = m_pointers;
					while (l_pointer_ref && *(l_pointer_ref->m_pointer_reference) != *p_pointer)
					{
						l_prev_pointer_ref = l_pointer_ref;
						l_pointer_ref = l_pointer_ref->m_next;
					}

					if (l_prev_pointer_ref && l_pointer_ref)
					{
						l_prev_pointer_ref->m_next = l_pointer_ref->m_next;
					}
					else if (!l_prev_pointer_ref && l_pointer_ref)
					{
						m_pointers = l_pointer_ref->m_next;
					}
					else
					{
						bcAssert(false);
					}

					return l_pointer_ref;*/
				}
#endif
			protected:
				
			private:
				static const bcINT32 m_free_mask = 1;
				static const bcINT32 m_address_mask = ~m_free_mask;
				
				bcUINTPTR m_data_address;
				bcSIZE m_size;
				heap_memblock* m_next;
				heap_memblock* m_prev;
				core_platform::bc_hybrid_mutex m_mutex;
#ifdef BC_MEMORY_DEFRAG
				/*pointer_refs* m_pointers;*/
				pointer_refs m_pointers;
#endif

			};

			void _move(this_type&& p_other)
			{
				heap_memblock* l_top = m_top.load(core_platform::bc_memory_order::acquire);

				m_first_memblock = p_other.m_first_memblock;
				m_heap = p_other.m_heap;
				m_size = p_other.m_size;
				m_remaining_free_space_limit = p_other.m_remaining_free_space_limit;
				m_num_fragmentation = p_other.m_num_fragmentation;
				m_block_allocator = std::move(p_other.m_block_allocator);
#ifdef BC_MEMORY_DEFRAG
				//m_pointer_ref_allocator = std::move(p_other.m_pointer_ref_allocator);
#endif

				p_other.m_first_memblock = nullptr;
				p_other.m_heap = nullptr;
				p_other.m_size = 0;
				p_other.m_remaining_free_space_limit = 0;
				p_other.m_num_fragmentation = 0;

				m_top.store(l_top, core_platform::bc_memory_order::release);
			}

			bc_memmng_freelist<heap_memblock> m_block_allocator;
#ifdef BC_MEMORY_DEFRAG
			//bc_memmng_freelist<heap_memblock::pointer_refs> m_pointer_ref_allocator;
#endif

			heap_memblock* m_first_memblock;
			core_platform::bc_atomic< heap_memblock* > m_top;
			bcUBYTE* m_heap;
			bcSIZE m_size;
			bcSIZE m_remaining_free_space_limit;
			bcSIZE m_num_fragmentation;
		};

#endif
	}
}