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
			if (is_initialized())
			{
				destroy();
			}
		}

		bc_concurrent_memory_pool& bc_concurrent_memory_pool::operator=(bc_concurrent_memory_pool&& p_other) noexcept
		{
			bc_initializable::operator=(std::move(p_other));
			const bcUINT32 l_allocated_block = p_other.m_allocated_block.load(core_platform::bc_memory_order::acquire);

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
			bcUINT32 l_allocated_block = m_allocated_block.load(core_platform::bc_memory_order::relaxed);

			for (bcUINT32 l_ite = l_allocated_block; (l_ite != l_allocated_block || !l_reached_end); ++l_ite)
			{
				if (l_ite >= m_num_bit_blocks)
				{
					l_ite = 0;
					l_reached_end = true;
				}
				
				auto l_current_block = m_blocks[l_ite].load(core_platform::bc_memory_order::relaxed);

				while (true)
				{
					const auto l_free_bit = _find_free_bit(l_current_block);
					if (l_free_bit == -1)
					{
						break;
					}

					const bit_block_type l_current_block_changed = l_current_block | (static_cast<bit_block_type>(1) << l_free_bit);
					if (m_blocks[l_ite].compare_exchange_strong
					(
						&l_current_block,
						l_current_block_changed,
						core_platform::bc_memory_order::relaxed,
						core_platform::bc_memory_order::relaxed
					))
					{
						l_block = l_ite * s_bit_block_size + l_free_bit;

						m_allocated_block.compare_exchange_strong(&l_allocated_block, l_ite, core_platform::bc_memory_order::relaxed);

						break;
					}
				}

				if (l_block != -1)
				{
					break;
				}
			}

			// Any free block were found
			if (l_block != -1 || l_block <= m_num_block)
			{
				l_result = reinterpret_cast<void*>(static_cast<bcUBYTE*>(m_heap) + l_block * m_block_size);
			}

			return l_result;
		}

		void bc_concurrent_memory_pool::free(void* p_pointer) noexcept
		{
			BC_ASSERT(contain_pointer(p_pointer));

			auto* l_pointer = static_cast<bcUBYTE*>(p_pointer);

			const bcUINT32 l_block = (l_pointer - m_heap) / m_block_size;
			const bcUINT32 l_chunk_index = l_block / s_bit_block_size;
			const bcUINT32 l_bit_index = l_block % s_bit_block_size;

#ifdef BC_MEMORY_DEBUG
			std::memset(l_pointer, 0, m_block_size);
#endif

			m_blocks[l_chunk_index].fetch_and(~(static_cast<bit_block_type>(1) << l_bit_index), core_platform::bc_memory_order::relaxed);
		}

		bool bc_concurrent_memory_pool::contain_pointer(const void* p_pointer) const noexcept
		{
			return p_pointer >= static_cast<bcUBYTE*>(m_heap) && p_pointer < static_cast<bcUBYTE*>(m_heap) + (m_block_size * m_num_block);
		}

		void bc_concurrent_memory_pool::clear() noexcept
		{
			for (bcUINT32 l_ite = 0; l_ite < m_num_bit_blocks; ++l_ite)
			{
				m_blocks[l_ite].store(0U, core_platform::bc_memory_order::relaxed);
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

			m_blocks = reinterpret_cast<core_platform::bc_atomic<bit_block_type>*>
			(
				BC_ALLOC(m_num_bit_blocks * sizeof(core_platform::bc_atomic<bit_block_type>), p_alloc_type)
			);
			if (!m_blocks)
			{
				throw std::bad_alloc();
			}

			m_heap = reinterpret_cast<bcUBYTE*>(BC_ALIGNED_ALLOC(m_block_size * m_num_block, BC_MEMORY_MIN_ALIGN, p_alloc_type));
			if (!m_heap)
			{
				_aligned_free(m_blocks);
				throw std::bad_alloc();
			}

			for (bcUINT32 l_ite = 0; l_ite < m_num_bit_blocks; ++l_ite)
			{
				m_blocks[l_ite].store(0U, core_platform::bc_memory_order::relaxed);
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
					const bool l_is_alive = l_current_block & (static_cast<bit_block_type>(1) << l_j);
					if (l_is_alive)
					{
						++l_num_alive_objects;
					}
				}
			}

			BC_ASSERT(l_num_alive_objects == 0);
#endif

			auto* l_blocks = m_blocks.release();
			auto* l_heap = m_heap.release();

			BC_FREE(l_blocks);
			BC_ALIGNED_FREE(l_heap);

			m_num_block = 0;
		}

		bcINT32 bc_concurrent_memory_pool::_find_free_bit(bit_block_type p_bit_block) const noexcept
		{
			// No free bit
			if (s_bit_block_mask == p_bit_block)
			{
				return -1;
			}

			// Start searching in sub blocks
			auto l_sub_block_size_bytes = sizeof(bit_block_type) / 2;
			auto l_sub_block_shift_bytes = 0U;

			while (l_sub_block_size_bytes >= 1)
			{
				for (auto l_sub_block_byte_ite = l_sub_block_shift_bytes; l_sub_block_byte_ite < sizeof(bit_block_type); l_sub_block_byte_ite += l_sub_block_size_bytes)
				{
					const auto l_sub_block_ite = (l_sub_block_byte_ite - l_sub_block_shift_bytes) / l_sub_block_size_bytes;
					const auto l_sub_block_mask = s_bit_block_mask >> ((sizeof(bit_block_type) - l_sub_block_size_bytes) * 8);
					const auto l_sub_block = (p_bit_block >> (l_sub_block_ite * l_sub_block_size_bytes * 8 + l_sub_block_shift_bytes * 8)) & l_sub_block_mask;

					// No free bit
					if ((l_sub_block & l_sub_block_mask) == l_sub_block_mask)
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
				if (static_cast<bit_block_type>(0) == (p_bit_block & (static_cast<bit_block_type>(1) << l_bit_ite)))
				{
					l_found_bit = l_bit_ite;
					break;
				}
			}

			return l_found_bit;
		}
	}
}