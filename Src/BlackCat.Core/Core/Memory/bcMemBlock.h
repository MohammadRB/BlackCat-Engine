//  [24/08/2013 MRB]

#pragma once

#include "Core/CorePCH.h"
#include "Core/bcUtility.h"

namespace black_cat::core
{
#ifdef BC_MEMORY_ENABLE

	struct bc_memblock
	{
	public:
		constexpr bc_memblock() noexcept;

		explicit bc_memblock(bcSIZE p_size) noexcept;

		bc_memblock(bcSIZE p_size, bcUINT8 p_alignment) noexcept;

		bc_memblock(const bc_memblock& p_other) noexcept = default;

		~bc_memblock() = default;
			
		bc_memblock& operator =(const bc_memblock& p_other) noexcept = default;

		bcSIZE size() const noexcept;

		void size(bcSIZE p_val) noexcept;

		bcUINT8 alignment() const noexcept;

		void alignment(bcUINT8 p_val) noexcept;

		bcUINT8 offset() const noexcept;

		void offset(bcUINT8 p_val) noexcept;

#ifdef BC_MEMORY_DEFRAG
			bool movable_pointer() const noexcept;

			void movable_pointer(bool p_val) noexcept;
#endif
		void* extra() const noexcept;

		void extra(void* p_val) noexcept;

		void initialize_after_allocation(void** p_pointer, bool p_movable, void* p_extra) noexcept;

		static constexpr bcSIZE get_aligned_size(bcSIZE p_size, bcUINT8 p_alignment) noexcept;

		/**
			 * \brief Retrieve MemBlock for pointer that point to actual location of data, or null if parameter is null
			 * \param p_pointer
			 * \return
			 */
		static bc_memblock* retrieve_mem_block(void* p_pointer) noexcept;

		static bcSIZE get_required_size(bcSIZE p_data_size, bcUINT8 p_alignment) noexcept;

		/**
			 * \brief Return required offset from beginning of block where actual data must be stored
			 * \param p_pointer
			 * \param p_alignment
			 * \return
			 */
		static bcSIZE get_required_offset_for_data(const void* p_pointer, bcUINT8 p_alignment) noexcept;
			
		/**
			 * \brief Store MemBlock for pointer that point to actual location of data
			 * \param p_pointer
			 * \param p_block
			 */
		static void store_mem_block(void* p_pointer, const bc_memblock* p_block) noexcept;

		static void free_mem_block(void* p_pointer) noexcept;

	private:
		bcSIZE m_size;			// Total size of block
		bcUINT8 m_alignment;
		bcUINT8 m_offset;		// Offset from begin of block where actual data is located
#ifdef BC_MEMORY_DEFRAG
			bool m_movable;
#endif
		void* m_extra;
	};

	inline constexpr  bc_memblock::bc_memblock() noexcept
		: m_size(0),
		  m_alignment(BC_MEMORY_MIN_ALIGN),
		  m_offset(0),
#ifdef BC_MEMORY_DEFRAG
			m_movable(false),
#endif
		  m_extra(nullptr)
	{
	}

	inline bc_memblock::bc_memblock(bcSIZE p_size) noexcept
		: bc_memblock(p_size, BC_MEMORY_MIN_ALIGN)
	{
	}

	inline bc_memblock::bc_memblock(bcSIZE p_size, bcUINT8 p_alignment) noexcept
		: m_size(get_required_size(p_size, p_alignment)),
		  m_alignment(p_alignment),
		  m_offset(0),
#ifdef BC_MEMORY_DEFRAG
			m_movable(false),
#endif
		  m_extra(nullptr)
	{
	}

	inline bcSIZE bc_memblock::size() const noexcept
	{
		return m_size;
	}

	inline void bc_memblock::size(bcSIZE p_val) noexcept
	{
		m_size = p_val;
	}

	inline bcUINT8 bc_memblock::alignment() const noexcept
	{
		return m_alignment;
	}

	inline void bc_memblock::alignment(bcUINT8 p_val) noexcept
	{
		m_alignment = p_val;
	}

	inline bcUINT8 bc_memblock::offset() const noexcept
	{
		return m_offset;
	}

	inline void bc_memblock::offset(bcUINT8 p_val) noexcept
	{
		m_offset = p_val;
	}

#ifdef BC_MEMORY_DEFRAG
		inline bool bc_memblock::movable_pointer() const noexcept
		{
			return m_movable;
		}

		inline void bc_memblock::movable_pointer(bool p_val) noexcept
		{
			m_movable = p_val;
		}
#endif

	inline void* bc_memblock::extra() const noexcept
	{
		return m_extra;
	}

	inline void bc_memblock::extra(void* p_val) noexcept
	{
		m_extra = p_val;
	}

	inline void bc_memblock::initialize_after_allocation(void** p_pointer, bool p_movable, void* p_extra) noexcept
	{
		offset(static_cast<bcUINT8>(get_required_offset_for_data(*p_pointer, alignment())));
#ifdef BC_MEMORY_DEFRAG
			movable_pointer(p_movable);
#endif
		extra(p_extra);

		*p_pointer = reinterpret_cast<void*>((static_cast<bcBYTE*>(*p_pointer) + offset()));

		bc_memblock::store_mem_block(*p_pointer, this);
	}

	inline constexpr bcSIZE bc_memblock::get_aligned_size(bcSIZE p_size, bcUINT8 p_alignment) noexcept
	{
		BC_ASSERT(bc_is_power_of_two(p_alignment));
		return (p_size + (p_alignment - 1)) & ~(p_alignment - 1);
	}

	inline bc_memblock* bc_memblock::retrieve_mem_block(void* p_pointer) noexcept
	{
		return reinterpret_cast<bc_memblock*>(static_cast<bcBYTE*>(p_pointer) - sizeof(bc_memblock));
	}

	inline bcSIZE bc_memblock::get_required_size(bcSIZE p_data_size, bcUINT8 p_alignment) noexcept
	{
		const bcSIZE l_extra_alignment = std::max(0, static_cast<bcINT32>(p_alignment) - BC_MEMORY_MIN_ALIGN);
		return get_aligned_size(sizeof(bc_memblock), BC_MEMORY_MIN_ALIGN) + l_extra_alignment + get_aligned_size(p_data_size, BC_MEMORY_MIN_ALIGN);
	}

	inline bcSIZE bc_memblock::get_required_offset_for_data(const void* p_pointer, bcUINT8 p_alignment) noexcept
	{
		const auto l_data_start = reinterpret_cast<bcUINTPTR>(p_pointer) + get_aligned_size(sizeof(bc_memblock), BC_MEMORY_MIN_ALIGN);
		const auto l_aligned_data_start = get_aligned_size(l_data_start, p_alignment);
			
		return l_aligned_data_start - reinterpret_cast<bcUINTPTR>(p_pointer);
	}
		
	inline void bc_memblock::store_mem_block(void* p_pointer, const bc_memblock* p_block) noexcept
	{
		*reinterpret_cast<bc_memblock*>((static_cast<bcBYTE*>(p_pointer) - sizeof(bc_memblock))) = *p_block;
	}

	inline void bc_memblock::free_mem_block(void* p_pointer) noexcept
	{
	}
#endif
}
