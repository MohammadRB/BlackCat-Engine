// [2/11/2015 MRB]

#pragma once

#include "Core/CorePCH.h"
#include "Core/Utility/bcObjectPool.h"

namespace black_cat
{
	namespace core
	{
		void bc_concurrent_memory_pool::initialize(bcUINT32 p_num_block, bcUINT32 p_block_size, bc_alloc_type p_alloc_type)
		{
			m_num_block = p_num_block;
			m_block_size = p_block_size;

			if (m_num_block % s_bitblock_size != 0)
			{
				m_num_block = ((m_num_block / s_bitblock_size) + 1) * s_bitblock_size;
			}

			m_num_bitblocks = m_num_block / s_bitblock_size;
			m_allocated_block.store(0U);

			m_blocks = static_cast<core_platform::bc_atomic< bitblock_type >*>(
				bcAlloc(m_num_bitblocks * sizeof(core_platform::bc_atomic< bitblock_type >), p_alloc_type));

			if (!m_blocks)
				throw std::bad_alloc();

			for (bcUINT32 i = 0; i < m_num_bitblocks; ++i)
				m_blocks[i].store(0U);

			m_heap = static_cast<bcUBYTE*>(bcAlloc((m_block_size * m_num_block) * sizeof(bcUBYTE), p_alloc_type));

			if (!m_heap)
			{
				_aligned_free(m_blocks);
				throw std::bad_alloc();
			}
		}

		void bc_concurrent_memory_pool::destroy() noexcept(true)
		{
			bcFree(m_blocks);
			bcFree(m_heap);

			m_num_block = 0;
			m_blocks = nullptr;
			m_heap = nullptr;
		}

		void* bc_concurrent_memory_pool::alloc() noexcept(true)
		{
			// Search for a free block /
			bcINT32 l_block = -1;
			void* l_result = nullptr;

			bool l_reach_end = false;
			// TODO
			bcUINT32 l_allocated_block = m_allocated_block.load(core_platform::bc_memory_order::seqcst);
			for (bcINT32 i = l_allocated_block; (i != l_allocated_block || !l_reach_end); ++i)
			{
				if (i >= m_num_bitblocks)
				{
					i = 0;
					l_reach_end = true;
				}
				bitblock_type l_current_block = m_blocks[i].load(core_platform::bc_memory_order::seqcst);

				// any free blocks in this chunk? /
				if (s_bitblock_mask != l_current_block)
				{
					// find a free entry in this chunk then allocate it and set the proper block index /
					for (int j = 0; j < s_bitblock_size; ++j)
					{
						if (bitblock_type(0) == (l_current_block & (bitblock_type(1) << j)))
						{
							bitblock_type l_current_block_changed = l_current_block | (bitblock_type(1) << j);
							if (m_blocks[i].compare_exchange_strong(
								&l_current_block,
								l_current_block_changed,
								core_platform::bc_memory_order::seqcst,
								core_platform::bc_memory_order::seqcst))
							{
								l_block = i * s_bitblock_size + j;

								m_allocated_block.compare_exchange_strong(&l_allocated_block, i, core_platform::bc_memory_order::seqcst);

								break;
							}

						}
					}

					if (l_block != -1)
						break;
				}
			}

			// A free block were found /
			if (-1 != l_block || l_block <= m_num_block)
			{
				l_result = reinterpret_cast<void*>(reinterpret_cast<bcUINT32>(m_heap)+(l_block * m_block_size));
			}

			return l_result;
		}

		void bc_concurrent_memory_pool::free(void* p_pointer) noexcept(true)
		{
			// is this pointer in our heap?
			bcAssert(contain_pointer(p_pointer));

			bcUBYTE* l_pointer = reinterpret_cast<bcUBYTE*>(p_pointer);

			// convert the pointer into a block index
			bcINT32 l_block = static_cast<bcINT32>(l_pointer - m_heap) / m_block_size;

			// reset the bit in our block management array
			bcINT32 l_chunk_index = l_block / s_bitblock_size;
			bcINT32 l_bit_index = l_block % s_bitblock_size;

			m_blocks[l_chunk_index].fetch_add(~(bitblock_type(1) << l_bit_index), core_platform::bc_memory_order::seqcst);

#ifdef BC_MEMORY_DEBUG
			std::memset(reinterpret_cast<void*>(l_pointer), 0, m_block_size);
#endif
		}

		bool bc_concurrent_memory_pool::contain_pointer(const void* p_pointer) const noexcept(true)
		{
			return (p_pointer >= m_heap && p_pointer < m_heap + (m_block_size * m_num_block)) ? true : false;
		}

		void bc_concurrent_memory_pool::clear() noexcept(true)
		{
			std::memset(m_blocks, 0, m_num_bitblocks * sizeof(core_platform::bc_atomic< bitblock_type >));
		}
	}
}