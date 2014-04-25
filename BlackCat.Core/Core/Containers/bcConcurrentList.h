//  [12/16/2013 MRB]

#pragma once

#include "CorePCH.h"
#include "bcAtomic.h"
#include "bcLockFreeMemMng.h"
#include "bcConcurrencyUtility.h"
#include "CoreUtility.h"
#include "bcContainer.h"
//#include "bcIterator.h"

namespace BlackCat
{
	namespace Core
	{
		// Based on the paper: "Lock-free deques and doubly linked lists", Sundell, et al. - 2008
		template< typename T, typename Allocator >
		class bcConcurrectListBase : public bcConcurrentContainer<T, Allocator>, public bcConcurrentIteratorProvider<T>, protected bcLockFreeContainerMemMngConfig<T>
		{
		protected:
			typedef bcConcurrentIteratorProvider<T>::NodeBaseType IteratorProviderNodeBaseType;
			typedef bcLockFreeNode<ValueType> NodeBaseType;

			struct Node : public NodeBaseType
			{
			public:
				struct Link : public bcLockFreeLink< Node >
				{
				public:
					static const bcINT32 dMask = 1;
					static const bcINT32 ptrMask = ~dMask;

					Link() throw() { bcAtomicOperation::bcAtomicStore(mPointer, (Node*)bcNULL, bcMemoryOrder_SeqCst); }
					Link(const Node* pPointer, bcBOOL pDel = bcFALSE) throw()
					{
						Node* lPointer = reinterpret_cast<Node*>(reinterpret_cast<bcINT32>(pPointer)& ptrMask); // Reset pointer flags
						bcAtomicOperation::bcAtomicStore(
							mPointer,
							reinterpret_cast<Node*>((reinterpret_cast<bcINT32>(lPointer) | (bcINT32)pDel)),
							bcMemoryOrder_SeqCst);
					}

					Link(const Link& pOther)
					{
						bcAtomicOperation::bcAtomicStore(
							mPointer,
							bcAtomicOperation::bcAtomicLoad(pOther.mPointer, bcMemoryOrder_SeqCst),
							bcMemoryOrder_SeqCst);
					}

					bcInline Node* pointer() const throw()
					{
						return reinterpret_cast<Node*>(
							reinterpret_cast<bcINT32>(bcAtomicOperation::bcAtomicLoad(mPointer, bcMemoryOrder_SeqCst)) & ptrMask);
					}
					bcInline bcBOOL del() const throw()
					{
						return reinterpret_cast<bcINT32>(bcAtomicOperation::bcAtomicLoad(mPointer, bcMemoryOrder_SeqCst)) & dMask;
					}
					bcInline NodeType* getNode() throw() override
					{
						return reinterpret_cast<Node*>(
							reinterpret_cast<bcINT32>(bcAtomicOperation::bcAtomicLoad(mPointer, bcMemoryOrder_SeqCst)) & ptrMask);
					}

					bcInline bcBOOL compareAndSwap(const Link& pExpected, const Link& pNew) throw()
					{
						Node* lExpected = bcAtomicOperation::bcAtomicLoad(pExpected.mPointer, bcMemoryOrder_SeqCst);
						Node* lNew = bcAtomicOperation::bcAtomicLoad(pNew.mPointer, bcMemoryOrder_SeqCst);

						if (bcAtomicOperation::bcAtomicCompareExchangeStrong(
							mPointer,
							&lExpected,
							lNew,
							bcMemoryOrder_SeqCst,
							bcMemoryOrder_SeqCst))
						{
							//TODO check here
							Node* lNewExpected = bcAtomicOperation::bcAtomicLoad(pExpected.mPointer, bcMemoryOrder_SeqCst);
							Node* lNewNew = bcAtomicOperation::bcAtomicLoad(pNew.mPointer, bcMemoryOrder_SeqCst);

							bcAssert(lNewExpected == lExpected && lNewNew == lNew);

							return bcTRUE;
						}

						return bcFALSE;
					}

