//  [1/21/2015 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcAtomicProvider.h"
#include "Core/CorePCH.h"
#include "Core/Container/bcContainer.h"
#include "Core/Concurrency/bcLockFreeMemMng.h"

namespace black_cat
{
	namespace core
	{
		template< typename T, typename TAllocator >
		class bc_concurrent_queue_base : bc_concurrent_container< T, TAllocator >
		{
		public:
			using this_type = bc_concurrent_queue_base;
			using base_type = bc_container< T, TAllocator >;
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
			using node_pointer = typename std::pointer_traits< pointer >::template rebind< node_type >;
			using internal_allocator_type = typename bc_allocator_traits< allocator_type >::template rebind_alloc< node_type >::other;

		public:
			bc_concurrent_queue_base(const allocator_type& p_allocator = allocator_type())
				: m_allocator(p_allocator)
			{
				static_assert(internal_allocator_type::is_movable_type::value == false, "Movable allocators aren't supported");
				static_assert(std::is_nothrow_move_assignable<value_type>::value == true, "T must be default constructible"); // TODO
				static_assert(std::is_nothrow_move_assignable<value_type>::value == true, "T must be nothrow move assignable");

				node_pointer l_node = bc_allocator_traits< internal_allocator_type >::allocate(m_allocator, 1);
				bc_allocator_traits< internal_allocator_type >::construct(m_allocator, l_node);
				m_tail.store(l_node, core_platform::bc_memory_order::relaxed);
				m_head.store(l_node, core_platform::bc_memory_order::relaxed);
			}

			bc_concurrent_queue_base(const this_type&) = delete;

			bc_concurrent_queue_base(this_type&& p_other)
			{
				_assing(std::move(p_other));
			}

			~bc_concurrent_queue_base()
			{
				bc_allocator_traits< internal_allocator_type >::deallocate(
					m_allocator, 
					m_head.load(core_platform::bc_memory_order::relaxed));
			};

			this_type& operator =(const this_type&) = delete;

			this_type& operator =(this_type&& p_other)
			{
				_assing(std::move(p_other));

				return *this;
			}

		protected:
			struct node : public bc_container_node< value_type >
			{
			public:
				node(const value_type& p_value) noexcept(std::is_nothrow_copy_constructible<bc_container_node<value_type>>::value)
					: bc_container_node(p_value),
					m_next(nullptr)
				{
				}

				node(value_type&& p_value) noexcept(std::is_nothrow_move_constructible<bc_container_node<value_type>>::value)
					: bc_container_node(std::move(p_value)),
					m_next(nullptr)
				{
				}

				template< typename ...TArgs >
				node(TArgs&&... p_args) noexcept(std::is_nothrow_constructible<bc_container_node<value_type>, TArgs...>::value)
					: bc_container_node(std::forward<TArgs>(p_args)...),
					m_next(nullptr)
				{
				}

				node(const node& p_other) noexcept(std::is_nothrow_copy_constructible<bc_container_node<value_type>>::value)
					: bc_container_node(p_other.m_value),
					m_next(p_other.m_next)
				{
				}

				node(node&& p_other) noexcept(std::is_nothrow_move_constructible<bc_container_node<value_type>>::value)
					: bc_container_node(std::move(p_other.m_value)),
					m_next(p_other.m_next)
				{
					p_other.m_next = nullptr;
				}

				~node()
				{
				}

				core_platform::bc_atomic< node_pointer > m_next;
			};

			template< typename ...TArgs >
			node_type* _enqueue(TArgs&&... p_args)
			{
				node_pointer l_node = bc_allocator_traits< internal_allocator_type >::allocate(m_allocator, 1);
				bc_allocator_traits< internal_allocator_type >::construct(m_allocator, l_node, std::forward<TArgs>(p_args)...);

				node_pointer l_tail_copy = m_tail.load(core_platform::bc_memory_order::seqcst);
				node_pointer l_tail = l_tail_copy;
				node_pointer l_expected;

				do
				{
					node_pointer l_prev = l_tail->m_next.load(core_platform::bc_memory_order::seqcst);
					while (l_prev != nullptr)
					{
						l_tail = l_prev;
						l_prev = l_tail->m_next.load(core_platform::bc_memory_order::seqcst);
					}

					l_expected = nullptr;
				} while (!l_tail->m_next.compare_exchange_weak(&l_expected, l_node, core_platform::bc_memory_order::seqcst));

				m_tail.compare_exchange_strong(&l_tail_copy, l_node, core_platform::bc_memory_order::seqcst);

				return l_node;
			}

