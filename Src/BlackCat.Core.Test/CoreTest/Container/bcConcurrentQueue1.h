// [13/04/2022 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "Core/CorePCH.h"
#include "Core/Container/bcContainer.h"
#include "Core/Concurrency/bcLockFreeMemMng.h"

namespace black_cat::core
{
	template<typename T, typename TAllocator>
	class bc_concurrent_queue1_base : bc_concurrent_container<T, TAllocator>
	{
	public:
		using this_type = bc_concurrent_queue1_base;
		using base_type = bc_container<T, TAllocator>;
		using value_type = typename base_type::value_type;
		using allocator_type = typename base_type::allocator_type;
		using pointer = value_type*;
		using const_pointer = const pointer;
		using reference = typename base_type::reference;
		using const_reference = typename base_type::const_reference;
		using difference_type = typename base_type::difference_type;
		using size_type = typename base_type::size_type;

	protected:
		struct node;

		using node_type = node;
		using node_pointer = typename std::pointer_traits<pointer>::template rebind<node_type>;
		using internal_allocator_type = typename bc_allocator_traits<allocator_type>::template rebind_alloc<node_type>::other;

	public:
		explicit bc_concurrent_queue1_base(const allocator_type& p_allocator = allocator_type())
			: m_allocator(p_allocator)
		{
			static_assert(internal_allocator_type::is_movable_type::value == false, "Movable allocators are not supported");
			static_assert(std::is_default_constructible_v<value_type> == true, "T must be default constructable");
			static_assert(std::is_nothrow_move_assignable_v<value_type> == true, "T must be nothrow move assignable");

			node_pointer l_node = bc_allocator_traits<internal_allocator_type>::allocate(m_allocator, 1);
			bc_allocator_traits<internal_allocator_type>::construct(m_allocator, l_node);

			m_head = l_node, platform::bc_memory_order::relaxed;
			m_tail = l_node, platform::bc_memory_order::relaxed;
		}

		bc_concurrent_queue1_base(const this_type&) = delete;

		bc_concurrent_queue1_base(this_type&& p_other) noexcept
		{
			_assign(std::move(p_other));
		}

		~bc_concurrent_queue1_base()
		{
			bc_allocator_traits<internal_allocator_type>::deallocate(m_allocator, m_head);
		}

		this_type& operator =(const this_type&) = delete;

		this_type& operator =(this_type&& p_other) noexcept
		{
			_assign(std::move(p_other));

			return *this;
		}

	protected:
		node_type* enqueue(node_pointer p_node)
		{
			{
				platform::bc_lock_guard l_lock(m_mutex);

				m_tail->m_next = p_node;
				m_tail = p_node;

				return p_node;
			}
		}

		node_type* dequeue(reference p_result)
		{
			node_pointer l_head;
			node_pointer l_next;

			{
				platform::bc_lock_guard l_lock(m_mutex);

				l_head = m_head;
				l_next = m_head->m_next;

				if(!l_next)
				{
					return nullptr;
				}

				m_head = l_next;
			}

			p_result = std::move(l_next->m_value);
				
			return l_head;
		}

		platform::bc_spin_mutex m_mutex;
		node_pointer m_head;
		node_pointer m_tail;
		internal_allocator_type m_allocator;

	private:
		void _assign(this_type&& p_other)
		{
			m_tail = p_other.m_tail;
			m_head = p_other.m_head;

			p_other.m_tail = nullptr;
			p_other.m_head = nullptr;

			m_allocator = std::move(p_other.m_allocator);
		}
	};

	template<typename T, typename TAllocator>
	struct bc_concurrent_queue1_base<T, TAllocator>::node : public bc_container_node<value_type>
	{
	public:
		node(const value_type& p_value) noexcept(std::is_nothrow_copy_constructible_v<bc_container_node<value_type>>)
			: bc_container_node<value_type>(p_value),
			  m_next(nullptr)
		{
		}

		node(value_type&& p_value) noexcept(std::is_nothrow_move_constructible_v<bc_container_node<value_type>>)
			: bc_container_node<value_type>(std::move(p_value)),
			  m_next(nullptr)
		{
		}

		node(const node& p_other) noexcept(std::is_nothrow_copy_constructible_v<bc_container_node<value_type>>)
			: bc_container_node<value_type>(p_other.m_value),
			  m_next(p_other.m_next)
		{
		}

		node(node&& p_other) noexcept(std::is_nothrow_move_constructible_v<bc_container_node<value_type>>)
			: bc_container_node<value_type>(std::move(p_other.m_value)),
			  m_next(p_other.m_next)
		{
			p_other.m_next = nullptr;
		}

