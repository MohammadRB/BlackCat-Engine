// [10/24/2014 MRB]

#pragma once

#include "Core/CorePCH.h"
#include "Core/bcCoreUtility.h"
#include "Core/Memory/bcMemory.h"
#include "Core/Memory/bcMemBlock.h"
#include "Core/Memory/bcMemoryTracer.h"

namespace black_cat
{
	namespace core
	{
#ifdef BC_MEMORY_ENABLE

		class BC_COREDLL_EXP bc_memory_fixed_size : public bc_memory
		{
		public:
			using this_type = bc_memory_fixed_size;
			using is_movable_type = std::false_type;

		public:
			bc_memory_fixed_size() noexcept(true);

			bc_memory_fixed_size(this_type&& p_other) noexcept(true);
			
			~bc_memory_fixed_size();

			this_type& operator =(this_type&& p_other) noexcept(true);

			void initialize(bcUINT32 p_num_block, bcUINT32 p_block_size, const bcCHAR* p_tag);

			void destroy() noexcept(true);

			bcUINT32 block_size() const { return m_block_size; };

			bcUINT32 num_block() const { return m_num_block; };

			void* alloc(bc_memblock* p_mem_block) noexcept(true) override;

			void free(const void* p_pointer, bc_memblock* p_mem_block) noexcept(true) override;

			bool contain_pointer(const void* p_pointer) const noexcept(true) override;

			void clear() noexcept(true) override;

		protected:

		private:
			bcInline void _move(this_type&& p_other)
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

#define FIXEDSIZE_INDEX_TYPE bcUINT32
#define FIXEDSIZE_INDEX_MASK 0xffffffff
#define FIXEDSIZE_INDEX_SIZE 32

			bcUINT32 m_num_block;
			bcUINT32 m_block_size;
			bcUINT32 m_num_bitblocks;
			// An index that searching for free block will continue from this place /
			core_platform::bc_atomic< bcUINT32 > m_allocated_block;
			// bit-vector indicating if a block is allocated or not /
			core_platform::bc_atomic< FIXEDSIZE_INDEX_TYPE >* m_blocks;
			// block of data /
			bcUBYTE* m_heap;
		};

#endif
	}
}