					bcBOOL operator==(const Link& pOther) throw()
					{
						return bcAtomicOperation::bcAtomicLoad(mPointer, bcMemoryOrder_SeqCst) ==
							bcAtomicOperation::bcAtomicLoad(pOther.mPointer, bcMemoryOrder_SeqCst);
					}
					bcBOOL operator!=(const Link& pOther) throw()
					{
						return !operator==(pOther);
					}
				};

				//ValueType mValue;
				Link mPrev;
				Link mNext;

				Node() {};
				Node(const ValueType& pValue) : bcLockFreeNode(pValue) {};
				Node(ValueType&& pValue) : bcLockFreeNode(std::move(pValue)) {};
				~Node() {}
			};

			typedef Node NodeType;
			typedef typename NodeType::Link LinkType;

		private:
			void _terminateNode(NodeBaseType* pNode, bcBOOL pConcurrent) override
			{
				NodeType* lNode = static_cast<NodeType*>(pNode);

				if (!pConcurrent)
				{
					mMemMng.storeRef(&lNode->mPrev, &LinkType(bcNULL, bcTRUE));
					mMemMng.storeRef(&lNode->mNext, &LinkType(bcNULL, bcTRUE));
				}
				else // TODO check here
				{
					while (bcTRUE)
					{
						LinkType* lPrev = &lNode->mPrev;
						if (mMemMng.compareAndSwapRef(&lNode->mPrev, lPrev, &LinkType(bcNULL, bcTRUE)))
							break;
					}
					while (bcTRUE)
					{
						LinkType* lNext = &lNode->mNext;
						if (mMemMng.compareAndSwapRef(&lNode->mNext, lNext, &LinkType(bcNULL, bcTRUE)))
							break;
					}
				}
			}
			void _cleanUpNode(NodeBaseType* pNode) override
			{
				NodeType* lNode = static_cast<NodeType*>(pNode);

				while (bcTRUE)
				{
					NodeType* lPrev = mMemMng.deRefLink(&lNode->mPrev);
					//if (!lPrev) break;
					// Release Ref even if lPrev is null, because we must release allocated index from FreeIndexList
					// in MemMng
					if (!lPrev || !lPrev->mPrev.del())
					{
						mMemMng.releaseRef(lPrev);
						break;
					}
					NodeType* lPrev2 = mMemMng.deRefLink(&lPrev->mPrev);
					mMemMng.compareAndSwapRef(&lNode->mPrev, &LinkType(lPrev, bcTRUE), &LinkType(lPrev2, bcTRUE));
					mMemMng.releaseRef(lPrev2);
					mMemMng.releaseRef(lPrev);
				}

				while (bcTRUE)
				{
					NodeType* lNext = mMemMng.deRefLink(&lNode->mNext);
					//if (!lNext) break;
					if (!lNext || !lNext->mNext.del())
					{
						mMemMng.releaseRef(lNext);
						break;
					}
					NodeType* lNext2 = mMemMng.deRefLink(&lNext->mNext);
					mMemMng.compareAndSwapRef(&lNode->mNext, &LinkType(lNext, bcTRUE), &LinkType(lNext2, bcTRUE));
					mMemMng.releaseRef(lNext2);
					mMemMng.releaseRef(lNext);
				}
			}

			NodeBaseType* _alloc() override
			{
				NodeType* lPointer = reinterpret_cast<NodeType*>(mAllocator.alloc(sizeof(NodeType)));

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
				bcAssert((reinterpret_cast<bcINT32>(lNode)& LinkType::dMask) == 0);
				new (lNode)NodeType(pValue);

				return lNode;
			}
			bcInline NodeType* _createNode(ValueType&& pValue)
			{
				NodeType* lNode = mMemMng.newNode();
				bcAssert((reinterpret_cast<bcINT32>(lNode)& LinkType::dMask) == 0);
				new (lNode)NodeType(pValue);

				return lNode;
			}
			bcInline void _setMark(LinkType* pLink)
			{
				/*while(bcTRUE)
				{
				Node* lValue = (bcAtomicOperation::bcAtomicLoad(*pLink->getAtomicNode(), bcMemoryOrder_Relaxed));
				Node* lResult = reinterpret_cast< Node* >(reinterpret_cast<bcINT32>(lValue) | dMask);

				if(bcAtomicOperation::bcAtomicCompareExchangeStrong(
				*pLink->getAtomicNode(),
				lValue,
				lResult,
				bcMemoryOrder_Relaxed,
				bcMemoryOrder_Relaxed))
				break;
				}*/

				while (bcTRUE)
				{
					LinkType lOld = *pLink;
					if (lOld.del() || pLink->compareAndSwap(lOld, LinkType(lOld.pointer(), bcTRUE)))
						break;
				}
			}

