//  [11/18/2013 MRB]

#pragma once

#include "CorePCH.h"
#include "bcContainer.h"
#include "bcLockFreeMemMng.h"


namespace BlackCat
{
	namespace Core
	{
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
					Link() { bcAtomicOperation::bcAtomicStore(mPointer, (Node*)bcNULL, bcMemoryOrder_SeqCst); }
					Link(Node* pNode) { bcAtomicOperation::bcAtomicStore(mPointer, pNode, bcMemoryOrder_SeqCst); }
					~Link() {}

					bcBOOL operator ==(const Link& pOther)
					{
						return bcAtomicOperation::bcAtomicLoad(mPointer, bcMemoryOrder_SeqCst) ==
							bcAtomicOperation::bcAtomicLoad(pOther.mPointer, bcMemoryOrder_SeqCst);
					}
					bcBOOL operator !=(const Link& pOther) { return !operator==(pOther); }
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

			void _terminateNode(NodeBaseType* pNode, bcBOOL pConcurrent) override
			{
				NodeType* lNode = static_cast<NodeType*>(pNode);

				if (!pConcurrent)
				{
					mMemMng.storeRef(&lNode->mNext, &LinkType(bcNULL));
				}
				else // TODO check here
				{
					while (bcTRUE)
					{
						LinkType* lNext = &lNode->mNext;
						if (mMemMng.compareAndSwapRef(&lNode->mNext, lNext, &LinkType(bcNULL)))
							break;
					}
				}
			}
			void _cleanUpNode(NodeBaseType* pNode) override
			{
				NodeType* lNode = static_cast<NodeType*>(pNode);

				while (bcTRUE)
				{
					NodeType* lNext = mMemMng.deRefLink(&lNode->mNext);
					if (!lNext || !bcAtomicOperation::bcAtomicLoad(lNext->mDel, bcMemoryOrder_SeqCst))
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
				//lNode->mNext = LinkType(bcNULL);

				NodeType* lOld = mMemMng.deRefLink(pTail);
				NodeType* lPrev = lOld;

				do
				{
					while (lPrev->mNext != LinkType(bcNULL))
					{
						NodeType* lPrev2 = mMemMng.deRefLink(&lPrev->mNext);
						if (lOld != lPrev) mMemMng.releaseRef(lPrev);
						lPrev = lPrev2;
					}
				} while (!mMemMng.compareAndSwapRef(&lPrev->mNext, &LinkType(bcNULL), &LinkType(lNode)));

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

			bcInline bcBOOL deQueue(LinkType* pHead, bcNullable<ValueType>* pData)
			{
				NodeType* lNode = bcNULL;
				NodeType* lNext = bcNULL;
				while (bcTRUE)
				{
					lNode = mMemMng.deRefLink(pHead);
					lNext = mMemMng.deRefLink(&lNode->mNext);

					if (lNext == bcNULL)
					{
						mMemMng.releaseRef(lNode);
						mMemMng.releaseRef(lNext);
						if (pData != bcNULL) *pData = bcNULL;
						return bcFALSE;
					}

					if (mMemMng.compareAndSwapRef(pHead, &LinkType(lNode), &LinkType(lNext))) break;

					mMemMng.releaseRef(lNode);
					mMemMng.releaseRef(lNext);
				}

				mMemMng.deleteNode(lNode);
				if (pData != bcNULL)
					*pData = std::move(lNext->mValue);

				mMemMng.releaseRef(lNext);

				return bcTRUE;
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
				//lNode->mNext = LinkType(bcNULL);

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
				bcAtomicOperation::bcAtomicFetchAdd(mSize, 1U, bcMemoryOrder_SeqCst);
			}

			void pushBack(const ValueType& pValue)
			{
				enQueue(&mTail, pValue);
				bcAtomicOperation::bcAtomicFetchAdd(mSize, 1U, bcMemoryOrder_SeqCst);
			}
			void pushBack(ValueType&& pValue)
			{
				enQueue(&mTail, std::forward<ValueType>(pValue));
				bcAtomicOperation::bcAtomicFetchAdd(mSize, 1U, bcMemoryOrder_SeqCst);
			}

			bcBOOL popFront(bcNullable<ValueType>* pData = bcNULL)
			{
				bcBOOL lDeleted = deQueue(&mHead, pData);
				if (lDeleted)
					bcAtomicOperation::bcAtomicFetchSub(mSize, 1U, bcMemoryOrder_SeqCst);
				return lDeleted;
			}

			void clear()
			{
				while (deQueue(&mHead, bcNULL));
			}
		};
	}
}