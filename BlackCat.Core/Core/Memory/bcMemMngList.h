//  [10/25/2013 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "Core/CorePCH.h"
#include "Core/Concurrency/bcLockFreeMemMng.h"
#include "Core/Concurrency/bcConcurrencyUtility.h"
#include "Core/bcCoreUtility.h"

namespace black_cat
{
	namespace core
	{
#ifdef BC_MEMORY_ENABLE

		// Based on the paper: "Lock-free deques and doubly linked lists", Sundell, et al. - 2008
		template< typename T >
		class bcMemMngList : protected bcLockFreeContainerMemMngConfig<T>
		{
		public:
			typedef T Type;
			//typedef Allocator AllocatorType;
			typedef bcLockFreeNode<Type> NodeBaseType;

			struct Node : public NodeBaseType
			{
			public:
				struct Link : public bcLockFreeLink< Node >
				{
				public:
					static const bcINT32 dMask = 1;
					static const bcINT32 ptrMask = ~dMask;

					Link() noexcept(true) { mPointer.store((Node*)nullptr, core_platform::bc_memory_order::seqcst); }
					Link(const Node* pPointer, bool pDel = false) noexcept(true)
					{
						Node* lPointer = reinterpret_cast<Node*>(reinterpret_cast<bcINT32>(pPointer) & ptrMask); // Reset pointer flags
						mPointer.store(reinterpret_cast<Node*>((reinterpret_cast<bcINT32>(lPointer) | (bcINT32)pDel)),
							core_platform::bc_memory_order::seqcst);
					}

					Link(const Link& pOther)
					{
						mPointer.store(pOther.mPointer.load(core_platform::bc_memory_order::seqcst),
							core_platform::bc_memory_order::seqcst);
					}

					bcInline Node* pointer() const noexcept(true)
					{
						return reinterpret_cast<Node*>(reinterpret_cast<bcINT32>(mPointer.load(core_platform::bc_memory_order::seqcst)) & ptrMask);
					}
					bcInline bool del() const noexcept(true)
					{
						return reinterpret_cast<bcINT32>(mPointer.load(core_platform::bc_memory_order::seqcst)) & dMask;
					}
					bcInline NodeType* getNode() noexcept(true) override
					{
						return reinterpret_cast<Node*>(reinterpret_cast<bcINT32>(mPointer.load(core_platform::bc_memory_order::seqcst)) & ptrMask);
					}

					bcInline bool compareAndSwap(const Link& pExpected, const Link& pNew) noexcept(true)
					{
						Node* lExpected = pExpected.mPointer.load(core_platform::bc_memory_order::seqcst);
						Node* lNew = pNew.mPointer.load(core_platform::bc_memory_order::seqcst);

						if (mPointer.compare_exchange_strong(
							&lExpected,
							lNew,
							core_platform::bc_memory_order::seqcst,
							core_platform::bc_memory_order::seqcst))
						{
							//TODO check here
							Node* lNewExpected = pExpected.mPointer.load(core_platform::bc_memory_order::seqcst);
							Node* lNewNew = pNew.mPointer.load(core_platform::bc_memory_order::seqcst);

							bcAssert(lNewExpected == lExpected && lNewNew == lNew);

							return true;
						}

						return false;
					}

					bool operator==(const Link& pOther) noexcept(true)
					{
						return mPointer.load(core_platform::bc_memory_order::seqcst) ==
							pOther.mPointer.load(core_platform::bc_memory_order::seqcst);
					}
					bool operator!=(const Link& pOther) noexcept(true)
					{
						return !operator==(pOther);
					}
				};

				//Type mValue;
				Link mPrev;
				Link mNext;

				Node() {};
				Node(const Type& pValue) : bcLockFreeNode(pValue) {};
				Node(Type&& pValue) : bcLockFreeNode(std::move(pValue)) {};
				~Node() {}
			};

			typedef Node NodeType;
			typedef typename NodeType::Link LinkType;

			class MemMngAllocator
			{
			public:
				void* alloc(bcSIZE pSize) { return std::malloc(pSize); }
				void free(void* pMemory) { std::free(pMemory); }
			};

#pragma region MemConfig
			//class MemoryConfig : public bcLockFreeContainerMemMngConfig<NodeType, LinkType>
			//{
			//private:

			//protected:
			//	AllocatorType mAllocator;

