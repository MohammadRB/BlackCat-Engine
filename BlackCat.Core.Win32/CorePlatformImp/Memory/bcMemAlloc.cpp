// [11/4/2014 MRB]

#include "coreplatform/memory/bcmemalloc.h"
#include "coreplatformimp/corewin32pch.h"

namespace black_cat
{
	namespace core_platform
	{
		BC_COREWIN32_DLL_EXP void* bc_mem_alloc(bcSIZE p_size)
		{
			return malloc(p_size);
		};

		BC_COREWIN32_DLL_EXP void* bc_mem_realloc(void* p_pointer, bcSIZE p_newsize)
		{
			return realloc(p_pointer, p_newsize);
		};

		BC_COREWIN32_DLL_EXP void bc_mem_free(void* p_pointer)
		{
			free(p_pointer);
		};

		BC_COREWIN32_DLL_EXP void* bc_mem_aligned_alloc(bcSIZE p_size, bcSIZE p_alignment)
		{
			return _aligned_malloc(p_size, p_alignment);
		};

		BC_COREWIN32_DLL_EXP void* bc_mem_aligned_realloc(void* p_pointer, bcSIZE p_newsize, bcSIZE p_alignment)
		{
			return _aligned_realloc(p_pointer, p_newsize, p_alignment);
		};

		BC_COREWIN32_DLL_EXP void bc_mem_aligned_free(void* p_pointer)
		{
			_aligned_free(p_pointer);
		};

		BC_COREWIN32_DLL_EXP void* bc_mem_page_reserve(bcSIZE p_size)
		{
			return VirtualAlloc(nullptr, p_size, MEM_RESERVE, PAGE_NOACCESS);
		};

		BC_COREWIN32_DLL_EXP void* bc_mem_page_commit(void* p_address, bcSIZE p_size)
		{
			return VirtualAlloc(p_address, p_size, MEM_COMMIT, PAGE_READWRITE);
		};

		BC_COREWIN32_DLL_EXP bool bc_mem_page_release(void* p_base_address)
		{
			return VirtualFree(p_base_address, 0, MEM_RELEASE);
		};

		BC_COREWIN32_DLL_EXP bool bc_mem_page_decommit(void* p_address, bcSIZE p_size)
		{
			return VirtualFree(p_address, p_size, MEM_DECOMMIT);
		};
	}
}

void* operator new(black_cat::bcSIZE p_size) noexcept(false)
{
	return black_cat::core_platform::bc_mem_alloc(p_size);
};

void* operator new[](black_cat::bcSIZE p_size) noexcept(false)
{
	return black_cat::core_platform::bc_mem_alloc(p_size);
};

void operator delete(void* p_pointer) noexcept(true)
{
	black_cat::core_platform::bc_mem_free(p_pointer);
};

void operator delete[](void* p_pointer) noexcept(true)
{
	black_cat::core_platform::bc_mem_free(p_pointer);
};