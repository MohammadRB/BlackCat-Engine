// [03/01/2022 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "Core/Memory/bcMemBlock.h"
#include "Core/Memory/bcMemory.h"
#include "Core/Utility/bcInitializable.h"
#include "Core/bcExport.h"

namespace black_cat
{
	namespace core
	{
#ifdef BC_MEMORY_ENABLE

		class _bc_memory_stack_block1
		{
		public:
			_bc_memory_stack_block1(bcUINT32 p_size)
				: m_size(p_size << 1)
			{
			}

			bool get_free() const noexcept
			{
				return m_size & 0b1;
			}

			void set_free() noexcept
			{
				m_size |= 0b1;
			}

			bcUINT32 get_size() const noexcept
			{
				return m_size >> 1;
			}

		private:
			bcUINT32 m_size;
			[[maybe_unused]]
			bcBYTE m_padding[BC_MEMORY_MIN_ALIGN - 4];
		};

		class BC_CORE_DLL bc_memory_stack1 : public bci_memory, public bc_initializable<bcSIZE, const bcCHAR*>
		{
		public:
			bc_memory_stack1() noexcept;

			bc_memory_stack1(bc_memory_stack1&& p_other) noexcept;

			~bc_memory_stack1() override;

			bc_memory_stack1& operator=(bc_memory_stack1&& p_other) noexcept;

			bcSIZE capacity() const noexcept;

			void* alloc(bc_memblock* p_mem_block) noexcept override;

			void free(void* p_pointer, bc_memblock* p_mem_block) noexcept override;

			bool contain_pointer(void* p_pointer) const noexcept override;

			void clear() noexcept override;

		private:
			void _initialize(bcSIZE p_capacity, const bcCHAR* p_tag) override;

			void _destroy() override;

			void _reclaim_free_blocks();

			bcSIZE m_capacity;
			bcUBYTE* m_heap;
			bcUBYTE* m_top;
			platform::bc_spin_mutex m_mutex;
		};

		inline bcSIZE bc_memory_stack1::capacity() const noexcept
		{
			return m_capacity;
		}

		inline bool bc_memory_stack1::contain_pointer(void* p_pointer) const noexcept
		{
			return p_pointer >= m_heap && p_pointer < m_heap + m_capacity;
		}

#endif
	}
}
