// [06/07/2021 MRB]

#include "Core/CorePCH.h"
#include "Core/File/bcMemoryStream.h"

namespace black_cat
{
	namespace core
	{
		inline void bc_memory_stream::_increase_capacity(bcSIZE p_required_size)
		{
			const auto l_new_position = m_position + p_required_size;
			const auto l_required_size = l_new_position - m_buffer_size;
			if (l_required_size <= 0)
			{
				return;
			}

			const auto l_new_buffer_size = m_buffer_size + l_required_size;
			auto* l_new_buffer = m_buffer ?
				static_cast<bcBYTE*>(BC_ALIGNED_REALLOC(m_buffer, l_new_buffer_size, BC_MEMORY_MIN_ALIGN, m_alloc_type)) :
				static_cast<bcBYTE*>(BC_ALIGNED_ALLOC(l_new_buffer_size, BC_MEMORY_MIN_ALIGN, m_alloc_type));

			m_buffer_size = l_new_buffer_size;
			m_buffer = l_new_buffer;
		}
	}	
}