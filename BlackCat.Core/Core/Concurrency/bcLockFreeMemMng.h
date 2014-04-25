//  [10/19/2013 MRB]

#pragma once

#include "CorePCH.h"
//#include "CoreExceptions.h"
#include "bcAtomic.h"
#include "bcConcurrencyUtility.h"
#include "bcIterator.h"

namespace BlackCat
{
	namespace Core
	{
		template<typename T, typename NodeT, typename LinkT, typename Allocator>
		class bcLockFreeMemMng;

		template< typename T >
		struct bcLockFreeNode : public bcContainerNode< T >
		{
		public:
			bcAtomic< bcUINT16 > mReferenceCount;
			bcAtomic< bcBOOL > mTrace;
			bcAtomic< bcBOOL > mDel;

			bcLockFreeNode() throw()
			{
				bcAtomicOperation::bcAtomicInit(mReferenceCount, bcUINT16(0));
				bcAtomicOperation::bcAtomicInit(mTrace, bcFALSE);
				bcAtomicOperation::bcAtomicInit(mDel, bcFALSE);
			}
			bcLockFreeNode(const T& pValue) throw() : bcContainerNode(pValue)
			{
				bcAtomicOperation::bcAtomicInit(mReferenceCount, bcUINT16(0));
				bcAtomicOperation::bcAtomicInit(mTrace, bcFALSE);
				bcAtomicOperation::bcAtomicInit(mDel, bcFALSE);
			}
			bcLockFreeNode(T&& pValue) throw() : bcContainerNode(std::move(pValue))
			{
				bcAtomicOperation::bcAtomicInit(mReferenceCount, bcUINT16(0));
				bcAtomicOperation::bcAtomicInit(mTrace, bcFALSE);
				bcAtomicOperation::bcAtomicInit(mDel, bcFALSE);
			}
			~bcLockFreeNode() throw() {}
		};

		template< typename NodeT >
		struct bcLockFreeLink
		{
		public:
			typedef NodeT NodeType;

		private:

		protected:
			bcAtomic< NodeType* > mPointer;

		public:
			bcLockFreeLink()
			{
				//bcAtomicOperation::bcAtomicInit(mPointer, (NodeType*)bcNULL);
			}
			~bcLockFreeLink() {}

			// == If nodes in underlying data structure modify bits(like doubly linked list) override this function
			// to return correct pointers ==
			bcInline virtual NodeType* getNode() throw()
			{
				return bcAtomicOperation::bcAtomicLoad(mPointer, bcMemoryOrder_SeqCst);
			}
			bcInline bcAtomic< NodeType* >* getAtomicNode() throw()
			{
				return &mPointer;
			}
		};

		template< typename T >
		class bcLockFreeContainerMemMngConfig
		{
		public:
			typedef bcLockFreeNode<T> NodeBaseType;
			//typedef LinkT LinkType;
		private:

		public:
			bcINT32 mNumThread;
			// == Number of links per node ==
			bcINT32 mNumLink;
			// == Number of links per node that may transiently point to a deleted node ==
			bcINT32 mNumLinkDel;
			// == Number of thread-local node references ==
			bcINT32 mNumHP;
			bcUINT32 mCleanUpThreshold;
			bcUINT32 mScanThreshold;

		public:
			bcLockFreeContainerMemMngConfig(
				bcUINT32 pNumThread, 
				bcUINT32 pNumLink, 
				bcUINT32 pNumLinkDel, 
				bcUINT32 pNumHP) throw() : 
				mNumThread(pNumThread), 
				mNumLink(pNumLink), 
				mNumLinkDel(pNumLinkDel), 
				mNumHP(pNumHP)
			{
				mCleanUpThreshold = pNumThread * (mNumHP + mNumLink + mNumLinkDel + 1);
				mScanThreshold = mNumHP * 2 < mCleanUpThreshold ? mNumHP * 2 : mCleanUpThreshold;
			}
			~bcLockFreeContainerMemMngConfig() throw() {}

