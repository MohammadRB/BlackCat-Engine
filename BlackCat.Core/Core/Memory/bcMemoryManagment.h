//  [8/30/2013 MRB]

#pragma once

#include "Core/CorePCH.h"
#include "Core/bcCoreUtility.h"
#include "Core/Memory/bcMemoryCRT.h"
#include "Core/Memory/bcMemoryFixedSize.h"
#include "Core/Memory/bcMemoryStack.h"
#include "Core/Memory/bcMemoryHeap.h"
#include "Core/Memory/bcMemMngHashMap.h"
#include "Core/Memory/bcMemoryExtender.h"

namespace black_cat
{
	namespace core
	{
#define B 1
#define KB 1024
#define MB 1048576

		enum class bc_alloc_type
		{
			program,
			level,
			frame,
			unknown,
			unknown_movale,
		};

#ifdef BC_MEMORY_ENABLE

		class BC_COREDLL_EXP bc_memmng : core_platform::bc_no_copy
		{
		public:
			bc_memmng() noexcept(true);

			~bc_memmng();

			void initialize(bcUINT32 p_fsa_start_size,
				bcUINT32 p_fsa_num,
				bcUINT32 p_fsa_num_allocations,
				bcUINT32 p_per_prg_heap_size,
				bcUINT32 p_per_lvl_heap_size,
				bcUINT32 p_per_frm_heap_size,
				bcUINT32 p_super_heap_size);

			void destroy() noexcept(true);

			static void startup(bcUINT32 p_fsa_start_size,
				bcUINT32 p_fsa_num,
				bcUINT32 p_fsa_num_allocations,
				bcUINT32 p_per_prg_heap_size,
				bcUINT32 p_per_lvl_heap_size,
				bcUINT32 p_per_frm_heap_size,
				bcUINT32 p_super_heap_size);

			static void close() noexcept(true);

			bcInline static bc_memmng& get() noexcept(true)
			{
				return bc_memmng::m_instance;
			};

			void* alloc(bcSIZE p_size, bc_alloc_type p_alloc_type, const bcCHAR* p_file, bcUINT32 p_line) noexcept(true);

			void free(void* p_pointer) noexcept(true);

			void* realloc(void* p_pointer, bcSIZE p_new_size, bc_alloc_type p_alloc_type, const bcCHAR* p_file, bcUINT32 p_line) noexcept(true);

			void* aligned_alloc(bcSIZE p_size, bcINT32 p_alignment, bc_alloc_type p_alloc_type, const bcCHAR* p_file, bcUINT32 p_line) noexcept(true);

			void aligned_free(void* p_pointer) noexcept(true);
			
			void* aligned_realloc(void* p_pointer, bcSIZE p_new_size, bcINT32 p_alignment, bc_alloc_type p_alloc_type, const bcCHAR* p_file, bcUINT32 p_line) noexcept(true);

			template<typename T, typename ...TArgs>
			T* new_(TArgs&&... p_args, bc_alloc_type p_alloc_type, const bcCHAR* p_file, bcUINT32 p_line)
			{
				T* l_return_pointer;

				while (true)
				{
					l_return_pointer = reinterpret_cast<T*>(alloc(sizeof(T), p_alloc_type, p_file, p_line));

					if (!l_return_pointer)
					{
						std::new_handler l_handler = std::get_new_handler();
						if (l_handler)
						{
							(*l_handler)();
							continue;
						}

						throw std::bad_alloc();
					}

					new(l_return_pointer)T(std::forward<T>(p_args)...);
					break;
				}

				return l_return_pointer;
			};

			template<typename T>
			T* new_array(bcUINT32 p_array_length, bc_alloc_type p_alloc_type, const bcCHAR* p_file, bcUINT32 p_line)
			{
				T* l_return_pointer;

				// alloc 4 byte more for array length, it will be used in last 4 byte of allocated block
				bcUINT32 l_size = sizeof(T)* p_array_length + sizeof(bcUINT32);
				l_return_pointer = reinterpret_cast<T*>(alloc(l_size, p_alloc_type, p_file, p_line));
				
				// store array length
				bc_memblock* l_block = bc_memblock::retrieve_mem_block(l_return_pointer);
				*(reinterpret_cast<bcUINT32*>(reinterpret_cast<bcUINTPTR>(l_return_pointer)-l_block->offset() + l_block->size() - sizeof(bcUINT32))) = p_array_length;

				T* l_first = l_return_pointer;
				for (register bcUINT32 i = 0; i < p_array_length; ++i, ++l_first)
					new(l_first)T();

				return l_return_pointer;
			};

			template<typename T>
			void delete_(T* p_pointer) noexcept(true)
			{
				if (!p_pointer) return;

				p_pointer->~T();
				free(p_pointer);
			};

			template< typename T >
			void delete_array(T* p_pointer) noexcept(true)
			{
				if (!p_pointer) return;

				T* l_first = p_pointer;

				// Retrieve array length
				bc_memblock* l_block = bc_memblock::retrieve_mem_block(l_first);
				bcUINT32 l_count = *(reinterpret_cast<bcUINT32*>(reinterpret_cast<bcUINTPTR>(p_pointer)-l_block->offset() + l_block->size() - sizeof(bcUINT32)));

				for (register bcUINT32 i = 0; i < l_count; ++i, ++l_first)
					l_first->~T();

				free(p_pointer);
			};