			bcInline bcBOOL _next(NodeType** pNode)
			{
				while (bcTRUE)
				{
					NodeType* lNode = *pNode;

					if (lNode == mTail)
					{
						bcAssert(bcFALSE);
						return bcFALSE;
					}

					NodeType* lNext = mMemMng.deRefLink(&lNode->mNext);
					bcBOOL del = lNext->mNext.del();

					if (del == bcTRUE && lNode->mNext != LinkType(lNext, bcTRUE))
					{
						_setMark(&lNext->mPrev);
						mMemMng.compareAndSwapRef(&lNode->mNext, &LinkType(lNext), &LinkType(lNext->mNext.pointer()));
						mMemMng.releaseRef(lNext);
						continue;
					}

					mMemMng.releaseRef(lNode);
					*pNode = lNext;

					if (!del /*&& lNext != mTail*/)
						return bcTRUE;
				}
			}
			bcInline bcBOOL _prev(NodeType** pNode)
			{
				while (bcTRUE)
				{
					NodeType* lNode = *pNode;

					if (lNode == mHead)
					{
						bcAssert(bcFALSE);
						return bcFALSE;
					}

					NodeType* lPrev = mMemMng.deRefLink(&lNode->mPrev);
					if (lPrev->mNext == LinkType(lNode, bcFALSE) && lNode->mNext.del() == bcFALSE)
					{
						mMemMng.releaseRef(lNode);
						*pNode = lPrev;
						return bcTRUE;
						/*if (lPrev != mHead)
						return bcTRUE;*/
					}
					else if (lNode->mNext.del())
					{
						mMemMng.releaseRef(lPrev);
						_next(pNode);
					}
					else
					{
						lPrev = _correctPrev(lPrev, *pNode);
						mMemMng.releaseRef(lPrev);
					}
				}
			}

			bcInline bcBOOL _read(NodeType* pNode, bcNullable< ValueType >* pData)
			{
				bcAssert(pNode != mHead || pNode != mTail);

				*pData = pNode->mValue;

				if (!pNode->mNext.del())
					return bcTRUE;

				pData = bcNULL;
				return bcFALSE;
			}

			bcInline void _insertBefore(NodeType* pNext, NodeType* pNode)
			{
				if (pNext == mHead)
					return _insertAfter(pNext, pNode);

				NodeType* lPrev = mMemMng.deRefLink(&pNext->mPrev);
				NodeType* lNext = pNext;
				mBackOff.reset();
				while (bcTRUE)
				{
					while (pNext->mNext.del())
					{
						_next(&pNext);
						lPrev = _correctPrev(lPrev, pNext);
					}

					mMemMng.storeRef(&pNode->mPrev, &LinkType(lPrev, bcFALSE));
					mMemMng.storeRef(&pNode->mNext, &LinkType(lNext, bcFALSE));

					if (mMemMng.compareAndSwapRef(&lPrev->mNext, &LinkType(pNext, bcFALSE), &LinkType(pNode, bcFALSE)))
						break;

					lPrev = _correctPrev(lPrev, pNext);
					mBackOff.inc();
					mBackOff.wait();
				}

				lPrev = _correctPrev(lPrev, lNext);
				mMemMng.releaseRef(lPrev);
				mMemMng.releaseRef(lNext);
			}
			bcInline void _insertAfter(NodeType* pPrev, NodeType* pNode)
			{
				if (pPrev == mTail)
					return _insertBefore(pPrev, pNode);

				NodeType* lPrev = pPrev;
				NodeType* lNext;
				mBackOff.reset();
				while (bcTRUE)
				{
					lNext = mMemMng.deRefLink(&lPrev->mNext);

					mMemMng.storeRef(&pNode->mPrev, &LinkType(lPrev, bcFALSE));
					mMemMng.storeRef(&pNode->mNext, &LinkType(lNext, bcFALSE));

					if (mMemMng.compareAndSwapRef(&pPrev->mNext, &LinkType(lNext, bcFALSE), &LinkType(pNode, bcFALSE)))
						break;

					mMemMng.releaseRef(lNext);

					if (lPrev->mNext.del())
					{
						return _insertBefore(pPrev, pNode);
					}

					mBackOff.inc();
					mBackOff.wait();
				}

				lPrev = _correctPrev(lPrev, lNext);
				mMemMng.releaseRef(lPrev);
				mMemMng.releaseRef(lNext);
			}

