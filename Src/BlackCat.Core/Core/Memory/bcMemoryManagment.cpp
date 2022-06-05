// [09/02/2013 MRB]

#include "Core/CorePCH.h"

#include "CorePlatformImp/Utility/bcHardwareInfo.h"
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
			static constexpr bcUINT32 s_filename_length = 20;

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
				const bcCHAR* p_file_name)
				: m_pointer(p_pointer),
				m_type(p_type),
				m_number(p_number),
				m_requested_size(p_requested_size),
				m_line_number(p_line_number),
				m_file_name()
			{
				std::strcpy
				(
					m_file_name,
					&p_file_name[std::max(std::strlen(p_file_name) - (s_filename_length - 1), 0U)]
				);
			}
		};
#endif
		
		bc_memory_manager bc_memory_manager::m_instance;

		bc_memory_manager::bc_memory_manager() noexcept
			: m_fsa_allocators_start_size(0),
			m_fsa_allocators_count(0),
			m_fsa_step_size(0),
			m_worker_frame_allocators_count(0),
			m_initialized(false),
			m_fsa_allocators(nullptr),
			m_program_allocator(nullptr),
			m_worker_frame_allocators(nullptr),
			m_frame_allocator(nullptr),
			m_heap_allocator(nullptr),
			m_crt_allocator(nullptr),
			m_last_used_frame_allocator(0)
#ifdef BC_MEMORY_LEAK_DETECTION
			,
			m_allocation_count(0),
			m_leak_allocator(nullptr)