			virtual void _terminateNode(NodeBaseType* pNode, bcBOOL pConcurrent) = 0;
			virtual void _cleanUpNode(NodeBaseType* pNode) = 0;

			virtual NodeBaseType* _alloc() = 0;
			virtual void _free(NodeBaseType*) = 0;
		};

		// Based on the paper: "Efficient and Reliable Lock-Free Memory Reclamation Based on Reference Counting", Gidenstam, et al. - 2005
		// and implementation of this paper in Honeycomb engine
		template< typename T, typename NodeT, typename LinkT, typename Allocator >
		class bcLockFreeMemMng
		{
		public:
			typedef T Type;
			typedef NodeT NodeType;
			typedef LinkT LinkType;
			typedef bcLockFreeNode<Type> NodeBaseType;
			typedef bcLockFreeLink<NodeType> LinkBaseType;
			typedef bcLockFreeContainerMemMngConfig<Type> ConfigType;
			typedef Allocator AllocatorType;
			//typedef bcINT16 IndexType;

		protected:
			template< typename T >
			class FreeIndexList
			{
				typedef T Type;
			private:

			protected:
				Type* mFreeList;
				bcUINT32 mCapacity;
				bcUINT32 mCount;

				AllocatorType* mAllocator;

			public:
				FreeIndexList(bcUINT32 pCapacity, AllocatorType* pAllocator, bcBOOL pInitToSequenceIndices = bcTRUE) : 
					mCapacity(pCapacity), mCount(0), mAllocator(pAllocator)
				{
					//mFreeList = new Type[mCapacity];
					mFreeList = static_cast<Type*>(mAllocator->alloc(sizeof(Type)* mCapacity));
					if (mFreeList != bcNULL)
						new(mFreeList)Type[mCapacity];
					else
						//throw Exception(L"Memory Allocation Error");
						// TODO check for allocation fail here
						bcAssert(bcFALSE);

					if (pInitToSequenceIndices)
					{
						for (bcUINT32 i = 0; i < mCapacity; ++i)
							addToFreeList(i);
					}
				}
				~FreeIndexList() 
				{
					//delete[] mFreeList;
					mAllocator->free(mFreeList);
				}

				bcInline void addToFreeList(Type pValue) throw()
				{
					mFreeList[mCount] = pValue;
					++mCount;
					
					bcAssert(mCount <= mCapacity);
				}

				bcInline Type getFreeItem() throw()
				{
					bcAssert(mCount > 0);

					Type lItem = mFreeList[mCount - 1];
					--mCount;

					return lItem;
				}
			};

			class ThreadData
			{
			private:

			protected:

			public:
				bcAtomic< NodeType* >* mHP;
				bcAtomic< NodeType* >* mDLNodes;
				bcAtomic< bcINT32 >* mDLClaims;
				bcAtomic< bcBOOL >* mDLDone;

				FreeIndexList< bcINT32 > mHPFreeList;
				FreeIndexList< bcINT32 > mDLNodesFreeList;

				bcINT32 mDList;
				bcUINT32 mDCount;
				bcINT32* mDLNexts;

				NodeType**  mPList; // == used in scan == TODO: replace searching method with some indexing

				AllocatorType* mAllocator;