			// == Delete node and move pointer forward or backward ==
			bcInline bcBOOL _delete(Node** pNode, bcNullable< ValueType >* pData = bcNULL, bcBOOL pMoveForward = bcTRUE)
			{
				NodeType* lNode = *pNode;

				bcAssert(lNode != mHead || lNode != mTail);

				while (bcTRUE)
				{
					bcBOOL lNextDel = (*pNode)->mNext.del();
					NodeType* lNext = mMemMng.deRefLink(&(*pNode)->mNext);

					if (lNextDel)
					{
						mMemMng.releaseRef(lNext);
						if (pData != bcNULL) *pData = bcNULL;
						return bcFALSE;
					}

					if (lNode->mNext.compareAndSwap(lNext, LinkType(lNext, bcTRUE)))
					{
						NodeType* lPrev;
						while (bcTRUE)
						{
							bcBOOL lPrevDel = lNode->mPrev.del();
							lPrev = mMemMng.deRefLink(&lNode->mPrev);
							if (lPrevDel || mMemMng.compareAndSwapRef(&lNode->mPrev, &LinkType(lPrev), &LinkType(lPrev, bcTRUE)))
								break;
							mMemMng.releaseRef(lPrev);
						}
						lPrev = _correctPrev(lPrev, lNext);
						mMemMng.releaseRef(lPrev);
						mMemMng.releaseRef(lNext);

						//return value;
						if (pData)
							*pData = std::move((*pNode)->mValue);

						// Because we use _next or _prev, this functions will release reference to current node
						// and in deleteNode we need to have a reference to deletion node, so add an additional
						// reference to node here, and release it in deleteNode function
						mMemMng.ref(*pNode);
						//releaseRef(pNode);
						if (pMoveForward)
							_next(pNode);
						else
							_prev(pNode);

						mMemMng.deleteNode(lNode);

						return bcTRUE;
					}
				}
			}

			bcInline NodeType* _correctPrev(NodeType* pPrev, NodeType* pNode)
			{
				NodeType* lPrev = pPrev;
				NodeType* lLastLink = bcNULL;
				mBackOffCP.reset();
				while (bcTRUE)
				{
					LinkType lLink = pNode->mPrev;
					if (lLink.del())
					{
						//// Node was deleted while correcting, prev may have advanced past node, so undo the last step
						//if (lLastLink != bcNULL)
						//{
						//	mMemMng.releaseRef(lPrev);
						//	lPrev = lLastLink;
						//	lLastLink = bcNULL;
						//}
						break;
					}

					bcBOOL lPrev2Del = lPrev->mNext.del();
					NodeType* lPrev2 = mMemMng.deRefLink(&lPrev->mNext);
					if (lPrev2Del)
					{
						if (lLastLink != bcNULL)
						{
							_setMark(&lPrev->mPrev);
							mMemMng.compareAndSwapRef(&lLastLink->mNext, &LinkType(lPrev), &LinkType(lPrev2, bcFALSE));

							mMemMng.releaseRef(lPrev2);
							mMemMng.releaseRef(lPrev);

							lPrev = lLastLink;
							lLastLink = bcNULL;

							continue;
						}

						mMemMng.releaseRef(lPrev2);
						lPrev2 = mMemMng.deRefLink(&lPrev->mPrev);
						mMemMng.releaseRef(lPrev);
						lPrev = lPrev2;
						continue;
					}
					if (lPrev2 != pNode)
					{
						if (lLastLink != bcNULL)
							mMemMng.releaseRef(lLastLink);
						lLastLink = lPrev;
						lPrev = lPrev2;
						continue;
					}

					mMemMng.releaseRef(lPrev2);
					if (mMemMng.compareAndSwapRef(&pNode->mPrev, &lLink, &LinkType(lPrev, bcFALSE)))
					{
						if (lPrev->mPrev.del())
							continue;
						break;
					}

					mBackOffCP.inc();
					mBackOffCP.wait();
				}

				if (lLastLink != bcNULL)
					mMemMng.releaseRef(lLastLink);

				return lPrev;
			}