#endif
		{
		}

		bc_memory_manager::~bc_memory_manager()
		{
			if(m_initialized)
			{
				destroy();
			}
		}

		void bc_memory_manager::initialize(bcSIZE p_max_num_thread,
			bcSIZE p_fsa_start_size,
			bcSIZE p_fsa_count,
			bcSIZE p_fsa_step_size,
			bcSIZE p_fsa_capacity,
			bcSIZE p_program_heap_capacity,
			bcSIZE p_frame_heap_capacity,
			bcSIZE p_super_heap_capacity)
		{
			platform::bc_basic_hardware_info l_hardware_info{};
			platform::bc_hardware_info::get_basic_info(l_hardware_info);

			m_fsa_allocators_start_size = p_fsa_start_size;
			m_fsa_allocators_count = p_fsa_count;
			m_fsa_step_size = p_fsa_step_size;
			m_worker_frame_allocators_count = l_hardware_info.m_processor_count / 2;

			try
			{
				m_fsa_allocators = new bc_memory_extender<bc_memory_fixed_size>[m_fsa_allocators_count];

				for (bcUINT32 l_ite = 0; l_ite < m_fsa_allocators_count; l_ite++)
				{
					auto l_initialize_delegate = bc_delegate<bc_memory_fixed_size*()>
					(
						[=]()
						{
							const auto l_block_size = m_fsa_allocators_start_size + l_ite * m_fsa_step_size;
							const auto l_tag = std::string("FSA") + std::to_string(l_ite);

							auto* l_result = new bc_memory_fixed_size();
							l_result->initialize(p_fsa_capacity, l_block_size, l_tag.c_str());

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

					m_fsa_allocators[l_ite].initialize(std::move(l_initialize_delegate), std::move(l_destroy_delegate));
				}

				m_program_allocator = new bc_memory_stack1();
				m_program_allocator->initialize(p_program_heap_capacity, "ProgramStack");

				m_worker_frame_allocators = new bc_memory_stack1[m_worker_frame_allocators_count];
				for (auto l_ite = 0U; l_ite < m_worker_frame_allocators_count; ++l_ite)
				{
					const auto l_tag = std::string("FrameStack") + std::to_string(l_ite);
					m_worker_frame_allocators[l_ite].initialize(p_frame_heap_capacity / 4, l_tag.c_str());
				}

				m_frame_allocator = new bc_memory_stack1();
				m_frame_allocator->initialize(p_frame_heap_capacity / 2, "FrameStack");

				// main thread frame allocator
				m_my_frame_allocator.set(m_frame_allocator);

#ifdef BC_MEMORY_DEFRAG
				m_super_heap = new bc_memory_heap();
				m_super_heap->initialize(p_super_heap_size, "SuperHeap");
#endif

				m_crt_allocator = new bc_memory_crt();
				m_crt_allocator->initialize("CrtAllocator");

#ifdef BC_MEMORY_LEAK_DETECTION
				m_allocation_count.store(0, platform::bc_memory_order::relaxed);
				m_leak_allocator = new std::unordered_map<void*, bc_mem_block_leak_information>();
#endif
			}
			catch (const std::bad_alloc&)
			{
				destroy();
				throw;
			}

			m_initialized = true;
		}

		void bc_memory_manager::destroy() noexcept
		{
			delete[] m_fsa_allocators;
			delete m_program_allocator;
			delete[] m_worker_frame_allocators;
			delete m_frame_allocator;
			delete m_heap_allocator;
			delete m_crt_allocator;
#ifdef BC_MEMORY_LEAK_DETECTION
			delete m_leak_allocator;
#endif

			m_initialized = false;
		}

		void bc_memory_manager::startup(bcSIZE p_max_num_thread,
			bcSIZE p_fsa_start_size,
			bcSIZE p_fsa_count,
			bcSIZE p_fsa_step_size,
			bcSIZE p_fsa_capacity,
			bcSIZE p_program_heap_capacity,
			bcSIZE p_frame_heap_capacity,
			bcSIZE p_super_heap_capacity)
		{
			bc_memory_manager::m_instance.initialize(p_max_num_thread,
				p_fsa_start_size,
				p_fsa_count,
				p_fsa_step_size,
				p_fsa_capacity,
				p_program_heap_capacity,
				p_frame_heap_capacity,
				p_super_heap_capacity);
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
			bc_memblock l_block(p_size);

			switch (p_alloc_type)
			{
				case bc_alloc_type::frame:
				{
					auto* l_frame_allocator = m_my_frame_allocator.get();
					if (l_frame_allocator == nullptr)
					{
						const auto l_my_allocator_index = m_last_used_frame_allocator.fetch_add(1U, platform::bc_memory_order::relaxed) % m_worker_frame_allocators_count;
						l_frame_allocator = &m_worker_frame_allocators[l_my_allocator_index];
						m_my_frame_allocator.set(l_frame_allocator);
					}

					l_result = l_frame_allocator->alloc(&l_block);
					l_allocator = l_frame_allocator;
					break;
				}
				case bc_alloc_type::unknown:
				case bc_alloc_type::unknown_movable:
				{
					const auto l_fsa_index = _get_fsa_index(l_block.size());
					if (l_fsa_index < m_fsa_allocators_count)
					{
						std::tie(l_result, l_allocator) = (m_fsa_allocators + l_fsa_index)->alloc1(&l_block);
					}
					break;
				}
				case bc_alloc_type::program:
				{
					l_result = m_program_allocator->alloc(&l_block);
					l_allocator = m_program_allocator;
					break;
				}
			}

#ifdef BC_MEMORY_DEFRAG
			if (!l_result && p_alloc_type == bc_alloc_type::unknown_movable)
			{
				l_result = m_super_heap->alloc(&l_block);
				l_allocator = m_super_heap;
			}
#endif

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
			const auto l_alloc_number = m_allocation_count.fetch_add(1, platform::bc_memory_order::relaxed) + 1;

			{
				platform::bc_mutex_guard l_lock(m_leak_allocator_mutex);
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

			l_block.initialize_after_allocation(&l_result, l_allocator == m_heap_allocator, l_allocator);
			
			return l_result;
		}

		void bc_memory_manager::free(void* p_pointer) noexcept
		{
			if (!p_pointer)
			{
				return;
			}

			bc_memblock* l_block = bc_memblock::retrieve_mem_block(p_pointer);
			void* l_pointer = static_cast<bcBYTE*>(p_pointer) - l_block->offset();

			BC_ASSERT(l_block);

			auto* l_allocator = static_cast<bci_memory*>(l_block->extra());
			l_allocator->free(l_pointer, l_block);

			bc_memblock::free_mem_block(l_pointer);

#ifdef BC_MEMORY_LEAK_DETECTION
			{
				platform::bc_mutex_guard l_lock(m_leak_allocator_mutex);

				const auto l_num_erased = m_leak_allocator->erase(l_pointer);
			}
#endif
		}
		
		void* bc_memory_manager::realloc(void* p_pointer, bcSIZE p_new_size, bc_alloc_type p_alloc_type, const bcCHAR* p_file, bcUINT32 p_line) noexcept
		{
			const auto* l_block = bc_memblock::retrieve_mem_block(p_pointer);

			auto* l_new_pointer = alloc(p_new_size, p_alloc_type, p_file, p_line);
			if (!l_new_pointer) 
			{
				return l_new_pointer;
			}

			const auto* l_new_block = bc_memblock::retrieve_mem_block(l_new_pointer);
			const bcSIZE l_min_size = std::min<bcSIZE>(l_block->size() - l_block->offset(), l_new_block->size() - l_new_block->offset());

			std::memcpy(l_new_pointer, p_pointer, l_min_size);

			free(p_pointer);

			return l_new_pointer;
		}
		
		void* bc_memory_manager::aligned_alloc(bcSIZE p_size, bcUINT8 p_alignment, bc_alloc_type p_alloc_type, const bcCHAR* p_file, bcUINT32 p_line) noexcept
		{
			BC_ASSERT(m_initialized);

			void* l_result = nullptr;
			bci_memory* l_allocator = nullptr;
			bc_memblock l_block(p_size, p_alignment);

			switch (p_alloc_type)
			{
				case bc_alloc_type::frame:
				{
					auto* l_frame_allocator = m_my_frame_allocator.get();
					if (l_frame_allocator == nullptr)
					{
						const auto l_my_allocator_index = platform::bc_thread::current_thread_id() % m_worker_frame_allocators_count;
						l_frame_allocator = &m_worker_frame_allocators[l_my_allocator_index];
						m_my_frame_allocator.set(l_frame_allocator);
					}

					l_result = l_frame_allocator->alloc(&l_block);
					l_allocator = l_frame_allocator;
					break;
				}
				case bc_alloc_type::unknown:
				case bc_alloc_type::unknown_movable:
				{
					const auto l_fsa_index = _get_fsa_index(l_block.size());
					if (l_fsa_index < m_fsa_allocators_count)
					{
						std::tie(l_result, l_allocator) = (m_fsa_allocators + l_fsa_index)->alloc1(&l_block);
					}
					break;
				}
				case bc_alloc_type::program:
				{
					l_result = m_program_allocator->alloc(&l_block);
					l_allocator = m_program_allocator;
					break;
				}
			}

#ifdef BC_MEMORY_DEFRAG
			if (!l_result && p_alloc_type == bc_alloc_type::unknown_movable)
			{
				l_result = m_super_heap->alloc(&l_block);
				l_allocator = m_super_heap;
			}
#endif

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
			const auto l_alloc_number = m_allocation_count.fetch_add(1, platform::bc_memory_order::relaxed) + 1;

			{
				platform::bc_mutex_guard l_lock(m_leak_allocator_mutex);

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
			l_block.initialize_after_allocation(&l_result, l_allocator == m_heap_allocator, l_allocator);

			return l_result;
		}
		
		void bc_memory_manager::aligned_free(void* p_pointer) noexcept
		{
			if (!p_pointer) 
			{
				return;
			}

			bc_memblock* l_block = bc_memblock::retrieve_mem_block(p_pointer);
			void* l_pointer = static_cast<bcBYTE*>(p_pointer) - l_block->offset();

			BC_ASSERT(l_block);

			auto* l_allocator = static_cast<bci_memory*>(l_block->extra());
			l_allocator->free(l_pointer, l_block);

			bc_memblock::free_mem_block(l_pointer);

#ifdef BC_MEMORY_LEAK_DETECTION
			{
				platform::bc_mutex_guard l_lock(m_leak_allocator_mutex);

				const auto l_num_erased = m_leak_allocator->erase(l_pointer);
			}
#endif
		}
		
		void* bc_memory_manager::aligned_realloc(void* p_pointer, bcSIZE p_new_size, bcUINT8 p_alignment, bc_alloc_type p_alloc_type, const bcCHAR* p_file, bcUINT32 p_line) noexcept
		{
			const auto* l_block = bc_memblock::retrieve_mem_block(p_pointer);

			auto* l_new_pointer = aligned_alloc(p_new_size, p_alignment, p_alloc_type, p_file, p_line);
			if (!l_new_pointer)
			{
				return l_new_pointer;
			}

			const auto* l_new_block = bc_memblock::retrieve_mem_block(l_new_pointer);
			const bcSIZE l_min_size = std::min<bcSIZE>(l_block->size() - l_block->offset(), l_new_block->size() - l_new_block->offset());

			std::memcpy(l_new_pointer, p_pointer, l_min_size);

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
				platform::bc_mutex_guard l_lock(m_leak_allocator_mutex);
#endif

				m_super_heap->defrag
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

			bool l_stack_allocators_are_free = m_frame_allocator->tracer().alloc_count() == 0;
			for(auto l_ite = 0U; l_ite < m_worker_frame_allocators_count; ++l_ite)
			{
				l_stack_allocators_are_free = l_stack_allocators_are_free && m_worker_frame_allocators[l_ite].tracer().alloc_count() == 0;
			}

			BC_ASSERT(l_stack_allocators_are_free);

			m_frame_allocator->clear();
			for (auto l_ite = 0U; l_ite < m_worker_frame_allocators_count; ++l_ite)
			{
				m_worker_frame_allocators[l_ite].clear();
			}
		}

		bcSIZE bc_memory_manager::get_total_size() const noexcept
		{
#ifdef BC_MEMORY_TRACING
			bcSIZE l_total_size = 0;

			for(bcUINT32 i = 0; i < m_fsa_allocators_count; i++)
			{
				l_total_size += m_fsa_allocators[i].tracer().total_size();
			}

			l_total_size += m_program_allocator->tracer().total_size();
			l_total_size += m_frame_allocator->tracer().total_size();
#ifdef BC_MEMORY_DEFRAG
			l_total_size += m_super_heap->tracer().total_size();
#endif

			return l_total_size;
#else
			return 0;
#endif
		}
		
		bcSIZE bc_memory_manager::get_used_size() const noexcept
		{
#ifdef BC_MEMORY_TRACING
			bcSIZE l_used_size = 0;

			for(bcUINT32 i = 0; i < m_fsa_allocators_count; i++)
			{
				l_used_size += m_fsa_allocators[i].tracer().used_size();
			}

			l_used_size += m_program_allocator->tracer().used_size();
			l_used_size += m_frame_allocator->tracer().used_size();
#ifdef BC_MEMORY_DEFRAG
			l_used_size += m_super_heap->tracer().used_size();
#endif

			return l_used_size;
#else
			return 0;
#endif
		}
		
		bcSIZE bc_memory_manager::get_overhead_size() const noexcept
		{
#ifdef BC_MEMORY_TRACING
			bcSIZE l_wasted_size = 0;

			for(bcUINT32 i = 0; i <m_fsa_allocators_count; i++)
			{
				l_wasted_size += m_fsa_allocators[i].tracer().overhead_size();
			}

			l_wasted_size += m_program_allocator->tracer().overhead_size();
			l_wasted_size += m_frame_allocator->tracer().overhead_size();
#ifdef BC_MEMORY_DEFRAG
			l_wasted_size += m_super_heap->tracer().overhead_size();
#endif

			return l_wasted_size;
#else
			return 0;
#endif
		}
		
		bcSIZE bc_memory_manager::get_max_used_size() const noexcept
		{
#ifdef BC_MEMORY_TRACING
			bcSIZE l_max_used_size = 0;

			for(bcUINT32 i = 0; i < m_fsa_allocators_count; i++)
			{
				l_max_used_size += m_fsa_allocators[i].tracer().max_used_size();
			}

			l_max_used_size += m_program_allocator->tracer().max_used_size();
			l_max_used_size += m_frame_allocator->tracer().max_used_size();
#ifdef BC_MEMORY_DEFRAG
			l_max_used_size += m_super_heap->tracer().max_used_size();
#endif

			return l_max_used_size;
#else
			return 0;
#endif
		}

#ifdef BC_MEMORY_LEAK_DETECTION
		bcUINT32 bc_memory_manager::report_memory_leaks() const noexcept
		{
			{
				platform::bc_mutex_guard l_lock(m_leak_allocator_mutex);

				const bcSIZE l_leak_count = m_leak_allocator->size();
				auto l_leak_array = std::vector<bc_mem_block_leak_information*>(l_leak_count);
				auto l_leak_ite = 0U;
				
				for (auto& l_leak : *m_leak_allocator)
				{
					l_leak_array[l_leak_ite] = &l_leak.second;
					++l_leak_ite;
				}

				std::sort(std::begin(l_leak_array), std::end(l_leak_array), [](const bc_mem_block_leak_information* p_leak1, const bc_mem_block_leak_information* p_leak2)
				{
					return p_leak1->m_number < p_leak2->m_number;
				});
				
				return l_leak_count;
			}
		}
#endif

		bcUINT32 bc_memory_manager::_fsa_max_size(bcUINT32 p_index) const noexcept
		{
			return m_fsa_allocators_start_size + p_index * m_fsa_step_size;
		}

		bcUINT32 bc_memory_manager::_get_fsa_index(bcUINT32 p_size) const noexcept
		{
			return static_cast<bcUINT32>
			(
				std::ceilf
				(
					std::max(0, static_cast<bcINT32>(p_size) - static_cast<bcINT32>(m_fsa_allocators_start_size)) / static_cast<bcFLOAT>(m_fsa_step_size)
				)
			);
		}
#endif
	}
}