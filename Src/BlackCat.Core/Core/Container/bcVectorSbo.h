// [13/12/2021 MRB]

#pragma once

#include "Core/Container/bcArray.h"
#include "Core/Container/bcVector.h"

namespace black_cat::core
{
	template<typename T, bcSIZE TBufferCapacity, bcUINT32 TAlignment>
	class bc_vector_sbo_allocator
	{
	public:
		using this_type = bc_vector_sbo_allocator<T, TBufferCapacity, TAlignment>;
		using value_type = T;
		using pointer = value_type*;
		using const_pointer = const value_type*;
		using reference = value_type&;
		using const_reference = const value_type&;
		using difference_type = bcPTRDIFF;
		using size_type = bcSIZE;
		using propagate_on_container_copy_assignment = std::false_type;
		using propagate_on_container_move_assignment = std::false_type;
		using propagate_on_container_swap = std::false_type;
		using is_movable_type = std::false_type;

		template<typename TOther>
		struct rebind
		{
			using other = bc_vector_sbo_allocator<TOther, TBufferCapacity>;
		};

	public:
		bc_vector_sbo_allocator() = default;

		bc_vector_sbo_allocator(const bc_vector_sbo_allocator&) noexcept = default;

		bc_vector_sbo_allocator(bc_vector_sbo_allocator&&) noexcept = default;

		~bc_vector_sbo_allocator() noexcept = default;

		bc_vector_sbo_allocator& operator=(const bc_vector_sbo_allocator&) noexcept = default;

		bc_vector_sbo_allocator& operator=(bc_vector_sbo_allocator&&) noexcept = default;

		pointer allocate(size_type p_count, const void* p_hint = nullptr)
		{
			if (p_count <= TBufferCapacity)
			{
				return &m_buffer[0];
			}

			if (TAlignment <= BC_MEMORY_MIN_ALIGN)
			{
				return static_cast<pointer>(BC_ALLOC_THROW(sizeof(value_type) * p_count, bc_alloc_type::unknown));
			}

			return static_cast<pointer>(BC_ALIGNED_ALLOC_THROW(sizeof(value_type) * p_count, TAlignment, bc_alloc_type::unknown));
		}

		void deallocate(pointer p_pointer, size_type n = 0)
		{
			if (p_pointer >= &m_buffer[0] && p_pointer < &m_buffer[TBufferCapacity])
			{
				return;
			}

			if (TAlignment <= BC_MEMORY_MIN_ALIGN)
			{
				BC_FREE(p_pointer);
				return;
			}

			BC_ALIGNED_FREE(p_pointer);
		}

		template<typename ...TArgs>
		void construct(value_type* p_pointer, TArgs&&... p_args) noexcept(std::is_nothrow_constructible_v<value_type, TArgs...>)
		{
			new (p_pointer) T(std::forward<TArgs>(p_args)...);
		}

		void destroy(value_type* p_pointer)
		{
			p_pointer->~T();
		}

	private:
		alignas(TAlignment)
		bc_array<T, TBufferCapacity> m_buffer;
	};

	template
	<
		typename T1,
		bcSIZE TBufferCapacity1,
		bcUINT32 TAlignment1,
		typename T2,
		bcSIZE TBufferCapacity2,
		bcUINT32 TAlignment2
	>
	bool operator ==(const bc_vector_sbo_allocator<T1, TBufferCapacity1, TAlignment1>& p_1, const bc_vector_sbo_allocator<T2, TBufferCapacity2, TAlignment2>& p_2)
	{
		return &p_1 == &p_2;
	}

	template
	<
		typename T1,
		bcSIZE TBufferCapacity1,
		bcUINT32 TAlignment1,
		typename T2,
		bcSIZE TBufferCapacity2,
		bcUINT32 TAlignment2
	>
	bool operator !=(const bc_vector_sbo_allocator<T1, TBufferCapacity1, TAlignment1>& p_1, const bc_vector_sbo_allocator<T2, TBufferCapacity2, TAlignment2>& p_2)
	{
		return !(p_1 == p_2);
	}

	template<typename T, bcSIZE TBufferCapacity = 4, bcUINT32 TAlignment = BC_MEMORY_MIN_ALIGN>
	class bc_vector_sbo : public bc_vector<T, bc_vector_sbo_allocator<T, TBufferCapacity, TAlignment>>
	{
	public:
		using vector_type = bc_vector<T, bc_vector_sbo_allocator<T, TBufferCapacity, TAlignment>>;
		using value_type = typename vector_type::value_type;
		using allocator_type = typename vector_type::allocator_type;
		using pointer = typename vector_type::pointer;
		using const_pointer = typename vector_type::const_pointer;
		using reference = typename vector_type::reference;
		using const_reference = typename vector_type::const_reference;
		using difference_type = typename vector_type::difference_type;
		using size_type = typename vector_type::size_type;
		using node_type = typename vector_type::node_type;
		using iterator = typename vector_type::iterator;
		using const_iterator = typename vector_type::const_iterator;
		using reverse_iterator = typename vector_type::reverse_iterator;
		using const_reverse_iterator = typename vector_type::const_reverse_iterator;

	public:
		bc_vector_sbo() noexcept;

		template<typename TInputIterator>
		bc_vector_sbo(TInputIterator p_first, TInputIterator p_last);

		bc_vector_sbo(std::initializer_list<value_type> p_initializer);

		bc_vector_sbo(const bc_vector_sbo&) = default;

		bc_vector_sbo(bc_vector_sbo&&) = default;

		~bc_vector_sbo() = default;

		bc_vector_sbo& operator=(const bc_vector_sbo&) = default;

		bc_vector_sbo& operator=(bc_vector_sbo&&) = default;
	};
	
	template <typename T, bcSIZE TBufferCapacity, bcUINT32 TAlignment>
	bc_vector_sbo<T, TBufferCapacity, TAlignment>::bc_vector_sbo() noexcept
		: vector_type()
	{
		vector_type::reserve(TBufferCapacity);
	}

	template <typename T, bcSIZE TBufferCapacity, bcUINT32 TAlignment>
	template <typename TInputIterator>
	bc_vector_sbo<T, TBufferCapacity, TAlignment>::bc_vector_sbo(TInputIterator p_first, TInputIterator p_last)
		: vector_type(p_first, p_last)
	{
		vector_type::reserve(TBufferCapacity);
	}

	template <typename T, bcSIZE TBufferCapacity, bcUINT32 TAlignment>
	bc_vector_sbo<T, TBufferCapacity, TAlignment>::bc_vector_sbo(std::initializer_list<value_type> p_initializer)
		: vector_type(p_initializer)
	{
		vector_type::reserve(TBufferCapacity);
	}
}