		protected:
			bcInline NodeType* _insertBefore(NodeType* pNext, ValueType& pValue)
			{
				NodeType* lNode = _createNode(pValue);
				_insertBefore(pNext, lNode);
				mMemMng.releaseRef(lNode);

				bcAtomicOperation::bcAtomicFetchAdd(mSize, 1U, bcMemoryOrder_SeqCst);

				return lNode;
			}
			bcInline NodeType* _insertBefore(NodeType* pNext, ValueType&& pValue)
			{
				NodeType* lNode = _createNode(pValue);
				_insertBefore(pNext, lNode);
				mMemMng.releaseRef(lNode);

				bcAtomicOperation::bcAtomicFetchAdd(mSize, 1U, bcMemoryOrder_SeqCst);

				return lNode;
			}
			bcInline NodeType* _insertAfter(NodeType* pPrev, ValueType& pValue)
			{
				NodeType* lNode = _createNode(pValue);
				_insertAfter(pPrev, lNode);
				mMemMng.releaseRef(lNode);

				bcAtomicOperation::bcAtomicFetchAdd(mSize, 1U, bcMemoryOrder_SeqCst);

				return lNode;
			}
			bcInline NodeType* _insertAfter(NodeType* pPrev, ValueType&& pValue)
			{
				NodeType* lNode = _createNode(pValue);
				_insertAfter(pPrev, lNode);
				mMemMng.releaseRef(lNode);

				bcAtomicOperation::bcAtomicFetchAdd(mSize, 1U, bcMemoryOrder_SeqCst);

				return lNode;
			}

			bcInline bcBOOL _erase(NodeType** pNode, bcNullable< ValueType >* pData)
			{
				bcBOOL lDeleted = _delete(pNode, pData);
				if (lDeleted)
					bcAtomicOperation::bcAtomicFetchSub(mSize, 1U, bcMemoryOrder_SeqCst);

				return lDeleted;
			}

			bcInline bcBOOL _moveForward(NodeType** pNode) { return _next(pNode); }
			bcInline bcBOOL _moveBackWard(NodeType** pNode) { return _prev(pNode); }

			bcLockFreeMemMng< ValueType, NodeType, LinkType, Allocator> mMemMng;

			NodeType* mHead;
			NodeType* mTail;

			bcBackOff mBackOff;
			bcBackOff mBackOffCP;

		public:
			bcConcurrectListBase(bcUINT32 pNumThread, bcUINT32 pMaxIterator) :
				bcLockFreeContainerMemMngConfig(pNumThread, 2, 2, 5 + pMaxIterator),
				mMemMng(this),
				mBackOff(pNumThread),
				mBackOffCP(pNumThread)
			{
				mHead = _createNode(ValueType());
				mTail = _createNode(ValueType());
				//mMemMng.storeRef(mHead, _createNode(Type()));
				//mMemMng.storeRef(mTail, _createNode(Type()));
				mMemMng.storeRef(&mHead->mNext, &LinkType(mTail));
				mMemMng.storeRef(&mHead->mPrev, &LinkType(mTail));
				mMemMng.storeRef(&mTail->mNext, &LinkType(mHead));
				mMemMng.storeRef(&mTail->mPrev, &LinkType(mHead));
				mMemMng.releaseRef(mHead);
				mMemMng.releaseRef(mTail);
			}
			~bcConcurrectListBase()
			{
				//clear();
				mMemMng.ref(mHead);
				mMemMng.ref(mTail);
				mMemMng.deleteNode(mHead);
				mMemMng.deleteNode(mTail);

				mMemMng.forceCleanUp();
			}