				ThreadData(bcUINT32 pNumThread, bcUINT32 pNumHP, bcUINT32 pCleanUpThreshold, AllocatorType* pAllocator) : 
					mDList(-1), 
					mDCount(0), 
					mHPFreeList(pNumHP, pAllocator),
					mDLNodesFreeList(pCleanUpThreshold, pAllocator),
					mAllocator(pAllocator)
				{
					bcAssert(pNumHP <= 32767);
					bcAssert(pCleanUpThreshold <= 32767);

					/*mHP = new bcAtomic< NodeType* >[pNumHP];*/
					mHP = static_cast<bcAtomic< NodeType* >*>(mAllocator->alloc(sizeof(bcAtomic< NodeType* >) * pNumHP));
					/*if (mHP != bcNULL)
						new(mHP)bcAtomic< NodeType* >[pNumHP];*/
					if (mHP == bcNULL)
					{
						//throw Exception(L"Memory Allocation Error");
						// TODO check for allocation fail here
						bcAssert(bcFALSE);
					}
					//mDLNodes = new bcAtomic< NodeType* >[pCleanUpThreshold];
					mDLNodes = static_cast<bcAtomic< NodeType* >*>(mAllocator->alloc(sizeof(bcAtomic< NodeType* >) * pCleanUpThreshold));
					/*if (mDLNodes != bcNULL)
						new(mDLNodes)bcAtomic< NodeType* >[pCleanUpThreshold];*/
					if (mDLNodes == bcNULL)
					{
						//delete[] mHP;
						mAllocator->free(mHP);
						//throw Exception(L"Memory Allocation Error");
						// TODO check for allocation fail here
						bcAssert(bcFALSE);
					}
					//mDLClaims = new bcAtomic< bcINT32 >[pCleanUpThreshold];
					mDLClaims = static_cast<bcAtomic< bcINT32 >*>(mAllocator->alloc(sizeof(bcAtomic< bcINT32 >) * pCleanUpThreshold));
					/*if (mDLClaims != bcNULL)
						new(mDLClaims)bcAtomic< bcINT32 >[pCleanUpThreshold];*/
					if (mDLClaims == bcNULL)
					{
						/*delete[] mHP;
						delete[] mDLNodes;*/
						mAllocator->free(mHP);
						mAllocator->free(mDLNodes);
						//throw Exception(L"Memory Allocation Error");
						// TODO check for allocation fail here
						bcAssert(bcFALSE);
					}
					//mDLDone = new bcAtomic< bcBOOL >[pCleanUpThreshold];
					mDLDone = static_cast<bcAtomic< bcBOOL >*>(mAllocator->alloc(sizeof(bcAtomic< bcBOOL >) * pCleanUpThreshold));
					/*if (mDLDone != bcNULL)
						new(mDLDone)bcAtomic< bcBOOL >[pCleanUpThreshold];*/
					if (mDLClaims == bcNULL)
					{
						/*delete[] mHP;
						delete[] mDLNodes;
						delete[] mDLClaims;*/
						mAllocator->free(mHP);
						mAllocator->free(mDLNodes);
						mAllocator->free(mDLClaims);
						//throw Exception(L"Memory Allocation Error");
						// TODO check for allocation fail here
						bcAssert(bcFALSE);
					}

					//mDLNexts = new bcINT32[pCleanUpThreshold];
					mDLNexts = static_cast<bcINT32*>(mAllocator->alloc(sizeof(bcINT32)* pCleanUpThreshold));
					/*if (mDLNexts != bcNULL)
						new(mDLNexts)bcINT32[pCleanUpThreshold];*/
					if (mDLNexts == bcNULL)
					{
						/*delete[] mHP;
						delete[] mDLNodes;
						delete[] mDLClaims;
						delete[] mDLDone;*/
						mAllocator->free(mHP);
						mAllocator->free(mDLNodes);
						mAllocator->free(mDLClaims);
						mAllocator->free(mDLDone);
						//throw Exception(L"Memory Allocation Error");
						// TODO check for allocation fail here
						bcAssert(bcFALSE);
					}

					bcUINT32 lNumPList = pNumThread * pNumHP;
					//mPList = new NodeType*[lNumPList];
					mPList = static_cast<NodeType**>(mAllocator->alloc(sizeof(NodeType*)* lNumPList));
					/*if (mPList != bcNULL)
						new(mPList)NodeType*[lNumPList];*/
					if (mPList == bcNULL)
					{
						/*delete[] mHP;
						delete[] mDLNodes;
						delete[] mDLClaims;
						delete[] mDLDone;
						delete[] mDLNexts;*/
						mAllocator->free(mHP);
						mAllocator->free(mDLNodes);
						mAllocator->free(mDLClaims);
						mAllocator->free(mDLDone);
						mAllocator->free(mDLNexts);
						//throw Exception(L"Memory Allocation Error");
						// TODO check for allocation fail here
						bcAssert(bcFALSE);
					}

					// == because sometimes in deRefLink we read null, we must use another value (1) for indicating 
					// non-using values, so our FreeIndexList can work property ==
					std::memset(mHP, reinterpret_cast<bcINT32>(reinterpret_cast<NodeType*>(1)), pNumHP * sizeof(bcAtomic< NodeType* >));
					//std::fill(mHP, mHP + pNumHP, bcNULL);
					std::memset(mDLNodes, reinterpret_cast<bcINT32>(bcNULL), pCleanUpThreshold * sizeof(bcAtomic< NodeType* >));
					//std::fill(mDLNodes, mDLNodes + pCleanUpThreshold, bcNULL);
					std::memset(mDLClaims, 0, pCleanUpThreshold * sizeof(bcAtomic< bcINT32 >));
					std::memset(mDLDone, bcFALSE, pCleanUpThreshold * sizeof(bcAtomic< bcBOOL >));
					std::memset(mDLNexts, -1, pCleanUpThreshold * sizeof(bcINT32));
					std::memset(mPList, reinterpret_cast<bcINT32>(bcNULL), sizeof(NodeType**)* lNumPList);
					//std::fill(mPList, mPList + lNumPList, bcNULL);
				}
				~ThreadData() 
				{
					/*delete[] mHP;
					delete[] mDLNodes;
					delete[] mDLClaims;
					delete[] mDLDone;
					delete[] mDLNexts;
					delete[] mPList;*/
					mAllocator->free(mHP);
					mAllocator->free(mDLNodes);
					mAllocator->free(mDLClaims);
					mAllocator->free(mDLDone);
					mAllocator->free(mDLNexts);
					mAllocator->free(mPList);
				}
			};

