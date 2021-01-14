//  [10/19/2013 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "Core/CorePCH.h"
#include "Core/Concurrency/bcConcurrencyUtility.h"
#include "Core/Memory/bcAlloc.h"
#include "Core/Container/bcIterator.h"

namespace black_cat
{
	namespace core
	{
		template<typename T, typename NodeT, typename LinkT, typename Allocator>
		class bcLockFreeMemMng;

		template< typename T >
		struct bcLockFreeNode : public bc_container_node< T >
		{
		public:
			core_platform::bc_atomic< bcUINT16 > mReferenceCount;
			core_platform::bc_atomic< bool > mTrace;
			core_platform::bc_atomic< bool > mDel;

			bcLockFreeNode() noexcept(true)
				: mReferenceCount(bcUINT16(0)),
				mTrace(false),
				mDel(false)
			{
			}

			bcLockFreeNode(const T& pValue) noexcept(true)
				: mReferenceCount(bcUINT16(0)),
				mTrace(false),
				mDel(false),
				bc_container_node(pValue)
			{
			}

			bcLockFreeNode(T&& pValue) noexcept(true) 
				: mReferenceCount(bcUINT16(0)),
				mTrace(false),
				mDel(false),
				bc_container_node(std::move(pValue))
			{
			}

			~bcLockFreeNode() noexcept(true) {}
		};

		template< typename NodeT >
		struct bcLockFreeLink
		{
		public:
			typedef NodeT NodeType;

		private:

		protected:
			core_platform::bc_atomic< NodeType* > mPointer;

		public:
			bcLockFreeLink()
			{
				//bcAtomicOperation::bcAtomicInit(mPointer, (NodeType*)nullptr);
			}
			~bcLockFreeLink() {}

			// If nodes in underlying data structure modify bits(like doubly linked list) override this function
			// to return correct pointers /
			bcInline virtual NodeType* getNode() noexcept(true)
			{
				return mPointer.load(core_platform::bc_memory_order::seqcst);
			}
			bcInline core_platform::bc_atomic< NodeType* >* getAtomicNode() noexcept(true)
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
			// Number of links per node /
			bcINT32 mNumLink;
			// Number of links per node that may transiently point to a deleted node /
			bcINT32 mNumLinkDel;
			// Number of thread-local node references /
			bcINT32 mNumHP;
			bcUINT32 mCleanUpThreshold;
			bcUINT32 mScanThreshold;

		public:
			bcLockFreeContainerMemMngConfig(
				bcUINT32 pNumThread, 
				bcUINT32 pNumLink, 
				bcUINT32 pNumLinkDel, 
				bcUINT32 pNumHP) noexcept(true) : 
				mNumThread(pNumThread), 
				mNumLink(pNumLink), 
				mNumLinkDel(pNumLinkDel), 
				mNumHP(pNumHP)
			{
				mCleanUpThreshold = pNumThread * (mNumHP + mNumLink + mNumLinkDel + 1);
				mScanThreshold = mNumHP * 2 < mCleanUpThreshold ? mNumHP * 2 : mCleanUpThreshold;
			}
			~bcLockFreeContainerMemMngConfig() noexcept(true) {}