			ValueType& nodeValue(IteratorProviderNodeBaseType* pNode) override
			{
				return (static_cast<NodeType*>(pNode))->mValue;
			}
			IteratorProviderNodeBaseType* moveForward(IteratorProviderNodeBaseType* pNode) override
			{
				NodeType* lNode = static_cast<NodeType*>(pNode);
				_moveForward(&lNode);
				return lNode;
			}
			IteratorProviderNodeBaseType* moveForward(IteratorProviderNodeBaseType* pNode, bcUINT32 pCount) override
			{
				NodeType* lNode = static_cast<NodeType*>(pNode);
				for (bcUINT32 i = 0; i < pCount; ++i)
				{
					_moveForward(&lNode);
				}
				return lNode;
			}
			IteratorProviderNodeBaseType* moveBackward(IteratorProviderNodeBaseType* pNode) override
			{
				NodeType* lNode = static_cast<NodeType*>(pNode);
				_moveBackWard(&lNode);
				return lNode;
			}
			IteratorProviderNodeBaseType* moveBackward(IteratorProviderNodeBaseType* pNode, bcUINT32 pCount) override
			{
				NodeType* lNode = static_cast<NodeType*>(pNode);
				for (bcUINT32 i = 0; i < pCount; ++i)
				{
					_moveBackWard(&lNode);
				}
				return lNode;
			}
			bcBOOL isValid(const IteratorProviderNodeBaseType* pNode) const override
			{
				const NodeType* lNode = static_cast<const NodeType*>(pNode);
				return (lNode && !lNode->mNext.del() && lNode != mHead && lNode != mTail);
			}

			void referenceIterator(bcConcurrentIteratorBase<ValueType>* pIte) override
			{
				NodeType* lNode = static_cast<NodeType*>(pIte->getNode());
				mMemMng.ref(lNode);
			}
			void releaseIterator(bcConcurrentIteratorBase<ValueType>* pIte) override
			{
				NodeType* lNode = static_cast<NodeType*>(pIte->getNode());
				mMemMng.releaseRef(lNode);
			}
		};

		template< typename T, typename Allocator = bcContainerAllocation<> >
		class bcConcurrentList : public bcConcurrectListBase< T, Allocator >
		{
		public:
			typedef bcConcurrentList < T, Allocator > ThisType;
			typedef bcIteratorBase<T> IteratorBaseType;
			typedef bcConcurrentIterator< T > IteratorType;
			typedef bcConstConcurrentIterator< T > ConstIteratorType;
			typedef bcReference< T > ReferenceType;
			typedef bcConstReference< T > ConstReferenceType;

		private:

		protected:

		public:
			bcConcurrentList(bcUINT32 pNumThread, bcUINT32 pMaxIterator) : bcConcurrectListBase(pNumThread, pMaxIterator) {}
			~bcConcurrentList() { clear(); }

