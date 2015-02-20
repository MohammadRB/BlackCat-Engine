//  [9/4/2013 MRB]

#pragma once

#include "CorePlatform/Memory/bcMemAlloc.h"
#include "Core/CorePCH.h"
#include "Core/Memory/bcMemoryManagment.h"

namespace black_cat
{
	namespace core
	{
		bcInline void register_movable_pointer(void** p_pointer) noexcept(true)
		{
#if defined(BC_MEMORY_ENABLE) && defined(BC_MEMORY_DEFRAG)
			bc_memmng::get().register_pointer_in_movable_allocators(p_pointer);
#endif
		}

		bcInline void unregister_movable_pointer(void** p_pointer) noexcept(true)
		{
#if defined(BC_MEMORY_ENABLE) && defined(BC_MEMORY_DEFRAG)
			bc_memmng::get().unregister_pointer_in_movable_allocators(p_pointer);
#endif
		}

		bcInline void* bc_mem_alloc(bcSIZE p_size, bc_alloc_type p_alloc_type, const bcCHAR* p_file, bcUINT32 p_line) noexcept(true)
		{
			void* l_result = nullptr;

#ifdef BC_MEMORY_ENABLE
			if (p_size == 0) p_size = 1;

			l_result = bc_memmng::get().alloc(p_size, p_alloc_type, p_file, p_line);
#else
			l_result = core_platform::bc_mem_alloc(p_size);
#endif

			return l_result;
		};

		bcInline void bc_mem_free(void* p_pointer) noexcept(true)
		{
			if (!p_pointer) return;

#ifdef BC_MEMORY_ENABLE
			bc_memmng::get().free(p_pointer);
#else
			core_platform::bc_mem_free(p_pointer);
#endif
		};

		bcInline void* bc_mem_realloc(void* p_pointer, bcSIZE p_new_size, bc_alloc_type p_alloc_type, const bcCHAR* p_file, bcUINT32 p_line) noexcept(true)
		{
#ifdef BC_MEMORY_ENABLE
			return bc_memmng::get().realloc(p_pointer, p_new_size, p_alloc_type, p_file, p_line);
#else
			return core_platform::bc_mem_realloc(p_pointer, p_new_size);
#endif
		};

		bcInline void* bc_mem_aligned_alloc(bcSIZE p_size, bcSIZE p_alignment, bc_alloc_type p_alloc_type, const bcCHAR* p_file, bcUINT32 p_line) noexcept(true)
		{
			void* l_result = nullptr;

#ifdef BC_MEMORY_ENABLE
			if (p_size == 0) p_size = 1;

			l_result = bc_memmng::get().aligned_alloc(p_size, p_alignment, p_alloc_type, p_file, p_line);
#else
			l_result = core_platform::bc_mem_aligned_alloc(p_size, p_alignment);
#endif

			return l_result;
		};

		bcInline void bc_mem_aligned_free(void* p_pointer) noexcept(true)
		{
			if (!p_pointer) return;

#ifdef BC_MEMORY_ENABLE
			bc_memmng::get().aligned_free(p_pointer);
#else
			core_platform::bc_mem_aligned_free(p_pointer);
#endif
		};

		bcInline void* bc_mem_aligned_realloc(void* p_pointer, bcSIZE p_new_size, bcSIZE p_alignment, bc_alloc_type p_alloc_type, const bcCHAR* p_file, bcUINT32 p_line) noexcept(true)
		{
#ifdef BC_MEMORY_ENABLE
			return bc_memmng::get().aligned_realloc(p_pointer, p_new_size, p_alignment, p_alloc_type, p_file, p_line);
#else
			return  core_platform::bc_mem_aligned_realloc(p_pointer, p_new_size, p_alignment);
#endif
		};

		bcInline void* bc_mem_alloc_throw(bcSIZE p_size, bc_alloc_type p_alloc_type, const bcCHAR* p_file, bcUINT32 p_line)
		{
			void* l_return_pointer;

			while (true)
			{
				l_return_pointer = bc_mem_alloc(p_size, p_alloc_type, p_file, p_line);

				if (!l_return_pointer)
				{
					std::new_handler l_handler = std::get_new_handler();
					if (l_handler)
					{
						(*l_handler)();
						continue;
					}

					throw std::bad_alloc();
				}

				break;
			}

			return l_return_pointer;
		}

		bcInline void* bc_mem_aligned_alloc_throw(bcSIZE p_size, bcSIZE p_alignment, bc_alloc_type p_alloc_type, const bcCHAR* p_file, bcUINT32 p_line)
		{
			void* l_return_pointer;

			while (true)
			{
				l_return_pointer = bc_mem_aligned_alloc(p_size, p_alignment, p_alloc_type, p_file, p_line);

				if (!l_return_pointer)
				{
					std::new_handler l_handler = std::get_new_handler();
					if (l_handler)
					{
						(*l_handler)();
						continue;
					}

					throw std::bad_alloc();
				}

				break;
			}

			return l_return_pointer;
		}

#define bcAlloc(p_size, p_alloc_type)										black_cat::core::bc_mem_alloc(p_size, p_alloc_type, __FILE__, __LINE__)
#define bcFree(p_pointer)													black_cat::core::bc_mem_free(p_pointer); p_pointer = nullptr
#define bcReAlloc(p_pointer, p_new_size, p_alloc_type)						black_cat::core::bc_mem_realloc(p_pointer, p_new_size, p_alloc_type, __FILE__, __LINE__)
#define bcAlignedAlloc(p_size, p_alignment, p_alloc_type)					black_cat::core::bc_mem_aligned_alloc(p_size, p_alignment, p_alloc_type, __FILE__, __LINE__)
#define bcAlignedFree(p_pointer)											black_cat::core::bc_mem_aligned_free(p_pointer); p_pointer = nullptr
#define bcAlignedReAlloc(p_pointer, p_new_size, p_alignment, p_alloc_type)	black_cat::core::bc_mem_aligned_realloc(p_pointer, p_new_size, p_alignment, p_alloc_type, __FILE__, __LINE__)
#define bcAllocThrow(p_size, p_alloc_type)									black_cat::core::bc_mem_alloc_throw(p_size, p_alloc_type, __FILE__, __LINE__)
#define bcAlignedAllocThrow(p_size, p_alignment, p_alloc_type)				black_cat::core::bc_mem_aligned_alloc_throw(p_size, p_alignment, p_alloc_type, __FILE__, __LINE__)