			virtual void _terminateNode(NodeBaseType* pNode, bool pConcurrent) = 0;
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
				FreeIndexList(bcUINT32 pCapacity, AllocatorType* pAllocator, bool pInitToSequenceIndices = true) : 
					mCapacity(pCapacity), mCount(0), mAllocator(pAllocator)
				{
					//mFreeList = new Type[mCapacity];
					mFreeList = static_cast<Type*>(mAllocator->alloc(sizeof(Type)* mCapacity));
					if (mFreeList != nullptr)
						new(mFreeList)Type[mCapacity];
					else
						//throw Exception(L"Memory Allocation Error");
						// TODO check for allocation fail here
						BC_ASSERT(false);

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

				bcInline void addToFreeList(Type pValue) noexcept(true)
				{
					mFreeList[mCount] = pValue;
					++mCount;
					
					BC_ASSERT(mCount <= mCapacity);
				}

				bcInline Type getFreeItem() noexcept(true)
				{
					BC_ASSERT(mCount > 0);

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
				core_platform::bc_atomic< NodeType* >* mHP;
				core_platform::bc_atomic< NodeType* >* mDLNodes;
				core_platform::bc_atomic< bcINT32 >* mDLClaims;
				core_platform::bc_atomic< bool >* mDLDone;

				FreeIndexList< bcINT32 > mHPFreeList;
				FreeIndexList< bcINT32 > mDLNodesFreeList;

				bcINT32 mDList;
				bcUINT32 mDCount;
				bcINT32* mDLNexts;

				NodeType**  mPList; // used in scan / TODO: replace searching method with some indexing

				AllocatorType* mAllocator;

				ThreadData(bcUINT32 pNumThread, bcUINT32 pNumHP, bcUINT32 pCleanUpThreshold, AllocatorType* pAllocator) : 
					mDList(-1), 
					mDCount(0), 
					mHPFreeList(pNumHP, pAllocator),
					mDLNodesFreeList(pCleanUpThreshold, pAllocator),
					mAllocator(pAllocator)
				{
					BC_ASSERT(pNumHP <= 32767);
					BC_ASSERT(pCleanUpThreshold <= 32767);

					/*mHP = new bcAtomic< NodeType* >[pNumHP];*/
					mHP = static_cast<core_platform::bc_atomic< NodeType* >*>(mAllocator->alloc(sizeof(core_platform::bc_atomic< NodeType* >) * pNumHP));
					/*if (mHP != nullptr)
						new(mHP)bcAtomic< NodeType* >[pNumHP];*/
					if (mHP == nullptr)
					{
						//throw Exception(L"Memory Allocation Error");
						// TODO check for allocation fail here
						BC_ASSERT(false);
					}
					//mDLNodes = new bcAtomic< NodeType* >[pCleanUpThreshold];
					mDLNodes = static_cast<core_platform::bc_atomic< NodeType* >*>(mAllocator->alloc(sizeof(core_platform::bc_atomic< NodeType* >) * pCleanUpThreshold));
					/*if (mDLNodes != nullptr)
						new(mDLNodes)bcAtomic< NodeType* >[pCleanUpThreshold];*/
					if (mDLNodes == nullptr)
					{
						//delete[] mHP;
						mAllocator->free(mHP);
						//throw Exception(L"Memory Allocation Error");
						// TODO check for allocation fail here
						BC_ASSERT(false);
					}
					//mDLClaims = new bcAtomic< bcINT32 >[pCleanUpThreshold];
					mDLClaims = static_cast<core_platform::bc_atomic< bcINT32 >*>(mAllocator->alloc(sizeof(core_platform::bc_atomic< bcINT32 >) * pCleanUpThreshold));
					/*if (mDLClaims != nullptr)
						new(mDLClaims)bcAtomic< bcINT32 >[pCleanUpThreshold];*/
					if (mDLClaims == nullptr)
					{
						/*delete[] mHP;
						delete[] mDLNodes;*/
						mAllocator->free(mHP);
						mAllocator->free(mDLNodes);
						//throw Exception(L"Memory Allocation Error");
						// TODO check for allocation fail here
						BC_ASSERT(false);
					}
					//mDLDone = new bcAtomic< bool >[pCleanUpThreshold];
					mDLDone = static_cast<core_platform::bc_atomic< bool >*>(mAllocator->alloc(sizeof(core_platform::bc_atomic< bool >) * pCleanUpThreshold));
					/*if (mDLDone != nullptr)
						new(mDLDone)bcAtomic< bool >[pCleanUpThreshold];*/
					if (mDLClaims == nullptr)
					{
						/*delete[] mHP;
						delete[] mDLNodes;
						delete[] mDLClaims;*/
						mAllocator->free(mHP);
						mAllocator->free(mDLNodes);
						mAllocator->free(mDLClaims);
						//throw Exception(L"Memory Allocation Error");
						// TODO check for allocation fail here
						BC_ASSERT(false);
					}

					//mDLNexts = new bcINT32[pCleanUpThreshold];
					mDLNexts = static_cast<bcINT32*>(mAllocator->alloc(sizeof(bcINT32)* pCleanUpThreshold));
					/*if (mDLNexts != nullptr)
						new(mDLNexts)bcINT32[pCleanUpThreshold];*/
					if (mDLNexts == nullptr)
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
						BC_ASSERT(false);
					}

					bcUINT32 lNumPList = pNumThread * pNumHP;
					//mPList = new NodeType*[lNumPList];
					mPList = static_cast<NodeType**>(mAllocator->alloc(sizeof(NodeType*)* lNumPList));
					/*if (mPList != nullptr)
						new(mPList)NodeType*[lNumPList];*/
					if (mPList == nullptr)
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
						BC_ASSERT(false);
					}

					// because sometimes in deRefLink we read nullptr, we must use another value (1) for indicating 
					// non-using values, so our FreeIndexList can work property /
					std::memset(mHP, reinterpret_cast<bcINT32>(reinterpret_cast<NodeType*>(1)), pNumHP * sizeof(core_platform::bc_atomic< NodeType* >));
					//std::fill(mHP, mHP + pNumHP, nullptr);
					std::memset(mDLNodes, reinterpret_cast<bcINT32>(nullptr), pCleanUpThreshold * sizeof(core_platform::bc_atomic< NodeType* >));
					//std::fill(mDLNodes, mDLNodes + pCleanUpThreshold, nullptr);
					std::memset(mDLClaims, 0, pCleanUpThreshold * sizeof(core_platform::bc_atomic< bcINT32 >));
					std::memset(mDLDone, false, pCleanUpThreshold * sizeof(core_platform::bc_atomic< bool >));
					std::memset(mDLNexts, -1, pCleanUpThreshold * sizeof(bcINT32));
					std::memset(mPList, reinterpret_cast<bcINT32>(nullptr), sizeof(NodeType**)* lNumPList);
					//std::fill(mPList, mPList + lNumPList, nullptr);
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
					NodeBaseType* lNode = lData.mDLNodes[lIndex].load(core_platform::bc_memory_order::seqcst);
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
						NodeBaseType* lNode = lData.mDLNodes[lIndex].load(core_platform::bc_memory_order::seqcst);
						if (lNode != nullptr && !lData.mDLDone[lIndex].load(core_platform::bc_memory_order::seqcst))
						{
							lData.mDLClaims[lIndex].fetch_add(1, core_platform::bc_memory_order::seqcst);
							if (lNode == lData.mDLNodes[lIndex].load(core_platform::bc_memory_order::seqcst))
							{
								mConfig->_cleanUpNode(static_cast<NodeType*>(lNode));
							}
							lData.mDLClaims[lIndex].fetch_sub(1, core_platform::bc_memory_order::seqcst);
						}
					}
				}
			}

