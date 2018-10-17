//  [9/2/2013 MRB]

#include "Core/CorePCH.h"
#include "Core/Memory/bcMemoryManagment.h"
#include "Core/Memory/bcMemoryExtender.h"

namespace black_cat
{
	namespace core
	{
#ifdef BC_MEMORY_ENABLE

		bc_memmng bc_memmng::m_instance;

		bc_memmng::bc_memmng() noexcept(true)
			: m_fsa_allocators_start_size(0),
			m_fsa_num_allocators(0),
			m_fsa_step_size(0),
			m_initialized(false),
			m_fsa_allocators(nullptr),
			m_per_program_stack(nullptr),
			m_per_level_stack(nullptr),
			m_per_frame_stack(nullptr),
			m_super_heap(nullptr),
			m_crt_allocator(nullptr)
#ifdef BC_MEMORY_LEAK_DETECTION
			,
			m_allocation_count(0),
			m_leak_allocator(nullptr)
#endif
		{
		}

		bc_memmng::~bc_memmng() = default;

		void bc_memmng::initialize(bcUINT32 p_fsa_start_size,
			bcUINT32 p_fsa_num,
			bcUINT32 p_fsa_step_size,
			bcUINT32 p_fsa_num_allocations,
			bcUINT32 p_per_prg_heap_size,
			bcUINT32 p_per_lvl_heap_size,
			bcUINT32 p_per_frm_heap_size,
			bcUINT32 p_super_heap_size)
		{
			m_fsa_allocators_start_size = p_fsa_start_size;
			m_fsa_num_allocators = p_fsa_num;
			m_fsa_step_size = p_fsa_step_size;

			try
			{
				m_fsa_allocators = new bc_memory_extender<bc_memory_fixed_size>[m_fsa_num_allocators];

				for (bcUINT32 i = 0; i < m_fsa_num_allocators; i++)
				{
					auto l_initialize_delegate = bc_delegate< bc_memory_fixed_size*() >
					(
						[=]()
						{
							const bcUINT32 l_block_size = m_fsa_allocators_start_size + i * m_fsa_step_size;
							std::string l_tag = std::string("FSA") + std::to_string(i);

							bc_memory_fixed_size* l_result = new bc_memory_fixed_size();
							l_result->initialize(p_fsa_num_allocations, l_block_size, l_tag.c_str());

							return l_result;
						}
					);
					auto l_destroy_delegate = bc_delegate< void(bc_memory_fixed_size*) >
					(
						[=](bc_memory_fixed_size* p_pointer)
						{
							delete p_pointer;
						}
					);

					m_fsa_allocators[i].initialize(std::move(l_initialize_delegate), std::move(l_destroy_delegate));
				}

				m_per_program_stack = new bc_memory_stack();
				m_per_program_stack->initialize(p_per_prg_heap_size, "PerProgramStack");
				m_per_level_stack = new bc_memory_stack();
				m_per_level_stack->initialize(p_per_lvl_heap_size, "PerLevelStack");
				m_per_frame_stack = new bc_memory_stack();
				m_per_frame_stack->initialize(p_per_frm_heap_size, "PerFrameStack");

				m_super_heap = new bc_memory_heap();
				m_super_heap->initialize(p_super_heap_size, "SuperHeap");

				m_crt_allocator = new bc_memory_crt();
				m_crt_allocator->initialize("CrtAllocator");

#ifdef BC_MEMORY_LEAK_DETECTION
				m_allocation_count.store(0, core_platform::bc_memory_order::relaxed);
				m_leak_allocator = new bc_memmng_hashmap<bc_mem_block_leak_information, 15, 5>();
#endif
			}
			catch (std::bad_alloc& ex)
			{
				destroy();
				throw;
			}

			m_initialized = true;
		}

		void bc_memmng::destroy() noexcept(true)
		{
			delete[] (m_fsa_allocators);
			delete (m_per_program_stack);
			delete (m_per_level_stack);
			delete (m_per_frame_stack);
			delete (m_super_heap);
			delete (m_crt_allocator);
#ifdef BC_MEMORY_LEAK_DETECTION
			delete (m_leak_allocator);
#endif

			m_initialized = false;
		}

