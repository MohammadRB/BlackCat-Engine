// [04/12/2020 MRB]

#pragma once

#include "Core/Container/bcList.h"
#include "Core/Utility/bcObjectPoolAllocator.h"

#pragma once

namespace black_cat
{
	namespace core
	{
		template<typename T>
		class bc_list_pool : private bc_list<T, bc_object_pool_allocator<T>>
		{
		public:
			using memory_pool_type = bc_concurrent_memory_pool;
			using list_type = bc_list<T, bc_object_pool_allocator<T>>;
			using value_type = typename list_type::value_type;
			using allocator_type = typename list_type::allocator_type;
			using pointer = typename list_type::pointer;
			using const_pointer = typename list_type::const_pointer;
			using reference = typename list_type::reference;
			using const_reference = typename list_type::const_reference;
			using difference_type = typename list_type::difference_type;
			using size_type = typename list_type::size_type;
			using node_type = typename list_type::node_type;
			using iterator = typename list_type::iterator;
			using const_iterator = typename list_type::const_iterator;
			using reverse_iterator = typename list_type::reverse_iterator;
			using const_reverse_iterator = typename list_type::const_reverse_iterator;
			
			using list_type::assign;
			using list_type::get_allocator;
			using list_type::front;
			using list_type::back;
			using list_type::begin;
			using list_type::cbegin;
			using list_type::end;
			using list_type::cend;
			using list_type::rbegin;
			using list_type::crbegin;
			using list_type::rend;
			using list_type::crend;
			using list_type::clear;
			using list_type::insert;
			using list_type::emplace;
			using list_type::erase;
			using list_type::push_back;
			using list_type::emplace_back;
			using list_type::pop_back;
			using list_type::push_front;
			using list_type::emplace_front;
			using list_type::pop_front;
			using list_type::resize;
			using list_type::swap;
			using list_type::merge;
			using list_type::splice;
			using list_type::remove;
			using list_type::remove_if;
			using list_type::reverse;
			using list_type::unique;
			using list_type::sort;
			using list_type::size;
			using list_type::empty;
			
		public:
			bc_list_pool(size_type p_pool_size, bc_alloc_type p_alloc_type = bc_alloc_type::unknown);

			bc_list_pool(bc_list_pool&& p_other);

			~bc_list_pool();

			bc_list_pool& operator=(bc_list_pool&& p_other);

			list_type& get_list() noexcept;

			memory_pool_type& get_memory_pool() noexcept;
			
		private:
			bc_concurrent_memory_pool m_memory_pool;
		};

		template<typename T>
		bc_list_pool<T>::bc_list_pool(size_type p_pool_size, bc_alloc_type p_alloc_type)
			: list_type(bc_object_pool_allocator<value_type>(m_memory_pool))
		{
			BC_ASSERT(p_alloc_type != bc_alloc_type::unknown_movable);
			m_memory_pool.initialize(p_pool_size, sizeof(typename list_type::node_type), p_alloc_type);
		}

		template<typename T>
		bc_list_pool<T>::bc_list_pool(bc_list_pool&& p_other)
			: list_type(std::move(p_other), bc_object_pool_allocator<value_type>(m_memory_pool)),
			m_memory_pool(std::move(p_other.m_memory_pool))
		{
		}

		template<typename T>
		bc_list_pool<T>::~bc_list_pool()
		{
			list_type::~list_type();
			m_memory_pool.destroy();
		}

		template<typename T>
		bc_list_pool<T>& bc_list_pool<T>::operator=(bc_list_pool&& p_other)
		{
			list_type::operator=(std::move(p_other));
			m_memory_pool = std::move(p_other.m_memory_pool);
			return *this;
		}

		template<typename T>
		typename bc_list_pool<T>::list_type& bc_list_pool<T>::get_list() noexcept
		{
			return static_cast<list_type>(*this);
		}

		template<typename T>
		bc_concurrent_memory_pool& bc_list_pool<T>::get_memory_pool() noexcept
		{
			return m_memory_pool;
		}
	}	
}