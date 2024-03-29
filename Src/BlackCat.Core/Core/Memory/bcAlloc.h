//  [04/09/2013 MRB]

#pragma once

#include "CorePlatformImp/Memory/bcMemAlloc.h"
#include "Core/CorePCH.h"
#include "Core/Memory/bcMemoryManagment.h"

namespace black_cat::core
{
	template <typename T, bool is_poly>
	struct bc_allocated_pointer_helper
	{
		static void* cast(T* p_ptr)
		{
			return p_ptr;
		}
	};

	template <typename T>
	struct bc_allocated_pointer_helper<T, true>
	{
		static void* cast(T* p_ptr)
		{
			return p_ptr;
			return dynamic_cast<void*>(p_ptr);
		}
	};

	template<typename T>
	void* bc_get_allocated_pointer(T* p_ptr)
	{
		constexpr bool l_is_poly = std::is_polymorphic_v<T>;
		return bc_allocated_pointer_helper<T, l_is_poly>::cast(p_ptr);
	}

	inline bool is_movable_pointer(void* p_pointer) noexcept
	{
#if defined(BC_MEMORY_ENABLE) && defined(BC_MEMORY_DEFRAG)
			if (!p_pointer)
			{
				return false;
			}

			const bc_memblock* l_memblock = bc_memblock::retrieve_mem_block(p_pointer);
			return l_memblock->movable_pointer();
#else
		return false;
#endif
	}

	inline void register_movable_pointer(void** p_pointer) noexcept
	{
#if defined(BC_MEMORY_ENABLE) && defined(BC_MEMORY_DEFRAG)
			BC_ASSERT(p_pointer && *p_pointer);

			bc_memory_manager::get().register_pointer_in_movable_allocators(p_pointer);
#endif
	}
		 
	inline void unregister_movable_pointer(void** p_pointer) noexcept
	{
#if defined(BC_MEMORY_ENABLE) && defined(BC_MEMORY_DEFRAG)
			BC_ASSERT(p_pointer && *p_pointer);

			bc_memory_manager::get().unregister_pointer_in_movable_allocators(p_pointer);
#endif
	}
		 
	inline void* bc_mem_alloc(bcSIZE p_size, bc_alloc_type p_alloc_type, const bcCHAR* p_file, bcUINT32 p_line) noexcept
	{
		void* l_result;

#ifdef BC_MEMORY_ENABLE
		if (p_size == 0)
		{
			p_size = 1;
		}

		l_result = bc_memory_manager::get().alloc(p_size, p_alloc_type, p_file, p_line);
#else
			l_result = platform::bc_mem_alloc(p_size);
#endif

		return l_result;
	}
		 
	inline void bc_mem_free(void* p_pointer) noexcept
	{
#ifdef BC_MEMORY_ENABLE
		bc_memory_manager::get().free(p_pointer);
#else
			platform::bc_mem_free(p_pointer);
#endif
	}
		 
	inline void* bc_mem_realloc(void* p_pointer, bcSIZE p_new_size, bc_alloc_type p_alloc_type, const bcCHAR* p_file, bcUINT32 p_line) noexcept
	{
#ifdef BC_MEMORY_ENABLE
		return bc_memory_manager::get().realloc(p_pointer, p_new_size, p_alloc_type, p_file, p_line);
#else
			return platform::bc_mem_realloc(p_pointer, p_new_size);
#endif
	}
		 
	inline void* bc_mem_aligned_alloc(bcSIZE p_size, bcUINT8 p_alignment, bc_alloc_type p_alloc_type, const bcCHAR* p_file, bcUINT32 p_line) noexcept
	{
		void* l_result;

#ifdef BC_MEMORY_ENABLE
		if (p_size == 0)
		{
			p_size = 1;
		}

		l_result = bc_memory_manager::get().aligned_alloc(p_size, p_alignment, p_alloc_type, p_file, p_line);
#else
			l_result = platform::bc_mem_aligned_alloc(p_size, p_alignment);
#endif

		return l_result;
	}
		 