			//public:
			//	MemoryConfig(
			//		bcUINT32 pNumThread,
			//		bcUINT32 pNumLink,
			//		bcUINT32 pNumLinkDel,
			//		bcUINT32 pNumHP) : bcLockFreeContainerMemMngConfig(pNumThread, pNumLink, pNumLinkDel, pNumHP) {}
			//	~MemoryConfig() {}

			//	void _terminateNode(NodeType* pNode, bool pConcurrent) override
			//	{
			//		NodeType* lNode = static_cast<NodeType*>(pNode);

			//		if (!pConcurrent)
			//		{
			//			mMemMng.storeRef(&lNode->mPrev, LinkType(nullptr, true));
			//			mMemMng.storeRef(&lNode->mNext, LinkType(nullptr, true));
			//		}
			//		else // TODO check here
			//		{
			//			while (true)
			//			{
			//				LinkType* lPrev = &lNode->mPrev;
			//				if (mMemMng.compareAndSwapRef(&lNode->mPrev, lPrev, LinkType(nullptr, true)))
			//					break;
			//			}
			//			while (true)
			//			{
			//				LinkType* lNext = &lNode->mNext;
			//				if (mMemMng.compareAndSwapRef(&lNode->mNext, lNext, LinkType(nullptr, true)))
			//					break;
			//			}
			//		}
			//	}
			//	void _cleanUpNode(NodeType* pNode) override
			//	{
			//		NodeType* lNode = static_cast<NodeType*>(pNode);

			//		while (true)
			//		{
			//			NodeType* lPrev = deRefLink(&lNode->mPrev);
			//			if (!lPrev) break;
			//			if (!lPrev->mPrev.del())
			//			{
			//				releaseRef(lPrev);
			//				break;
			//			}
			//			NodeType* lPrev2 = deRefLink(lPrev->mPrev);
			//			compareAndSwapRef(lNode->mPrev, LinkType(lPrev, true), LinkType(lPrev2, true));
			//			releaseRef(lPrev2);
			//			releaseRef(lPrev);
			//		}

			//		while (true)
			//		{
			//			NodeType* lNext = deRefLink(&lNode->mNext);
			//			if (!lNext) break;
			//			if (!lNext->mNext.del())
			//			{
			//				releaseRef(lNext);
			//				break;
			//			}
			//			NodeType* lNext2 = deRefLink(lNext->mNext);
			//			compareAndSwapRef(lNode->mNext, LinkType(lNext, true), LinkType(lNext2, true));
			//			releaseRef(lNext2);
			//			releaseRef(lNext);
			//		}
			//	}

			//	NodeType* _alloc() override
			//	{
			//		void* lPointer = mAllocator.alloc(sizeof(NodeType));
			//		//Node* lResult = new (lPointer) Node;

			//		//return lResult;
			//		return lPointer;
			//	}
			//	void _free(NodeType* pPointer) override
			//	{
			//		mAllocator.free(pPointer);
			//	}
			//};
#pragma endregion MemConfig

			template< typename T >
			class Iterator
			{
				typedef T Type;
				typedef Iterator< T > ThisType;
				friend class bcMemMngList<T>;
			private:

			protected:
				Node* mPointer;
				bcMemMngList* mList;

				bcInline void _assign(Node* pNode, bcMemMngList* pList)
				{
					mPointer = pNode;
					mList = pList;

					if (mPointer)
						mList->refIterator(*this);
				}
				bcInline void _assign(const ThisType& pOther)
				{
					_unAssign();
					_assign(pOther.mPointer, pOther.mList);
				}
				bcInline void _unAssign()
				{
					if (mPointer)
						mList->releaseIterator(*this);
						//mList->releaseRef(mPointer);

					mPointer = nullptr;
					mList = nullptr;
				}

				bcInline void _next()
				{
					mList->_moveForward(&mPointer);
				}
				bcInline void _prev()
				{
					mList->_moveBackWard(&mPointer);
				}

			public:
				Iterator() noexcept(true) : mPointer(nullptr), mList(nullptr) {}
				Iterator(nullptr_t) noexcept(true) : mPointer(nullptr), mList(nullptr) {}
				Iterator(Node* pNode, bcMemMngList* pList) noexcept(true) : mPointer(nullptr), mList(nullptr) { _assign(pNode, pList); }
				Iterator(const ThisType& pOther) noexcept(true) : mPointer(nullptr), mList(nullptr) { _assign(pOther); }

				~Iterator() { _unAssign(); }

