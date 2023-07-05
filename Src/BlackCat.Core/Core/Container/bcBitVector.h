#pragma once

#include "Core/bcExport.h"
#include "Core/Container/bcContainer.h"
#include "Core/Container/bcVector.h"

namespace black_cat::core
{
	class BC_CORE_DLL bc_bit_vector
	{
	public:
		bc_bit_vector();
			
		/**
			 * \brief Initialize bits with false values
			 * \param p_capacity 
			 */
		explicit bc_bit_vector(bcUINT32 p_capacity);

		bc_bit_vector(const bc_bit_vector& p_other);

		bc_bit_vector(bc_bit_vector&& p_other) noexcept;

		~bc_bit_vector();

		bc_bit_vector& operator =(const bc_bit_vector& p_other);

		bc_bit_vector& operator =(bc_bit_vector&& p_other) noexcept;

		bcUINT32 capacity() const noexcept;
			
		bool operator[](bcUINT32 p_index) const noexcept;

		bool at(bcUINT32 p_index) const;
			
		void set(bcUINT32 p_index, bool p_value);
			
		void make_false(bcUINT32 p_index);

		void make_true(bcUINT32 p_index);

		void make_all_false() noexcept;

		void make_all_true() noexcept;

		bool find_first_false(bcUINT32& p_result, bcUINT32 p_search_start = 0) const noexcept;

		bc_vector_frame<bcUINT32> find_true_indices() const noexcept;
			
		/**
			 * \brief Change the capacity of vector and initialize newly created bits with false values
			 * \param p_capacity 
			 * \param p_alloc_type 
			 */
		void resize(bcUINT32 p_capacity, bc_alloc_type p_alloc_type = bc_alloc_type::unknown_movable);

	private:
		using block_t = bcUINT32;
		const block_t m_block_mask = 0xffffffff;
			
		bcUINT32 m_capacity;
		bcUINT32 m_block_count;
		block_t* m_blocks;
	};

	inline bcUINT32 bc_bit_vector::capacity() const noexcept
	{
		return m_capacity;
	}

	inline void bc_bit_vector::make_false(bcUINT32 p_index)
	{
		set(p_index, false);
	}

	inline void bc_bit_vector::make_true(bcUINT32 p_index)
	{
		set(p_index, true);
	}

}
