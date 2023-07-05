// [03/01/2019 MRB]

#pragma once

#include "Core/Utility/bcObjectPool.h"
#include "Core/bcException.h"

namespace black_cat::core
{
	template<typename T>
	class bc_object_pool_allocator
	{
		template<typename TOther>
		friend class bc_object_pool_allocator;

	public:
		using this_type = bc_object_pool_allocator<T>;
		using value_type = T;
		using pointer = value_type*;
		using const_pointer = const value_type*;
		using reference = value_type & ;
		using const_reference = const value_type&;
		using difference_type = bcPTRDIFF;
		using size_type = bcSIZE;
		using propagate_on_container_copy_assignment = std::true_type;
		using propagate_on_container_move_assignment = std::true_type;
		using propagate_on_container_swap = std::true_type;
		using is_movable_type = std::false_type;

		template<typename TOther>
		struct rebind
		{
			using other = bc_object_pool_allocator<TOther>;
		};

	public:
		bc_object_pool_allocator() noexcept
			: m_memory_pool(nullptr)
		{
		}

		bc_object_pool_allocator(bc_concurrent_memory_pool& p_memory_pool) noexcept
			: m_memory_pool(&p_memory_pool)
		{
		}

		bc_object_pool_allocator(const this_type& p_other) noexcept
			: m_memory_pool(p_other.m_memory_pool)
		{
		}

		template< typename TOther >
		bc_object_pool_allocator(const bc_object_pool_allocator<TOther>& p_other) noexcept
			: m_memory_pool(p_other.m_memory_pool)
		{
		}

		bc_object_pool_allocator(this_type&& p_other) noexcept
			: m_memory_pool(p_other.m_memory_pool)
		{
			p_other.m_memory_pool = nullptr;
		}

		~bc_object_pool_allocator() = default;

		this_type& operator=(const this_type& p_other) noexcept
		{
			m_memory_pool = p_other.m_memory_pool;
			return *this;
		}

		template<typename TOther>
		this_type& operator=(const bc_object_pool_allocator<TOther>& p_other) noexcept
		{
			m_memory_pool = p_other.m_memory_pool;
			return *this;
		}

		this_type& operator =(this_type&& p_other) noexcept
		{
			m_memory_pool = p_other.m_memory_pool;
			p_other.m_memory_pool = nullptr;
			return *this;
		}

		pointer address(reference p_object) const noexcept
		{
			return std::addressof(p_object);
		}

		const_pointer address(const_reference p_object) const noexcept
		{
			return std::addressof(p_object);
		}

		pointer allocate(size_type p_count, const void* p_hint = nullptr)
		{
			BC_ASSERT(p_count == 1);
			BC_ASSERT(m_memory_pool != nullptr);
			BC_ASSERT(m_memory_pool->entry_size() >= sizeof(value_type));
				
			auto l_pointer = static_cast<pointer>(m_memory_pool->alloc());
			if (!l_pointer)
			{
				l_pointer = static_cast<pointer>(BC_ALLOC_THROW(sizeof(value_type), bc_alloc_type::unknown));
			}

			return l_pointer;
		}

		void deallocate(pointer p_pointer, size_type p_count = 1)
		{
			BC_ASSERT(p_count == 1);
			BC_ASSERT(m_memory_pool != nullptr);
			BC_ASSERT(m_memory_pool->entry_size() >= sizeof(value_type));
				
			if(m_memory_pool->contain_pointer(p_pointer))
			{
				m_memory_pool->free(p_pointer);
			}
			else
			{
				BC_FREE(p_pointer);
			}
		}

		template<class TU, class... TArgs>
		void construct(TU* p_pointer, TArgs&&... p_args)
		{
			new(p_pointer)TU(std::forward< TArgs >(p_args)...);
		}

		template<class TU>
		void destroy(TU* p_pointer)
		{
			p_pointer->~TU();
		}

		size_type max_size() const noexcept
		{
			return (std::numeric_limits<size_type>::max)();
		}

		void register_pointer(pointer* p_pointer) noexcept
		{
		}

		void unregister_pointer(pointer* p_pointer) noexcept
		{
		}

	private:
		bc_concurrent_memory_pool* m_memory_pool;
	};
}