			//Allocator mAllocator;
			
		private:
			bcInline void _cleanUpLocal()
			{
				bcUINT32 lThreadIndex = _getCurrentThreadIndex();
				ThreadData& lData = mThreadData[lThreadIndex];

				bcINT32 lIndex = lData.mDList;
				while(lIndex != -1)
				{
					NodeBaseType* lNode = bcAtomicOperation::bcAtomicLoad(lData.mDLNodes[lIndex], bcMemoryOrder_SeqCst);
					mConfig->_cleanUpNode(static_cast<NodeType*>(lNode));
					lIndex = lData.mDLNexts[lIndex];
				}
			}

			bcInline void _cleanUpAll()
			{
				for(bcUINT32 lThread = 0; lThread < mConfig->mNumThread; ++lThread)
				{
					for (bcUINT32 lIndex = 0; lIndex < mConfig->mCleanUpThreshold; ++lIndex)
					{
						ThreadData& lData = mThreadData[lThread];
						NodeBaseType* lNode = bcAtomicOperation::bcAtomicLoad(lData.mDLNodes[lIndex], bcMemoryOrder_SeqCst);
						if (lNode != bcNULL && !bcAtomicOperation::bcAtomicLoad(lData.mDLDone[lIndex], bcMemoryOrder_SeqCst))
						{
							bcAtomicOperation::bcAtomicFetchAdd(lData.mDLClaims[lIndex], 1, bcMemoryOrder_SeqCst);
							if (lNode == bcAtomicOperation::bcAtomicLoad(lData.mDLNodes[lIndex], bcMemoryOrder_SeqCst))
							{
								mConfig->_cleanUpNode(static_cast<NodeType*>(lNode));
							}
							bcAtomicOperation::bcAtomicFetchSub(lData.mDLClaims[lIndex], 1, bcMemoryOrder_SeqCst);
						}
					}
				}
			}

			bcInline bcBOOL _isInHPList(NodeType** pPlist, bcUINT32 pNumList, NodeType* lNode)
			{
				for(bcUINT32 i = 0; i < pNumList; ++i)
				{
					if(pPlist[i] == lNode)
						return bcTRUE;
				}
				return bcFALSE;
			}
				