			IteratorType pushBack()
			{
				// Because in _insertBefore we release next, we need an additional reference to mTail
				mMemMng.ref(mTail);
				return IteratorType(_insertBefore(mTail, ValueType()), this);
			}
			IteratorType pushBack(ValueType& pValue)
			{
				// Because in _insertBefore we release next, we need an additional reference to mTail
				mMemMng.ref(mTail);
				return IteratorType(_insertBefore(mTail, pValue), this);
			}
			IteratorType pushBack(ValueType&& pValue)
			{
				// Because in _insertBefore we release next, we need an additional reference to mTail
				mMemMng.ref(mTail);
				return IteratorType(_insertBefore(mTail, pValue), this);
			}
			IteratorType pushFront()
			{
				// Because in _insertAfter we release prev, we need an additional reference to mHead
				mMemMng.ref(mHead);
				return IteratorType(_insertAfter(mHead, ValueType()), this);
			}
			IteratorType pushFront(ValueType& pValue)
			{
				// Because in _insertAfter we release prev, we need an additional reference to mHead
				mMemMng.ref(mHead);
				return IteratorType(_insertAfter(mHead, pValue), this);
			}
			IteratorType pushFront(ValueType&& pValue)
			{
				// Because in _insertAfter we release prev, we need an additional reference to mHead
				mMemMng.ref(mHead);
				return IteratorType(_insertAfter(mHead, pValue), this);
			}

			void popBack(bcNullable< ValueType >* pData = bcNULL)
			{
				NodeType* lNode = mMemMng.deRefLink(mTail->mPrev);
				_erase(&lNode, pData);
				mMemMng.releaseRef(lNode)
			}
			void popFront(bcNullable< ValueType >* pData = bcNULL)
			{
				NodeType* lNode = mMemMng.deRefLink(mHead->mNext);
				_erase(&lNode, pData);
				mMemMng.releaseRef(lNode);
			}

			IteratorType insertAfter(IteratorType& pWhere, ValueType& pValue)
			{
				// Because in _insertAfter we release prev, we need an additional reference to mPointer
				NodeType* lNode = static_cast<NodeType*>(pWhere.getNode());
				mMemMng.ref(lNode);
				return IteratorType(_insertAfter(lNode, pValue), this);
			}
			IteratorType insertAfter(IteratorType& pWhere, ValueType&& pValue)
			{
				// Because in _insertAfter we release prev, we need an additional reference to mPointer
				NodeType* lNode = static_cast<NodeType*>(pWhere.getNode());
				mMemMng.ref(lNode);
				return IteratorType(_insertAfter(lNode, pValue), this);
			}
			IteratorType insertBefore(IteratorType& pWhere, ValueType& pValue)
			{
				// Because in _insertBefore we release next, we need an additional reference to mPointer
				NodeType* lNode = static_cast<NodeType*>(pWhere.getNode());
				mMemMng.ref(lNode);
				return IteratorType(insertBefore(lNode, pValue), this);
			}
			IteratorType insertBefore(IteratorType& pWhere, ValueType&& pValue)
			{
				// Because in _insertBefore we release next, we need an additional reference to mPointer
				NodeType* lNode = static_cast<NodeType*>(pWhere.getNode());
				mMemMng.ref(lNode);
				return IteratorType(insertBefore(lNode, pValue), this);
			}

			void erase(IteratorBaseType& pIte, bcNullable< ValueType >* pData = bcNULL)
			{
				NodeType* lNode = static_cast<NodeType*>(pIte.getNode());
				_erase(&lNode, pData);
				pIte.setNode(lNode);
				//pIte.clear();
			}

			IteratorType begin()
			{
				NodeType* lNode = mMemMng.deRefLink(&mHead->mNext);
				IteratorType lIte(lNode, this);
				mMemMng.releaseRef(lNode);

				return lIte;
			}
			IteratorType end()
			{
				return IteratorType(mTail, this);
			}
			/*IteratorType createIterator(NodeType* pNode)
			{
				return IteratorType(pNode, this);
			}*/
			/*void refIterator(IteratorType& pIte)
			{
				mMemMng.ref(pIte.mPointer);
			}
			void releaseIterator(IteratorType& pIte)
			{
				mMemMng.releaseRef(pIte.mPointer);
			}*/
			IteratorType createIteratorFromReference(ReferenceType pReference)
			{
				bcIterator lIterator(pReference.getNode(), this);
#ifdef BC_CONTAINER_DEBUG
				bcAssert(lIterator.validate());
#endif
				return lIterator;
			}

			void clear()
			{
				for (auto lIte = begin(); lIte != end();)
					erase(lIte);
			}
		};
	}
}