			node_type* _dequeue(pointer p_result)
			{
				node_pointer l_local_head = nullptr;
				node_pointer l_next = nullptr;

				while (true)
				{
					l_local_head = m_head.load(core_platform::bc_memory_order::seqcst);
					l_next = l_local_head->m_next.load(core_platform::bc_memory_order::seqcst);

					if (l_next == nullptr)
					{
						return nullptr;
					}

					if (m_head.compare_exchange_strong(&l_local_head, l_next))
						break;
				}

				if (p_result)
					*p_result = std::move(l_next->m_value);
				
				return l_local_head;
			}

			core_platform::bc_atomic< node_pointer > m_tail;
			core_platform::bc_atomic< node_pointer > m_head;
			internal_allocator_type m_allocator;

		private:
			void _assing(this_type&& p_other)
			{
				m_tail.store(
					p_other.m_tail.load(core_platform::bc_memory_order::relaxed), 
					core_platform::bc_memory_order::relaxed);
				m_head.store(
					p_other.m_head.load(core_platform::bc_memory_order::relaxed),
					core_platform::bc_memory_order::relaxed);

				p_other.m_tail.store(nullptr, core_platform::bc_memory_order::relaxed);
				p_other.m_head.store(nullptr, core_platform::bc_memory_order::relaxed);

				m_allocator = std::move(p_other.m_allocator);
			}

		};

		template< typename T, typename TAllocator = bc_allocator< T > >
		class bc_concurrent_queue : private bc_concurrent_queue_base< T, TAllocator >
		{
			friend class bc_lockfree_memmng_node_traits<bc_concurrent_queue>;

		public:
			using this_type = bc_concurrent_queue;
			using base_type = bc_concurrent_queue_base< T, TAllocator >;
			using value_type = typename base_type::value_type;
			using allocator_type = typename base_type::allocator_type;
			using pointer = typename base_type::pointer;
			using const_pointer = typename base_type::const_pointer;
			using reference = typename base_type::reference;
			using const_reference = typename base_type::const_reference;
			using difference_type = typename base_type::difference_type;
			using size_type = typename base_type::size_type;

		protected:
			using node_type = typename base_type::node_type;
			using node_pointer = typename base_type::node_pointer;
			using internal_allocator_type = typename base_type::internal_allocator_type;

		public:
			bc_concurrent_queue() : base_type()
			{
			}

			bc_concurrent_queue(const allocator_type& p_allocator) : base_type(p_allocator)
			{
			}

			bc_concurrent_queue(const this_type&) = delete;

			bc_concurrent_queue(this_type&& p_other) noexcept(true)
				: base_type(std::move(p_other)),
				m_memmng(std::move(p_other.m_memmng))
			{
			}

			~bc_concurrent_queue()
			{
				while (base_type::_dequeue(nullptr));
			}

			this_type& operator =(const this_type&) = delete;

			this_type& operator =(this_type&& p_other) noexcept(true)
			{
				base_type::operator=(std::move(p_other));
				m_memmng = std::move(p_other.m_memmng);

				return *this;
			}

			void empty()
			{
				return base_type::m_head.load(core_platform::bc_memory_order::relaxed)->m_next == nullptr;
			}

			void push(const value_type& p_value)
			{
				base_type::_enqueue(p_value);
			}

			void push(value_type&& p_value)
			{
				base_type::_enqueue(std::move(p_value));
			}

			template< typename ...TArgs >
			void emplace(TArgs&&... p_args)
			{
				base_type::_enqueue(std::forward<TArgs>(p_args)...);
			}

			bool pop(reference p_result) noexcept(true)
			{
				m_memmng.enter_pop();

				node_type* l_node = base_type::_dequeue(&p_result);
				if (!l_node)
					return false;

				m_memmng.try_reclaim(*this, l_node);

				return true;
			}

			void swap(this_type& p_other) noexcept(true)
			{
				using std::swap;

				node_pointer l_tail = base_type::m_tail.load(core_platform::bc_memory_order::relaxed);
				node_pointer l_head = base_type::m_head.load(core_platform::bc_memory_order::relaxed);
				base_type::m_tail.store(
					p_other.m_tail.load(core_platform::bc_memory_order::relaxed), 
					core_platform::bc_memory_order::relaxed);
				base_type::m_head.store(
					p_other.m_head.load(core_platform::bc_memory_order::relaxed),
					core_platform::bc_memory_order::relaxed);
				p_other.m_tail.store(l_tail, core_platform::bc_memory_order::relaxed);
				p_other.m_head.store(l_head, core_platform::bc_memory_order::relaxed);
				
				swap(base_type::m_allocator, p_other.m_allocator);
				swap(m_memmng, p_other.m_memmng);
			}

