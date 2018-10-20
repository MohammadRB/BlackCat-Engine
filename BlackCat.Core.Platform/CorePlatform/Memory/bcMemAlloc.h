// [11/3/2014 MRB]

#pragma once

#include "CorePlatform/bcType.h"

namespace black_cat
{
	namespace core_platform
	{
		void* bc_mem_alloc(bcSIZE p_size);

		void* bc_mem_realloc(void* p_pointer, bcSIZE p_newsize);

		void bc_mem_free(void* p_pointer);

		void* bc_mem_aligned_alloc(bcSIZE p_size, bcSIZE p_alignment);

		void* bc_mem_aligned_realloc(void* p_pointer, bcSIZE p_newsize, bcSIZE p_alignment);

		void bc_mem_aligned_free(void* p_pointer);

		void* bc_mem_page_reserve(bcSIZE p_size);

		void* bc_mem_page_commit(void* p_address, bcSIZE p_size);

		bool bc_mem_page_release(void* p_base_address);

		bool bc_mem_page_decommit(void* p_address, bcSIZE p_size);
	}
}

void* operator new(black_cat::bcSIZE p_size) noexcept(false);

void* operator new[](black_cat::bcSIZE p_size) noexcept(false);

void operator delete(void* p_pointer) noexcept(true);

void operator delete[](void* p_pointer) noexcept(true);