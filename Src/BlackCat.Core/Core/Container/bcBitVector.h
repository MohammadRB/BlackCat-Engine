#pragma once

#include "Core/bcExport.h"
#include "Core/Container/bcContainer.h"

namespace black_cat
{
	namespace core
	{
		class BC_CORE_DLL bc_bit_vector
		{
		public:
			bc_bit_vector(bcUINT32 p_size);

			bc_bit_vector(const bc_bit_vector& p_other);

			bc_bit_vector(bc_bit_vector&& p_other);

			~bc_bit_vector();

			bc_bit_vector& operator =(const bc_bit_vector& p_other);

			bc_bit_vector& operator =(bc_bit_vector&& p_other);

			void resize(bcUINT32 p_size, bc_alloc_type p_alloc_type = bc_alloc_type::unknown);

			bool operator[](bcUINT32 p_index);

			void set(bcUINT32 p_index, bool p_value);

			void false_all();

			void true_all();

			bcUINT32 size()
			{
				return m_size;
			}

		protected:

		private:
			bcUINT32* m_array;
			bcUINT32 m_size;
		};
	}
}