		protected:

		private:
			void reclaim_node(node_type* p_node)
			{
				bc_allocator_traits< internal_allocator_type >::destruct(base_type::m_allocator, p_node);
				bc_allocator_traits< internal_allocator_type >::deallocate(base_type::m_allocator, p_node);
			}

			node_type* next(node_type* p_node) const noexcept(true)
			{
				return p_node->m_next.load(core_platform::bc_memory_order::seqcst);
			}

			void next(node_type* p_node, node_type* p_next) const noexcept(true)
			{
				p_node->m_next.store(p_next, core_platform::bc_memory_order::seqcst);
			}

			bc_lockfree_memmng< this_type > m_memmng;
			
		};

		template< typename T, typename TAllocator >
		void swap(bc_concurrent_queue< T, TAllocator > p_first, bc_concurrent_queue< T, TAllocator > p_second)
		{
			p_first.swap(p_second);
		}

		/*
		template<typename T, typename Allocator>
		class bcConcurentQueueBase : public bcConcurrentContainer<T, Allocator>, protected bcLockFreeContainerMemMngConfig<T>
		{
			typedef bcConcurentQueueBase<T, Allocator> ThisType;
		private:

		protected:
			struct Node : bcLockFreeNode<ValueType>
			{
				struct Link : bcLockFreeLink<Node>
				{
				public:
					Link() { mPointer.store((Node*)nullptr, core_platform::bc_memory_order::seqcst); }
					Link(Node* pNode) { mPointer.store(pNode, core_platform::bc_memory_order::seqcst); }
					~Link() {}

					bool operator ==(const Link& pOther)
					{
						return mPointer.load(core_platform::bc_memory_order::seqcst) ==
							pOther.mPointer.load(core_platform::bc_memory_order::seqcst);
					}
					bool operator !=(const Link& pOther) { return !operator==(pOther); }
				};

				//ValueType mValue;
				Link mNext;

				Node() { }
				Node(const ValueType& pValue) : bcLockFreeNode(pValue) { }
				Node(ValueType&& pValue) : bcLockFreeNode(std::move(pValue)) { }
				~Node() {}
			};
			typedef Node NodeType;
			typedef typename NodeType::Link LinkType;

			bcLockFreeMemMng<T, NodeType, LinkType, Allocator> mMemMng;

			void _terminateNode(NodeBaseType* pNode, bool pConcurrent) override
			{
				NodeType* lNode = static_cast<NodeType*>(pNode);

				if (!pConcurrent)
				{
					mMemMng.storeRef(&lNode->mNext, &LinkType(nullptr));
				}
				else // TODO check here
				{
					while (true)
					{
						LinkType* lNext = &lNode->mNext;
						if (mMemMng.compareAndSwapRef(&lNode->mNext, lNext, &LinkType(nullptr)))
							break;
					}
				}
			}
			void _cleanUpNode(NodeBaseType* pNode) override
			{
				NodeType* lNode = static_cast<NodeType*>(pNode);

				while (true)
				{
					NodeType* lNext = mMemMng.deRefLink(&lNode->mNext);
					if (!lNext || !lNext->mDel.load(core_platform::bc_memory_order::seqcst))
					{
						mMemMng.releaseRef(lNext);
						break;
					}
					NodeType* lNext2 = mMemMng.deRefLink(&lNext->mNext);
					mMemMng.compareAndSwapRef(&lNode->mNext, &LinkType(lNext), &LinkType(lNext2));
					mMemMng.releaseRef(lNext2);
					mMemMng.releaseRef(lNext);
				}
			}

			NodeBaseType* _alloc() override
			{
				NodeType* lPointer = static_cast<NodeType*>(static_cast<void*>(mAllocator.alloc(sizeof(NodeType))));
				//Node* lResult = new (lPointer) Node;

				//return lResult;
				return lPointer;
			}
			void _free(NodeBaseType* pPointer) override
			{
				NodeType* lNode = static_cast<NodeType*>(pPointer);
				lNode->~Node();
				mAllocator.free(lNode);
			}

			bcInline NodeType* _createNode(const ValueType& pValue)
			{
				NodeType* lNode = mMemMng.newNode();
				new (lNode)NodeType(pValue);

				return lNode;
			}
			bcInline NodeType* _createNode(ValueType&& pValue)
			{
				NodeType* lNode = mMemMng.newNode();
				new (lNode)NodeType(std::forward<ValueType>(pValue));

				return lNode;
			}

			bcInline void _enQueue(LinkType* pTail, NodeType* lNode)
			{
				//lNode->mNext = LinkType(nullptr);

				NodeType* lOld = mMemMng.deRefLink(pTail);
				NodeType* lPrev = lOld;

				do
				{
					while (lPrev->mNext != LinkType(nullptr))
					{
						NodeType* lPrev2 = mMemMng.deRefLink(&lPrev->mNext);
						if (lOld != lPrev) mMemMng.releaseRef(lPrev);
						lPrev = lPrev2;
					}
				} while (!mMemMng.compareAndSwapRef(&lPrev->mNext, &LinkType(nullptr), &LinkType(lNode)));

				mMemMng.compareAndSwapRef(pTail, &LinkType(lOld), &LinkType(lNode));

				if (lOld != lPrev) mMemMng.releaseRef(lPrev);
				mMemMng.releaseRef(lOld);
				mMemMng.releaseRef(lNode);
			}

		public:
			bcConcurentQueueBase(bcUINT32 pNumThread) : bcLockFreeContainerMemMngConfig(pNumThread, 1, 1, 3), mMemMng(this)
			{}
			~bcConcurentQueueBase() 
			{}

			bcConcurentQueueBase(const ThisType&) = delete;
			ThisType& operator =(const ThisType&) = delete;

			bcInline bool deQueue(LinkType* pHead, bcNullable<ValueType>* pData)
			{
				NodeType* lNode = nullptr;
				NodeType* lNext = nullptr;
				while (true)
				{
					lNode = mMemMng.deRefLink(pHead);
					lNext = mMemMng.deRefLink(&lNode->mNext);

					if (lNext == nullptr)
					{
						mMemMng.releaseRef(lNode);
						mMemMng.releaseRef(lNext);
						if (pData != nullptr) *pData = nullptr;
						return false;
					}

					if (mMemMng.compareAndSwapRef(pHead, &LinkType(lNode), &LinkType(lNext))) break;

					mMemMng.releaseRef(lNode);
					mMemMng.releaseRef(lNext);
				}

				mMemMng.deleteNode(lNode);
				if (pData != nullptr)
					*pData = std::move(lNext->m_value);

				mMemMng.releaseRef(lNext);

				return true;
			}

			bcInline void enQueue(LinkType* pTail, const ValueType& pData)
			{
				NodeType* lNode = _createNode(pData);
				_enQueue(pTail, lNode);
			}
			bcInline void enQueue(LinkType* pTail, ValueType&& pData)
			{
				NodeType* lNode = _createNode(std::forward<ValueType>(pData));
				_enQueue(pTail, lNode);
			}
		};

		template<typename T, typename Allocator = bcContainerAllocation<>>
		class bcConcurrentQueue : protected bcConcurentQueueBase<T, Allocator>
		{
			typedef bcConcurrentQueue<T, Allocator> ThisType;
		private:

		protected:
			LinkType mHead;
			LinkType mTail;

		public:
			bcConcurrentQueue(bcUINT32 pNumThread) : bcConcurentQueueBase(pNumThread)
			{
				NodeType* lNode = _createNode(ValueType());
				//lNode->mNext = LinkType(nullptr);

				mMemMng.storeRef(&mHead, &LinkType(lNode));
				mMemMng.storeRef(&mTail, &LinkType(lNode));

				mMemMng.releaseRef(lNode);
			}
			~bcConcurrentQueue()
			{
				clear();
				mMemMng.deleteNode(mMemMng.deRefLink(&mHead));

				mMemMng.forceCleanUp();
			}

			bcConcurrentQueue(const ThisType&) = delete;
			ThisType& operator =(const ThisType&) = delete;

			void pushBack()
			{
				enQueue(&mTail, ValueType());
				mSize.fetch_add(1U, core_platform::bc_memory_order::seqcst);
			}

			void pushBack(const ValueType& pValue)
			{
				enQueue(&mTail, pValue);
				mSize.fetch_add(1U, core_platform::bc_memory_order::seqcst);
			}
			void pushBack(ValueType&& pValue)
			{
				enQueue(&mTail, std::forward<ValueType>(pValue));
				mSize.fetch_add(1U, core_platform::bc_memory_order::seqcst);
			}

			bool popFront(bcNullable<ValueType>* pData = nullptr)
			{
				bool lDeleted = deQueue(&mHead, pData);
				if (lDeleted)
					mSize.fetch_sub(1U, core_platform::bc_memory_order::seqcst);
				return lDeleted;
			}

			void clear()
			{
				while (deQueue(&mHead, nullptr));
			}
		};
		*/
	}
}