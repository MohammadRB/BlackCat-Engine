//  [8/23/2013 MRB]

#include "Core/CorePCH.h"
#include "Core/Memory/bcMemoryFixedSize.h"
#include "CorePlatformImp/Concurrency/bcAtomicProvider.h"

namespace black_cat
{
	namespace core
	{
#ifdef BC_MEMORY_ENABLE

		bc_memory_fixed_size::bc_memory_fixed_size() noexcept(true)
		{
		};

		bc_memory_fixed_size::bc_memory_fixed_size(bc_memory_fixed_size::this_type&& p_other) noexcept(true) : bc_memory(std::move(p_other))
		{
			_move(std::move(p_other));
		};

		bc_memory_fixed_size::~bc_memory_fixed_size() noexcept
		{
			destroy();
		};

		bc_memory_fixed_size::this_type& bc_memory_fixed_size::operator =(bc_memory_fixed_size::this_type&& p_other) noexcept(true)
		{
			bc_memory::operator =(std::move(p_other));
			_move(std::move(p_other));

			return *this;
		};

		void bc_memory_fixed_size::initialize(bcUINT32 p_num_block, bcUINT32 p_block_size, const bcCHAR* p_tag)
		{
			m_num_block = p_num_block;
			m_block_size = p_block_size;

			if (m_num_block % FIXEDSIZE_INDEX_SIZE == 0)
			{
				m_num_bitblocks = m_num_block / FIXEDSIZE_INDEX_SIZE;
			}
			else
			{
				m_num_block = ((m_num_block / FIXEDSIZE_INDEX_SIZE) + 1) * FIXEDSIZE_INDEX_SIZE;
				m_num_bitblocks = m_num_block / FIXEDSIZE_INDEX_SIZE;
			}
			m_allocated_block.store(0U);
			
			m_blocks = static_cast<core_platform::bc_atomic< FIXEDSIZE_INDEX_TYPE >*>
				(core_platform::bc_mem_aligned_alloc(m_num_bitblocks * sizeof(core_platform::bc_atomic< FIXEDSIZE_INDEX_TYPE >), BC_MEMORY_MIN_ALIGN));

			if (!m_blocks)
				throw std::bad_alloc();
			
			for (bcUINT32 i = 0; i < m_num_bitblocks; ++i)
				m_blocks[i].store(0U);

			// We alloc m_heap by min align defined in CorePCH because we want all of our allocations have MIN_Align /
			m_heap = static_cast<bcUBYTE*>
				(core_platform::bc_mem_aligned_alloc((m_block_size * m_num_block) * sizeof(bcUBYTE), BC_MEMORY_MIN_ALIGN));

			if (!m_heap)
			{
				_aligned_free(m_blocks);
				throw std::bad_alloc();
			}

			m_tracer.initialize((m_block_size * m_num_block) * sizeof(bcUBYTE));
			if (p_tag) bc_memory::tag(p_tag);
		};

		void bc_memory_fixed_size::destroy() noexcept(true)
		{
			core_platform::bc_mem_aligned_free(m_blocks);
			core_platform::bc_mem_aligned_free(m_heap);
		};

		void* bc_memory_fixed_size::alloc(bc_memblock* p_mem_block) noexcept(true)
		{
			bcUINT32 l_size = p_mem_block->size();

			// we can't handle allocations larger than our block size /
			bcAssert(l_size <= m_block_size);
			bcAssert(l_size > 0);

			// Search for a free block /
			bcINT32 l_block = -1;
			void* l_result = nullptr;

			bool l_reach_end = false;
			// TODO
			bcUINT32 l_allocated_block = m_allocated_block.load(core_platform::bc_memory_order::seqcst);
			for (register bcINT32 i = l_allocated_block; (i != l_allocated_block || !l_reach_end); ++i)
			{
				if (i >= m_num_bitblocks)
				{
					i = 0;
					l_reach_end = true;
				}
				FIXEDSIZE_INDEX_TYPE l_current_block = m_blocks[i].load(core_platform::bc_memory_order::seqcst);

				// any free blocks in this chunk? /
				if (FIXEDSIZE_INDEX_MASK != l_current_block)
				{
					// find a free entry in this chunk then allocate it and set the proper block index /
					for (int j = 0; j < FIXEDSIZE_INDEX_SIZE; ++j)
					{
						if (FIXEDSIZE_INDEX_TYPE(0) == (l_current_block & (FIXEDSIZE_INDEX_TYPE(1) << j)))
						{
							FIXEDSIZE_INDEX_TYPE l_current_block_changed = l_current_block | (FIXEDSIZE_INDEX_TYPE(1) << j);
							if (m_blocks[i].compare_exchange_strong(
								&l_current_block,
								l_current_block_changed,
								core_platform::bc_memory_order::seqcst,
								core_platform::bc_memory_order::seqcst))
							{
								l_block = i * FIXEDSIZE_INDEX_SIZE + j;

								m_allocated_block.compare_exchange_strong(&l_allocated_block, i, core_platform::bc_memory_order::seqcst);

								break;
							}

						}
					}

					if (l_block != -1)
						break;
				}
			}

			// A free block found /
			if (-1 != l_block || l_block <= m_num_block)
			{
				l_result = reinterpret_cast<void*>(reinterpret_cast<bcUINT32>(m_heap) + (l_block * m_block_size));
				m_tracer.accept_alloc(l_size);
			}
			else
			{
				m_tracer.reject_alloc(l_size);
			}

			return l_result;
		};

		void bc_memory_fixed_size::free(const void* p_pointer, bc_memblock* p_mem_block) noexcept(true)
		{
			bcUBYTE* l_pointer = reinterpret_cast<bcUBYTE*>(const_cast<void*>(p_pointer));

			// is this pointer in our heap? /
			bcAssert((l_pointer >= m_heap) && (l_pointer < m_heap + m_block_size * m_num_block));

			// convert the pointer into a block index /
			bcINT32 l_block = static_cast<bcINT32>(l_pointer - m_heap) / m_block_size;

			// reset the bit in our block management array /
			bcINT32 l_chunk_index = l_block / FIXEDSIZE_INDEX_SIZE;
			bcINT32 l_bit_index = l_block % FIXEDSIZE_INDEX_SIZE;

			m_blocks[l_chunk_index].fetch_add(~(FIXEDSIZE_INDEX_TYPE(1) << l_bit_index), core_platform::bc_memory_order::seqcst);

			p_mem_block->free(true, core_platform::bc_memory_order::seqcst);
#ifdef BC_MEMORY_DEBUG
			std::memset(reinterpret_cast<void*>(l_pointer), 0, p_mem_block->size() - p_mem_block->offset());
#endif

			m_tracer.accept_free(p_mem_block->size());
		};

		bool bc_memory_fixed_size::contain_pointer(const void* p_pointer) const noexcept(true)
		{
			return (p_pointer >= m_heap && p_pointer < m_heap + (m_block_size * m_num_block)) ? true : false;
		};

		void bc_memory_fixed_size::clear() noexcept(true)
		{
			std::memset(m_blocks, 0, m_num_bitblocks * sizeof(core_platform::bc_atomic< FIXEDSIZE_INDEX_TYPE >));

			m_tracer.accept_clear();
		};

#endif
	}
}