			bcInline void _scan()
			{
				bcUINT32 lThreadIndex = _getCurrentThreadIndex();
				ThreadData& lData = mThreadData[lThreadIndex];

				bcINT32 lIndex = lData.mDList;
				while(lIndex != -1)
				{
					NodeBaseType* lNode = bcAtomicOperation::bcAtomicLoad(lData.mDLNodes[lIndex], bcMemoryOrder_SeqCst);
					if (bcAtomicOperation::bcAtomicLoad(lNode->mReferenceCount, bcMemoryOrder_SeqCst) == 0)
					{
						bcAtomicOperation::bcAtomicStore(lNode->mTrace, bcTRUE, bcMemoryOrder_SeqCst);
						if (bcAtomicOperation::bcAtomicLoad(lNode->mReferenceCount, bcMemoryOrder_SeqCst) != 0)
						{
							bcAtomicOperation::bcAtomicStore(lNode->mTrace, bcFALSE, bcMemoryOrder_SeqCst);
						}
					}
					lIndex = lData.mDLNexts[lIndex];
				}

				NodeType** lPlist = lData.mPList;
				bcUINT32 lNumPList = 0;
				bcINT32 lNewDList = -1;
				bcUINT32 lNewDCount = 0;
				for(bcUINT32 lThread = 0; lThread < mConfig->mNumThread; ++lThread)
				{
					for(bcUINT32 lH = 0; lH < mConfig->mNumHP; ++lH)
					{
						NodeType* lNode = bcAtomicOperation::bcAtomicLoad(mThreadData[lThread].mHP[lH], bcMemoryOrder_SeqCst);
						if(lNode != bcNULL)
						{
							/*bcBOOL lExist = bcFALSE;
							for (bcUINT32 i = 0; i < lNumPList; ++i)
							{
							if(lPlist[i] == lNode)
							{
							lExist = bcTRUE;
							break;
							}
							}
							if(lExist == bcFALSE)*/
							if(!_isInHPList(lPlist, lNumPList, lNode))
							{
								lPlist[lNumPList] = lNode;
								++lNumPList;
							}
						}
					}
				}

				while(lData.mDList != -1)
				{
					lIndex = lData.mDList;
					NodeType* lNode = bcAtomicOperation::bcAtomicLoad(lData.mDLNodes[lIndex], bcMemoryOrder_SeqCst);
					lData.mDList = lData.mDLNexts[lIndex];

					if (bcAtomicOperation::bcAtomicLoad(lNode->mReferenceCount, bcMemoryOrder_SeqCst) == 0 &&
						bcAtomicOperation::bcAtomicLoad(lNode->mTrace, bcMemoryOrder_SeqCst) == bcTRUE &&
					   !_isInHPList(lPlist, lNumPList, lNode))
					{
						bcAtomicOperation::bcAtomicStore(lData.mDLNodes[lIndex], (NodeType*)bcNULL, bcMemoryOrder_SeqCst);
						if (bcAtomicOperation::bcAtomicLoad(lData.mDLClaims[lIndex], bcMemoryOrder_SeqCst) == 0)
						{
							mConfig->_terminateNode(static_cast<NodeType*>(lNode), bcFALSE);
							//mAllocator.Free(lNode);
							mConfig->_free(static_cast<NodeType*>(lNode));

							lData.mDLNodesFreeList.addToFreeList(lIndex);

							continue;
						}
						mConfig->_terminateNode(static_cast<NodeType*>(lNode), bcTRUE);
						bcAtomicOperation::bcAtomicStore(lData.mDLDone[lIndex], bcTRUE, bcMemoryOrder_SeqCst);
						bcAtomicOperation::bcAtomicStore(lData.mDLNodes[lIndex], lNode, bcMemoryOrder_SeqCst);
					}

					lData.mDLNexts[lIndex] = lNewDList;
					lNewDList = lIndex;
					++lNewDCount;
				}

				lData.mDList = lNewDList;
				lData.mDCount = lNewDCount;
			}

		protected:
			ConfigType* mConfig;
			ThreadData* mThreadData;

			bcAtomic<bcUINT32> mLastThreadIndex;
			bcThreadLocal<bcINT32, bcINT32> mMyIndex;

