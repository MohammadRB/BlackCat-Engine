//  [23/8/2013 MRB]

#pragma once

#include "CorePlatformImp/Memory/bcMemAlloc.h"
#include "Core/CorePCH.h"
#include "Core/Memory/bcMemory.h"
#include "Core/Memory/bcMemBlock.h"
#include "Core/Memory/bcMemoryTracer.h"

namespace black_cat
{
	namespace core
	{
#ifdef BC_MEMORY_ENABLE

		class bc_memory_crt : public bci_memory
		{
		public:
			using this_type = bc_memory_crt;
			using is_movable_type = std::false_type;

		public:
			bc_memory_crt();

			bc_memory_crt(this_type&& p_other) noexcept;

			~bc_memory_crt();

			this_type& operator =(this_type&& p_other) noexcept;

			void initialize(const bcCHAR* p_tag);

			void destroy();

			void* alloc(bc_memblock* p_memblock) noexcept override;

			void free(void* p_pointer, bc_memblock* p_memblock) noexcept override;

			bool contain_pointer(void* p_pointer) const noexcept override;

			void clear() noexcept override;
		};

		inline bc_memory_crt::bc_memory_crt() = default;
		
		inline bc_memory_crt::bc_memory_crt(this_type&& p_other) noexcept = default;
		
		inline bc_memory_crt::~bc_memory_crt() = default;

		inline bc_memory_crt::this_type& bc_memory_crt::operator=(this_type&& p_other) noexcept
		{
			return *this;
		}

		inline void bc_memory_crt::initialize(const bcCHAR* p_tag)
		{
			m_tracer.initialize(0xffffffff);
			if (p_tag)
			{
				bci_memory::tag(p_tag);
			}
		}

		inline void bc_memory_crt::destroy()
		{
		}

		inline void* bc_memory_crt::alloc(bc_memblock* p_memblock) noexcept
		{
			void* result = platform::bc_mem_aligned_alloc(p_memblock->size(), BC_MEMORY_MIN_ALIGN);
			if (result)
			{
				m_tracer.accept_alloc(p_memblock->size());
			}
			else
			{
				m_tracer.reject_alloc(p_memblock->size());
			}

			return result;
		}

		inline void bc_memory_crt::free(void* p_pointer, bc_memblock* p_memblock) noexcept
		{
			m_tracer.accept_free(p_memblock->size());
			platform::bc_mem_aligned_free(p_pointer);
		}

		inline bool bc_memory_crt::contain_pointer(void* p_pointer) const noexcept
		{
			return true;
		}

		inline void bc_memory_crt::clear() noexcept
		{
		}
#endif
	}
}