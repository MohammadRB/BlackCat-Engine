//  [16/12/2013 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "Core/CorePCH.h"
#include "Core/Utility/bcInitializable.h"
#include "Core/Concurrency/bcLockFreeMemMng.h"
#include "Core/Concurrency/bcConcurrencyUtility.h"
#include "Core/Container/bcContainer.h"
//#include "bcIterator.h"

namespace black_cat
{
	namespace core
	{
		// Based on the paper: "Lock-free deques and doubly linked lists", Sundell, et al. - 2008
		template< typename T, typename Allocator >
		class bcConcurrectListBase : public bc_concurrent_container<T, Allocator>, public bcConcurrentIteratorProvider<T>, protected bcLockFreeContainerMemMngConfig<T>
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

					Link() noexcept(true) { mPointer.store((Node*)nullptr, platform::bc_memory_order::seqcst); }
					Link(const Node* pPointer, bool pDel = false) noexcept(true)
					{
						Node* lPointer = reinterpret_cast<Node*>(reinterpret_cast<bcINT32>(pPointer)& ptrMask); // Reset pointer flags
						mPointer.store(reinterpret_cast<Node*>((reinterpret_cast<bcINT32>(lPointer) | (bcINT32)pDel)),
							platform::bc_memory_order::seqcst);
					}

					Link(const Link& pOther)
					{
						mPointer.store(pOther.mPointer.load(platform::bc_memory_order::seqcst),
							platform::bc_memory_order::seqcst);
					}

					BC_INLINE Node* pointer() const noexcept(true)
					{
						return reinterpret_cast<Node*>(
							reinterpret_cast<bcINT32>(mPointer.load(platform::bc_memory_order::seqcst)) & ptrMask);
					}
					BC_INLINE bool del() const noexcept(true)
					{
						return reinterpret_cast<bcINT32>(mPointer.load(platform::bc_memory_order::seqcst)) & dMask;
					}
					BC_INLINE NodeType* getNode() noexcept(true) override
					{
						return reinterpret_cast<Node*>(
							reinterpret_cast<bcINT32>(mPointer.load(platform::bc_memory_order::seqcst)) & ptrMask);
					}

					BC_INLINE bool compareAndSwap(const Link& pExpected, const Link& pNew) noexcept(true)
					{
						Node* lExpected = pExpected.mPointer.load(platform::bc_memory_order::seqcst);
						Node* lNew = pNew.mPointer.load(platform::bc_memory_order::seqcst);

						if (mPointer.compare_exchange_strong(
							&lExpected,
							lNew,
							platform::bc_memory_order::seqcst,
							platform::bc_memory_order::seqcst))
						{
							//TODO check here
							Node* lNewExpected = pExpected.mPointer.load(platform::bc_memory_order::seqcst);
							Node* lNewNew = pNew.mPointer.load(platform::bc_memory_order::seqcst);

							BC_ASSERT(lNewExpected == lExpected && lNewNew == lNew);

							return true;
						}

						return false;
					}

