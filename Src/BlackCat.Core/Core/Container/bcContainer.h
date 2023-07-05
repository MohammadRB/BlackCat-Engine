// [08/09/2013 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "Core/CorePCH.h"
#include "Core/Memory/bcAlloc.h"
#include "Core/Container/bcAllocator.h"
#include "Core/Container/bcIterator.h"

namespace black_cat::core
{
	template<typename T>
	struct bc_container_node
	{
	public:
		using this_type = bc_container_node;
		using value_type = T;

	public:
		bc_container_node(const value_type& p_value) noexcept(std::is_nothrow_copy_constructible<value_type>::value)
			: m_value(p_value)
		{
		}

		bc_container_node(value_type&& p_value) noexcept(std::is_nothrow_move_constructible<value_type>::value)
			: m_value(std::move(p_value))
		{
		}

		bc_container_node(const this_type& p_other) noexcept(std::is_nothrow_copy_constructible<value_type>::value)
			: m_value(p_other.m_value)
		{
		}

		bc_container_node(this_type&& p_other) noexcept(std::is_nothrow_move_constructible<value_type>::value)
			: m_value(std::move(p_other.m_value))
		{
		}
			
		template<typename ...TArgs>
		bc_container_node(TArgs&&... p_args) noexcept(std::is_nothrow_constructible<value_type, TArgs...>::value)
			: m_value(std::forward<TArgs>(p_args)...)
		{
			static_assert(std::is_constructible<value_type, TArgs...>::value, "container node value is not constructable with args");
		}

		~bc_container_node() = default;

		value_type m_value;
	};
		
	template <typename T, class TAllocator>
	class bc_container
	{
	public:
		using value_type = T;
		using allocator_type = typename bc_allocator_traits<TAllocator>::allocator_type;
		using pointer = typename bc_allocator_traits<TAllocator>::pointer;
		using const_pointer = typename bc_allocator_traits<TAllocator>::const_pointer;
		using reference = value_type&;
		using const_reference = const value_type&;
		using difference_type = typename bc_allocator_traits<allocator_type>::difference_type;
		using size_type = typename bc_allocator_traits<allocator_type>::size_type;
		
	public:
		bc_container() : m_size(0) {}

		~bc_container() = default;

		size_type size() const { return m_size; }

		bool empty() const { return size() == 0 ? true : false; }

	protected:
		template<typename TInputIterator>
		void check_iterator()
		{
			static_assert(std::is_convertible<typename std::iterator_traits<TInputIterator>::value_type, value_type>::value, "Types must be convertible");
		}

		size_type m_size;
	};

	template<typename T, class TAllocator>
	class bc_concurrent_container
	{
	public:
		using value_type = T;
		using allocator_type = typename bc_allocator_traits<TAllocator>::allocator_type;
		using pointer = typename bc_allocator_traits<TAllocator>::pointer;
		using const_pointer = typename bc_allocator_traits<TAllocator>::const_pointer;
		using reference = value_type&;
		using const_reference = const value_type&;
		using difference_type = typename bc_allocator_traits<allocator_type>::difference_type;
		using size_type = typename bc_allocator_traits<allocator_type>::size_type;
	};
}
