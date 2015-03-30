// [10/24/2014 MRB]

#pragma once

#include "Core/CorePCH.h"
#include "Core/Memory/bcMemBlock.h"
#include "Core/Memory/bcMemory.h"
#include "Core/bcCoreUtility.h"
#include "Core/Memory/bcMemoryTracer.h"
#include "CorePlatformImp/Concurrency/bcAtomic.h"

namespace black_cat
{
	namespace core
	{
#ifdef BC_MEMORY_ENABLE

		class BC_COREDLL_EXP bc_memory_stack : public bc_memory, bc_initializable<bcUINT32, const bcCHAR*>
		{
		public:
			using this_type = bc_memory_stack;
			using is_movable_type = std::false_type;

		public:
			bc_memory_stack() noexcept(true);

			bc_memory_stack(this_type&& p_other) noexcept(true);

			~bc_memory_stack();

			this_type& operator =(this_type&& p_other) noexcept(true);

			void initialize(bcUINT32 p_size, const bcCHAR* p_tag) override;

			void destroy() noexcept(true) override;

			bcUINT32 size() const { return m_size; };

			void* push(bc_memblock* p_mem_block) noexcept(true);

			void pop(void* p_pointer, bc_memblock* p_mem_block) noexcept(true);

			void* alloc(bc_memblock* p_mem_block) noexcept(true) override;

			void free(void* p_pointer, bc_memblock* p_mem_block) noexcept(true) override;

			bool contain_pointer(void* p_pointer) const noexcept(true) override;

			void clear() noexcept(true) override;

		protected:

		private:
			void _move(this_type&& p_other)
			{
				bcUBYTE* l_top = p_other.m_top.load(core_platform::bc_memory_order::acquire);

				m_size = p_other.m_size;
				m_heap = p_other.m_heap;

				p_other.m_size = 0;
				p_other.m_top.store(0, core_platform::bc_memory_order::relaxed);
				m_heap = nullptr;

				m_top.store(l_top, core_platform::bc_memory_order::release);
			}

			bcUINT32 m_size;
			core_platform::bc_atomic< bcUBYTE* > m_top;
			bcUBYTE* m_heap;

		};

#endif
	}
}