	inline void bc_mem_aligned_free(void* p_pointer) noexcept
	{
#ifdef BC_MEMORY_ENABLE
		bc_memory_manager::get().aligned_free(p_pointer);
#else
			platform::bc_mem_aligned_free(p_pointer);
#endif
	}
		 
	inline void* bc_mem_aligned_realloc(void* p_pointer, bcSIZE p_new_size, bcUINT8 p_alignment, bc_alloc_type p_alloc_type, const bcCHAR* p_file, bcUINT32 p_line) noexcept
	{
#ifdef BC_MEMORY_ENABLE
		return bc_memory_manager::get().aligned_realloc(p_pointer, p_new_size, p_alignment, p_alloc_type, p_file, p_line);
#else
			return  platform::bc_mem_aligned_realloc(p_pointer, p_new_size, p_alignment);
#endif
	}
		 
	inline void* bc_mem_alloc_throw(bcSIZE p_size, bc_alloc_type p_alloc_type, const bcCHAR* p_file, bcUINT32 p_line)
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

	inline void* bc_mem_aligned_alloc_throw(bcSIZE p_size, bcUINT8 p_alignment, bc_alloc_type p_alloc_type, const bcCHAR* p_file, bcUINT32 p_line)
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

	template<typename T, typename ...TArgs>
	T* bc_mem_new(TArgs&&... p_args, bc_alloc_type p_alloc_type, const bcCHAR* p_file, bcUINT32 p_line)
	{
		T* l_return_pointer = static_cast<T*>(bc_mem_alloc_throw(sizeof(T), p_alloc_type, p_file, p_line));
			
		new(l_return_pointer)T(std::forward<T>(p_args)...);

		return l_return_pointer;
	}

	template<typename T>
	T* bc_mem_new_array(bcUINT32 p_array_length, bc_alloc_type p_alloc_type, const bcCHAR* p_file, bcUINT32 p_line)
	{
		// alloc 4 byte more for array length, it will be used in last 4 byte of allocated block
		const bcUINT32 l_size = (sizeof(T) * p_array_length) + sizeof(bcUINT32);
		T* l_return_pointer = static_cast<T*>(bc_mem_alloc_throw(l_size, p_alloc_type, p_file, p_line));

		// store array length
		const bc_memblock* l_block = bc_memblock::retrieve_mem_block(l_return_pointer);
		*(reinterpret_cast<bcUINT32*>(reinterpret_cast<bcUINTPTR>(l_return_pointer) - l_block->offset() + l_block->size() - sizeof(bcUINT32))) = p_array_length;

		T* l_first = l_return_pointer;
		for (bcUINT32 i = 0; i < p_array_length; ++i, ++l_first)
		{
			new(l_first)T();
		}

		return l_return_pointer;
	}

	template<typename T>
	void bc_mem_delete(T* p_pointer) noexcept
	{
		if (!p_pointer)
		{
			return;
		}

		p_pointer->~T();
		bc_mem_free(bc_get_allocated_pointer(p_pointer));
	}

	template<typename T>
	void bc_mem_delete_array(T* p_pointer) noexcept
	{
		if (!p_pointer)
		{
			return;
		}

		T* l_first = p_pointer;

		// Retrieve array length
		const bc_memblock* l_block = bc_memblock::retrieve_mem_block(l_first);
		const bcUINT32 l_count = *(reinterpret_cast<bcUINT32*>(reinterpret_cast<bcUINTPTR>(p_pointer) - l_block->offset() + l_block->size() - sizeof(bcUINT32)));

		for (bcUINT32 i = 0; i < l_count; ++i, ++l_first)
		{
			l_first->~T();
		}

		bc_mem_free(static_cast<void*>(p_pointer));
	}

