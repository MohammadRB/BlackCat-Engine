//  [8/23/2013 MRB]

#pragma once

#include "CorePlatform/Memory/bcMemAlloc.h"
#include "Core/CorePCH.h"
#include "Core/Memory/bcMemory.h"
#include "Core/Memory/bcMemBlock.h"
#include "Core/Memory/bcMemoryTracer.h"

namespace black_cat
{
	namespace core
	{
#ifdef BC_MEMORY_ENABLE

		class bc_memory_crt : public bc_memory
		{
		public:
			using this_type = bc_memory_crt;
			using is_movable_type = std::false_type;

		public:
			bc_memory_crt() = default;

			bc_memory_crt(this_type&& p_other) noexcept = default;

			~bc_memory_crt() = default;

			this_type& operator =(this_type&& p_other) noexcept
			{
				return *this;
			}

			void initialize(const bcCHAR* p_tag)
			{
				m_tracer.initialize(0xffffffff);
				if (p_tag) bc_memory::tag(p_tag);
			}

			void destroy() 
			{
			}

			void* alloc(bc_memblock* p_memblock) noexcept override
			{
				void* result = core_platform::bc_mem_aligned_alloc(p_memblock->size(), BC_MEMORY_MIN_ALIGN);
				if(result)
					m_tracer.accept_alloc(p_memblock->size());
				else
					m_tracer.reject_alloc(p_memblock->size());

				return result;
			}

			void free(void* p_pointer, bc_memblock* p_memblock) noexcept override
			{
				m_tracer.accept_free(p_memblock->size());
				core_platform::bc_mem_aligned_free(const_cast<void*>(p_pointer));
			}

			bool contain_pointer(void* p_pointer) const noexcept override
			{
				return true;
			}
			
			void clear() noexcept override
			{
			}

		protected:

		private:
		};

#endif
	}
}