				ThisType& operator =(const ThisType& pOther) { _assign(pOther); return *this; }
				ThisType& operator =(nullptr_t) { _unAssign(); return *this; }

				ThisType& operator ++() { _next(); return *this; }
				ThisType& operator --() { _prev(); return *this; }
				ThisType& operator ++(bcINT32) { auto lTemp = *this; _next(); return lTemp; }
				ThisType& operator --(bcINT32) { auto lTemp = *this; _prev(); return lTemp; }

				bool operator ==(ThisType& pOther) const { return mPointer == pOther.mPointer; }
				bool operator !=(ThisType& pOther) const { return mPointer != pOther.mPointer; }
				bool operator ==(nullptr_t) const { return mPointer == nullptr; }
				bool operator !=(nullptr_t) const { return mPointer != nullptr; }

				Type& operator *() 
				{ 
#ifdef BC_MEMORY_DEBUG
					bcAssert(isValid());
#endif
					return mPointer->mValue; 
				}
				Type* operator ->() 
				{ 
#ifdef BC_MEMORY_DEBUG
					bcAssert(isValid());
#endif
					return &mPointer->mValue; 
				}

				Type* get() { return &mPointer->mValue; }
				Node* getNode() { return mPointer; }
				
				bool isValid() { return mPointer && !mPointer->mNext.del() && mPointer != mList->mHead && mPointer != mList->mTail; }
			};

			typedef Iterator< Type > IteratorType;
			friend class Iterator<T>;

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
				NodeType* lPointer = mAllocator.alloc();
				//NodeType* lResult = new (lPointer) NodeType;

				//return lResult;
				return lPointer;
			}
			void _free(NodeBaseType* pPointer) override
			{
				NodeType* lNode = static_cast<NodeType*>(pPointer);
				lNode->~Node();
				mAllocator.free(lNode);
			}

			bcInline NodeType* _createNode(const Type& pValue)
			{
				NodeType* lNode = mMemMng.newNode();
				bcAssert((reinterpret_cast<bcINT32>(lNode) & LinkType::dMask) == 0);
				new (lNode) NodeType(pValue);

				return lNode;
			}
			bcInline NodeType* _createNode(Type&& pValue)
			{
				NodeType* lNode = mMemMng.newNode();
				bcAssert((reinterpret_cast<bcINT32>(lNode)& LinkType::dMask) == 0);
				new (lNode)NodeType(pValue);

				return lNode;
			}
			bcInline void _setMark(LinkType* pLink)
			{
				/*while(true)
				{
				Node* lValue = (bcAtomicOperation::bcAtomicLoad(*pLink->getAtomicNode(), core_platform::bc_memory_order::relaxed));
				Node* lResult = reinterpret_cast< Node* >(reinterpret_cast<bcINT32>(lValue) | dMask);

				if(bcAtomicOperation::bcAtomicCompareExchangeStrong(
				*pLink->getAtomicNode(), 
				lValue,
				lResult,
				core_platform::bc_memory_order::relaxed,
				core_platform::bc_memory_order::relaxed))
				break;
				}*/
				
				while (true)
				{
					LinkType lOld = *pLink;
					if (lOld.del() || pLink->compareAndSwap(lOld, LinkType(lOld.pointer(), true)))
						break;
				}
			}

