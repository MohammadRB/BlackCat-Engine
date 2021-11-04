//  [9/2/2013 MRB]

#include "Core/CorePCH.h"
#include "Core/Memory/bcMemoryManagment.h"
#include "Core/Memory/bcMemoryExtender.h"

namespace black_cat
{
	namespace core
	{
#ifdef BC_MEMORY_ENABLE

#ifdef BC_MEMORY_LEAK_DETECTION
		struct bc_mem_block_leak_information
		{
			static const bcUINT32 s_filename_length = 20;

			void* m_pointer;
			bc_alloc_type m_type;
			bcSIZE m_number;
			bcSIZE m_requested_size;
			bcSIZE m_line_number;
			bcCHAR m_file_name[s_filename_length];

			bc_mem_block_leak_information()
				: m_pointer(nullptr),
				m_type(bc_alloc_type::unknown),
				m_number(0),
				m_requested_size(0),
				m_line_number(0),
				m_file_name()
			{
			}

			bc_mem_block_leak_information(void* p_pointer,
				bc_alloc_type p_type,
				bcSIZE p_number,
				bcSIZE p_requested_size,
				bcSIZE p_line_number,
				const bcCHAR* p_file_name) :
				m_pointer(p_pointer),
				m_type(p_type),
				m_number(p_number),
				m_requested_size(p_requested_size),
				m_line_number(p_line_number),
				m_file_name()
			{
				std::strcpy
				(
					m_file_name,
					&p_file_name[std::max<bcINT32>(static_cast<bcINT32>(std::strlen(p_file_name)) - (s_filename_length - 1), 0)]
				);
			}
		};
#endif
		
		bc_memory_manager bc_memory_manager::m_instance;

		bc_memory_manager::bc_memory_manager() noexcept
			: m_fsa_allocators_start_size(0),
			m_fsa_num_allocators(0),
			m_fsa_step_size(0),
			m_initialized(false),
			m_fsa_allocators(nullptr),
			m_per_program_stack(nullptr),
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

		bc_memory_manager::~bc_memory_manager() = default;

		void bc_memory_manager::initialize(bcSIZE p_max_num_thread,
			bcSIZE p_fsa_start_size,
			bcSIZE p_fsa_num,
			bcSIZE p_fsa_step_size,
			bcSIZE p_fsa_num_allocations,
			bcSIZE p_per_prg_heap_size,
			bcSIZE p_per_frm_heap_size,
			bcSIZE p_super_heap_size)
		{
			m_fsa_allocators_start_size = p_fsa_start_size;
			m_fsa_num_allocators = p_fsa_num;
			m_fsa_step_size = p_fsa_step_size;

			try
			{
				m_fsa_allocators = new bc_memory_extender<bc_memory_fixed_size>[m_fsa_num_allocators];

				for (bcUINT32 i = 0; i <m_fsa_num_allocators; i++)
				{
					auto l_initialize_delegate = bc_delegate<bc_memory_fixed_size*()>
					(
						[=]()
						{
							const bcSIZE l_block_size = m_fsa_allocators_start_size + i * m_fsa_step_size;
							const std::string l_tag = std::string("FSA") + std::to_string(i);

							auto* l_result = new bc_memory_fixed_size();
							l_result->initialize(p_fsa_num_allocations, l_block_size, l_tag.c_str());

							return l_result;
						}
					);
					auto l_destroy_delegate = bc_delegate<void(bc_memory_fixed_size*)>
					(
						[=](bc_memory_fixed_size* p_pointer)
						{
							p_pointer->destroy();
							delete p_pointer;
						}
					);

					m_fsa_allocators[i].initialize(std::move(l_initialize_delegate), std::move(l_destroy_delegate));
				}

				m_per_program_stack = new bc_memory_stack();
				m_per_program_stack->initialize(p_max_num_thread, p_per_prg_heap_size, "PerProgramStack");
				m_per_frame_stack = new bc_memory_stack();
				m_per_frame_stack->initialize(p_max_num_thread, p_per_frm_heap_size, "PerFrameStack");

				m_super_heap = new bc_memory_heap();
				m_super_heap->initialize(p_super_heap_size, "SuperHeap");

				m_crt_allocator = new bc_memory_crt();
				m_crt_allocator->initialize("CrtAllocator");

#ifdef BC_MEMORY_LEAK_DETECTION
				m_allocation_count.store(0, core_platform::bc_memory_order::relaxed);
				m_leak_allocator = new std::unordered_map<void*, bc_mem_block_leak_information>();
#endif
			}
			catch (std::bad_alloc& ex)
			{
				destroy();
				throw;
			}

			m_initialized = true;
		}