			AllocatorType mAllocator;

			bcUINT32 _getCurrentThreadIndex()
			{
				if (*mMyIndex == -1)
					*mMyIndex = bcAtomicOperation::bcAtomicFetchAdd(mLastThreadIndex, 1U, bcMemoryOrder_Relaxed);
				return *mMyIndex;
			}

			/*virtual void _terminateNode(NodeType* pNode, bcBOOL pConcurrent) = 0;
			virtual void _cleanUpNode(NodeType* pNode) = 0;

			virtual NodeType* _alloc() = 0;
			virtual void _free(NodeType*) = 0;*/

		public:
			bcLockFreeMemMng(ConfigType* pConfig) : mConfig(pConfig), mLastThreadIndex(0)
			{
				//mThreadData = new ThreadData[mConfig->mNumThread](mConfig->mNumThread, mConfig->mNumHP, mConfig->mCleanUpThreshold);
				//mThreadData = static_cast<ThreadData*>(std::malloc(sizeof(ThreadData) * mConfig->mNumThread));
				mThreadData = static_cast<ThreadData*>(mAllocator.alloc(sizeof(ThreadData)* mConfig->mNumThread));
				if (mThreadData == bcNULL)
				{
					// TODO check for allocation fail here
					bcAssert(bcFALSE);
				}
				for (bcUINT32 i = 0; i < mConfig->mNumThread; ++i)
					new (&mThreadData[i]) ThreadData(mConfig->mNumThread, mConfig->mNumHP, mConfig->mCleanUpThreshold, &mAllocator);

				mMyIndex.Initialize(mConfig->mNumThread , -1);
			}
			~bcLockFreeMemMng()
			{
				for (bcUINT32 i = 0; i < mConfig->mNumThread; ++i)
					mThreadData[i].~ThreadData();
				mAllocator.free(mThreadData);

				mMyIndex.Destroy();
			}

			bcInline NodeType* deRefLink(LinkType* pLink)
			{
				LinkType* lLink = pLink;

				bcUINT32 lThreadIndex = _getCurrentThreadIndex();
				bcINT32 lIndex = mThreadData[lThreadIndex].mHPFreeList.getFreeItem();
				bcAtomic< NodeType* >& lHP = mThreadData[lThreadIndex].mHP[lIndex];
	
				NodeType* lNode = bcNULL;
				while(bcTRUE)
				{
					lNode = static_cast< NodeType* >(lLink->getNode());

					bcAtomicOperation::bcAtomicStore(lHP, lNode, bcMemoryOrder_SeqCst);

					if (static_cast<NodeType*>(lLink->getNode()) == lNode)
					{
						return static_cast<NodeType*>(lNode);
					}
				}
			}
			bcInline void ref(NodeType* pNode)
			{
				NodeType* lNode = pNode;

				bcUINT32 lThreadIndex = _getCurrentThreadIndex();
				bcINT32 lIndex = mThreadData[lThreadIndex].mHPFreeList.getFreeItem();
				bcAtomic< NodeType* >& lHP = mThreadData[lThreadIndex].mHP[lIndex];

				bcAtomicOperation::bcAtomicStore(lHP, lNode, bcMemoryOrder_SeqCst);
			}
			bcInline void releaseRef(NodeType* pNode)
			{
				bcUINT32 lThreadIndex = _getCurrentThreadIndex();
				bcINT32 lIndex;
				NodeType* lNode = static_cast< NodeType* >(pNode);
				ThreadData& lData = mThreadData[lThreadIndex];
				for(bcUINT32 i = 0; i < mConfig->mNumHP; ++i)
				{
					NodeType* lNodeCopy = lNode;
					// == because sometimes in deRefLink we read null, we must use another value (1) for indicating 
					// non-using values, so our FreeIndexList can work property ==
					NodeType* lNewValue = reinterpret_cast<NodeType*>(1); 
					if (bcAtomicOperation::bcAtomicCompareExchangeStrong(
						lData.mHP[i],
						&lNodeCopy,
						lNewValue,
						bcMemoryOrder_SeqCst,
						bcMemoryOrder_SeqCst))
					{
						lData.mHPFreeList.addToFreeList(i);
						return;
					}
				}

				bcAssert(bcFALSE);
			}

