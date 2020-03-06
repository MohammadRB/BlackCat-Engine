// [01/03/2019 MRB]

#pragma once

#include "Core/Utility/bcObjectPool.h"
#include "Core/bcException.h"

namespace black_cat
{
	namespace core
	{
		template< typename T >
		class bc_memory_pool_allocator
		{
		private:
			template<typename TOther>
			friend class bc_memory_pool_allocator;

		public:
			using this_type = bc_memory_pool_allocator< T >;
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

			template< typename TOther >
			struct rebind
			{
				using other = bc_memory_pool_allocator< TOther >;
			};

		public:
			bc_memory_pool_allocator() noexcept
				: m_memory_pool(nullptr)
			{	
			}

			bc_memory_pool_allocator(bc_concurrent_memory_pool& p_memory_pool) noexcept
				: m_memory_pool(&p_memory_pool)
			{
			}

			bc_memory_pool_allocator(const this_type& p_other) noexcept
			{
				operator=(p_other);
			}

			template< typename TOther >
			bc_memory_pool_allocator(const bc_memory_pool_allocator< TOther >& p_other) noexcept
			{
				operator=(p_other);
			}

			bc_memory_pool_allocator(this_type&& p_other) noexcept
			{
				operator=(std::move(p_other));
			}

			~bc_memory_pool_allocator() = default;

			this_type& operator =(const this_type& p_other) noexcept
			{
				m_memory_pool = p_other.m_memory_pool;
				return *this;
			}

			template< typename TOther >
			this_type& operator =(const bc_memory_pool_allocator< TOther >& p_other) noexcept
			{
				m_memory_pool = p_other.m_memory_pool;
				return *this;
			}

			this_type& operator =(this_type&& p_other) noexcept
			{
				m_memory_pool = p_other.m_memory_pool;
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

			pointer allocate(size_type p_count, std::allocator<void>::const_pointer p_hint = nullptr)
			{
				if (p_count > 1)
				{
					throw bc_invalid_operation_exception("Allocation of more that one object is not supported in memory pool allocator");
				}
				if (!m_memory_pool)
				{
					throw bc_invalid_operation_exception("Uninitialized memory pool");
				}
				if (m_memory_pool->block_size() != sizeof(value_type))
				{
					throw bc_invalid_operation_exception("Memory pool with different size than one that is expected");
				}

				pointer l_pointer = static_cast< pointer >(m_memory_pool->alloc());
				if (!l_pointer)
				{
					l_pointer = static_cast< pointer >(bcAllocThrow(sizeof(value_type), bc_alloc_type::unknown));
				}

				return l_pointer;
			}

			void deallocate(pointer p_pointer, size_type n = 0)
			{
				if (n > 1)
				{
					throw bc_invalid_operation_exception("Allocation of more that one object is not supported in memory pool allocator");
				}
				if (!m_memory_pool)
				{
					throw bc_invalid_operation_exception("Uninitialized memory pool");
				}
				if (m_memory_pool->block_size() != sizeof(T))
				{
					throw bc_invalid_operation_exception("Memory pool with different size than one that is expected");
				}

				if(m_memory_pool->contain_pointer(p_pointer))
				{
					m_memory_pool->free(p_pointer);
				}
				else
				{
					bcFree(p_pointer);
				}
			}

			template< class TU, class... TArgs >
			void construct(TU* p_pointer, TArgs&&... p_args)
			{
				new(p_pointer)TU(std::forward< TArgs >(p_args)...);
			}

			template< class TU >
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
}