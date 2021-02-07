
#include "Core/CorePCH.h"
#include "Core/bcException.h"
#include "Core/Container/bcBitVector.h"

namespace black_cat
{
	namespace core
	{
		bcSIZE _get_block_count(bcSIZE p_size)
		{
			auto l_block_count = p_size / 32;
			if (p_size % 32 != 0)
			{
				l_block_count += 1;
			}

			return l_block_count;
		}

		bc_bit_vector::bc_bit_vector(bcUINT32 p_size)
			: m_array(nullptr),
			m_size(0)
		{
			resize(p_size);
		}

		bc_bit_vector::bc_bit_vector(const bc_bit_vector& p_other)
			: bc_bit_vector(p_other.m_size)
		{
			std::memcpy(m_array, p_other.m_array, _get_block_count(m_size));
		}

		bc_bit_vector::bc_bit_vector(bc_bit_vector&& p_other) noexcept
			: m_array(p_other.m_array),
			m_size(p_other.m_size)
		{
			p_other.m_array = nullptr;
			p_other.m_size = 0;
		}

		bc_bit_vector::~bc_bit_vector()
		{
			if (m_array != nullptr)
			{
				BC_FREE(m_array);
			}

			m_array = nullptr;
		}

		bc_bit_vector& bc_bit_vector::operator=(const bc_bit_vector& p_other)
		{
			resize(p_other.m_size);
			std::memcpy(m_array, p_other.m_array, _get_block_count(m_size));

			return *this;
		}

		bc_bit_vector& bc_bit_vector::operator=(bc_bit_vector&& p_other) noexcept
		{
			m_array = p_other.m_array;
			m_size = p_other.m_size;

			p_other.m_array = nullptr;
			p_other.m_size = 0;

			return *this;
		}

		void bc_bit_vector::resize(bcUINT32 p_size, bc_alloc_type p_alloc_type)
		{
			if (p_size == 0 || m_size == p_size)
			{
				return;
			}

			const auto l_block_count = _get_block_count(m_size);
			const auto l_new_block_count = _get_block_count(p_size);

			bcUINT32* l_new_array = reinterpret_cast< bcUINT32* >(BC_ALLOC_THROW(l_new_block_count * sizeof(bcUINT32), p_alloc_type));

			const bcINT32 l_min_block_count = (std::min)(l_block_count, l_new_block_count);
			std::memcpy(l_new_array, m_array, l_min_block_count * sizeof(bcUINT32));

			if (l_block_count < l_new_block_count)
			{
				std::memset(l_new_array + l_block_count, 0, l_new_block_count - l_block_count);
			}

			if (m_array != nullptr)
			{
				BC_FREE(m_array);
			}

			m_array = l_new_array;
			m_size = p_size;
		}

		bool bc_bit_vector::operator[](bcUINT32 p_index) const
		{
			if (p_index >= m_size)
			{
				throw std::out_of_range("Index out of range");
			}

			const bcINT32 l_cell = p_index / 32;
			const bcINT32 l_bit = p_index % 32;

			return (m_array[l_cell] & (1 << l_bit)) >> l_bit;
		}

		void bc_bit_vector::set(bcUINT32 p_index, bool p_value)
		{
			if (p_index >= m_size)
			{
				throw std::out_of_range("Index out of range");
			}

			const bcINT32 l_cell = p_index / 32;
			const bcINT32 l_bit = p_index % 32;

			if (p_value)
			{
				m_array[l_cell] = (m_array[l_cell] | (1 << l_bit));
			}
			else
			{
				m_array[l_cell] = (m_array[l_cell] & (~(1 << l_bit)));
			}
		}

		void bc_bit_vector::false_all()
		{
			auto l_block_count = _get_block_count(m_size);
			std::memset(m_array, 0, l_block_count * sizeof(bcINT32));
		}

		void bc_bit_vector::true_all()
		{
			auto l_block_count = _get_block_count(m_size);
			std::memset(m_array, 1, l_block_count * sizeof(bcINT32));
		}
	}
}