			/*bcInline bcBOOL compareAndSwapRef(LinkType* pLink, NodeType* pExpected, NodeType* pNew)
			{
				LinkBaseType* lLink = static_cast< LinkBaseType* >(pLink);
				NodeBaseType* lExpected = static_cast< NodeBaseType* >(pExpected);

				if(bcAtomicOperation::bcAtomicCompareExchangeStrong(
					*lLink->getAtomicNode(),
					&lExpected,
					pNew,
					bcMemoryOrder_Relaxed,
					bcMemoryOrder_Relaxed))
				{
					if(pNew != bcNULL)
					{
						bcAtomicOperation::bcAtomicFetchAdd(pNew->mReferenceCount, bcUINT16(1), bcMemoryOrder_Relaxed);
						bcAtomicOperation::bcAtomicStore(pNew->mTrace, bcFALSE, bcMemoryOrder_Relaxed);
					}
					if(lExpected != bcNULL)
					{
						bcAtomicOperation::bcAtomicFetchSub(lExpected->mReferenceCount, bcUINT16(1), bcMemoryOrder_Relaxed);
					}
					return bcTRUE;
				}

				return bcFALSE;
			}*/
			// TODO check this function for concurrency problem
			bcInline bcBOOL compareAndSwapRef(LinkType* pLink, LinkType* pExpected, LinkType* pNew)
			{
				LinkType* lLink = pLink;
				LinkType* lExpected = pExpected;
				LinkType* lNew = pNew;

				//NodeBaseType* lLinkCopy = static_cast<NodeBaseType*>(lLink->getNode());
				
				while(bcTRUE)
				{
					NodeType* lExpectedNode = lExpected->getNode();
					NodeType* lNewNode = lNew->getNode();

					if(bcAtomicOperation::bcAtomicCompareExchangeStrong(
						*lLink->getAtomicNode(),
						&lExpectedNode,
						lNewNode,
						bcMemoryOrder_SeqCst,
						bcMemoryOrder_SeqCst))
					{
						//TODO check here
						NodeType* lNewExpected = lExpected->getNode();
						NodeType* lNewNew = lNew->getNode();

						bcAssert(lNewExpected == lExpectedNode && lNewNew == lNewNode);
						/*if(!(lNewExpected == lExpected && lNewNew == lNew))
						{
							bcAtomicOperation::bcAtomicStore(*lLink->getAtomicNode(), lLinkCopy, bcMemoryOrder_Relaxed);
							continue;
						}*/

						if(lNewNode != bcNULL)
						{
							bcAtomicOperation::bcAtomicFetchAdd(lNewNode->mReferenceCount, bcUINT16(1), bcMemoryOrder_SeqCst);
							bcAtomicOperation::bcAtomicStore(lNewNode->mTrace, bcFALSE, bcMemoryOrder_SeqCst);
						}
						if(lExpectedNode != bcNULL)
						{
							bcAtomicOperation::bcAtomicFetchSub(lExpectedNode->mReferenceCount, bcUINT16(1), bcMemoryOrder_SeqCst);
						}
						return bcTRUE;
					}

					return bcFALSE;
				}
			}
			/*bcInline void storeRef(LinkType* pLink, NodeType* pNode)
			{
				NodeType* lOldNode = pLink->getNode();

				bcAtomicOperation::bcAtomicStore(*pLink->getAtomicNode(), pNode, bcMemoryOrder_Relaxed);
				if(pNode != bcNULL)
				{
					bcAtomicOperation::bcAtomicFetchAdd(pNode->mReferenceCount, bcUINT16(1), bcMemoryOrder_Relaxed);
					bcAtomicOperation::bcAtomicStore(pNode->mTrace, bcFALSE, bcMemoryOrder_Relaxed);
				}
				if(lOldNode != bcNULL)
				{
					bcAtomicOperation::bcAtomicFetchSub(lOldNode->mReferenceCount, bcUINT16(1), bcMemoryOrder_Relaxed);
				}
			}*/
			// TODO check this function for concurrency problem
			bcInline void storeRef(LinkType* pLink, LinkType* pNew)
			{
				LinkType* lLink = pLink;
				LinkType* lNew = pNew;

				NodeType* lOldNode = lLink->getNode();
				NodeType* lNewNode = lNew->getNode();

				bcAtomicOperation::bcAtomicStore(*lLink->getAtomicNode(), lNewNode, bcMemoryOrder_SeqCst);
				if(lNewNode != bcNULL)
				{
					bcAtomicOperation::bcAtomicFetchAdd(lNewNode->mReferenceCount, bcUINT16(1), bcMemoryOrder_SeqCst);
					bcAtomicOperation::bcAtomicStore(lNewNode->mTrace, bcFALSE, bcMemoryOrder_SeqCst);
				}
				if(lOldNode != bcNULL)
				{
					bcAtomicOperation::bcAtomicFetchSub(lOldNode->mReferenceCount, bcUINT16(1), bcMemoryOrder_SeqCst);
				}
			}