			bcInline bool _isInHPList(NodeType** pPlist, bcUINT32 pNumList, NodeType* lNode)
			{
				for(bcUINT32 i = 0; i < pNumList; ++i)
				{
					if(pPlist[i] == lNode)
						return true;
				}
				return false;
			}
				
			bcInline void _scan()
			{
				bcUINT32 lThreadIndex = _getCurrentThreadIndex();
				ThreadData& lData = mThreadData[lThreadIndex];

				bcINT32 lIndex = lData.mDList;
				while(lIndex != -1)
				{
					NodeBaseType* lNode = lData.mDLNodes[lIndex].load(core_platform::bc_memory_order::seqcst);
					if (lNode->mReferenceCount.load(core_platform::bc_memory_order::seqcst) == 0)
					{
						lNode->mTrace.store(true, core_platform::bc_memory_order::seqcst);
						if (lNode->mReferenceCount.load(core_platform::bc_memory_order::seqcst) != 0)
						{
							lNode->mTrace.store(false, core_platform::bc_memory_order::seqcst);
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
						NodeType* lNode = mThreadData[lThread].mHP[lH].load(core_platform::bc_memory_order::seqcst);
						if(lNode != nullptr)
						{
							/*bool lExist = false;
							for (bcUINT32 i = 0; i < lNumPList; ++i)
							{
							if(lPlist[i] == lNode)
							{
							lExist = true;
							break;
							}
							}
							if(lExist == false)*/
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
					NodeType* lNode = lData.mDLNodes[lIndex].load(core_platform::bc_memory_order::seqcst);
					lData.mDList = lData.mDLNexts[lIndex];

					if (lNode->mReferenceCount.load(core_platform::bc_memory_order::seqcst) == 0 &&
						lNode->mTrace.load(core_platform::bc_memory_order::seqcst) == true &&
					   !_isInHPList(lPlist, lNumPList, lNode))
					{
						lData.mDLNodes[lIndex].store((NodeType*)nullptr, core_platform::bc_memory_order::seqcst);
						if (lData.mDLClaims[lIndex].load(core_platform::bc_memory_order::seqcst) == 0)
						{
							mConfig->_terminateNode(static_cast<NodeType*>(lNode), false);
							//mAllocator.Free(lNode);
							mConfig->_free(static_cast<NodeType*>(lNode));

							lData.mDLNodesFreeList.addToFreeList(lIndex);

							continue;
						}
						mConfig->_terminateNode(static_cast<NodeType*>(lNode), true);
						lData.mDLDone[lIndex].store(true, core_platform::bc_memory_order::seqcst);
						lData.mDLNodes[lIndex].store(lNode, core_platform::bc_memory_order::seqcst);
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

			core_platform::bc_atomic<bcUINT32> mLastThreadIndex;
			core_platform::bc_thread_local<bcINT32> mMyIndex;

			AllocatorType mAllocator;

			bcUINT32 _getCurrentThreadIndex()
			{
				/*if (*mMyIndex == -1)
				*mMyIndex = mLastThreadIndex.fetch_add(1U, core_platform::bc_memory_order::relaxed);
				return *mMyIndex;*/

				bcINT32* l_index = mMyIndex.get();
				
				if (!l_index)
				{
					l_index = BC_NEW(bc_alloc_type::unknown) bcINT32;
					*l_index = mLastThreadIndex.fetch_add(1U, core_platform::bc_memory_order::relaxed);

					mMyIndex.set(l_index);
				}

				return *l_index;
			}

			/*virtual void _terminateNode(NodeType* pNode, bool pConcurrent) = 0;
			virtual void _cleanUpNode(NodeType* pNode) = 0;

			virtual NodeType* _alloc() = 0;
			virtual void _free(NodeType*) = 0;*/

		public:
			bcLockFreeMemMng(ConfigType* pConfig) 
				: mConfig(pConfig), 
				mLastThreadIndex(0),
				mMyIndex([](bcINT32* p_pointer)->void { BC_DELETE(p_pointer); })
			{
				//mThreadData = new ThreadData[mConfig->mNumThread](mConfig->mNumThread, mConfig->mNumHP, mConfig->mCleanUpThreshold);
				//mThreadData = static_cast<ThreadData*>(std::malloc(sizeof(ThreadData) * mConfig->mNumThread));
				mThreadData = static_cast<ThreadData*>(mAllocator.alloc(sizeof(ThreadData)* mConfig->mNumThread));
				if (mThreadData == nullptr)
				{
					// TODO check for allocation fail here
					BC_ASSERT(false);
				}
				for (bcUINT32 i = 0; i < mConfig->mNumThread; ++i)
					new (&mThreadData[i]) ThreadData(mConfig->mNumThread, mConfig->mNumHP, mConfig->mCleanUpThreshold, &mAllocator);

				//mMyIndex.Initialize(mConfig->mNumThread , -1);
			}
			~bcLockFreeMemMng()
			{
				for (bcUINT32 i = 0; i < mConfig->mNumThread; ++i)
					mThreadData[i].~ThreadData();
				mAllocator.free(mThreadData);

				//mMyIndex.Destroy();
			}

			bcInline NodeType* deRefLink(LinkType* pLink)
			{
				LinkType* lLink = pLink;

				bcUINT32 lThreadIndex = _getCurrentThreadIndex();
				bcINT32 lIndex = mThreadData[lThreadIndex].mHPFreeList.getFreeItem();
				core_platform::bc_atomic< NodeType* >& lHP = mThreadData[lThreadIndex].mHP[lIndex];
	
				NodeType* lNode = nullptr;
				while(true)
				{
					lNode = static_cast< NodeType* >(lLink->getNode());

					lHP.store(lNode, core_platform::bc_memory_order::seqcst);

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
				core_platform::bc_atomic< NodeType* >& lHP = mThreadData[lThreadIndex].mHP[lIndex];

				lHP.store(lNode, core_platform::bc_memory_order::seqcst);
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
					// because sometimes in deRefLink we read nullptr, we must use another value (1) for indicating 
					// non-using values, so our FreeIndexList can work property /
					NodeType* lNewValue = reinterpret_cast<NodeType*>(1); 
					if (lData.mHP[i].compare_exchange_strong(
						&lNodeCopy,
						lNewValue,
						core_platform::bc_memory_order::seqcst,
						core_platform::bc_memory_order::seqcst))
					{
						lData.mHPFreeList.addToFreeList(i);
						return;
					}
				}

				BC_ASSERT(false);
			}

			/*bcInline bool compareAndSwapRef(LinkType* pLink, NodeType* pExpected, NodeType* pNew)
			{
				LinkBaseType* lLink = static_cast< LinkBaseType* >(pLink);
				NodeBaseType* lExpected = static_cast< NodeBaseType* >(pExpected);

				if(bcAtomicOperation::bcAtomicCompareExchangeStrong(
					*lLink->getAtomicNode(),
					&lExpected,
					pNew,
					core_platform::bc_memory_order::relaxed,
					core_platform::bc_memory_order::relaxed))
				{
					if(pNew != nullptr)
					{
						bcAtomicOperation::bcAtomicFetchAdd(pNew->mReferenceCount, bcUINT16(1), core_platform::bc_memory_order::relaxed);
						bcAtomicOperation::bcAtomicStore(pNew->mTrace, false, core_platform::bc_memory_order::relaxed);
					}
					if(lExpected != nullptr)
					{
						bcAtomicOperation::bcAtomicFetchSub(lExpected->mReferenceCount, bcUINT16(1), core_platform::bc_memory_order::relaxed);
					}
					return true;
				}

				return false;
			}*/
			// TODO check this function for concurrency problem
			bcInline bool compareAndSwapRef(LinkType* pLink, LinkType* pExpected, LinkType* pNew)
			{
				LinkType* lLink = pLink;
				LinkType* lExpected = pExpected;
				LinkType* lNew = pNew;

				//NodeBaseType* lLinkCopy = static_cast<NodeBaseType*>(lLink->getNode());
				
				while(true)
				{
					NodeType* lExpectedNode = lExpected->getNode();
					NodeType* lNewNode = lNew->getNode();

					if (lLink->getAtomicNode()->compare_exchange_strong(
						&lExpectedNode,
						lNewNode,
						core_platform::bc_memory_order::seqcst,
						core_platform::bc_memory_order::seqcst))
					{
						//TODO check here
						NodeType* lNewExpected = lExpected->getNode();
						NodeType* lNewNew = lNew->getNode();

						BC_ASSERT(lNewExpected == lExpectedNode && lNewNew == lNewNode);
						/*if(!(lNewExpected == lExpected && lNewNew == lNew))
						{
							bcAtomicOperation::bcAtomicStore(*lLink->getAtomicNode(), lLinkCopy, core_platform::bc_memory_order::relaxed);
							continue;
						}*/

						if(lNewNode != nullptr)
						{
							lNewNode->mReferenceCount.fetch_add(bcUINT16(1), core_platform::bc_memory_order::seqcst);
							lNewNode->mTrace.store(false, core_platform::bc_memory_order::seqcst);
						}
						if(lExpectedNode != nullptr)
						{
							lExpectedNode->mReferenceCount.fetch_sub(bcUINT16(1), core_platform::bc_memory_order::seqcst);
						}
						return true;
					}

					return false;
				}
			}
			/*bcInline void storeRef(LinkType* pLink, NodeType* pNode)
			{
				NodeType* lOldNode = pLink->getNode();

				bcAtomicOperation::bcAtomicStore(*pLink->getAtomicNode(), pNode, core_platform::bc_memory_order::relaxed);
				if(pNode != nullptr)
				{
					bcAtomicOperation::bcAtomicFetchAdd(pNode->mReferenceCount, bcUINT16(1), core_platform::bc_memory_order::relaxed);
					bcAtomicOperation::bcAtomicStore(pNode->mTrace, false, core_platform::bc_memory_order::relaxed);
				}
				if(lOldNode != nullptr)
				{
					bcAtomicOperation::bcAtomicFetchSub(lOldNode->mReferenceCount, bcUINT16(1), core_platform::bc_memory_order::relaxed);
				}
			}*/
			// TODO check this function for concurrency problem
			bcInline void storeRef(LinkType* pLink, LinkType* pNew)
			{
				LinkType* lLink = pLink;
				LinkType* lNew = pNew;

				NodeType* lOldNode = lLink->getNode();
				NodeType* lNewNode = lNew->getNode();

				lLink->getAtomicNode()->store(lNewNode, core_platform::bc_memory_order::seqcst);
				if(lNewNode != nullptr)
				{
					lNewNode->mReferenceCount.fetch_add(bcUINT16(1), core_platform::bc_memory_order::seqcst);
					lNewNode->mTrace.store(false, core_platform::bc_memory_order::seqcst);
				}
				if(lOldNode != nullptr)
				{
					lOldNode->mReferenceCount.fetch_sub(bcUINT16(1), core_platform::bc_memory_order::seqcst);
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

				lData.mHP[lIndex].store(lNode, core_platform::bc_memory_order::seqcst);

				return lNode;
			}
			// Call object deconstructure in _free function
			bcInline void deleteNode(NodeType* pNode)
			{
				NodeType* lNode = pNode;
				releaseRef(lNode);
				lNode->m_del.store(true, core_platform::bc_memory_order::seqcst);
				lNode->mTrace.store(false, core_platform::bc_memory_order::seqcst);

				bcUINT32 lThreadIndex = _getCurrentThreadIndex();
				ThreadData& lData = mThreadData[lThreadIndex];
				bcINT32 lIndex = lData.mDLNodesFreeList.getFreeItem();

				lData.mDLDone[lIndex].store(false, core_platform::bc_memory_order::seqcst);
				lData.mDLNodes[lIndex].store(lNode, core_platform::bc_memory_order::seqcst);
				lData.mDLNexts[lIndex] = lData.mDList;
				lData.mDList = lIndex;
				lData.mDCount += 1;

				while(true)
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

			// Free allocated space for all deleted nodes, call this in underlying data structure destructure /
			void forceCleanUp()
			{
				for (bcUINT32 tIndex = 0; tIndex < mConfig->mNumThread; tIndex++)
				{
					ThreadData& lData = mThreadData[tIndex];
					while (lData.mDList != -1)
					{
						bcINT32 lIndex = lData.mDList;
						NodeType* lNode = lData.mDLNodes[lIndex].load(core_platform::bc_memory_order::seqcst);

						mConfig->_free(static_cast<NodeType*>(lNode));

						lData.mDList = lData.mDLNexts[lIndex];
					}
				}
			}
		};
	}
}