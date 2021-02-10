
#include "Core/CorePCH.h"
#include "Core/bcException.h"
#include "Core/Container/bcBitVector.h"

namespace black_cat
{
	namespace core
	{
		bc_bit_vector::bc_bit_vector()
			: m_capacity(0),
			m_block_count(0),
			m_blocks(nullptr)
		{
		}

		bc_bit_vector::bc_bit_vector(bcUINT32 p_capacity)
			: bc_bit_vector()
		{
			resize(p_capacity);
			make_all_false();
		}

		bc_bit_vector::bc_bit_vector(const bc_bit_vector& p_other)
		{
			operator=(p_other);
		}

		bc_bit_vector::bc_bit_vector(bc_bit_vector&& p_other) noexcept
		{
			operator=(std::move(p_other));
		}

		bc_bit_vector::~bc_bit_vector()
		{
			if (m_blocks != nullptr)
			{
				BC_FREE(m_blocks);
			}

			m_capacity = 0;
			m_block_count = 0;
			m_blocks = nullptr;
		}

		bc_bit_vector& bc_bit_vector::operator=(const bc_bit_vector& p_other)
		{
			resize(p_other.m_capacity);
			std::memcpy(m_blocks, p_other.m_blocks, m_block_count * sizeof(block_t));

			return *this;
		}

		bc_bit_vector& bc_bit_vector::operator=(bc_bit_vector&& p_other) noexcept
		{
			m_capacity = p_other.m_capacity;
			m_block_count = p_other.m_block_count;
			m_blocks = p_other.m_blocks;

			p_other.m_capacity = 0;
			p_other.m_block_count = 0;
			p_other.m_blocks = nullptr;

			return *this;
		}

		bool bc_bit_vector::operator[](bcUINT32 p_index) const noexcept
		{
			BC_ASSERT(p_index < m_capacity);
			
			const bcINT32 l_cell = p_index / (sizeof(block_t) * 8);
			const bcINT32 l_bit = p_index % (sizeof(block_t) * 8);

			return (m_blocks[l_cell] & (1 << l_bit)) >> l_bit;
		}

		bool bc_bit_vector::at(bcUINT32 p_index) const
		{
			if (p_index >= m_capacity)
			{
				throw std::out_of_range("Index out of range");
			}

			return operator[](p_index);
		}

		void bc_bit_vector::set(bcUINT32 p_index, bool p_value)
		{
			if (p_index >= m_capacity)
			{
				throw std::out_of_range("Index out of range");
			}

			const bcINT32 l_cell = p_index / (sizeof(block_t) * 8);
			const bcINT32 l_bit = p_index % (sizeof(block_t) * 8);

			if (p_value)
			{
				m_blocks[l_cell] = (m_blocks[l_cell] | (static_cast<block_t>(1) << l_bit));
			}
			else
			{
				m_blocks[l_cell] = (m_blocks[l_cell] & (~(static_cast<block_t>(1) << l_bit)));
			}
		}

		void bc_bit_vector::make_false(bcUINT32 p_index)
		{
			if (p_index >= m_capacity)
			{
				throw std::out_of_range("Index out of range");
			}

			const bcINT32 l_cell = p_index / (sizeof(block_t) * 8);
			const bcINT32 l_bit = p_index % (sizeof(block_t) * 8);

			m_blocks[l_cell] = (m_blocks[l_cell] & (~(static_cast<block_t>(1) << l_bit)));
		}

		void bc_bit_vector::make_all_false() noexcept
		{
			std::memset(m_blocks, static_cast<bcBYTE>(0), m_block_count * sizeof(block_t));
		}

		void bc_bit_vector::make_true(bcUINT32 p_index) noexcept
		{
			if (p_index >= m_capacity)
			{
				throw std::out_of_range("Index out of range");
			}

			const bcINT32 l_cell = p_index / (sizeof(block_t) * 8);
			const bcINT32 l_bit = p_index % (sizeof(block_t) * 8);

			m_blocks[l_cell] = (m_blocks[l_cell] | (static_cast<block_t>(1) << l_bit));
		}

		void bc_bit_vector::make_all_true() noexcept
		{
			std::memset(m_blocks, static_cast<bcBYTE>(255), m_block_count * sizeof(block_t));
		}