			// If you don't call object constructure in _alloc function, don't forget to call that after this function
			bcInline NodeType* newNode()
			{
				//NodeType* lNode = mAllocator.Alloc(sizeof(NodeType));
				NodeType* lNode = static_cast<NodeType*>(mConfig->_alloc());
				//new (lNode) NodeType;

				bcUINT32 lThreadIndex = _getCurrentThreadIndex();
				ThreadData& lData = mThreadData[lThreadIndex];
				bcINT32 lIndex = lData.mHPFreeList.getFreeItem();

				bcAtomicOperation::bcAtomicStore(lData.mHP[lIndex], lNode, bcMemoryOrder_SeqCst);

				return lNode;
			}
			// Call object deconstructure in _free function
			bcInline void deleteNode(NodeType* pNode)
			{
				NodeType* lNode = pNode;
				releaseRef(lNode);
				bcAtomicOperation::bcAtomicStore(lNode->mDel, bcTRUE, bcMemoryOrder_SeqCst);
				bcAtomicOperation::bcAtomicStore(lNode->mTrace, bcFALSE, bcMemoryOrder_SeqCst);

				bcUINT32 lThreadIndex = _getCurrentThreadIndex();
				ThreadData& lData = mThreadData[lThreadIndex];
				bcINT32 lIndex = lData.mDLNodesFreeList.getFreeItem();

				bcAtomicOperation::bcAtomicStore(lData.mDLDone[lIndex], bcFALSE, bcMemoryOrder_SeqCst);
				bcAtomicOperation::bcAtomicStore(lData.mDLNodes[lIndex], lNode, bcMemoryOrder_SeqCst);
				lData.mDLNexts[lIndex] = lData.mDList;
				lData.mDList = lIndex;
				lData.mDCount += 1;

				while(bcTRUE)
				{
					if(lData.mDCount == mConfig->mCleanUpThreshold)
						_cleanUpLocal();
					if(lData.mDCount >= mConfig->mScanThreshold)
						_scan();
					if(lData.mDCount == mConfig->mCleanUpThreshold)
						_cleanUpAll();
					else
						break;
				}
			}

			// == Free allocated space for all deleted nodes, call this in underlying data structure destructure ==
			void forceCleanUp()
			{
				for (bcUINT32 tIndex = 0; tIndex < mConfig->mNumThread; tIndex++)
				{
					ThreadData& lData = mThreadData[tIndex];
					while (lData.mDList != -1)
					{
						bcINT32 lIndex = lData.mDList;
						NodeType* lNode = bcAtomicOperation::bcAtomicLoad(lData.mDLNodes[lIndex], bcMemoryOrder_SeqCst);

						mConfig->_free(static_cast<NodeType*>(lNode));

						lData.mDList = lData.mDLNexts[lIndex];
					}
				}
			}
		};
	}
}