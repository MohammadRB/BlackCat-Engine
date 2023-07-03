// [24/10/2014 MRB]

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

		class BC_CORE_DLL bc_memory_fixed_size : public bci_memory, public bc_initializable<bcUINT32, bcUINT32, const bcCHAR*>
		{
		public:
			using this_type = bc_memory_fixed_size;
			using is_movable_type = std::false_type;

		public:
			bc_memory_fixed_size() noexcept;

			bc_memory_fixed_size(this_type&& p_other) noexcept;
			
			~bc_memory_fixed_size() noexcept override;

			this_type& operator =(this_type&& p_other) noexcept;

			bcSIZE capacity() const noexcept;

			bcSIZE block_size() const noexcept;

			bcSIZE num_block() const noexcept;

			void* alloc(bc_memblock* p_mem_block) noexcept override;

			void free(void* p_pointer, bc_memblock* p_mem_block) noexcept override;

			bool contain_pointer(void* p_pointer) const noexcept override;

			void clear() noexcept override;

		private:
			void _initialize(bcUINT32 p_num_block, bcUINT32 p_block_size, const bcCHAR* p_tag) override;

			void _destroy() noexcept override;

			void _move(this_type&& p_other);

			using bit_block_type = bcUINT32;
			static constexpr bit_block_type s_bit_block_mask = 0xffffffff;
			static constexpr bcSIZE s_bit_block_size = sizeof(bit_block_type) * 8;

			bcUINT32 m_num_block;
			bcUINT32 m_block_size;
			bcUINT32 m_num_bit_blocks;
			platform::bc_atomic<bcUINT32> m_allocated_block;			// An index that searching for free block will continue from this place
			platform::bc_atomic<bit_block_type>* m_blocks;				// bit-vector indicating if a block is allocated or not
			bcUBYTE* m_heap;												// block of data
		};

		inline bcSIZE bc_memory_fixed_size::capacity() const noexcept
		{
			return block_size() * num_block();
		}

		inline bcSIZE bc_memory_fixed_size::block_size() const noexcept
		{
			return m_block_size;
		}

		inline bcSIZE bc_memory_fixed_size::num_block() const noexcept
		{
			return m_num_block;
		}

		inline bool bc_memory_fixed_size::contain_pointer(void* p_pointer) const noexcept
		{
			return p_pointer >= m_heap && p_pointer < m_heap + (m_block_size * m_num_block);
		}

#endif
	}
}