		void bc_memmng::startup(bcUINT32 p_fsa_start_size,
			bcUINT32 p_fsa_num,
			bcUINT32 p_fsa_step_size,
			bcUINT32 p_fsa_num_allocations,
			bcUINT32 p_per_prg_heap_size,
			bcUINT32 p_per_lvl_heap_size,
			bcUINT32 p_per_frm_heap_size,
			bcUINT32 p_super_heap_size)
		{		
						
			bc_memmng::m_instance.initialize(p_fsa_start_size,
				p_fsa_num,
				p_fsa_step_size,
				p_fsa_num_allocations,
				p_per_prg_heap_size,
				p_per_lvl_heap_size,
				p_per_frm_heap_size,
				p_super_heap_size);
		}
		
		void bc_memmng::close() noexcept(true)
		{
			bc_memmng::m_instance.destroy();
		}
		
		void* bc_memmng::alloc(bcSIZE p_size, bc_alloc_type p_allocType, const bcCHAR* p_file, bcUINT32 p_line) noexcept(true)
		{
			bcAssert(m_initialized);

			void* l_result = nullptr;
			bc_memory* l_allocator = nullptr;
			bc_memblock l_block;
			bc_memblock::initialize_mem_block_before_allocation(p_size, BC_MEMORY_MIN_ALIGN, &l_block);
			bcSIZE l_size = l_block.size();

			switch (p_allocType)
			{
			case bc_alloc_type::frame:
				l_result = m_per_frame_stack->alloc(&l_block);
				l_allocator = m_per_frame_stack;
				break;

			case bc_alloc_type::unknown:
			case bc_alloc_type::unknown_movable:
				// Check request can be handled by FSAllocators
				if (l_size <= _fsa_index_max_size(m_fsa_num_allocators - 1))
				{
					bcUINT32 l_fsa_index = _get_fsa_index(l_size);
					l_result = (m_fsa_allocators + l_fsa_index)->alloc(&l_block);
					l_allocator = (m_fsa_allocators + l_fsa_index);
				}
				break;

			case bc_alloc_type::level:
				l_result = m_per_level_stack->alloc(&l_block);
				l_allocator = m_per_level_stack;
				break;

			case bc_alloc_type::program:
				l_result = m_per_program_stack->alloc(&l_block);
				l_allocator = m_per_program_stack;
				break;
			}

			if (!l_result && p_allocType == bc_alloc_type::unknown_movable)
			{
				l_result = m_super_heap->alloc(&l_block);
				l_allocator = m_super_heap;
			}

			if (!l_result)
			{
				l_result = m_crt_allocator->alloc(&l_block);
				l_allocator = m_crt_allocator;
			}

			if (!l_result)
			{
				return l_result;
			}

			bc_memblock::initialize_mem_block_after_allocation(&l_result, (l_allocator == m_super_heap), l_allocator, &l_block);

#ifdef BC_MEMORY_LEAK_DETECTION
			m_leak_allocator->insert
			(
				reinterpret_cast< void* >(reinterpret_cast< bcUINTPTR >(l_result) - l_block.offset()),
				bc_mem_block_leak_information
				(
					l_result,
					m_allocation_count.fetch_add(1, core_platform::bc_memory_order::relaxed) + 1,
					p_size,
					p_line,
					p_file
				)
			);
#endif

			return l_result;
		}
		
		void bc_memmng::free(void* p_pointer) noexcept(true)
		{
			if (!p_pointer)
			{
				return;
			}

			bc_memblock* l_block = bc_memblock::retrieve_mem_block(p_pointer);
			void* l_pointer = reinterpret_cast< void* >(reinterpret_cast< bcUINTPTR >(p_pointer) - l_block->offset());

			bcAssert(l_block);

			bc_memory* l_allocator = reinterpret_cast<bc_memory*>(l_block->extra());
			l_allocator->free(l_pointer, l_block);

			bc_memblock::free_mem_block(l_pointer);

#ifdef BC_MEMORY_LEAK_DETECTION
			m_leak_allocator->remove(l_pointer);
#endif
		}
		
