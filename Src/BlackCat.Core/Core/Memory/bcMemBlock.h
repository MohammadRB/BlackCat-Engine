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
			bc_memblock();

			bc_memblock(const bc_memblock& p_other) noexcept;

			~bc_memblock();
			
			bc_memblock& operator =(const bc_memblock& p_other) noexcept;

			bcSIZE size() const noexcept;

			void size(bcSIZE p_val) noexcept;

			bcUBYTE alignment() const noexcept;

			void alignment(bcUBYTE p_val) noexcept;

			bcUBYTE offset() const noexcept;

			void offset(bcUBYTE p_val) noexcept;

#ifdef BC_MEMORY_DEFRAG
			bool movable_pointer() const noexcept;

			void movable_pointer(bool p_val) noexcept;
#endif
			void* extra() const noexcept;

			void extra(void* p_val) noexcept;

			void* allocators_extra() const noexcept;

			void allocators_extra(void* p_val) noexcept;

			static constexpr bcSIZE get_aligned_size(bcSIZE p_size, bcSIZE p_alignment) noexcept;

			/**
			 * \brief Retrieve MemBlock for pointer that point to actual location of data, or null if parameter is null
			 * \param p_pointer
			 * \return
			 */
			static bc_memblock* retrieve_mem_block(const void* p_pointer) noexcept;

			static bcSIZE get_required_size(bcSIZE p_data_size, bcSIZE p_alignment) noexcept;

			/**
			 * \brief Return required offset from beginning of block where actual data must be stored
			 * \param p_pointer
			 * \param p_alignment
			 * \return
			 */
			static bcSIZE get_required_offset_for_data(const void* p_pointer, bcSIZE p_alignment) noexcept;

			static bcSIZE get_required_offset_for_mem_block(const void* p_pointer, bcSIZE p_alignment) noexcept;

			static void initialize_mem_block_before_allocation(bcSIZE p_size, bcSIZE p_alignment, bc_memblock* p_block) noexcept;

			/**
			 * \brief Fill MemBlock with data and shift pPointer with required offset for actual data
			 * \param p_pointer
			 * \param p_movable
			 * \param p_extra
			 * \param p_block
			 */
			static void initialize_mem_block_after_allocation(void** p_pointer,
				bool p_movable,
				void* p_extra,
				bc_memblock* p_block) noexcept;

			/**
			 * \brief Store MemBlock for pointer that point to actual location of data
			 * \param p_pointer
			 * \param p_block
			 */
			static void store_mem_block(const void* p_pointer, const bc_memblock* p_block) noexcept;

			static void free_mem_block(const void* p_pointer) noexcept;

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

		inline bc_memblock::bc_memblock(): m_size(0),
			m_alignment(BC_MEMORY_MIN_ALIGN),
			m_offset(0),
			m_movable(false),
			m_extra(nullptr),
			m_allocators_extra(nullptr)
		{
		}

		inline bc_memblock::bc_memblock(const bc_memblock& p_other) noexcept = default;

		inline bc_memblock::~bc_memblock() = default;
		
		inline bc_memblock& bc_memblock::operator=(const bc_memblock& p_other) noexcept = default;

		inline bcSIZE bc_memblock::size() const noexcept
		{
			return m_size;
		}

		inline void bc_memblock::size(bcSIZE p_val) noexcept
		{
			m_size = p_val;
		}

		inline bcUBYTE bc_memblock::alignment() const noexcept
		{
			return m_alignment;
		}

		inline void bc_memblock::alignment(bcUBYTE p_val) noexcept
		{
			m_alignment = p_val;
		}

		inline bcUBYTE bc_memblock::offset() const noexcept
		{
			return m_offset;
		}

		inline void bc_memblock::offset(bcUBYTE p_val) noexcept
		{
			m_offset = p_val;
		}

		inline bool bc_memblock::movable_pointer() const noexcept
		{
			return m_movable;
		}

		inline void bc_memblock::movable_pointer(bool p_val) noexcept
		{
			m_movable = p_val;
		}

		inline void* bc_memblock::extra() const noexcept
		{
			return m_extra;
		}

		inline void bc_memblock::extra(void* p_val) noexcept
		{
			m_extra = p_val;
		}

		inline void* bc_memblock::allocators_extra() const noexcept
		{
			return m_allocators_extra;
		}

		inline void bc_memblock::allocators_extra(void* p_val) noexcept
		{
			m_allocators_extra = p_val;
		}

		constexpr bcSIZE bc_memblock::get_aligned_size(bcSIZE p_size, bcSIZE p_alignment) noexcept
		{
			return (p_size % p_alignment == 0) ? p_size : (p_size - (p_size % p_alignment)) + p_alignment;
		}

		inline bc_memblock* bc_memblock::retrieve_mem_block(const void* p_pointer) noexcept
		{
			if (p_pointer != nullptr)
			{
				return reinterpret_cast< bc_memblock* >(reinterpret_cast< bcUINTPTR >(p_pointer) - sizeof(bc_memblock));
			}
			else
			{
				return nullptr;
			}
		}

		inline bcSIZE bc_memblock::get_required_size(bcSIZE p_data_size, bcSIZE p_alignment) noexcept
		{
			const bcSIZE l_extra_alignment = std::max(0, static_cast< bcINT32 >(p_alignment) - BC_MEMORY_MIN_ALIGN);

			return get_aligned_size(sizeof(bc_memblock), BC_MEMORY_MIN_ALIGN) + 
				l_extra_alignment + 
				get_aligned_size(p_data_size, BC_MEMORY_MIN_ALIGN);
		}

		inline bcSIZE bc_memblock::get_required_offset_for_data(const void* p_pointer, bcSIZE p_alignment) noexcept
		{
			bcSIZE l_mem_block_offset = reinterpret_cast< bcUINTPTR >(p_pointer) + get_aligned_size(sizeof(bc_memblock), BC_MEMORY_MIN_ALIGN);
			const bcSIZE l_mis_alignment = l_mem_block_offset % p_alignment;

			if (l_mis_alignment > 0)
			{
				l_mem_block_offset += (p_alignment - l_mis_alignment);
			}

			return l_mem_block_offset - reinterpret_cast< bcUINTPTR >(p_pointer);
		}

		inline bcSIZE bc_memblock::get_required_offset_for_mem_block(const void* p_pointer, bcSIZE p_alignment) noexcept
		{
			return (get_required_offset_for_data(p_pointer, p_alignment) - sizeof(bc_memblock)) - reinterpret_cast< bcUINTPTR >(p_pointer);
		}

		inline void bc_memblock::initialize_mem_block_before_allocation(bcSIZE p_size, bcSIZE p_alignment, bc_memblock* p_block) noexcept
		{
			p_block->size(get_required_size(p_size, p_alignment));
			p_block->alignment(static_cast<bcUBYTE>(p_alignment));
			p_block->offset(0);
#ifdef BC_MEMORY_DEFRAG
			p_block->movable_pointer(false);
#endif
			p_block->extra(nullptr);
			p_block->allocators_extra(nullptr);
		}

		inline void bc_memblock::initialize_mem_block_after_allocation(void** p_pointer,
			bool p_movable,
			void* p_extra,
			bc_memblock* p_block) noexcept
		{
			p_block->offset(static_cast<bcUBYTE>(get_required_offset_for_data(*p_pointer, p_block->alignment())));
#ifdef BC_MEMORY_DEFRAG
			p_block->movable_pointer(p_movable);
#endif
			p_block->extra(p_extra);

			*p_pointer = reinterpret_cast< void* >((reinterpret_cast< bcUINTPTR >(*p_pointer) + p_block->offset()));

			bc_memblock::store_mem_block(*p_pointer, p_block);
		}

		inline void bc_memblock::store_mem_block(const void* p_pointer, const bc_memblock* p_block) noexcept
		{
			*(reinterpret_cast< bc_memblock* >((reinterpret_cast< bcUINTPTR >(p_pointer) - sizeof(bc_memblock)))) = *p_block;
		}

		inline void bc_memblock::free_mem_block(const void* p_pointer) noexcept
		{
		}
#endif
	}
}