		void bc_memory_manager::destroy() noexcept
		{
			delete[] (m_fsa_allocators);
			delete (m_per_program_stack);
			delete (m_per_frame_stack);
			delete (m_super_heap);
			delete (m_crt_allocator);
#ifdef BC_MEMORY_LEAK_DETECTION
			delete (m_leak_allocator);
#endif

			m_initialized = false;
		}

		void bc_memory_manager::startup(bcSIZE p_max_num_thread,
			bcSIZE p_fsa_start_size,
			bcSIZE p_fsa_num,
			bcSIZE p_fsa_step_size,
			bcSIZE p_fsa_num_allocations,
			bcSIZE p_per_prg_heap_size,
			bcSIZE p_per_frm_heap_size,
			bcSIZE p_super_heap_size)
		{
			bc_memory_manager::m_instance.initialize(p_max_num_thread,
				p_fsa_start_size,
				p_fsa_num,
				p_fsa_step_size,
				p_fsa_num_allocations,
				p_per_prg_heap_size,
				p_per_frm_heap_size,
				p_super_heap_size);
		}
		
		void bc_memory_manager::close() noexcept
		{
			bc_memory_manager::m_instance.destroy();
		}
		
		void* bc_memory_manager::alloc(bcSIZE p_size, bc_alloc_type p_alloc_type, const bcCHAR* p_file, bcUINT32 p_line) noexcept
		{
			BC_ASSERT(m_initialized);

			void* l_result = nullptr;
			bci_memory* l_allocator = nullptr;
			bc_memblock l_block;
			bc_memblock::initialize_mem_block_before_allocation(p_size, BC_MEMORY_MIN_ALIGN, &l_block);
			const bcSIZE l_size = l_block.size();

			switch (p_alloc_type)
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
					const bcUINT32 l_fsa_index = _get_fsa_index(l_size);
					l_result = (m_fsa_allocators + l_fsa_index)->alloc(&l_block);
					l_allocator = (m_fsa_allocators + l_fsa_index);
				}
				break;

			case bc_alloc_type::program:
				l_result = m_per_program_stack->alloc(&l_block);
				l_allocator = m_per_program_stack;
				break;
			}

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

#ifdef BC_MEMORY_LEAK_DETECTION
			const auto l_alloc_number = m_allocation_count.fetch_add(1, core_platform::bc_memory_order::relaxed) + 1;

			{
				core_platform::bc_mutex_guard l_lock(m_leak_allocator_mutex);
				m_leak_allocator->insert
				(
					std::make_pair
					(
						l_result,
						bc_mem_block_leak_information
						(
							l_result,
							p_alloc_type,
							l_alloc_number,
							p_size,
							p_line,
							p_file
						)
					)
				);
			}
#endif

			bc_memblock::initialize_mem_block_after_allocation(&l_result, (l_allocator == m_super_heap), l_allocator, &l_block);
			
