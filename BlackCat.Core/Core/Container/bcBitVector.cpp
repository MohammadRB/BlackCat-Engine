
#include "Core/CorePCH.h"
#include "Core/CoreExceptions.h"
#include "Core/Container/bcBitVector.h"

namespace black_cat
{
	namespace core
	{
		bc_bit_vector::bc_bit_vector(bcUINT32 p_size)
			: m_array(nullptr),
			m_size(0)
		{
			resize(p_size);
		};

		bc_bit_vector::bc_bit_vector(const bc_bit_vector& p_other)
			: bc_bit_vector(p_other.m_size)
		{
			std::memcpy(m_array, p_other.m_array, m_size);
		};

		bc_bit_vector::bc_bit_vector(bc_bit_vector&& p_other)
			: m_array(p_other.m_array),
			m_size(p_other.m_size)
		{
			p_other.m_array = nullptr;
			p_other.m_size = 0;
		};

		bc_bit_vector::~bc_bit_vector()
		{
			if (m_array != nullptr)
				bcFree(m_array);

			m_array = nullptr;
		};

		bc_bit_vector& bc_bit_vector::operator=(const bc_bit_vector& p_other)
		{
			resize(p_other.m_size);
			std::memcpy(m_array, p_other.m_array, m_size);

			return *this;
		};

		bc_bit_vector& bc_bit_vector::operator=(bc_bit_vector&& p_other)
		{
			m_array = p_other.m_array;
			m_size = p_other.m_size;

			p_other.m_array = nullptr;
			p_other.m_size = 0;

			return *this;
		};

		void bc_bit_vector::resize(bcUINT32 p_size, bc_alloc_type p_alloc_type)
		{
			if (p_size == 0 || m_size == p_size)
				return;

			bcUINT32* l_new_vector;

			if (p_size % 32 == 0)
				p_size = p_size / 32;
			else
				p_size = (p_size / 32) + 1;

			l_new_vector = reinterpret_cast<bcUINT32*>(bcAllocThrow(((p_size / 32) + 1) * sizeof(bcUINT32), p_alloc_type));

			bcINT32 l_min = std::min(p_size, m_size);
			std::memcpy(l_new_vector, m_array, l_min * sizeof(bcUINT32));

			if (m_size < p_size)
				std::memset(m_array + m_size, 0, p_size - m_size);

			if (m_array != nullptr)
				bcFree(m_array);

			m_array = l_new_vector;
			m_size = p_size;
		};

		bool bc_bit_vector::operator[](bcUINT32 p_index)
		{
			if (p_index >= m_size)
				throw std::out_of_range("Index out of range");

			bcINT32 l_cell = p_index / 32;
			bcINT32 l_bit = p_index % 32;

			return (m_array[l_cell] & (1 << l_bit)) >> l_bit;
		};

		void bc_bit_vector::set(bcUINT32 p_index, bool p_value)
		{
			if (p_index >= m_size)
				throw std::out_of_range("Index out of range");

			bcINT32 l_cell = p_index / 32;
			bcINT32 l_bit = p_index % 32;

			if (p_value == true)
				m_array[l_cell] = (m_array[l_cell] | (1 << l_bit));
			else
				m_array[l_cell] = (m_array[l_cell] & (~(1 << l_bit)));
		};

		void bc_bit_vector::false_all()
		{
			std::memset(m_array, 0, ((m_size / 32) + 1) * sizeof(bcINT32));
		};

		void bc_bit_vector::true_all()
		{
			std::memset(m_array, 1, ((m_size / 32) + 1) * sizeof(bcINT32));
		};
	}
}