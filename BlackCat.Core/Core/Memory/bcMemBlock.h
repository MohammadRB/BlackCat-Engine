//  [08/24/2013 MRB]

#pragma once

#include "CorePlatform/Concurrency/bcConcurrencyDef.h"
#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "Core/CorePCH.h"

namespace black_cat
{
	namespace core
	{
#ifdef BC_MEMORY_ENABLE

		// Represent a block of memory
		struct bc_memblock
		{
		public:
			bc_memblock()
				: m_size(0),
				//m_refrence_count(0), 
				m_alignment(BC_MEMORY_MIN_ALIGN),
				m_offset(0),
				m_movable(false),
				m_extra(nullptr),
				m_allocators_extra(nullptr)
			{
			}

			bc_memblock(const bc_memblock& p_other)
				: m_size(p_other.size()),
				//m_refrence_count(p_other.refrence_count(core_platform::bc_memory_order::relaxed)),
				m_alignment(p_other.alignment()),
				m_offset(p_other.offset()),
				m_movable(p_other.movable_pointer()),
				m_extra(p_other.extra()),
				m_allocators_extra(p_other.allocators_extra())
			{
			}

			bc_memblock& operator =(const bc_memblock& p_other)
			{
				size(p_other.size());
				//refrence_count(p_other.refrence_count(core_platform::bc_memory_order::relaxed), core_platform::bc_memory_order::relaxed);
				alignment(p_other.alignment());
				offset(p_other.offset());
				movable_pointer(p_other.movable_pointer());
				extra(p_other.extra());
				allocators_extra(p_other.allocators_extra());

				return *this;
			}

			bcSIZE size() const { return m_size; }

			void size(bcSIZE p_val) { m_size = p_val; }

			bcUBYTE alignment() const { return m_alignment; }

			void alignment(bcUBYTE p_val) { m_alignment = p_val; }

			bcUBYTE offset() const { return m_offset; }

			void offset(bcUBYTE p_val) { m_offset = p_val; }

#ifdef BC_MEMORY_DEFRAG
			bool movable_pointer() const { return m_movable; }

			void movable_pointer(bool p_val) { m_movable = p_val; }
#endif
			void* extra() const { return m_extra; }

			void extra(void* p_val) { m_extra = p_val; }

			void* allocators_extra() const { return m_allocators_extra; }

			void allocators_extra(void* p_val) { m_allocators_extra = p_val; }

			bcInline static bcSIZE get_aligned_size(bcSIZE p_size, bcSIZE p_alignment)
			{
				bcSIZE l_mis_alignment = p_size % p_alignment;
				return (l_mis_alignment == 0) ? p_size : (p_size - l_mis_alignment) + p_alignment;
			}
			
			// Retrieve MemBlock for pointer that point to actual location of data, or null if parameter is null
			bcInline static bc_memblock* retrieve_mem_block(const void* p_pointer)
			{
				if (p_pointer != nullptr)
					return reinterpret_cast<bc_memblock*>(reinterpret_cast<bcUINTPTR>(p_pointer)-sizeof(bc_memblock));
				else
					return nullptr;
			}

			bcInline static bcSIZE get_required_size(bcSIZE p_data_size, bcSIZE p_alignment)
			{
				bcSIZE l_extra_alignment = p_alignment - BC_MEMORY_MIN_ALIGN;
				l_extra_alignment = std::max(0U, l_extra_alignment);

				return
					get_aligned_size(sizeof(bc_memblock), BC_MEMORY_MIN_ALIGN) +
					l_extra_alignment +
					get_aligned_size(p_data_size, BC_MEMORY_MIN_ALIGN);
			}

			// Return required offset from begging of block where actual data must be stored
			bcInline static bcSIZE get_required_offset_for_data(const void* p_pointer, bcSIZE p_alignment)
			{
				bcSIZE l_mem_block_offset = reinterpret_cast< bcUINTPTR >(p_pointer) +
					get_aligned_size(sizeof(bc_memblock), BC_MEMORY_MIN_ALIGN);
				bcSIZE l_mis_alignment = l_mem_block_offset % p_alignment;
				l_mem_block_offset += l_mis_alignment;

				return l_mem_block_offset - reinterpret_cast< bcUINTPTR >(p_pointer);
			}

			bcInline static bcSIZE get_required_offset_for_mem_block(const void* p_pointer, bcSIZE p_alignment)
			{
				return 
					(get_required_offset_for_data(p_pointer, p_alignment) - sizeof(bc_memblock)) - 
					reinterpret_cast<bcUINTPTR>(p_pointer);
			}

			bcInline static void initialize_mem_block_before_allocation(bcSIZE p_size, bcSIZE p_alignment, bc_memblock* p_block)
			{
				p_block->size(get_required_size(p_size, p_alignment));
				//p_block->refrence_count(0, core_platform::bc_memory_order::relaxed);
				p_block->alignment(p_alignment);
				p_block->offset(0);
#ifdef BC_MEMORY_DEFRAG
				p_block->movable_pointer(false);
#endif
				p_block->extra(nullptr);
				p_block->allocators_extra(nullptr);
			}

			// Fill MemBlock with data and shift pPointer with requeried offset for actual data
			bcInline static void initialize_mem_block_after_allocation(void** p_pointer,
				bool p_dynamic,
				void* p_extra,
				bc_memblock* p_block)
			{
				p_block->offset(bc_memblock::get_required_offset_for_data(*p_pointer, p_block->alignment()));
#ifdef BC_MEMORY_DEFRAG
				p_block->movable_pointer(p_dynamic);
#endif
				p_block->extra(p_extra);

				*p_pointer = reinterpret_cast<void*>((reinterpret_cast<bcUINTPTR>(*p_pointer) + p_block->offset()));

				bc_memblock::store_mem_block(*p_pointer, p_block);
			}

			// Store MemBlock for pointer that point to actual location of data
			bcInline static void store_mem_block(const void* p_pointer, const bc_memblock* p_block)
			{
				*(reinterpret_cast<bc_memblock*>((reinterpret_cast<bcUINTPTR>(p_pointer) - sizeof(bc_memblock)))) = *p_block;
			}

			bcInline static void free_mem_block(const void* p_pointer) {}

		protected:

		private:
			bcSIZE m_size;			// Total size of block
			bcUBYTE m_alignment;
			bcUBYTE m_offset;		// Offset from begin of block where actual data is located
#ifdef BC_MEMORY_DEFRAG
			bool m_movable;
#endif
			void* m_extra;
			void* m_allocators_extra;
		};

#ifdef BC_MEMORY_LEAK_DETECTION
		struct bc_mem_block_leak_information
		{
			static const bcUINT32 s_filename_length = 20;

			void* m_pointer;
			bcSIZE m_number;
			bcSIZE m_requested_size;
			bcSIZE m_line_number;
			bcCHAR m_file_name[s_filename_length];

			bc_mem_block_leak_information() : m_pointer(nullptr), m_number(0), m_requested_size(0), m_line_number(0)
			{
			}

			bc_mem_block_leak_information(void* p_pointer, bcSIZE p_number, bcSIZE p_requested_size, bcSIZE p_line_number, const bcCHAR* p_file_name)
				: m_pointer(p_pointer), 
				m_number(p_number),
				m_requested_size(p_requested_size),
				m_line_number(p_line_number)
			{
				std::strcpy
					(
						m_file_name,
						&p_file_name[std::max< bcINT32 >(static_cast< bcINT32 >(std::strlen(p_file_name)) - (s_filename_length - 1), 0)]
					);
			}
		};
#endif

#endif
	}
}