			return l_result;
		}

		void bc_memory_manager::free(void* p_pointer) noexcept
		{
			if (!p_pointer)
			{
				return;
			}

			bc_memblock* l_block = bc_memblock::retrieve_mem_block(p_pointer);
			void* l_pointer = reinterpret_cast<void*>(reinterpret_cast<bcUINTPTR>(p_pointer) - l_block->offset());

			BC_ASSERT(l_block);

			auto* l_allocator = static_cast<bci_memory*>(l_block->extra());
			l_allocator->free(l_pointer, l_block);

			bc_memblock::free_mem_block(l_pointer);

#ifdef BC_MEMORY_LEAK_DETECTION
			{
				core_platform::bc_mutex_guard l_lock(m_leak_allocator_mutex);

				const auto l_num_erased = m_leak_allocator->erase(l_pointer);
			}
#endif
		}
		
		void* bc_memory_manager::realloc(void* p_pointer, bcSIZE p_new_size, bc_alloc_type p_alloc_type, const bcCHAR* p_file, bcUINT32 p_line) noexcept
		{
			auto* l_block = bc_memblock::retrieve_mem_block(p_pointer);

			auto* l_new_pointer = alloc(p_new_size, p_alloc_type, p_file, p_line);
			if (!l_new_pointer) 
			{
				return l_new_pointer;
			}

			auto* l_new_block = bc_memblock::retrieve_mem_block(l_new_pointer);
			const bcSIZE l_min_size = std::min<bcSIZE>(l_block->size() - l_block->offset(), l_new_block->size() - l_new_block->offset());

			std::memcpy
			(
				reinterpret_cast<void*>(reinterpret_cast<bcUINTPTR>(l_new_pointer)),
				reinterpret_cast<void*>(reinterpret_cast<bcUINTPTR>(p_pointer)),
				l_min_size
			);

			free(p_pointer);

			return l_new_pointer;
		}
		
		void* bc_memory_manager::aligned_alloc(bcSIZE p_size, bcINT32 p_alignment, bc_alloc_type p_alloc_type, const bcCHAR* p_file, bcUINT32 p_line) noexcept
		{
			BC_ASSERT(m_initialized);

			void* l_result = nullptr;
			bci_memory* l_allocator = nullptr;
			bc_memblock l_block;
			bc_memblock::initialize_mem_block_before_allocation(p_size, p_alignment, &l_block);
			const bcSIZE l_size = l_block.size();

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

#ifdef BC_MEMORY_LEAK_DETECTION
			const auto l_alloc_number = m_allocation_count.fetch_add(1, core_platform::bc_memory_order::relaxed) + 1;

			{
				core_platform::bc_mutex_guard l_lock(m_leak_allocator_mutex);

				m_leak_allocator->insert
				(
					std::make_pair
					(
						l_result,
						bc_mem_block_leak_information
						(
							l_result,
							p_alloc_type,
							l_alloc_number,
							p_size,
							p_line,
							p_file
						)
					)
				);
			}
#endif
			bc_memblock::initialize_mem_block_after_allocation(&l_result, (l_allocator == m_super_heap), l_allocator, &l_block);

			return l_result;
		}
		
		void bc_memory_manager::aligned_free(void* p_pointer) noexcept
		{
			if (!p_pointer) 
			{
				return;
			}

			bc_memblock* l_block = bc_memblock::retrieve_mem_block(p_pointer);
			void* l_pointer = reinterpret_cast<void*>(reinterpret_cast<bcUINTPTR>(p_pointer) - l_block->offset());

			BC_ASSERT(l_block);

			auto* l_allocator = static_cast<bci_memory*>(l_block->extra());
			l_allocator->free(l_pointer, l_block);

			bc_memblock::free_mem_block(l_pointer);

#ifdef BC_MEMORY_LEAK_DETECTION
			{
				core_platform::bc_mutex_guard l_lock(m_leak_allocator_mutex);

				const auto l_num_erased = m_leak_allocator->erase(l_pointer);
			}
#endif
		}
		
		void* bc_memory_manager::aligned_realloc(void* p_pointer, bcSIZE p_new_size, bcINT32 p_alignment, bc_alloc_type p_alloc_type, const bcCHAR* p_file, bcUINT32 p_line) noexcept
		{
			auto* l_block = bc_memblock::retrieve_mem_block(p_pointer);

			auto* l_new_pointer = aligned_alloc(p_new_size, p_alignment, p_alloc_type, p_file, p_line);
			if (!l_new_pointer)
			{
				return l_new_pointer;
			}
			
			auto* l_new_block = bc_memblock::retrieve_mem_block(l_new_pointer);
			const bcSIZE l_min_size = std::min<bcSIZE>(l_block->size() - l_block->offset(), l_new_block->size() - l_new_block->offset());

			std::memcpy
			(
				reinterpret_cast<void*>(reinterpret_cast<bcUINTPTR>(l_new_pointer)),
				reinterpret_cast<void*>(reinterpret_cast<bcUINTPTR>(p_pointer)),
				l_min_size
			);

			free(p_pointer);

			return l_new_pointer;
		}

		void bc_memory_manager::end_of_frame() noexcept
		{
#ifdef BC_MEMORY_DEFRAG
			auto l_num_fragment = m_super_heap->fragmentation_count();
			l_num_fragment = l_num_fragment ? l_num_fragment / 3 + 1 : 0;

			{
#ifdef BC_MEMORY_LEAK_DETECTION
				core_platform::bc_mutex_guard l_lock(m_leak_allocator_mutex);
#endif

				m_super_heap->defragment
				(
					l_num_fragment,
					[this](void* p_old, void* p_new)
					{
#ifdef BC_MEMORY_LEAK_DETECTION
						auto l_old_ite = m_leak_allocator->find(p_old);
						m_leak_allocator->insert(std::make_pair(p_new, l_old_ite->second));
						m_leak_allocator->erase(l_old_ite);
#endif
					}
				);
			}
#endif

			BC_ASSERT(m_per_frame_stack->tracer().alloc_count() == 0);
			m_per_frame_stack->clear();
		}

		bcSIZE bc_memory_manager::get_total_size() const noexcept
		{
#ifdef BC_MEMORY_TRACING
			bcSIZE l_total_size = 0;

			for(bcUINT32 i = 0; i <m_fsa_num_allocators; i++)
			{
				l_total_size += m_fsa_allocators[i].tracer().total_size();
			}

			l_total_size += m_per_program_stack->tracer().total_size();
			l_total_size += m_per_frame_stack->tracer().total_size();
			l_total_size += m_super_heap->tracer().total_size();

			return l_total_size;
#endif
			return 0;
		}
		
		bcSIZE bc_memory_manager::get_used_size() const noexcept
		{
#ifdef BC_MEMORY_TRACING
			bcSIZE l_used_size = 0;

			for(bcUINT32 i = 0; i <m_fsa_num_allocators; i++)
				l_used_size += m_fsa_allocators[i].tracer().used_size();

			l_used_size += m_per_program_stack->tracer().used_size();
			l_used_size += m_per_frame_stack->tracer().used_size();
			l_used_size += m_super_heap->tracer().used_size();

			return l_used_size;
#endif
			return 0;
		}
		
		bcSIZE bc_memory_manager::get_overhead_size() const noexcept
		{
#ifdef BC_MEMORY_TRACING
			bcSIZE l_wasted_size = 0;

			for(bcUINT32 i = 0; i <m_fsa_num_allocators; i++)
				l_wasted_size += m_fsa_allocators[i].tracer().overhead_size();

			l_wasted_size += m_per_program_stack->tracer().overhead_size();
			l_wasted_size += m_per_frame_stack->tracer().overhead_size();
			l_wasted_size += m_super_heap->tracer().overhead_size();

			return l_wasted_size;
#endif
			return 0;
		}
		
		bcSIZE bc_memory_manager::get_max_used_size() const noexcept
		{
#ifdef BC_MEMORY_TRACING
			bcSIZE l_max_used_size = 0;

			for(bcUINT32 i = 0; i <m_fsa_num_allocators; i++)
				l_max_used_size += m_fsa_allocators[i].tracer().max_used_size();

			l_max_used_size += m_per_program_stack->tracer().max_used_size();
			l_max_used_size += m_per_frame_stack->tracer().max_used_size();
			l_max_used_size += m_super_heap->tracer().max_used_size();

			return l_max_used_size;
#endif
			return 0;
		}

#ifdef BC_MEMORY_LEAK_DETECTION
		bcUINT32 bc_memory_manager::report_memory_leaks() const noexcept
		{
			{
				core_platform::bc_mutex_guard l_lock(m_leak_allocator_mutex);

				const bcSIZE l_leak_count = m_leak_allocator->size();
				auto l_leak_array = std::vector<bc_mem_block_leak_information*>(l_leak_count);
				auto l_leak_ite = 0U;
				
				for (auto& l_leak : *m_leak_allocator)
				{
					l_leak_array[l_leak_ite] = &l_leak.second;
					++l_leak_ite;
				}

				std::sort(std::begin(l_leak_array), std::end(l_leak_array), [](bc_mem_block_leak_information* p_leak1, bc_mem_block_leak_information* p_leak2)
				{
					return p_leak1->m_number < p_leak2->m_number;
				});
				
				return l_leak_count;
			}
		}
#endif
#endif
	}
}