		void* bc_memmng::realloc(void* p_pointer, bcSIZE p_new_size, bc_alloc_type p_alloc_type, const bcCHAR* p_file, bcUINT32 p_line) noexcept(true)
		{
			bc_memblock* l_block = bc_memblock::retrieve_mem_block(p_pointer);

			void* l_new_pointer = alloc(p_new_size, p_alloc_type, p_file, p_line);
			if (!l_new_pointer) 
			{
				return l_new_pointer;
			}

			bc_memblock* l_new_block = bc_memblock::retrieve_mem_block(l_new_pointer);
			bcSIZE l_min_size = std::min<bcSIZE>(l_block->size() - l_block->offset(), l_new_block->size() - l_new_block->offset());

			std::memcpy
			(
				reinterpret_cast< void* >(reinterpret_cast< bcUINTPTR >(l_new_pointer) + l_new_block->offset()),
				reinterpret_cast< void* >(reinterpret_cast< bcUINTPTR >(p_pointer) + l_block->offset()),
				l_min_size
			);

			free(p_pointer);

			return l_new_pointer;
		};
		
		void* bc_memmng::aligned_alloc(bcSIZE p_size, bcINT32 p_alignment, bc_alloc_type p_alloc_type, const bcCHAR* p_file, bcUINT32 p_line) noexcept(true)
		{
			bcAssert(m_initialized);

			void* l_result = nullptr;
			bc_memory* l_allocator = nullptr;
			bc_memblock l_block;
			bc_memblock::initialize_mem_block_before_allocation(p_size, p_alignment, &l_block);
			bcSIZE l_size = l_block.size();

			switch (p_alloc_type)
			{
			case bc_alloc_type::frame:
				l_result = m_per_frame_stack->alloc(&l_block);
				l_allocator = m_per_frame_stack;
				break;

			case bc_alloc_type::unknown:
			case bc_alloc_type::unknown_movable:
				// Check can Request handled by FSAllocators
				if (l_size <= _fsa_index_max_size(m_fsa_num_allocators - 1))
				{
					l_result = (m_fsa_allocators + _get_fsa_index(l_size))->alloc(&l_block);
					l_allocator = (m_fsa_allocators + _get_fsa_index(l_size));
				}
				break;

			case bc_alloc_type::level:
				l_result = m_per_level_stack->alloc(&l_block);
				l_allocator = m_per_level_stack;
				break;

			case bc_alloc_type::program:
				l_result = m_per_program_stack->alloc(&l_block);
				l_allocator = m_per_program_stack;
				break;
			};

			if (!l_result && p_alloc_type == bc_alloc_type::unknown_movable)
			{
				l_result = m_super_heap->alloc(&l_block);
				l_allocator = m_super_heap;
			}

			if (!l_result)
			{
				l_result = m_crt_allocator->alloc(&l_block);
				l_allocator = m_crt_allocator;
			}

			if (!l_result)
			{
				return l_result;
			}

			bc_memblock::initialize_mem_block_after_allocation(&l_result, (l_allocator == m_super_heap), l_allocator, &l_block);

#ifdef BC_MEMORY_LEAK_DETECTION
			m_leak_allocator->insert
			(
				reinterpret_cast< void* >(reinterpret_cast< bcUINTPTR >(l_result) - l_block.offset()),
				bc_mem_block_leak_information
				(
					l_result,
					m_allocation_count.fetch_add(1, core_platform::bc_memory_order::relaxed) + 1,
					p_size,
					p_line,
					p_file
				)
			);
#endif

			return l_result;
		};
		
		void bc_memmng::aligned_free(void* p_pointer) noexcept(true)
		{
			if (!p_pointer) 
			{
				return;
			}

			bc_memblock* l_block = bc_memblock::retrieve_mem_block(p_pointer);
			void* l_pointer = reinterpret_cast< void* >(reinterpret_cast< bcUINTPTR >(p_pointer) - l_block->offset());

			bcAssert(l_block);

			bc_memory* l_allocator = reinterpret_cast<bc_memory*>(l_block->extra());
			l_allocator->free(l_pointer, l_block);

			bc_memblock::free_mem_block(l_pointer);

#ifdef BC_MEMORY_LEAK_DETECTION
			m_leak_allocator->remove(l_pointer);
#endif
		};
		
		void* bc_memmng::aligned_realloc(void* p_pointer, bcSIZE p_new_size, bcINT32 p_alignment, bc_alloc_type p_alloc_type, const bcCHAR* p_file, bcUINT32 p_line) noexcept(true)
		{
			bc_memblock* l_block = bc_memblock::retrieve_mem_block(p_pointer);

			void* l_new_pointer = aligned_alloc(p_new_size, p_alignment, p_alloc_type, p_file, p_line);
			bc_memblock* l_new_block = bc_memblock::retrieve_mem_block(l_new_pointer);

			bcSIZE l_min_size = std::min<bcSIZE>(l_block->size() - l_block->offset(), l_new_block->size() - l_new_block->offset());

			std::memcpy
			(
				reinterpret_cast< void* >(reinterpret_cast< bcUINTPTR >(l_new_pointer) + l_new_block->offset()),
				reinterpret_cast< void* >(reinterpret_cast< bcUINTPTR >(p_pointer) + l_block->offset()),
				l_min_size
			);

			free(p_pointer);

			return l_new_pointer;
		};