			template<typename T, typename ...TArgs>
			T* aligned_new(TArgs&&... p_args, bcUINT32 p_alignment, bc_alloc_type p_alloc_type, const bcCHAR* p_file, bcUINT32 p_line)
			{
				T* l_return_pointer;

				while (true)
				{
					l_return_pointer = reinterpret_cast<T*>(aligned_alloc(sizeof(T), p_alignment, p_alloc_type, p_file, p_line));

					if (!l_return_pointer)
					{
						std::new_handler l_handler = std::get_new_handler();
						if (l_handler)
						{
							(*l_handler)();
							continue;
						}

						throw std::bad_alloc();
					}

					new(l_return_pointer)T(std::forward<T>(p_args)...);
					break;
				}

				return l_return_pointer;
			};

			template<typename T>
			T* aligned_new_array(bcUINT32 p_array_length, bcUINT32 p_alignment, bc_alloc_type p_alloc_type, const bcCHAR* p_file, bcUINT32 p_line)
			{
				T* l_return_pointer;

				// alloc 4 byte more for array length, it will be used in last 4 byte of allocated block
				bcUINT32 l_size = sizeof(T)* p_array_length + sizeof(bcUINT32);
				l_return_pointer = reinterpret_cast<T*>(aligned_alloc(l_size, p_alignment, p_alloc_type, p_file, p_line));
				
				// Store array length
				bc_memblock* l_block = bc_memblock::retrieve_mem_block(l_return_pointer);
				*(reinterpret_cast<bcUINT32*>((reinterpret_cast<bcUINTPTR>(l_return_pointer)-l_block->offset() + l_block->size() - sizeof(bcUINT32)))) = p_array_length;

				T* l_first = l_return_pointer;
				for (register bcUINT32 i = 0; i < p_array_length; ++i, ++l_first)
					new(l_first)T();

				return l_return_pointer;
			};

			template<typename T>
			void aligned_delete(T* p_pointer) noexcept(true)
			{
				if (!p_pointer) return;

				p_pointer->~T();
				aligned_free(p_pointer);
			};

			template<typename T>
			void aligned_delete_array(T* p_pointer) noexcept(true)
			{
				if (!p_pointer) return;

				T* l_first = p_pointer;

				// Retrieve array length
				bc_memblock* l_block = bc_memblock::retrieve_mem_block(l_first);
				bcUINT32 l_count = *(reinterpret_cast<bcUINT32*>(reinterpret_cast<bcUINTPTR>(p_pointer)-l_block->offset() + l_block->size() - sizeof(bcUINT32)));

				for (register bcUINT32 i = 0; i < l_count; ++i, ++l_first)
					l_first->~T();

				aligned_free(p_pointer);
			};

#ifdef BC_MEMORY_DEFRAG
			bcInline void register_pointer_in_movable_allocators(void** p_pointer) noexcept(true)
			{
				if (m_super_heap->contain_pointer(*p_pointer))
				{
					m_super_heap->register_pointer(p_pointer);
				}
			};

			bcInline void unregister_pointer_in_movable_allocators(void** p_pointer) noexcept(true)
			{
				if (m_super_heap->contain_pointer(*p_pointer))
				{
					m_super_heap->unregister_pointer(p_pointer);
				}
			};
#endif

			void end_of_frame() noexcept(true);

			bcSIZE get_total_size();

			bcSIZE get_used_size();

			bcSIZE get_overhead_size();

			bcSIZE get_max_used_size();

#ifdef BC_MEMORY_LEAK_DETECTION
			bcUINT32 report_memory_leaks();
#endif

		protected:

		private:
			bcInline bcUINT32 _fsa_index_max_size(bcUINT32 p_index)
			{
				return m_fsa_allocators_start_size + p_index * s_fsa_step_size;
			};

			bcInline bcUINT32 _get_fsa_index(bcSIZE p_size)
			{
				bcUINT32 l_index = std::ceil((p_size - m_fsa_allocators_start_size) / s_fsa_step_size);
				return l_index;
			};

			// We don't use static pointer because it is possible after we free pointer on exit, some call to free be called
			static bc_memmng m_instance;
			static const bcUINT32 s_fsa_step_size = BC_MEMORY_MIN_ALIGN;

			bcUINT32 m_fsa_allocators_start_size;
			bcUINT32 m_fsa_num_allocators;
			bool m_initialized;
			bc_memory_extender<bc_memory_fixed_size>* m_fsa_allocators;
			bc_memory_stack* m_per_program_stack;
			bc_memory_stack* m_per_level_stack;
			bc_memory_stack* m_per_frame_stack;
			bc_memory_heap* m_super_heap;
			bc_memory_crt* m_crt_allocator;
#ifdef BC_MEMORY_LEAK_DETECTION
			bc_memmng_hashmap<bc_mem_block_leak_information, 15, 5>* m_leak_allocator;
#endif
		};

#endif

	}
}