					bool operator==(const Link& pOther) noexcept(true)
					{
						return mPointer.load(platform::bc_memory_order::seqcst) ==
							pOther.mPointer.load(platform::bc_memory_order::seqcst);
					}
					bool operator!=(const Link& pOther) noexcept(true)
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
			void _terminateNode(NodeBaseType* pNode, bool pConcurrent) override
			{
				NodeType* lNode = static_cast<NodeType*>(pNode);

				if (!pConcurrent)
				{
					mMemMng.storeRef(&lNode->mPrev, &LinkType(nullptr, true));
					mMemMng.storeRef(&lNode->mNext, &LinkType(nullptr, true));
				}
				else // TODO check here
				{
					while (true)
					{
						LinkType* lPrev = &lNode->mPrev;
						if (mMemMng.compareAndSwapRef(&lNode->mPrev, lPrev, &LinkType(nullptr, true)))
							break;
					}
					while (true)
					{
						LinkType* lNext = &lNode->mNext;
						if (mMemMng.compareAndSwapRef(&lNode->mNext, lNext, &LinkType(nullptr, true)))
							break;
					}
				}
			}
			void _cleanUpNode(NodeBaseType* pNode) override
			{
				NodeType* lNode = static_cast<NodeType*>(pNode);

				while (true)
				{
					NodeType* lPrev = mMemMng.deRefLink(&lNode->mPrev);
					//if (!lPrev) break;
					// Release Ref even if lPrev is nullptr, because we must release allocated index from FreeIndexList
					// in MemMng
					if (!lPrev || !lPrev->mPrev.del())
					{
						mMemMng.releaseRef(lPrev);
						break;
					}
					NodeType* lPrev2 = mMemMng.deRefLink(&lPrev->mPrev);
					mMemMng.compareAndSwapRef(&lNode->mPrev, &LinkType(lPrev, true), &LinkType(lPrev2, true));
					mMemMng.releaseRef(lPrev2);
					mMemMng.releaseRef(lPrev);
				}

				while (true)
				{
					NodeType* lNext = mMemMng.deRefLink(&lNode->mNext);
					//if (!lNext) break;
					if (!lNext || !lNext->mNext.del())
					{
						mMemMng.releaseRef(lNext);
						break;
					}
					NodeType* lNext2 = mMemMng.deRefLink(&lNext->mNext);
					mMemMng.compareAndSwapRef(&lNode->mNext, &LinkType(lNext, true), &LinkType(lNext2, true));
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
				lNode->~node();
				mAllocator.free(lNode);
			}

			BC_INLINE NodeType* _createNode(const ValueType& pValue)
			{
				NodeType* lNode = mMemMng.newNode();
				BC_ASSERT((reinterpret_cast<bcINT32>(lNode)& LinkType::dMask) == 0);
				new (lNode)NodeType(pValue);

				return lNode;
			}
			BC_INLINE NodeType* _createNode(ValueType&& pValue)
			{
				NodeType* lNode = mMemMng.newNode();
				BC_ASSERT((reinterpret_cast<bcINT32>(lNode)& LinkType::dMask) == 0);
				new (lNode)NodeType(pValue);

				return lNode;
			}
			BC_INLINE void _setMark(LinkType* pLink)
			{
				/*while(true)
				{
				Node* lValue = (bcAtomicOperation::bcAtomicLoad(*pLink->getAtomicNode(), platform::bc_memory_order::relaxed));
				Node* lResult = reinterpret_cast< Node* >(reinterpret_cast<bcINT32>(lValue) | dMask);

				if(bcAtomicOperation::bcAtomicCompareExchangeStrong(
				*pLink->getAtomicNode(),
				lValue,
				lResult,
				platform::bc_memory_order::relaxed,
				platform::bc_memory_order::relaxed))
				break;
				}*/

				while (true)
				{
					LinkType lOld = *pLink;
					if (lOld.del() || pLink->compareAndSwap(lOld, LinkType(lOld.pointer(), true)))
						break;
				}
			}

			BC_INLINE bool _next(NodeType** pNode)
			{
				while (true)
				{
					NodeType* lNode = *pNode;

					if (lNode == mTail)
					{
						BC_ASSERT(false);
						return false;
					}

					NodeType* lNext = mMemMng.deRefLink(&lNode->mNext);
					bool del = lNext->mNext.del();

					if (del == true && lNode->mNext != LinkType(lNext, true))
					{
						_setMark(&lNext->mPrev);
						mMemMng.compareAndSwapRef(&lNode->mNext, &LinkType(lNext), &LinkType(lNext->mNext.pointer()));
						mMemMng.releaseRef(lNext);
						continue;
					}

					mMemMng.releaseRef(lNode);
					*pNode = lNext;

					if (!del /*&& lNext != mTail*/)
						return true;
				}
			}
			BC_INLINE bool _prev(NodeType** pNode)
			{
				while (true)
				{
					NodeType* lNode = *pNode;

					if (lNode == mHead)
					{
						BC_ASSERT(false);
						return false;
					}

					NodeType* lPrev = mMemMng.deRefLink(&lNode->mPrev);
					if (lPrev->mNext == LinkType(lNode, false) && lNode->mNext.del() == false)
					{
						mMemMng.releaseRef(lNode);
						*pNode = lPrev;
						return true;
						/*if (lPrev != mHead)
						return true;*/
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

			BC_INLINE bool _read(NodeType* pNode, bcNullable< ValueType >* pData)
			{
				BC_ASSERT(pNode != mHead || pNode != mTail);

				*pData = pNode->mValue;

				if (!pNode->mNext.del())
					return true;

				pData = nullptr;
				return false;
			}

			BC_INLINE void _insertBefore(NodeType* pNext, NodeType* pNode)
			{
				if (pNext == mHead)
					return _insertAfter(pNext, pNode);

				NodeType* lPrev = mMemMng.deRefLink(&pNext->mPrev);
				NodeType* lNext = pNext;
				mBackOff.reset();
				while (true)
				{
					while (pNext->mNext.del())
					{
						_next(&pNext);
						lPrev = _correctPrev(lPrev, pNext);
					}

					mMemMng.storeRef(&pNode->mPrev, &LinkType(lPrev, false));
					mMemMng.storeRef(&pNode->mNext, &LinkType(lNext, false));

					if (mMemMng.compareAndSwapRef(&lPrev->mNext, &LinkType(pNext, false), &LinkType(pNode, false)))
						break;

					lPrev = _correctPrev(lPrev, pNext);
					mBackOff.inc();
					mBackOff.wait();
				}

				lPrev = _correctPrev(lPrev, lNext);
				mMemMng.releaseRef(lPrev);
				mMemMng.releaseRef(lNext);
			}
			BC_INLINE void _insertAfter(NodeType* pPrev, NodeType* pNode)
			{
				if (pPrev == mTail)
					return _insertBefore(pPrev, pNode);

				NodeType* lPrev = pPrev;
				NodeType* lNext;
				mBackOff.reset();
				while (true)
				{
					lNext = mMemMng.deRefLink(&lPrev->mNext);

					mMemMng.storeRef(&pNode->mPrev, &LinkType(lPrev, false));
					mMemMng.storeRef(&pNode->mNext, &LinkType(lNext, false));

					if (mMemMng.compareAndSwapRef(&pPrev->mNext, &LinkType(lNext, false), &LinkType(pNode, false)))
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

			// Delete node and move pointer forward or backward /
			BC_INLINE bool _delete(Node** pNode, bcNullable< ValueType >* pData = nullptr, bool pMoveForward = true)
			{
				NodeType* lNode = *pNode;

				BC_ASSERT(lNode != mHead || lNode != mTail);

				while (true)
				{
					bool lNextDel = (*pNode)->mNext.del();
					NodeType* lNext = mMemMng.deRefLink(&(*pNode)->mNext);

					if (lNextDel)
					{
						mMemMng.releaseRef(lNext);
						if (pData != nullptr) *pData = nullptr;
						return false;
					}

					if (lNode->mNext.compareAndSwap(lNext, LinkType(lNext, true)))
					{
						NodeType* lPrev;
						while (true)
						{
							bool lPrevDel = lNode->mPrev.del();
							lPrev = mMemMng.deRefLink(&lNode->mPrev);
							if (lPrevDel || mMemMng.compareAndSwapRef(&lNode->mPrev, &LinkType(lPrev), &LinkType(lPrev, true)))
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

						return true;
					}
				}
			}

			BC_INLINE NodeType* _correctPrev(NodeType* pPrev, NodeType* pNode)
			{
				NodeType* lPrev = pPrev;
				NodeType* lLastLink = nullptr;
				mBackOffCP.reset();
				while (true)
				{
					LinkType lLink = pNode->mPrev;
					if (lLink.del())
					{
						//// Node was deleted while correcting, prev may have advanced past node, so undo the last step
						//if (lLastLink != nullptr)
						//{
						//	mMemMng.releaseRef(lPrev);
						//	lPrev = lLastLink;
						//	lLastLink = nullptr;
						//}
						break;
					}

					bool lPrev2Del = lPrev->mNext.del();
					NodeType* lPrev2 = mMemMng.deRefLink(&lPrev->mNext);
					if (lPrev2Del)
					{
						if (lLastLink != nullptr)
						{
							_setMark(&lPrev->mPrev);
							mMemMng.compareAndSwapRef(&lLastLink->mNext, &LinkType(lPrev), &LinkType(lPrev2, false));

							mMemMng.releaseRef(lPrev2);
							mMemMng.releaseRef(lPrev);

							lPrev = lLastLink;
							lLastLink = nullptr;

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
						if (lLastLink != nullptr)
							mMemMng.releaseRef(lLastLink);
						lLastLink = lPrev;
						lPrev = lPrev2;
						continue;
					}

					mMemMng.releaseRef(lPrev2);
					if (mMemMng.compareAndSwapRef(&pNode->mPrev, &lLink, &LinkType(lPrev, false)))
					{
						if (lPrev->mPrev.del())
							continue;
						break;
					}

					mBackOffCP.inc();
					mBackOffCP.wait();
				}

				if (lLastLink != nullptr)
					mMemMng.releaseRef(lLastLink);

				return lPrev;
			}

		protected:
			BC_INLINE NodeType* _insertBefore(NodeType* pNext, ValueType& pValue)
			{
				NodeType* lNode = _createNode(pValue);
				_insertBefore(pNext, lNode);
				mMemMng.releaseRef(lNode);

				mSize.fetch_add(1U, platform::bc_memory_order::seqcst);

				return lNode;
			}
			BC_INLINE NodeType* _insertBefore(NodeType* pNext, ValueType&& pValue)
			{
				NodeType* lNode = _createNode(pValue);
				_insertBefore(pNext, lNode);
				mMemMng.releaseRef(lNode);

				mSize.fetch_add(1U, platform::bc_memory_order::seqcst);

				return lNode;
			}
			BC_INLINE NodeType* _insertAfter(NodeType* pPrev, ValueType& pValue)
			{
				NodeType* lNode = _createNode(pValue);
				_insertAfter(pPrev, lNode);
				mMemMng.releaseRef(lNode);

				mSize.fetch_add(1U, platform::bc_memory_order::seqcst);

				return lNode;
			}
			BC_INLINE NodeType* _insertAfter(NodeType* pPrev, ValueType&& pValue)
			{
				NodeType* lNode = _createNode(pValue);
				_insertAfter(pPrev, lNode);
				mMemMng.releaseRef(lNode);

				mSize.fetch_add(1U, platform::bc_memory_order::seqcst);

				return lNode;
			}

			BC_INLINE bool _erase(NodeType** pNode, bcNullable< ValueType >* pData)
			{
				bool lDeleted = _delete(pNode, pData);
				if (lDeleted)
					mSize.fetch_sub(1U, platform::bc_memory_order::seqcst);

				return lDeleted;
			}

			BC_INLINE bool _moveForward(NodeType** pNode) { return _next(pNode); }
			BC_INLINE bool _moveBackWard(NodeType** pNode) { return _prev(pNode); }

			bcLockFreeMemMng< ValueType, NodeType, LinkType, Allocator> mMemMng;

			NodeType* mHead;
			NodeType* mTail;

			bc_back_off mBackOff;
			bc_back_off mBackOffCP;

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
			bool isValid(const IteratorProviderNodeBaseType* pNode) const override
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

			void popBack(bcNullable< ValueType >* pData = nullptr)
			{
				NodeType* lNode = mMemMng.deRefLink(mTail->mPrev);
				_erase(&lNode, pData);
				mMemMng.releaseRef(lNode);
			}
			void popFront(bcNullable< ValueType >* pData = nullptr)
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

			void erase(IteratorBaseType& pIte, bcNullable< ValueType >* pData = nullptr)
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
				BC_ASSERT(lIterator.validate());
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