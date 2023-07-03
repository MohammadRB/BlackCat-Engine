// [4/11/2014 MRB]

#pragma once

#include "CorePlatform/Memory/bcMemAlloc.h"
#include "CorePlatformImp/CorePlatformImpPCH.h"

namespace black_cat
{
	namespace platform
	{
		inline void* bc_mem_alloc(bcSIZE p_size) noexcept
		{
			return malloc(p_size);
		}

		inline void* bc_mem_realloc(void* p_pointer, bcSIZE p_newsize) noexcept
		{
			return realloc(p_pointer, p_newsize);
		}

		inline void bc_mem_free(void* p_pointer) noexcept
		{
			free(p_pointer);
		}

		inline void* bc_mem_aligned_alloc(bcSIZE p_size, bcSIZE p_alignment) noexcept
		{
			return _aligned_malloc(p_size, p_alignment);
		}

		inline void* bc_mem_aligned_realloc(void* p_pointer, bcSIZE p_newsize, bcSIZE p_alignment) noexcept
		{
			return _aligned_realloc(p_pointer, p_newsize, p_alignment);
		}

		inline void bc_mem_aligned_free(void* p_pointer) noexcept
		{
			_aligned_free(p_pointer);
		}

		inline void* bc_mem_page_reserve(bcSIZE p_size) noexcept
		{
			return VirtualAlloc(nullptr, p_size, MEM_RESERVE, PAGE_NOACCESS);
		}

		inline void* bc_mem_page_commit(void* p_address, bcSIZE p_size) noexcept
		{
			return VirtualAlloc(p_address, p_size, MEM_COMMIT, PAGE_READWRITE);
		}

		inline bool bc_mem_page_release(void* p_base_address) noexcept
		{
			return VirtualFree(p_base_address, 0, MEM_RELEASE);
		}

		inline bool bc_mem_page_decommit(void* p_address, bcSIZE p_size) noexcept
		{
			return VirtualFree(p_address, p_size, MEM_DECOMMIT);
		}
	}
}

inline void* operator new(black_cat::bcSIZE p_size) noexcept
{
	return black_cat::platform::bc_mem_alloc(p_size);
}

inline void* operator new[](black_cat::bcSIZE p_size) noexcept
{
	return black_cat::platform::bc_mem_alloc(p_size);
}

inline void operator delete(void* p_pointer) noexcept
{
	black_cat::platform::bc_mem_free(p_pointer);
}

inline void operator delete[](void* p_pointer) noexcept
{
	black_cat::platform::bc_mem_free(p_pointer);
}