			bcInline bool _next(NodeType** pNode)
			{
				while(true)
				{
					NodeType* lNode = *pNode;

					if(lNode == mTail)
					{
						bcAssert(false);
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
			bcInline bool _prev(NodeType** pNode)
			{
				while(true)
				{
					NodeType* lNode = *pNode;

					if(lNode == mHead)
					{
						bcAssert(false);
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
					else if(lNode->mNext.del())
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

			bcInline bool _read(NodeType* pNode, bcNullable< Type >* pData)
			{
				bcAssert(pNode != mHead || pNode != mTail);

				*pData = pNode->mValue;

				if(!pNode->mNext.del())
					return true;
				
				pData = nullptr;
				return false;
			}

			bcInline void _insertBefore(NodeType* pNext, NodeType* pNode)
			{
				if(pNext == mHead)
					return _insertAfter(pNext, pNode);

				NodeType* lPrev = mMemMng.deRefLink(&pNext->mPrev);
				NodeType* lNext = pNext;
				mBackOff.reset();
				while (true)
				{
					while(pNext->mNext.del())
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
			bcInline void _insertAfter(NodeType* pPrev, NodeType* pNode)
			{
				if(pPrev == mTail)
					return _insertBefore(pPrev, pNode);

				NodeType* lPrev = pPrev;
				NodeType* lNext;
				mBackOff.reset();
				while(true)
				{
					lNext = mMemMng.deRefLink(&lPrev->mNext);

					mMemMng.storeRef(&pNode->mPrev, &LinkType(lPrev, false));
					mMemMng.storeRef(&pNode->mNext, &LinkType(lNext, false));

					if (mMemMng.compareAndSwapRef(&pPrev->mNext, &LinkType(lNext, false), &LinkType(pNode, false)))
						break;

					mMemMng.releaseRef(lNext);

					if(lPrev->mNext.del())
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
			bcInline bool _delete(Node** pNode, bcNullable< Type >* pData = nullptr, bool pMoveForward = true)
			{
				NodeType* lNode = *pNode;

				bcAssert(lNode != mHead || lNode != mTail);

				while(true)
				{
					bool lNextDel = (*pNode)->mNext.del();
					NodeType* lNext = mMemMng.deRefLink(&(*pNode)->mNext);
					
					if(lNextDel)
					{
						mMemMng.releaseRef(lNext);
						if (pData != nullptr) *pData = nullptr;
						return false;
					}

					if (lNode->mNext.compareAndSwap(lNext, LinkType(lNext, true)))
					{
						NodeType* lPrev;
						while(true)
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
						if(pData)
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

			bcInline NodeType* _correctPrev(NodeType* pPrev, NodeType* pNode)
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
			bcInline NodeType* _insertBefore(NodeType* pNext, Type& pValue)
			{
				NodeType* lNode = _createNode(pValue);
				_insertBefore(pNext, lNode);
				mMemMng.releaseRef(lNode);

				mCount.fetch_add(1U, core_platform::bc_memory_order::seqcst);

				return lNode;
			}
			bcInline NodeType* _insertBefore(NodeType* pNext, Type&& pValue)
			{
				NodeType* lNode = _createNode(pValue);
				_insertBefore(pNext, lNode);
				mMemMng.releaseRef(lNode);

				mCount.fetch_add(1U, core_platform::bc_memory_order::seqcst);

				return lNode;
			}
			bcInline NodeType* _insertAfter(NodeType* pPrev, Type& pValue)
			{
				NodeType* lNode = _createNode(pValue);
				_insertAfter(pPrev, lNode);
				mMemMng.releaseRef(lNode);

				mCount.fetch_add(1U, core_platform::bc_memory_order::seqcst);

				return lNode;
			}
			bcInline NodeType* _insertAfter(NodeType* pPrev, Type&& pValue)
			{
				NodeType* lNode = _createNode(pValue);
				_insertAfter(pPrev, lNode);
				mMemMng.releaseRef(lNode);

				mCount.fetch_add(1U, core_platform::bc_memory_order::seqcst);

				return lNode;
			}

			bcInline bool _erase(NodeType** pNode, bcNullable< Type >* pData)
			{ 
				bool lDeleted = _delete(pNode, pData); 
				if (lDeleted)
					mCount.fetch_sub(1U, core_platform::bc_memory_order::seqcst);

				return lDeleted;
			}

			bcInline bool _moveForward(NodeType** pNode) { return _next(pNode); }
			bcInline bool _moveBackWard(NodeType** pNode) { return _prev(pNode); }

			bcLockFreeMemMng<T, NodeType, LinkType, MemMngAllocator> mMemMng;

			//MemoryConfig mMemoryConfig;
			NodeType* mHead;
			NodeType* mTail;
			core_platform::bc_atomic< bcUINT32 > mCount;

			//AllocatorType mAllocator;
			bc_memmng_freelist<NodeType> mAllocator;

			bc_back_off mBackOff;
			bc_back_off mBackOffCP;

		public:
			bcMemMngList(bcUINT32 pNumThread, bcUINT32 pMaxIterator) :
				bcLockFreeContainerMemMngConfig(pNumThread, 2, 2, 5 + pMaxIterator),
				mMemMng(this),
				mBackOff(pNumThread),
				mBackOffCP(pNumThread)
			{
				mAllocator.initialize();

				mHead = _createNode(Type());
				mTail = _createNode(Type());
				//mMemMng.storeRef(mHead, _createNode(Type()));
				//mMemMng.storeRef(mTail, _createNode(Type()));
				mMemMng.storeRef(&mHead->mNext, &LinkType(mTail));
				mMemMng.storeRef(&mHead->mPrev, &LinkType(mTail));
				mMemMng.storeRef(&mTail->mNext, &LinkType(mHead));
				mMemMng.storeRef(&mTail->mPrev, &LinkType(mHead));
				mMemMng.releaseRef(mHead);
				mMemMng.releaseRef(mTail);
			}
			~bcMemMngList()
			{
				clear();
				mMemMng.ref(mHead);
				mMemMng.ref(mTail);
				mMemMng.deleteNode(mHead);
				mMemMng.deleteNode(mTail);

				mMemMng.forceCleanUp();
			}

			IteratorType pushBack() 
			{  
				// Because in _insertBefore we release next, we need an additional reference to mTail
				mMemMng.ref(mTail);
				return IteratorType(_insertBefore(mTail, Type()), this);
			}
			IteratorType pushBack(Type& pValue)
			{
				// Because in _insertBefore we release next, we need an additional reference to mTail
				mMemMng.ref(mTail);
				return IteratorType(_insertBefore(mTail, pValue), this);
			}
			IteratorType pushBack(Type&& pValue)
			{
				// Because in _insertBefore we release next, we need an additional reference to mTail
				mMemMng.ref(mTail);
				return IteratorType(_insertBefore(mTail, pValue), this);
			}
			IteratorType pushFront()
			{
				// Because in _insertAfter we release prev, we need an additional reference to mHead
				mMemMng.ref(mHead);
				return IteratorType(_insertAfter(mHead, Type()), this);
			}
			IteratorType pushFront(Type& pValue)
			{
				// Because in _insertAfter we release prev, we need an additional reference to mHead
				mMemMng.ref(mHead);
				return IteratorType(_insertAfter(mHead, pValue), this);
			}
			IteratorType pushFront(Type&& pValue)
			{
				// Because in _insertAfter we release prev, we need an additional reference to mHead
				mMemMng.ref(mHead);
				return IteratorType(_insertAfter(mHead, pValue), this);
			}

			void popBack(bcNullable< Type >* pData = nullptr)
			{
				NodeType* lNode = mMemMng.deRefLink(mTail->mPrev);
				_erase(&lNode, pData);
				mMemMng.releaseRef(lNode);
			}
			void popFront(bcNullable< Type >* pData = nullptr)
			{
				NodeType* lNode = mMemMng.deRefLink(mHead->mNext);
				_erase(&lNode, pData);
				mMemMng.releaseRef(lNode);
			}

			IteratorType insertAfter(IteratorType& pWhere, Type& pValue)
			{
				// Because in _insertAfter we release prev, we need an additional reference to mPointer
				mMemMng.ref(pWhere.mPointer);
				return IteratorType(_insertAfter(pWhere.mPointer, pValue), this);
			}
			IteratorType insertAfter(IteratorType& pWhere, Type&& pValue)
			{
				// Because in _insertAfter we release prev, we need an additional reference to mPointer
				mMemMng.ref(pWhere.mPointer);
				return IteratorType(_insertAfter(pWhere.mPointer, pValue), this);
			}
			IteratorType insertBefore(IteratorType& pWhere, Type& pValue)
			{
				// Because in _insertBefore we release next, we need an additional reference to mPointer
				mMemMng.ref(pWhere.mPointer);
				return IteratorType(insertBefore(pWhere.mPointer, pValue), this);
			}
			IteratorType insertBefore(IteratorType& pWhere, Type&& pValue)
			{
				// Because in _insertBefore we release next, we need an additional reference to mPointer
				mMemMng.ref(pWhere.mPointer);
				return IteratorType(insertBefore(pWhere.mPointer, pValue), this);
			}

			void erase(IteratorType& pIte, bcNullable< Type >* pData = nullptr)
			{
				_erase(&pIte.mPointer, pData);
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
			IteratorType createIterator(NodeType* pNode)
			{
				return IteratorType(pNode, this);
			}
			void refIterator(IteratorType& pIte)
			{
				mMemMng.ref(pIte.mPointer);
			}
			void releaseIterator(IteratorType& pIte)
			{
				mMemMng.releaseRef(pIte.mPointer);
			}
			
			void clear()
			{
				for (auto lIte = begin(); lIte != end();)
					erase(lIte);
			}
			bcSIZE count() { return mCount.load(core_platform::bc_memory_order::seqcst); }
		};

#endif // BC_MEMORY_ENABLE
	}
}