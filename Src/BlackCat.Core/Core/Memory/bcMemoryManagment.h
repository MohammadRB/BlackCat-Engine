//  [8/30/2013 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "Core/CorePCH.h"
#include "Core/bcExport.h"
#include "Core/Memory/bcMemoryCRT.h"
#include "Core/Memory/bcMemoryFixedSize.h"
#include "Core/Memory/bcMemoryStack.h"
#include "Core/Memory/bcMemoryHeap.h"
#include <unordered_map>

namespace black_cat
{
	namespace core
	{
		enum class bc_mem_size
		{
			b = 1,
			kb = 1024,
			mb = 1048576
		};

		enum class bc_alloc_type : bcUINT8
		{
			program,				// Alloc in program stack
			frame,					// Alloc in frame stack
			unknown,				// Try to alloc in fixed size allocators, otherwise alloc in crt memory
			unknown_movable,		// Try to alloc in fixed size allocators, then in super heap that is defragmentable , otherwise in crt memory
		};

#ifdef BC_MEMORY_LEAK_DETECTION
		struct bc_mem_block_leak_information;
#endif
		
#ifdef BC_MEMORY_ENABLE

		template< typename TMemory >
		class bc_memory_extender; // used forward declaration duo to dependencies between bcPtr and bcDelegate

		class BC_CORE_DLL bc_memory_manager : core_platform::bc_no_copy
		{
		public:
			bc_memory_manager() noexcept;

			~bc_memory_manager();

			void initialize(bcSIZE p_max_num_thread,
				bcSIZE p_fsa_start_size,
				bcSIZE p_fsa_num,
				bcSIZE p_fsa_step_size,
				bcSIZE p_fsa_num_allocations,
				bcSIZE p_per_prg_heap_size,
				bcSIZE p_per_frm_heap_size,
				bcSIZE p_super_heap_size);

			void destroy() noexcept;

			static void startup(bcSIZE p_max_num_thread,
				bcSIZE p_fsa_start_size,
				bcSIZE p_fsa_num,
				bcSIZE p_fsa_step_size,
				bcSIZE p_fsa_num_allocations,
				bcSIZE p_per_prg_heap_size,
				bcSIZE p_per_frm_heap_size,
				bcSIZE p_super_heap_size);

			static void close() noexcept;

			static bc_memory_manager& get() noexcept
			{
				return bc_memory_manager::m_instance;
			}

			void* alloc(bcSIZE p_size, bc_alloc_type p_alloc_type, const bcCHAR* p_file, bcUINT32 p_line) noexcept;

			void free(void* p_pointer) noexcept;

			void* realloc(void* p_pointer, bcSIZE p_new_size, bc_alloc_type p_alloc_type, const bcCHAR* p_file, bcUINT32 p_line) noexcept;

			void* aligned_alloc(bcSIZE p_size, bcINT32 p_alignment, bc_alloc_type p_alloc_type, const bcCHAR* p_file, bcUINT32 p_line) noexcept;

			void aligned_free(void* p_pointer) noexcept;
			
			void* aligned_realloc(void* p_pointer, bcSIZE p_new_size, bcINT32 p_alignment, bc_alloc_type p_alloc_type, const bcCHAR* p_file, bcUINT32 p_line) noexcept;

#ifdef BC_MEMORY_DEFRAG
			void register_pointer_in_movable_allocators(void** p_pointer) noexcept;

			void unregister_pointer_in_movable_allocators(void** p_pointer) noexcept;
#endif

			void end_of_frame() noexcept;

			bcSIZE get_total_size() const;

			bcSIZE get_used_size() const;

			bcSIZE get_overhead_size() const;

			bcSIZE get_max_used_size() const;

#ifdef BC_MEMORY_LEAK_DETECTION
			bcUINT32 report_memory_leaks() const;
#endif

		private:
			bcUINT32 _fsa_index_max_size(bcUINT32 p_index) const;

			bcUINT32 _get_fsa_index(bcUINT32 p_size) const;

			// We don't use static pointer because it is possible after we free pointer on exit, some call to free be called
			static bc_memory_manager m_instance;

			bcSIZE m_fsa_allocators_start_size;
			bcSIZE m_fsa_num_allocators;
			bcSIZE m_fsa_step_size;
			bool m_initialized;
			bc_memory_extender<bc_memory_fixed_size>* m_fsa_allocators;
			bc_memory_stack* m_per_program_stack;
			bc_memory_stack* m_per_frame_stack;
			bc_memory_heap* m_super_heap;
			bc_memory_crt* m_crt_allocator;
#ifdef BC_MEMORY_LEAK_DETECTION
			core_platform::bc_atomic< bcUINT32 > m_allocation_count;
			mutable core_platform::bc_mutex m_leak_allocator_mutex;
			std::unordered_map<void*, bc_mem_block_leak_information>* m_leak_allocator;
#endif
		};

#ifdef BC_MEMORY_DEFRAG
		inline void bc_memory_manager::register_pointer_in_movable_allocators(void** p_pointer) noexcept
		{
			if (m_super_heap->contain_pointer(*p_pointer))
			{
				bc_memblock* l_block = bc_memblock::retrieve_mem_block(*p_pointer);

				m_super_heap->register_pointer(p_pointer, l_block);
			}
		}

		inline void bc_memory_manager::unregister_pointer_in_movable_allocators(void** p_pointer) noexcept
		{
			if (m_super_heap->contain_pointer(*p_pointer))
			{
				bc_memblock* l_block = bc_memblock::retrieve_mem_block(*p_pointer);

				m_super_heap->unregister_pointer(p_pointer, l_block);
			}
		}
#endif

		inline bcUINT32 bc_memory_manager::_fsa_index_max_size(bcUINT32 p_index) const
		{
			return m_fsa_allocators_start_size + p_index * m_fsa_step_size;
		}

		inline bcUINT32 bc_memory_manager::_get_fsa_index(bcUINT32 p_size) const
		{
			return std::ceil((p_size - m_fsa_allocators_start_size) / m_fsa_step_size);
		}

#endif
	}
}