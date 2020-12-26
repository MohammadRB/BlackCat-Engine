// [10/24/2014 MRB]

#pragma once

#include "Core/bcExport.h"
#include "Core/Utility/bcInitializable.h"
#include "Core/Memory/bcMemory.h"
#include "Core/Memory/bcMemBlock.h"
#include "Core/Memory/bcMemoryTracer.h"

namespace black_cat
{
	namespace core
	{
#ifdef BC_MEMORY_ENABLE

		class BC_CORE_DLL bc_memory_fixed_size : public bc_memory, public bc_initializable<bcUINT32, bcUINT32, const bcCHAR*>
		{
		public:
			using this_type = bc_memory_fixed_size;
			using is_movable_type = std::false_type;

		public:
			bc_memory_fixed_size() noexcept;

			bc_memory_fixed_size(this_type&& p_other) noexcept;
			
			~bc_memory_fixed_size() noexcept;

			this_type& operator =(this_type&& p_other) noexcept;

			bcSIZE capacity() const noexcept { return block_size() * num_block(); }
			
			bcSIZE block_size() const noexcept { return m_block_size; }

			bcSIZE num_block() const noexcept { return m_num_block; }

			void* alloc(bc_memblock* p_mem_block) noexcept override;

			void free(void* p_pointer, bc_memblock* p_mem_block) noexcept override;

			bool contain_pointer(void* p_pointer) const noexcept override;

			void clear() noexcept override;

		private:
			void _initialize(bcUINT32 p_num_block, bcUINT32 p_block_size, const bcCHAR* p_tag) override;

			void _destroy() noexcept override;

			void _move(this_type&& p_other);

			using bitblock_type = bcUINT32;
			static const bitblock_type s_bitblock_mask = 0xffffffff;
			static const bcSIZE s_bitblock_size = sizeof(bitblock_type) * 8;

			bcUINT32 m_num_block;
			bcUINT32 m_block_size;
			bcUINT32 m_num_bitblocks;
			core_platform::bc_atomic< bcUINT32 > m_allocated_block;		// An index that searching for free block will continue from this place
			core_platform::bc_atomic< bitblock_type >* m_blocks;			// bit-vector indicating if a block is allocated or not
			bcUBYTE* m_heap;												// block of data
		};

#endif
	}
}