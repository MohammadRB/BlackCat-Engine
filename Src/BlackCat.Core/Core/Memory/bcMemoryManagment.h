// [08/30/2013 MRB]

#pragma once

#include <unordered_map>
#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "CorePlatformImp/Concurrency/bcThreadLocal.h"
#include "Core/CorePCH.h"
#include "Core/bcExport.h"
#include "Core/Memory/bcMemoryCRT.h"
#include "Core/Memory/bcMemoryFixedSize.h"
#include "Core/Memory/bcMemoryStack1.h"
#include "Core/Memory/bcMemoryHeap.h"

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

		// used forward declaration duo to dependencies between bcPtr and bcDelegate
		template<typename TMemory>
		class bc_memory_extender; 

		class BC_CORE_DLL bc_memory_manager : platform::bc_no_copy
		{
		public:
			bc_memory_manager() noexcept;

			~bc_memory_manager();

			void initialize(bcSIZE p_max_num_thread,
				bcSIZE p_fsa_start_size,
				bcSIZE p_fsa_count,
				bcSIZE p_fsa_step_size,
				bcSIZE p_fsa_capacity,
				bcSIZE p_program_heap_capacity,
				bcSIZE p_frame_heap_capacity,
				bcSIZE p_super_heap_capacity);

			void destroy() noexcept;

			static void startup(bcSIZE p_max_num_thread,
				bcSIZE p_fsa_start_size,
				bcSIZE p_fsa_count,
				bcSIZE p_fsa_step_size,
				bcSIZE p_fsa_capacity,
				bcSIZE p_program_heap_capacity,
				bcSIZE p_frame_heap_capacity,
				bcSIZE p_super_heap_capacity);

			static void close() noexcept;

			static bc_memory_manager& get() noexcept
			{
				return bc_memory_manager::m_instance;
			}

			void* alloc(bcSIZE p_size, bc_alloc_type p_alloc_type, const bcCHAR* p_file, bcUINT32 p_line) noexcept;

			void free(void* p_pointer) noexcept;

			void* realloc(void* p_pointer, bcSIZE p_new_size, bc_alloc_type p_alloc_type, const bcCHAR* p_file, bcUINT32 p_line) noexcept;

			void* aligned_alloc(bcSIZE p_size, bcSIZE p_alignment, bc_alloc_type p_alloc_type, const bcCHAR* p_file, bcUINT32 p_line) noexcept;

			void aligned_free(void* p_pointer) noexcept;
			
			void* aligned_realloc(void* p_pointer, bcSIZE p_new_size, bcSIZE p_alignment, bc_alloc_type p_alloc_type, const bcCHAR* p_file, bcUINT32 p_line) noexcept;

#ifdef BC_MEMORY_DEFRAG
			void register_pointer_in_movable_allocators(void** p_pointer) noexcept;

			void unregister_pointer_in_movable_allocators(void** p_pointer) noexcept;
#endif

			void end_of_frame() noexcept;

			bcSIZE get_total_size() const noexcept;

			bcSIZE get_used_size() const noexcept;

			bcSIZE get_overhead_size() const noexcept;

			bcSIZE get_max_used_size() const noexcept;

#ifdef BC_MEMORY_LEAK_DETECTION
			bcUINT32 report_memory_leaks() const noexcept;
#endif

		private:
			bcUINT32 _fsa_max_size(bcUINT32 p_index) const noexcept;

			bcUINT32 _get_fsa_index(bcUINT32 p_size) const noexcept;

			// We don't use static pointer because it is possible after we free pointer on exit, some call to free be called
			static bc_memory_manager m_instance;

			bcSIZE m_fsa_allocators_start_size;
			bcSIZE m_fsa_allocators_count;
			bcSIZE m_fsa_step_size;
			bcUINT32 m_worker_frame_allocators_count;
			bool m_initialized;

			bc_memory_extender<bc_memory_fixed_size>* m_fsa_allocators;
			bc_memory_stack1* m_program_allocator;
			bc_memory_stack1* m_worker_frame_allocators;
			bc_memory_stack1* m_frame_allocator;
			bc_memory_heap* m_heap_allocator;
			bc_memory_crt* m_crt_allocator;

			platform::bc_atomic<bcUINT32> m_last_used_frame_allocator;
			platform::bc_thread_local<bc_memory_stack1> m_my_frame_allocator;

#ifdef BC_MEMORY_LEAK_DETECTION
			platform::bc_atomic<bcUINT32> m_allocation_count;
			mutable platform::bc_mutex m_leak_allocator_mutex;
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
#endif
	}
}