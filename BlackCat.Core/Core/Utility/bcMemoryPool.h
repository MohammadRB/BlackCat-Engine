// [2/11/2015 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcAtomicProvider.h"
#include "Core/CorePCH.h"
#include "Core/Memory/bcAlloc.h"
#include "Core/Container/bcBitVector.h"

namespace black_cat
{
	namespace core
	{
		class BC_COREDLL_EXP bc_concurrent_memory_pool
		{
		public:
			bc_concurrent_memory_pool() = default;

			bc_concurrent_memory_pool(bc_concurrent_memory_pool&& p_other) noexcept(true)
			{
				_move(std::move(p_other));
			}

			~bc_concurrent_memory_pool()
			{
				
			}

			bc_concurrent_memory_pool& operator =(bc_concurrent_memory_pool&& p_other) noexcept(true)
			{
				_move(std::move(p_other));

				return *this;
			}

			void initialize(bcUINT32 p_num_block, bcUINT32 p_block_size, bc_alloc_type p_alloc_type);

			void destroy() noexcept(true);

			bcUINT32 block_size() const noexcept(true) { return m_block_size; };

			bcUINT32 num_block() const noexcept(true) { return m_num_block; };

			void* heap() const noexcept(true) { return m_heap; }

			// Return null pointer in case allocation failure
			void* alloc() noexcept(true);

			void free(void* p_pointer) noexcept(true);

			bool contain_pointer(const void* p_pointer) const noexcept(true);

			void clear() noexcept(true);

			bc_bit_vector get_state(bc_alloc_type p_alloc_type = bc_alloc_type::unknown) const;

		protected:
			using bitblock_type = bcUINT32;
			static const bitblock_type s_bitblock_mask = 0xffffffff;
			static const bcSIZE s_bitblock_size = sizeof(bitblock_type);

			bcUINT32 m_num_block;
			bcUINT32 m_block_size;
			bcUINT32 m_num_bitblocks;
			// An index that searching for free block will continue from this place /
			core_platform::bc_atomic< bcUINT32 > m_allocated_block;
			// bit-vector indicating if a block is allocated or not /
			core_platform::bc_atomic< bitblock_type >* m_blocks;
			// block of data /
			bcUBYTE* m_heap;

		private:
			void _move(bc_concurrent_memory_pool&& p_other)
			{
				bcUINT32 l_allocated_block = p_other.m_allocated_block.load(core_platform::bc_memory_order::acquire);

				m_num_block = p_other.m_num_block;
				m_block_size = p_other.m_block_size;
				m_num_bitblocks = p_other.m_num_bitblocks;
				m_blocks = p_other.m_blocks;
				m_heap = p_other.m_heap;

				p_other.m_num_block = 0;
				p_other.m_block_size = 0;
				p_other.m_num_bitblocks = 0;
				p_other.m_blocks = nullptr;
				p_other.m_heap = nullptr;

				m_allocated_block.store(l_allocated_block, core_platform::bc_memory_order::release);
			}

		};
	}
}