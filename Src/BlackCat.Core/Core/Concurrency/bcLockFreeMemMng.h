// [19/1/2014 MRB]

#pragma once

#include "Core/CorePCH.h"
#include "CorePlatformImp/Concurrency/bcAtomic.h"

namespace black_cat::core
{
	template<typename TContainer>
	struct bc_lockfree_memmng_container_traits
	{
		using container_type = TContainer;
		using node_type = typename container_type::node_type;
		using node_pointer = typename container_type::node_pointer;

	public:
		static void reclaim_node(container_type& p_container, node_type* p_node) 
		noexcept(noexcept(std::declval<container_type>().reclaim_node(p_node)))
		{
			p_container.reclaim_node(p_node);
		}

		static node_pointer next(const container_type& p_container, node_type* p_node) noexcept
		{
			return p_container.next(p_node);
		}

		static void next(const container_type& p_container, node_type* p_node, node_type* p_next) noexcept
		{
			return p_container.next(p_node, p_next);
		}
	};

	template<typename TContainer>
	class bc_lockfree_memmng
	{
		using this_type = bc_lockfree_memmng;
		using container_type = TContainer;
		using node_type = typename bc_lockfree_memmng_container_traits<container_type>::node_type;
		using node_pointer = typename bc_lockfree_memmng_container_traits<container_type>::node_pointer;

	public:
		bc_lockfree_memmng() noexcept
			: m_num_thread(0),
			  m_to_delete(nullptr)
		{
		}

		bc_lockfree_memmng(const this_type&) = delete;

		bc_lockfree_memmng(this_type&& p_other) noexcept
		{
			_assign(std::move(p_other));
		}

		~bc_lockfree_memmng() = default;

		this_type& operator =(const this_type&) = delete;

		this_type& operator =(this_type&& p_other) noexcept
		{
			_assign(std::move(p_other));

			return *this;
		}

		bcUINT32 enter() noexcept
		{
			return m_num_thread.fetch_add(1, platform::bc_memory_order::relaxed) + 1;
		}

		void exist_without_reclaim() noexcept
		{
			m_num_thread.fetch_sub(1, platform::bc_memory_order::relaxed);
		}

		void try_reclaim(container_type& p_container) noexcept(noexcept(_delete_nodes(p_container, std::declval<node_pointer>())))
		{
			if (m_num_thread.load(platform::bc_memory_order::seqcst) == 1)
			{
				node_pointer l_nodes_to_delete = m_to_delete.exchange(nullptr, platform::bc_memory_order::seqcst);
				if (m_num_thread.fetch_sub(1, platform::bc_memory_order::seqcst) - 1 == 0)
				{
					_delete_nodes(p_container, l_nodes_to_delete);
				}
				else if (l_nodes_to_delete)
				{
					_chain_pending_nodes(p_container, l_nodes_to_delete);
				}
			}
			else
			{
				m_num_thread.fetch_sub(1, platform::bc_memory_order::seqcst);
			}
		}

		void try_reclaim(container_type& p_container, node_pointer p_node) noexcept(noexcept(_delete_nodes(p_container, std::declval<node_pointer>())))
		{
			if (m_num_thread.load(platform::bc_memory_order::seqcst) == 1)
			{
				node_pointer l_nodes_to_delete = m_to_delete.exchange(nullptr, platform::bc_memory_order::seqcst);
				if (m_num_thread.fetch_sub(1, platform::bc_memory_order::seqcst) - 1 == 0)
				{
					_delete_nodes(p_container, l_nodes_to_delete);
				}
				else if (l_nodes_to_delete)
				{
					_chain_pending_nodes(p_container, l_nodes_to_delete);
				}

				bc_lockfree_memmng_container_traits<container_type>::reclaim_node(p_container, p_node);
			}
			else
			{
				_chain_pending_node(p_container, p_node);
				m_num_thread.fetch_sub(1, platform::bc_memory_order::seqcst);
			}
		}

		void swap(this_type& p_other) noexcept
		{
			const bcUINT32 l_num_thread = m_num_thread.load(platform::bc_memory_order::relaxed);
			node_type* l_to_delete = m_to_delete.load(platform::bc_memory_order::relaxed);

			m_num_thread.store(
				p_other.m_num_thread.load(platform::bc_memory_order::relaxed),
				platform::bc_memory_order::relaxed);
			m_to_delete.store(
				p_other.m_to_delete.load(platform::bc_memory_order::relaxed),
				platform::bc_memory_order::relaxed);

			p_other.m_num_thread.store(l_num_thread, platform::bc_memory_order::relaxed);
			p_other.m_to_delete.store(l_to_delete, platform::bc_memory_order::relaxed);
		}

	private:
		void _assign(this_type&& p_other)
		{
			m_num_thread.store(
				p_other.m_num_thread.load(platform::bc_memory_order::relaxed),
				platform::bc_memory_order::relaxed);
			m_to_delete.store(
				p_other.m_to_delete.load(platform::bc_memory_order::relaxed),
				platform::bc_memory_order::relaxed);

			p_other.m_num_thread.store(0, platform::bc_memory_order::relaxed);
			p_other.m_to_delete.store(nullptr, platform::bc_memory_order::relaxed);
		}

		static void _delete_nodes(container_type& p_container, node_pointer p_nodes)
		noexcept(noexcept(bc_lockfree_memmng_container_traits<container_type>::reclaim_node(p_container, p_nodes)))
		{
			while (p_nodes)
			{
				node_pointer l_next = bc_lockfree_memmng_container_traits<container_type>::next(p_container, p_nodes);
				bc_lockfree_memmng_container_traits<container_type>::reclaim_node(p_container, p_nodes);
					
				p_nodes = l_next;
			}
		}

		void _chain_pending_nodes(container_type& p_container, node_pointer p_nodes) noexcept
		{
			node_pointer l_last = p_nodes;
			while (node_pointer const l_next = bc_lockfree_memmng_container_traits<container_type>::next(p_container, l_last))
			{
				l_last = l_next;
			}

			_chain_pending_nodes(p_container, p_nodes, l_last);
		}

		void _chain_pending_nodes(container_type& p_container, node_pointer p_first, node_pointer p_last) noexcept
		{
			node_pointer l_to_delete = m_to_delete.load(platform::bc_memory_order::seqcst);

			do
			{
				bc_lockfree_memmng_container_traits<container_type>::next
				(
					p_container,
					p_last,
					l_to_delete
				);
			}
			while (!m_to_delete.compare_exchange_weak
				(
					&l_to_delete,
					p_first,
					platform::bc_memory_order::seqcst
				));
		}

		void _chain_pending_node(container_type& p_traits, node_pointer p_node) noexcept
		{
			_chain_pending_nodes(p_traits, p_node, p_node);
		}

		platform::bc_atomic<bcUINT32> m_num_thread;
		platform::bc_atomic<node_pointer> m_to_delete;
	};

	template<typename TNodeTraits>
	void swap(bc_lockfree_memmng<TNodeTraits>& p_first, bc_lockfree_memmng<TNodeTraits>& p_second) noexcept(noexcept(p_first.swap(p_second)))
	{
		p_first.swap(p_second);
	}
}
