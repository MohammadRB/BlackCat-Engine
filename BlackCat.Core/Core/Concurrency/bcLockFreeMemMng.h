// [1/19/2014 MRB]

#pragma once

#include "Core/CorePCH.h"
#include "CorePlatformImp/Concurrency/bcAtomic.h"

namespace black_cat
{
	namespace core
	{
		template< typename TNodeTraits >
		struct bc_lockfree_memmng_node_traits
		{
			using traits_type = TNodeTraits;
			using node_type = typename traits_type::node_type;
			using node_pointer = typename traits_type::node_pointer;

		public:
			static void reclaim_node(traits_type& p_traits, node_type* p_node) 
				noexcept(noexcept(std::declval<traits_type>().reclaim_node(p_node)))
			{
				p_traits.reclaim_node(p_node);
			}

			static node_pointer next(const traits_type& p_traits, node_type* p_node) noexcept(true)
			{
				return p_traits.next(p_node);
			}

			static void next(const traits_type& p_traits, node_type* p_node, node_type* p_next) noexcept(true)
			{
				return p_traits.next(p_node, p_next);
			}
		};

		template< typename TNodeTraits >
		class bc_lockfree_memmng
		{
			using this_type = bc_lockfree_memmng;
			using traits_type = TNodeTraits;
			using node_type = typename bc_lockfree_memmng_node_traits< traits_type >::node_type;
			using node_pointer = typename bc_lockfree_memmng_node_traits< traits_type >::node_pointer;

		public:
			bc_lockfree_memmng() noexcept(true)
				: m_num_thread(0),
				m_to_delete(nullptr)
			{
			}

			bc_lockfree_memmng(const this_type&) = delete;

			bc_lockfree_memmng(this_type&& p_other) noexcept(true)
			{
				_assign(std::move(p_other));
			}

			~bc_lockfree_memmng()
			{
			}

			this_type& operator =(const this_type&) = delete;

			this_type& operator =(this_type&& p_other) noexcept(true)
			{
				_assign(std::move(p_other));

				return *this;
			}

			bcUINT32 enter_pop() noexcept(true)
			{
				return m_num_thread.fetch_add(1, core_platform::bc_memory_order::relaxed);
			}

			void exist_pop_widthou_reclaim() noexcept(true)
			{
				m_num_thread.fetch_sub(1, core_platform::bc_memory_order::relaxed);
			}

			void try_reclaim(traits_type& p_traits, node_pointer p_node)
				noexcept(noexcept(_delete_nodes(p_traits, p_node)))
			{
				if (m_num_thread.load(core_platform::bc_memory_order::seqcst) == 1)
				{
					node_pointer l_nodes_to_delete = m_to_delete.exchange(nullptr, core_platform::bc_memory_order::seqcst);
					if (m_num_thread.fetch_sub(1, core_platform::bc_memory_order::seqcst) - 1 == 0)
					{
						_delete_nodes(p_traits, l_nodes_to_delete);
					}
					else if (l_nodes_to_delete)
					{
						_chain_pending_nodes(p_traits, l_nodes_to_delete);
					}

					bc_lockfree_memmng_node_traits< traits_type >::reclaim_node(p_traits, p_node);
				}
				else
				{
					_chain_pending_node(p_traits, p_node);
					m_num_thread.fetch_sub(1, core_platform::bc_memory_order::seqcst);
				}
			}

			void swap(this_type& p_other) noexcept(true)
			{
				bcUINT32 l_num_thread = m_num_thread.load(core_platform::bc_memory_order::relaxed);
				node_type* l_to_delete = m_to_delete.load(core_platform::bc_memory_order::relaxed);

				m_num_thread.store(
					p_other.m_num_thread.load(core_platform::bc_memory_order::relaxed),
					core_platform::bc_memory_order::relaxed);
				m_to_delete.store(
					p_other.m_to_delete.load(core_platform::bc_memory_order::relaxed),
					core_platform::bc_memory_order::relaxed);

				p_other.m_num_thread.store(l_num_thread, core_platform::bc_memory_order::relaxed);
				p_other.m_to_delete.store(l_to_delete, core_platform::bc_memory_order::relaxed);
			}

		protected:

		private:
			void _assign(this_type&& p_other)
			{
				m_num_thread.store(
					p_other.m_num_thread.load(core_platform::bc_memory_order::relaxed),
					core_platform::bc_memory_order::relaxed);
				m_to_delete.store(
					p_other.m_to_delete.load(core_platform::bc_memory_order::relaxed),
					core_platform::bc_memory_order::relaxed);

				p_other.m_num_thread.store(0, core_platform::bc_memory_order::relaxed);
				p_other.m_to_delete.store(nullptr, core_platform::bc_memory_order::relaxed);
			}

			static void _delete_nodes(traits_type& p_traits, node_pointer p_nodes)
				noexcept(noexcept(bc_lockfree_memmng_node_traits< traits_type >::reclaim_node(p_traits, p_nodes)))
			{	
				while (p_nodes)
				{
					node_pointer l_next = bc_lockfree_memmng_node_traits< traits_type >::next(p_traits, p_nodes);
					
					bc_lockfree_memmng_node_traits< traits_type >::reclaim_node(p_traits, p_nodes);
					
					p_nodes = l_next;
				}
			}

			void _chain_pending_nodes(traits_type& p_traits, node_pointer p_nodes) noexcept(true)
			{
				node_pointer l_last = p_nodes;
				while (node_pointer const l_next = bc_lockfree_memmng_node_traits< traits_type >::next(p_traits, l_last))
				{
					l_last = l_next;
				}

				_chain_pending_nodes(p_traits, p_nodes, l_last);
			}

			void _chain_pending_nodes(traits_type& p_traits, node_pointer p_first, node_pointer p_last) noexcept(true)
			{
				node_pointer l_to_delete = m_to_delete.load(core_platform::bc_memory_order::seqcst);

				do
				{
					bc_lockfree_memmng_node_traits< traits_type >::next(
						p_traits,
						p_last,
						l_to_delete);
				}
				while (!m_to_delete.compare_exchange_weak(
					        &l_to_delete,
					        p_first,
					        core_platform::bc_memory_order::seqcst));
			}

			void _chain_pending_node(traits_type& p_traits, node_pointer p_node) noexcept(true)
			{
				_chain_pending_nodes(p_traits, p_node, p_node);
			}

			core_platform::bc_atomic<bcUINT32> m_num_thread;
			core_platform::bc_atomic<node_pointer> m_to_delete;
		};

		template< typename TNodeTraits >
		void swap(bc_lockfree_memmng< TNodeTraits >& p_first, bc_lockfree_memmng< TNodeTraits >& p_second)
		{
			p_first.swap(p_second);
		}
	}
}