		//TODO check array allocation
#define bcNew(p_type, p_alloc_type)											new (p_alloc_type, __FILE__, __LINE__) p_type
#define bcNewArray(p_type, p_length, p_alloc_type)							bc_memmng::get().new_array<p_type>(p_length, p_alloc_type, __FILE__, __LINE__)
#define bcDelete(p_t)														bc_memmng::get().delete_(p_t); p_t = nullptr
#define bcDeleteArray(p_t)													bc_memmng::get().delete_array(p_t); p_t = nullptr
#define bcAlignedNew(p_type, p_alignment, p_alloc_type)						new (pAlignment, p_alloc_type, __FILE__, __LINE__) p_type
#define bcAlignedNewArray(p_alignment, p_length, p_alloc_type)				bc_memmng::get().aligned_new_array<p_type>(p_length, p_alignment, p_alloc_type, __FILE__, __LINE__)
#define bcAlignedDelete(p_t)												bc_memmng::get().aligned_delete(p_t); p_t = nullptr
#define bcAlignedDeleteArray(p_t)											bc_memmng::get().aligned_delete_array(p_t); p_t = nullptr
	}
}

bcInline void* operator new(black_cat::bcSIZE p_size, black_cat::core::bc_alloc_type p_alloc_type, const black_cat::bcCHAR* p_file, black_cat::bcUINT32 p_line) noexcept(false)
{
	return black_cat::core::bc_mem_alloc_throw(p_size, p_alloc_type, p_file, p_line);
};

bcInline void* operator new[](black_cat::bcSIZE p_size, black_cat::core::bc_alloc_type p_alloc_type, const black_cat::bcCHAR* p_file, black_cat::bcUINT32 p_line) noexcept(false)
{
	return black_cat::core::bc_mem_alloc_throw(p_size, p_alloc_type, p_file, p_line);
};

bcInline void* operator new(black_cat::bcSIZE p_size, black_cat::bcSIZE p_alignment, black_cat::core::bc_alloc_type p_alloc_type, const black_cat::bcCHAR* p_file, black_cat::bcUINT32 p_line) noexcept(false)
{
	return black_cat::core::bc_mem_aligned_alloc_throw(p_size, p_alignment, p_alloc_type, p_file, p_line);
};

bcInline void* operator new[](black_cat::bcSIZE p_size, black_cat::bcSIZE p_alignment, black_cat::core::bc_alloc_type p_alloc_type, const black_cat::bcCHAR* p_file, black_cat::bcUINT32 p_line) noexcept(false)
{
	return black_cat::core::bc_mem_aligned_alloc_throw(p_size, p_alignment, p_alloc_type, p_file, p_line);
};

bcInline void operator delete(void* p_pointer, black_cat::core::bc_alloc_type p_alloc_type, const black_cat::bcCHAR* p_file, black_cat::bcUINT32 p_line) noexcept(true)
{
	black_cat::core::bc_mem_free(p_pointer);
};

bcInline void operator delete[](void* p_pointer, black_cat::core::bc_alloc_type p_alloc_type, const black_cat::bcCHAR* p_file, black_cat::bcUINT32 p_line) noexcept(true)
{
	black_cat::core::bc_mem_free(p_pointer);
};

bcInline void operator delete(void* p_pointer, black_cat::bcSIZE p_alignment, black_cat::core::bc_alloc_type p_alloc_type, const black_cat::bcCHAR* p_file, black_cat::bcUINT32 p_line) noexcept(true)
{
	black_cat::core::bc_mem_aligned_free(p_pointer);
};

bcInline void operator delete[](void* p_pointer, black_cat::bcSIZE p_alignment, black_cat::core::bc_alloc_type p_alloc_type, const black_cat::bcCHAR* p_file, black_cat::bcUINT32 p_line) noexcept(true)
{
	black_cat::core::bc_mem_aligned_free(p_pointer);
};

//bcInline void* operator new(black_cat::bcSIZE p_size) noexcept(false)
//{
//	return black_cat::core::bc_mem_alloc_throw(p_size, black_cat::core::bc_alloc_type::unknown, "unknown", 0);
//};
//
//bcInline void* operator new[](black_cat::bcSIZE p_size) noexcept(false)
//{
//	return black_cat::core::bc_mem_alloc_throw(p_size, black_cat::core::bc_alloc_type::unknown, "unknown", 0);
//};
//
//bcInline void operator delete(void* p_pointer) noexcept(true)
//{
//	black_cat::core::bc_mem_free(p_pointer);
//};
//
//bcInline void operator delete[](void* p_pointer) noexcept(true)
//{
//	black_cat::core::bc_mem_free(p_pointer);
//};