	template<typename T, typename ...TArgs>
	T* bc_mem_aligned_new(TArgs&&... p_args, bcUINT8 p_alignment, bc_alloc_type p_alloc_type, const bcCHAR* p_file, bcUINT32 p_line)
	{
		T* l_return_pointer = static_cast<T*>(bc_mem_aligned_alloc_throw(sizeof(T), p_alignment, p_alloc_type, p_file, p_line));

		new(l_return_pointer)T(std::forward<T>(p_args)...);
			
		return l_return_pointer;
	}

	template<typename T>
	T* bc_mem_aligned_new_array(bcUINT32 p_array_length, bcUINT8 p_alignment, bc_alloc_type p_alloc_type, const bcCHAR* p_file, bcUINT32 p_line)
	{
		// alloc 4 byte more for array length, it will be used in last 4 byte of allocated block
		const bcUINT32 l_size = sizeof(T)* p_array_length + sizeof(bcUINT32);
		T* l_return_pointer = static_cast<T*>(bc_mem_aligned_alloc_throw(l_size, p_alignment, p_alloc_type, p_file, p_line));

		// Store array length
		const bc_memblock* l_block = bc_memblock::retrieve_mem_block(l_return_pointer);
		*(reinterpret_cast<bcUINT32*>((reinterpret_cast<bcUINTPTR>(l_return_pointer) - l_block->offset() + l_block->size() - sizeof(bcUINT32)))) = p_array_length;

		T* l_first = l_return_pointer;
		for (bcUINT32 i = 0; i < p_array_length; ++i, ++l_first)
		{
			new(l_first)T();
		}

		return l_return_pointer;
	}

	template<typename T>
	void bc_mem_aligned_delete(T* p_pointer) noexcept
	{
		if (!p_pointer)
		{
			return;
		}

		p_pointer->~T();
		bc_mem_aligned_free(bc_get_allocated_pointer(p_pointer));
	}

	template<typename T>
	void bc_mem_aligned_delete_array(T* p_pointer) noexcept
	{
		if (!p_pointer)
		{
			return;
		}

		T* l_first = p_pointer;

		// Retrieve array length
		const bc_memblock* l_block = bc_memblock::retrieve_mem_block(l_first);
		const bcUINT32 l_count = *(reinterpret_cast<bcUINT32*>(reinterpret_cast<bcUINTPTR>(p_pointer) - l_block->offset() + l_block->size() - sizeof(bcUINT32)));

		for (bcUINT32 i = 0; i < l_count; ++i, ++l_first)
		{
			l_first->~T();
		}

		bc_mem_aligned_free(p_pointer);
	}

#define BC_ALLOC(p_size, p_alloc_type)											black_cat::core::bc_mem_alloc(p_size, p_alloc_type, __FILE__, __LINE__)
#define BC_FREE(p_pointer)														black_cat::core::bc_mem_free(p_pointer); (p_pointer) = nullptr
#define BC_REALLOC(p_pointer, p_new_size, p_alloc_type)							black_cat::core::bc_mem_realloc(p_pointer, p_new_size, p_alloc_type, __FILE__, __LINE__)
#define BC_ALIGNED_ALLOC(p_size, p_alignment, p_alloc_type)						black_cat::core::bc_mem_aligned_alloc(p_size, p_alignment, p_alloc_type, __FILE__, __LINE__)
#define BC_ALIGNED_FREE(p_pointer)												black_cat::core::bc_mem_aligned_free(p_pointer); (p_pointer) = nullptr
#define BC_ALIGNED_REALLOC(p_pointer, p_new_size, p_alignment, p_alloc_type)	black_cat::core::bc_mem_aligned_realloc(p_pointer, p_new_size, p_alignment, p_alloc_type, __FILE__, __LINE__)
#define BC_ALLOC_THROW(p_size, p_alloc_type)									black_cat::core::bc_mem_alloc_throw(p_size, p_alloc_type, __FILE__, __LINE__)
#define BC_ALIGNED_ALLOC_THROW(p_size, p_alignment, p_alloc_type)				black_cat::core::bc_mem_aligned_alloc_throw(p_size, p_alignment, p_alloc_type, __FILE__, __LINE__)

#define BC_NEW(p_type, p_alloc_type)											new (p_alloc_type, __FILE__, __LINE__) p_type
#define BC_NEW_ARRAY(p_type, p_length, p_alloc_type)							black_cat::core::bc_mem_new_array<p_type>(p_length, p_alloc_type, __FILE__, __LINE__)
#define BC_DELETE(p_pointer)													black_cat::core::bc_mem_delete(p_pointer); (p_pointer) = nullptr
#define BC_DELETE_ARRAY(p_pointer)												black_cat::core::bc_mem_delete_array(p_pointer); (p_pointer) = nullptr
#define BC_ALIGNED_NEW(p_type, p_alignment, p_alloc_type)						new (p_alignment, p_alloc_type, __FILE__, __LINE__) p_type
#define BC_ALIGNED_NEW_ARRAY(p_alignment, p_length, p_alloc_type)				black_cat::core::bc_mem_aligned_new_array<p_type>(p_length, p_alignment, p_alloc_type, __FILE__, __LINE__)
#define BC_ALIGNED_DELETE(p_pointer)											black_cat::core::bc_mem_aligned_delete(p_pointer); (p_pointer) = nullptr
#define BC_ALIGNED_DELETE_ARRAY(p_pointer)										black_cat::core::bc_mem_aligned_delete_array(p_pointer); (p_pointer) = nullptr
}