		void bc_memmng::end_of_frame() noexcept(true)
		{
#ifdef BC_MEMORY_DEFRAG
			bcUINT32 l_num_fragment = m_super_heap->fragmentation_count();
			l_num_fragment = l_num_fragment == 0 ? 0 : l_num_fragment / 3 + 1;

			m_super_heap->defragment
			(
				l_num_fragment,
#ifdef BC_MEMORY_LEAK_DETECTION
				[this](void* p_old, void* p_new)
				{
					this->m_leak_allocator->relocate(p_old, p_new);
				}
#else
				bc_memory_heap::defrag_callback()
#endif
			);
#endif

			bcAssert(m_per_frame_stack->tracer().alloc_count() == 0);
			m_per_frame_stack->clear();
		};

		bcSIZE bc_memmng::get_total_size() const
		{
#ifdef BC_MEMORY_TRACING
			bcSIZE l_total_size = 0;

			for(bcUINT32 i = 0; i < m_fsa_num_allocators; i++)
			{
				l_total_size += m_fsa_allocators[i].tracer().total_size();
			}

			l_total_size += m_per_program_stack->tracer().total_size();
			l_total_size += m_per_level_stack->tracer().total_size();
			l_total_size += m_per_frame_stack->tracer().total_size();
			l_total_size += m_super_heap->tracer().total_size();

			return l_total_size;
#endif
			return 0;
		};
		
		bcSIZE bc_memmng::get_used_size() const
		{
#ifdef BC_MEMORY_TRACING
			bcSIZE l_used_size = 0;

			for(bcUINT32 i = 0; i < m_fsa_num_allocators; i++)
				l_used_size += m_fsa_allocators[i].tracer().used_size();

			l_used_size += m_per_program_stack->tracer().used_size();
			l_used_size += m_per_level_stack->tracer().used_size();
			l_used_size += m_per_frame_stack->tracer().used_size();
			l_used_size += m_super_heap->tracer().used_size();

			return l_used_size;
#endif
			return 0;
		};
		
		bcSIZE bc_memmng::get_overhead_size() const
		{
#ifdef BC_MEMORY_TRACING
			bcSIZE l_wasted_size = 0;

			for(bcUINT32 i = 0; i < m_fsa_num_allocators; i++)
				l_wasted_size += m_fsa_allocators[i].tracer().overhead_size();

			l_wasted_size += m_per_program_stack->tracer().overhead_size();
			l_wasted_size += m_per_level_stack->tracer().overhead_size();
			l_wasted_size += m_per_frame_stack->tracer().overhead_size();
			l_wasted_size += m_super_heap->tracer().overhead_size();

			return l_wasted_size;
#endif
			return 0;
		};
		
		bcSIZE bc_memmng::get_max_used_size() const
		{
#ifdef BC_MEMORY_TRACING
			bcSIZE l_max_used_size = 0;

			for(bcUINT32 i = 0; i < m_fsa_num_allocators; i++)
				l_max_used_size += m_fsa_allocators[i].tracer().max_used_size();

			l_max_used_size += m_per_program_stack->tracer().max_used_size();
			l_max_used_size += m_per_level_stack->tracer().max_used_size();
			l_max_used_size += m_per_frame_stack->tracer().max_used_size();
			l_max_used_size += m_super_heap->tracer().max_used_size();

			return l_max_used_size;
#endif
			return 0;
		};

#ifdef BC_MEMORY_LEAK_DETECTION
		bcUINT32 bc_memmng::report_memory_leaks() const
		{
			bcSIZE l_leak_count = m_leak_allocator->count();
			
			m_leak_allocator->iterate_over([](void* p_key, bc_mem_block_leak_information& p_value)->void
			{
				bc_memblock* l_memblock = bc_memblock::retrieve_mem_block(p_value.m_pointer);
			});

			return l_leak_count;
		};
#endif

#endif
	}
}