		bool bc_bit_vector::find_first_false(bcUINT32& p_result, bcUINT32 p_search_start) const noexcept
		{
			const auto l_start_block = p_search_start / (sizeof(block_t) * 8);
			for (auto l_block_ite = l_start_block; l_block_ite < m_block_count; ++l_block_ite)
			{
				const auto l_block = m_blocks[l_block_ite];

				if ((l_block & m_block_mask) == m_block_mask) // No false bit
				{
					continue;
				}

				// Start searching in sub blocks
				auto l_sub_block_size_bytes = sizeof(block_t) / 2;
				auto l_sub_block_shift_bytes = 0U;

				while (l_sub_block_size_bytes >= 1)
				{
					for (auto l_sub_block_byte_ite = l_sub_block_shift_bytes; l_sub_block_byte_ite < sizeof(block_t); l_sub_block_byte_ite += l_sub_block_size_bytes)
					{
						const auto l_sub_block_ite = (l_sub_block_byte_ite - l_sub_block_shift_bytes) / l_sub_block_size_bytes;
						const auto l_sub_block_mask = m_block_mask >> ((sizeof(block_t) - l_sub_block_size_bytes) * 8);
						const auto l_sub_block = (l_block >> (l_sub_block_ite * l_sub_block_size_bytes * 8 + l_sub_block_shift_bytes * 8))& l_sub_block_mask;

						if ((l_sub_block & l_sub_block_mask) == l_sub_block_mask) // No free bit
						{
							continue;
						}

						l_sub_block_shift_bytes += l_sub_block_size_bytes * l_sub_block_ite;
						break;
					}

					l_sub_block_size_bytes /= 2;
				}

				const auto l_start_bit = l_sub_block_shift_bytes * 8;
				auto l_found_bit = 0U;

				for (auto l_bit_ite = l_start_bit; l_bit_ite < l_start_bit + 8; ++l_bit_ite)
				{
					if (static_cast<block_t>(0) == (l_block & (static_cast<block_t>(1) << l_bit_ite)))
					{
						l_found_bit = l_bit_ite;
						break;
					}
				}

				p_result = (l_block_ite * (sizeof(block_t) * 8)) + l_found_bit;
				return l_found_bit < m_capacity;
			}

			return false;
		}

		bc_vector_frame<bcUINT32> bc_bit_vector::find_true_indices() const noexcept
		{
			bc_vector_frame<bcUINT32> l_result(m_capacity);
			bcSIZE l_false_count = 0U;
			
			for (auto l_block_ite = 0U; l_block_ite < m_block_count; ++l_block_ite)
			{
				const auto l_block = m_blocks[l_block_ite];

				if ((l_block & m_block_mask) == 0) // No true bit
				{
					continue;
				}
				
				for(auto l_bit_ite = 0U; l_bit_ite < sizeof(block_t) * 8; ++l_bit_ite)
				{
					const auto l_is_true = static_cast<bool>(l_block & (1 << l_bit_ite));
					const auto l_bit_index = l_block_ite * sizeof(block_t) * 8 + l_bit_ite;

					if(l_is_true && l_bit_index < m_capacity)
					{
						l_result[l_false_count] = l_bit_index;
						++l_false_count;
					}
				}
			}

			l_result.resize(l_false_count);
			return l_result;
		}

		void bc_bit_vector::resize(bcUINT32 p_capacity, bc_alloc_type p_alloc_type)
		{
			const auto l_new_block_count = static_cast<bcSIZE>(std::ceilf(p_capacity / (sizeof(block_t) * 8.0f)));
			if(l_new_block_count == m_block_count)
			{
				m_capacity = p_capacity;
				return;
			}
			
			auto* l_new_array = static_cast<block_t*>(BC_ALLOC_THROW(l_new_block_count * sizeof(block_t), p_alloc_type));

			const bcINT32 l_min_block_count = (std::min)(m_block_count, l_new_block_count);
			std::memcpy(l_new_array, m_blocks, l_min_block_count * sizeof(block_t));

			if (m_block_count < l_new_block_count)
			{
				std::memset(l_new_array + m_block_count, 0, (l_new_block_count - m_block_count) * sizeof(block_t));
			}

			if (m_blocks != nullptr)
			{
				BC_FREE(m_blocks);
			}

			m_capacity = p_capacity;
			m_block_count = l_new_block_count;
			m_blocks = l_new_array;
		}
	}
}