		template<typename ...TArgs>
		node(TArgs&&... p_args) noexcept(std::is_nothrow_constructible_v<bc_container_node<value_type>, TArgs...>)
			: bc_container_node<value_type>(std::forward<TArgs>(p_args)...),
			  m_next(nullptr)
		{
		}

		~node() = default;

		node& operator=(const node&) = delete;

		node& operator=(node&&) = delete;

		node_pointer m_next;
	};

	template<typename T, typename TAllocator = bc_allocator<T>>
	class bc_concurrent_queue1 : private bc_concurrent_queue1_base<T, TAllocator>
	{
		friend struct bc_lockfree_memmng_container_traits<bc_concurrent_queue1>;

	public:
		using this_type = bc_concurrent_queue1;
		using base_type = bc_concurrent_queue1_base<T, TAllocator>;
		using value_type = typename base_type::value_type;
		using allocator_type = typename base_type::allocator_type;
		using pointer = typename base_type::pointer;
		using const_pointer = typename base_type::const_pointer;
		using reference = typename base_type::reference;
		using const_reference = typename base_type::const_reference;
		using difference_type = typename base_type::difference_type;
		using size_type = typename base_type::size_type;
		using node_type = typename base_type::node_type;

	protected:
		using node_pointer = typename base_type::node_pointer;
		using internal_allocator_type = typename base_type::internal_allocator_type;

	public:
		bc_concurrent_queue1()
			: base_type()
		{
		}

		bc_concurrent_queue1(const allocator_type& p_allocator)
			: base_type(p_allocator)
		{
		}

		bc_concurrent_queue1(const this_type&) = delete;

		bc_concurrent_queue1(this_type&& p_other) noexcept
			: base_type(std::move(p_other))
		{
		}

		~bc_concurrent_queue1()
		{
			// In case if container has move head and tail are null
			if(!base_type::m_head)
			{
				return;
			}

			value_type l_value;
			node_type* l_node = nullptr;

			while ((l_node = base_type::dequeue(l_value)))
			{
				reclaim_node(l_node);
			}
		}

		this_type& operator =(const this_type&) = delete;

		this_type& operator =(this_type&& p_other) noexcept
		{
			base_type::operator=(std::move(p_other));

			return *this;
		}

		void empty()
		{
			{
				platform::bc_lock_guard l_lock(base_type::m_mutex);

				return base_type::m_head->m_next == nullptr;
			}
		}

		void push(const value_type& p_value)
		{
			node_pointer l_node = bc_allocator_traits<internal_allocator_type>::allocate(base_type::m_allocator, 1);
			bc_allocator_traits<internal_allocator_type>::construct(base_type::m_allocator, l_node, p_value);

			base_type::enqueue(l_node);
		}

		void push(value_type&& p_value)
		{
			node_pointer l_node = bc_allocator_traits<internal_allocator_type>::allocate(base_type::m_allocator, 1);
			bc_allocator_traits<internal_allocator_type>::construct(base_type::m_allocator, l_node, std::move(p_value));

			base_type::enqueue(l_node);
		}

		template<typename ...TArgs>
		void emplace(TArgs&&... p_args)
		{
			node_pointer l_node = bc_allocator_traits<internal_allocator_type>::allocate(base_type::m_allocator, 1);
			bc_allocator_traits<internal_allocator_type>::construct(base_type::m_allocator, l_node, std::forward<TArgs>(p_args)...);

			base_type::enqueue(l_node);
		}

		bool pop(reference p_result) noexcept
		{
			node_type* l_node = base_type::dequeue(p_result);

			if (!l_node)
			{
				return false;
			}

			reclaim_node(l_node);

			return true;
		}

		void swap(this_type& p_other) noexcept
		{
			using std::swap;

			swap(base_type::m_tail, p_other.m_tail);
			swap(base_type::m_head, p_other.m_head);
			swap(base_type::m_allocator, p_other.m_allocator);
		}

	private:
		void reclaim_node(node_type* p_node)
		{
			bc_allocator_traits<internal_allocator_type>::destroy(base_type::m_allocator, p_node);
			bc_allocator_traits<internal_allocator_type>::deallocate(base_type::m_allocator, p_node);
		}
	};

	template<typename T, template<typename> typename TAllocator>
	using bc_concurrent_queue1_a = bc_concurrent_queue1<T, TAllocator<T>>;

	template<typename T, typename TAllocator>
	void swap(bc_concurrent_queue1<T, TAllocator> p_first, bc_concurrent_queue1<T, TAllocator> p_second)
	{
		p_first.swap(p_second);
	}
}