inline void* operator new(black_cat::bcSIZE p_size, black_cat::core::bc_alloc_type p_alloc_type, const black_cat::bcCHAR* p_file, black_cat::bcUINT32 p_line) noexcept(false)
{
	return black_cat::core::bc_mem_alloc_throw(p_size, p_alloc_type, p_file, p_line);
}

inline void* operator new[](black_cat::bcSIZE p_size, black_cat::core::bc_alloc_type p_alloc_type, const black_cat::bcCHAR* p_file, black_cat::bcUINT32 p_line) noexcept(false)
{
	return black_cat::core::bc_mem_alloc_throw(p_size, p_alloc_type, p_file, p_line);
}

inline void* operator new(black_cat::bcSIZE p_size, black_cat::bcUINT8 p_alignment, black_cat::core::bc_alloc_type p_alloc_type, const black_cat::bcCHAR* p_file, black_cat::bcUINT32 p_line) noexcept(false)
{
	return black_cat::core::bc_mem_aligned_alloc_throw(p_size, p_alignment, p_alloc_type, p_file, p_line);
}

inline void* operator new[](black_cat::bcSIZE p_size, black_cat::bcUINT8 p_alignment, black_cat::core::bc_alloc_type p_alloc_type, const black_cat::bcCHAR* p_file, black_cat::bcUINT32 p_line) noexcept(false)
{
	return black_cat::core::bc_mem_aligned_alloc_throw(p_size, p_alignment, p_alloc_type, p_file, p_line);
}

inline void operator delete(void* p_pointer, black_cat::core::bc_alloc_type p_alloc_type, const black_cat::bcCHAR* p_file, black_cat::bcUINT32 p_line) noexcept
{
	black_cat::core::bc_mem_free(p_pointer);
}

inline void operator delete[](void* p_pointer, black_cat::core::bc_alloc_type p_alloc_type, const black_cat::bcCHAR* p_file, black_cat::bcUINT32 p_line) noexcept
{
	black_cat::core::bc_mem_free(p_pointer);
}

inline void operator delete(void* p_pointer, black_cat::bcUINT8 p_alignment, black_cat::core::bc_alloc_type p_alloc_type, const black_cat::bcCHAR* p_file, black_cat::bcUINT32 p_line) noexcept
{
	black_cat::core::bc_mem_aligned_free(p_pointer);
}

inline void operator delete[](void* p_pointer, black_cat::bcUINT8 p_alignment, black_cat::core::bc_alloc_type p_alloc_type, const black_cat::bcCHAR* p_file, black_cat::bcUINT32 p_line) noexcept
{
	black_cat::core::bc_mem_aligned_free(p_pointer);
}