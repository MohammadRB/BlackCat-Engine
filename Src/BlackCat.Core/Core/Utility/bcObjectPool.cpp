// [2/11/2015 MRB]

#include "Core/CorePCH.h"
#include "Core/Utility/bcObjectPool.h"

namespace black_cat
{
	namespace core
	{
		bc_concurrent_memory_pool::bc_concurrent_memory_pool()
			: m_num_block(0),
			m_block_size(0),
			m_num_bit_blocks(0),
			m_blocks(nullptr),
			m_heap(nullptr)
		{
		}

		bc_concurrent_memory_pool::bc_concurrent_memory_pool(bc_concurrent_memory_pool&& p_other) noexcept
			: bc_initializable(std::move(p_other))
		{
			operator=(std::move(p_other));
		}

		bc_concurrent_memory_pool::~bc_concurrent_memory_pool()
		{
			if (m_initialized)
			{
				destroy();
			}
		}

		bc_concurrent_memory_pool& bc_concurrent_memory_pool::operator=(bc_concurrent_memory_pool&& p_other) noexcept
		{
			bc_initializable::operator=(std::move(p_other));
			bcUINT32 l_allocated_block = p_other.m_allocated_block.load(core_platform::bc_memory_order::acquire);

			m_num_block = p_other.m_num_block;
			m_block_size = p_other.m_block_size;
			m_num_bit_blocks = p_other.m_num_bit_blocks;
			m_blocks = p_other.m_blocks;
			m_heap = p_other.m_heap;

			p_other.m_num_block = 0;
			p_other.m_block_size = 0;
			p_other.m_num_bit_blocks = 0;
			p_other.m_blocks = nullptr;
			p_other.m_heap = nullptr;

			m_allocated_block.store(l_allocated_block, core_platform::bc_memory_order::release);

			return *this;
		}

		void* bc_concurrent_memory_pool::alloc() noexcept
		{
			void* l_result = nullptr;
			bool l_reached_end = false;
			bcINT32 l_block = -1;
			bcUINT32 l_allocated_block = m_allocated_block.load(core_platform::bc_memory_order::seqcst);

			for (bcUINT32 i = l_allocated_block; (i != l_allocated_block || !l_reached_end); ++i)
			{
				if (i >= m_num_bit_blocks)
				{
					i = 0;
					l_reached_end = true;
				}
				bit_block_type l_current_block = m_blocks[i].load(core_platform::bc_memory_order::seqcst);

				// any free blocks in this chunk?
				if (s_bit_block_mask != l_current_block)
				{
					// find a free entry in this chunk then allocate it and set the proper block index
					for (bcUINT32 j = 0; j < s_bit_block_size; ++j)
					{
						if (bit_block_type(0) == (l_current_block & (bit_block_type(1) << j)))
						{
							const bit_block_type l_current_block_changed = l_current_block | (bit_block_type(1) << j);
							if (m_blocks[i].compare_exchange_strong
								(
									&l_current_block,
									l_current_block_changed,
									core_platform::bc_memory_order::seqcst,
									core_platform::bc_memory_order::seqcst
								))
							{
								l_block = i * s_bit_block_size + j;

								m_allocated_block.compare_exchange_strong(&l_allocated_block, i, core_platform::bc_memory_order::seqcst);

								break;
							}
						}
					}

					if (l_block != -1)
					{
						break;
					}
				}
			}

			// A free block were found
			if (l_block != -1 || l_block <= m_num_block)
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
			const bcINT32 l_block = static_cast<bcINT32>(l_pointer - m_heap) / m_block_size;

			// reset the bit in our block management array
			const bcINT32 l_chunk_index = l_block / s_bit_block_size;
			const bcINT32 l_bit_index = l_block % s_bit_block_size;

			m_blocks[l_chunk_index].fetch_and(~(bit_block_type(1) << l_bit_index), core_platform::bc_memory_order::seqcst);

#ifdef BC_MEMORY_DEBUG
			std::memset(reinterpret_cast<void*>(l_pointer), 0, m_block_size);
#endif
		}

		bool bc_concurrent_memory_pool::contain_pointer(const void* p_pointer) const noexcept
		{
			return p_pointer >= m_heap && p_pointer < m_heap + (m_block_size * m_num_block);
		}

		void bc_concurrent_memory_pool::clear() noexcept
		{
			for (bcUINT32 i = 0; i < m_num_bit_blocks; ++i)
			{
				m_blocks[i].store(0U);
			}
		}

		void bc_concurrent_memory_pool::_initialize(bcUINT32 p_object_count, bcUINT32 p_object_size, bc_alloc_type p_alloc_type)
		{
			m_num_block = p_object_count;
			m_block_size = p_object_size;

			if (m_num_block % s_bit_block_size != 0)
			{
				m_num_block = ((m_num_block / s_bit_block_size) + 1) * s_bit_block_size;
			}

			m_num_bit_blocks = m_num_block / s_bit_block_size;
			m_allocated_block.store(0U);

			m_blocks = static_cast< core_platform::bc_atomic< bit_block_type >* >
			(
				bcAlloc(m_num_bit_blocks * sizeof(core_platform::bc_atomic< bit_block_type >), p_alloc_type)
			);
			if (!m_blocks)
			{
				throw std::bad_alloc();
			}

			for (bcUINT32 i = 0; i < m_num_bit_blocks; ++i)
			{
				m_blocks[i].store(0U);
			}

			m_heap = static_cast<bcUBYTE*>(bcAlignedAlloc((m_block_size * m_num_block) * sizeof(bcUBYTE), BC_MEMORY_MIN_ALIGN, p_alloc_type));

			if (!m_heap)
			{
				_aligned_free(m_blocks);
				throw std::bad_alloc();
			}
		}

		void bc_concurrent_memory_pool::_destroy()
		{
#if defined(BC_DEBUG)
			bcSIZE l_num_alive_objects = 0;
			for (bcUINT32 l_i = 0; l_i < m_num_bit_blocks; ++l_i)
			{
				const auto l_current_block = m_blocks[l_i].load(core_platform::bc_memory_order::relaxed);
				for (bcUINT32 l_j = 0; l_j < s_bit_block_size; ++l_j)
				{
					const bool l_is_alive = l_current_block & (bit_block_type(1) << l_j);
					if (l_is_alive)
					{
						++l_num_alive_objects;
					}
				}
			}

			bcAssert(l_num_alive_objects == 0);
#endif

			bcFree(m_blocks);
			bcAlignedFree(m_heap);

			m_num_block = 0;
			m_blocks = nullptr;
			m_heap = nullptr;
		}
	}
}