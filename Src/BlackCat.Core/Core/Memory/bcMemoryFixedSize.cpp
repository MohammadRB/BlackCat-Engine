//  [23/8/2013 MRB]

#include "Core/CorePCH.h"
#include "CorePlatformImp/Memory/bcMemAlloc.h"
#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "Core/Memory/bcMemoryFixedSize.h"

namespace black_cat
{
	namespace core
	{
#ifdef BC_MEMORY_ENABLE
		bc_memory_fixed_size::bc_memory_fixed_size() noexcept
			: m_num_block(0),
			m_block_size(0),
			m_num_bit_blocks(0),
			m_blocks(nullptr),
			m_heap(nullptr)
		{
		}

		bc_memory_fixed_size::bc_memory_fixed_size(this_type&& p_other) noexcept
			: bci_memory(std::move(p_other))
		{
			_move(std::move(p_other));
		}

		bc_memory_fixed_size::~bc_memory_fixed_size() noexcept
		{
			if (is_initialized())
			{
				destroy();
			}
		}

		bc_memory_fixed_size::this_type& bc_memory_fixed_size::operator=(this_type&& p_other) noexcept
		{
			bci_memory::operator =(std::move(p_other));
			_move(std::move(p_other));

			return *this;
		}

		void* bc_memory_fixed_size::alloc(bc_memblock* p_mem_block) noexcept
		{
			const bcUINT32 l_size = p_mem_block->size();

			BC_ASSERT(l_size <= m_block_size);
			BC_ASSERT(l_size > 0);

			// Search for a free block
			bcINT32 l_block = -1;
			void* l_result = nullptr;

			auto l_allocated_block = m_allocated_block.load(platform::bc_memory_order::relaxed);
			const auto l_end = l_allocated_block + m_num_bit_blocks;
			for (bcUINT32 l_i = l_allocated_block; l_i < l_end; ++l_i)
			{
				const auto l_bit_block_ite = l_i % m_num_bit_blocks;
				auto l_current_block = m_blocks[l_bit_block_ite].load(platform::bc_memory_order::relaxed);

				// any free blocks in this chunk?
				if (s_bit_block_mask == l_current_block)
				{
					continue;
				}
				
				// find a free entry in this chunk then allocate it and set the proper block index
				for (bcUINT32 l_bit_ite = 0U; l_bit_ite < s_bit_block_size; ++l_bit_ite)
				{
					if (static_cast< bit_block_type >(0) == (l_current_block & (static_cast< bit_block_type >(1) << l_bit_ite)))
					{
						const auto l_current_block_changed = l_current_block | (static_cast< bit_block_type >(1) << l_bit_ite);
						if (m_blocks[l_bit_block_ite].compare_exchange_strong
						(
							&l_current_block,
							l_current_block_changed,
							platform::bc_memory_order::relaxed,
							platform::bc_memory_order::relaxed
						))
						{
							l_block = l_bit_block_ite * s_bit_block_size + l_bit_ite;

							m_allocated_block.compare_exchange_strong(&l_allocated_block, l_bit_block_ite, platform::bc_memory_order::seqcst);

							break;
						}

					}
				}

				if (l_block != -1)
				{
					break;
				}
			}

			// A free block found
			if (-1 != l_block || l_block <= m_num_block)
			{
				l_result = reinterpret_cast<void*>(m_heap + l_block * m_block_size);
				m_tracer.accept_alloc(l_size);
			}
			else
			{
				m_tracer.reject_alloc(l_size);
			}

			return l_result;
		}

		void bc_memory_fixed_size::free(void* p_pointer, bc_memblock* p_mem_block) noexcept
		{
			auto* l_pointer = static_cast<bcUBYTE*>(p_pointer);

			// is this pointer in our heap?
			BC_ASSERT((l_pointer >= m_heap) && (l_pointer < m_heap + m_block_size * m_num_block));

			// convert the pointer into a block index
			const bcINT32 l_block = (l_pointer - m_heap) / m_block_size;

			// reset the bit in our block management array
			const bcINT32 l_chunk_index = l_block / s_bit_block_size;
			const bcINT32 l_bit_index = l_block % s_bit_block_size;

			m_blocks[l_chunk_index].fetch_and(~(static_cast<bit_block_type>(1) << l_bit_index), platform::bc_memory_order::seqcst);

			m_tracer.accept_free(p_mem_block->size());

//#ifdef BC_MEMORY_DEBUG
//			std::memset(l_pointer, 0, p_mem_block->size());
//#endif
		}

		void bc_memory_fixed_size::clear() noexcept
		{
			for (bcUINT32 i = 0; i < m_num_bit_blocks; ++i)
			{
				m_blocks[i].store(0U);
			}

			m_tracer.accept_clear();
		}

		void bc_memory_fixed_size::_initialize(bcUINT32 p_num_block, bcUINT32 p_block_size, const bcCHAR* p_tag)
		{
			m_num_block = p_num_block;
			m_block_size = p_block_size;

			if (m_num_block % s_bit_block_size == 0)
			{
				m_num_bit_blocks = m_num_block / s_bit_block_size;
			}
			else
			{
				m_num_block = ((m_num_block / s_bit_block_size) + 1) * s_bit_block_size;
				m_num_bit_blocks = m_num_block / s_bit_block_size;
			}
			m_allocated_block.store(0U);

			m_blocks = static_cast< platform::bc_atomic< bit_block_type >* >
			(
				platform::bc_mem_aligned_alloc(m_num_bit_blocks * sizeof(platform::bc_atomic< bit_block_type >), BC_MEMORY_MIN_ALIGN)
			);

			if (!m_blocks)
			{
				throw std::bad_alloc();
			}

			for (bcUINT32 i = 0; i < m_num_bit_blocks; ++i)
			{
				m_blocks[i].store(0U);
			}

			// We alloc m_heap by min align defined in CorePCH because we want all of our allocations have MIN_Align /
			m_heap = static_cast< bcUBYTE* >
			(
				platform::bc_mem_aligned_alloc((m_block_size * m_num_block) * sizeof(bcUBYTE), BC_MEMORY_MIN_ALIGN)
			);

			if (!m_heap)
			{
				bc_mem_aligned_free(m_blocks);
				throw std::bad_alloc();
			}

			m_tracer.initialize((m_block_size * m_num_block) * sizeof(bcUBYTE));
			if (p_tag)
			{
				tag(p_tag);
			}
		}

		void bc_memory_fixed_size::_destroy() noexcept
		{
			platform::bc_mem_aligned_free(m_blocks);
			platform::bc_mem_aligned_free(m_heap);
		}

		void bc_memory_fixed_size::_move(this_type&& p_other)
		{
			const bcUINT32 l_allocated_block = p_other.m_allocated_block.load(platform::bc_memory_order::acquire);

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

			m_allocated_block.store(l_allocated_block, platform::bc_memory_order::release);
		}
#endif
	}
}