// [10/24/2014 MRB]

#pragma once

#include "Core/CorePCH.h"
#include "Core/bcExport.h"
#include "Core/Memory/bcMemBlock.h"
#include "Core/Memory/bcMemory.h"
#include "Core/Utility/bcInitializable.h"
#include "Core/Memory/bcMemoryTracer.h"
#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "CorePlatformImp/Concurrency/bcMutex.h"

namespace black_cat
{
	namespace core
	{
#ifdef BC_MEMORY_ENABLE

		struct _bc_memory_stack_block
		{
		public:
			_bc_memory_stack_block(void* p_address, bcSIZE p_size)
			{
				m_address = p_address;
				m_size = p_size;
			}

			void* m_address;
			bcSIZE m_size;
		protected:

		private:
		};

		class BC_CORE_DLL bc_memory_stack : public bc_memory, public bc_initializable<bcUINT32, const bcCHAR*>
		{
		public:
			using this_type = bc_memory_stack;
			using is_movable_type = std::false_type;

		public:
			bc_memory_stack() noexcept;

			bc_memory_stack(this_type&& p_other) noexcept;

			~bc_memory_stack() noexcept;

			this_type& operator =(this_type&& p_other) noexcept;

			bcUINT32 size() const noexcept { return m_size; }

			void* push(bc_memblock* p_mem_block) noexcept;

			bool pop(void* p_pointer, bc_memblock* p_mem_block) noexcept;

			void* alloc(bc_memblock* p_mem_block) noexcept override;
			
			void free(void* p_pointer, bc_memblock* p_mem_block) noexcept override;

			bool contain_pointer(void* p_pointer) const noexcept override;

			void clear() noexcept override;

		protected:

		private:
			void _initialize(bcUINT32 p_size, const bcCHAR* p_tag) override;

			void _destroy() noexcept(true) override;

			void _add_free_block(void* p_pointer, bc_memblock* p_memblock);

			void _reclaim_free_blocks();

			void _move(this_type&& p_other);

			bcUINT32 m_size;
			bcUBYTE* m_heap;
			core_platform::bc_atomic< bcUBYTE* > m_top;
			core_platform::bc_atomic< bcUINT32 > m_pop_thread_count;
			core_platform::bc_atomic< bcUINT32 > m_free_block_count;
			core_platform::bc_shared_mutex m_free_block_mutex;
		};

#endif
	}
}