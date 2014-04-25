#pragma once

#ifndef MemMngFreeList_H
#define MemMngFreeList_H

#include "CorePCH.h"
#include "bcAtomic.h"
#include "bcMutex.h"
//#include "bcConditionVariable.h"

namespace BlackCat
{
	namespace Core
	{
#ifdef BC_MEMORY_ENABLE

		template< typename T >
		class bcMemMngFreeList
		{
		private:
			struct FreeListChunk
			{
				void* mPointer;
				FreeListChunk* mNext;
			};

			struct MemMngFreeListItem
			{
				MemMngFreeListItem* mNextFree;
				T mData;
			};

		protected:
			bcSIZE mAllocSize;
			FreeListChunk* mFirstBlock;
			bcAtomic< MemMngFreeListItem* > mFree;
			bcAtomic< bcUINT32 > mCount;
			bcMutex mMutex;

			bcInline void _allocNewBlock()
			{
				const bcSIZE lAllocSize = mAllocSize;
				//const bcSIZE lAllocSize = 36;
				const bcSIZE lNumAllocPerBatch = lAllocSize / sizeof(MemMngFreeListItem);

				// Record allocated blocks to free on deconstructure
				FreeListChunk* lPrevLastBlock = NULL;
				FreeListChunk* lLastBlock = mFirstBlock;
				while (lLastBlock)
				{
					lPrevLastBlock = lLastBlock;
					lLastBlock = lLastBlock->mNext;
				}

				lLastBlock = (FreeListChunk*)std::malloc(sizeof(FreeListChunk));
				if (lLastBlock == bcNULL)
				{
					// TODO check for allocation fail here
					bcAssert(bcFALSE);
				}
				lLastBlock->mPointer = NULL;
				lLastBlock->mNext = NULL;

				if (lPrevLastBlock)
					lPrevLastBlock->mNext = lLastBlock;
				else
					mFirstBlock = lLastBlock;

				lLastBlock->mPointer = std::malloc(lAllocSize);

				MemMngFreeListItem* lAllocBatch = (MemMngFreeListItem*)(lLastBlock->mPointer);

				// == Because we protect this method with mutex we can load mFree to a local variable == MRB ==
				MemMngFreeListItem* lLocalFree = bcAtomicOperation::bcAtomicLoad(mFree, bcMemoryOrder_Relaxed);
				for (bcUINT32 i = 0; i < lNumAllocPerBatch; ++i)
				{
					lAllocBatch->mNextFree = lLocalFree;
					lLocalFree = lAllocBatch;
					++lAllocBatch;
				}
				bcAtomicOperation::bcAtomicStore(mFree, lLocalFree, bcMemoryOrder_Relaxed);
			}

		public:
			bcMemMngFreeList() :
				mFirstBlock(NULL)
			{
				bcAtomicOperation::bcAtomicStore(mFree, (MemMngFreeListItem*)bcNULL);
				bcAtomicOperation::bcAtomicStore(mCount, 0U);
			};
			~bcMemMngFreeList()
			{
				FreeListChunk* lCurrent = mFirstBlock;
				while (lCurrent)
				{
					std::free(lCurrent->mPointer);
					lCurrent = lCurrent->mNext;
				}
			};

			// == Reserved space in byte ==
			void initialize(bcSIZE pAllocSize = 65536)
			{
				mAllocSize = pAllocSize;
				_allocNewBlock();
			}

			// Call object constructure after this function
			bcInline T* alloc()
			{
				MemMngFreeListItem* lLocalFree = bcAtomicOperation::bcAtomicLoad(mFree, bcMemoryOrder_SeqCst);
				while (lLocalFree != bcNULL)
				{
					if (bcAtomicOperation::bcAtomicCompareExchangeStrong(
						mFree,
						&lLocalFree,
						lLocalFree->mNextFree,
						bcMemoryOrder_SeqCst,
						bcMemoryOrder_SeqCst))
					{
						lLocalFree->mNextFree = bcNULL;
						T* lResult = &lLocalFree->mData;

						bcAtomicOperation::bcAtomicFetchAdd(mCount, 1U, bcMemoryOrder_SeqCst);

						return lResult;
					}
				}
				// If next block is NUll, allocate a new chunk and connect current
				// block (first block in current chunk) to changed mFree that point
				// to last block to new chunk
				if (lLocalFree == bcNULL)
				{
					bcLockGaurd lGaurd(mMutex);

					MemMngFreeListItem* lFreePtr = bcAtomicOperation::bcAtomicLoad(mFree, bcMemoryOrder_Relaxed);

					if (lFreePtr == bcNULL)
					{
						//// Change mFree to NULL. This is for AllocNewBlock function, because 
						//// in this function for every block we call Free function, so we 
						//// changed mFree to NULL, so first block in new chunk will point to
						//// NULL in its mNext pointer
						//bcAtomicOperation::bcAtomicStore(mFree, (MemMngFreeListItem*)bcNULL, bcMemoryOrder_Relaxed);
						// This function will change mFree to last block in next allocated
						// chunk
						_allocNewBlock();
						//// Connect current block (first block in current chunk) to mFree 
						//// that point to last block in next chunk
						//lFreePtr->mNextFree = bcAtomicOperation::bcAtomicLoad(mFree, bcMemoryOrder_Relaxed);

						lFreePtr = bcAtomicOperation::bcAtomicLoad(mFree, bcMemoryOrder_Relaxed);
						bcAtomicOperation::bcAtomicStore(mFree, lFreePtr->mNextFree, bcMemoryOrder_Relaxed);

						lFreePtr->mNextFree = bcNULL;
						T* lResult = &lFreePtr->mData;

						bcAtomicOperation::bcAtomicFetchAdd(mCount, 1U, bcMemoryOrder_Relaxed);

						return lResult;
					}
				}

				return alloc();
			}
			// Call object destructure before this function
			bcInline void free(T* pPtr)
			{
				MemMngFreeListItem* lNode = (MemMngFreeListItem*)((bcUBYTE*)pPtr - sizeof(MemMngFreeListItem*));

				MemMngFreeListItem* lLocalFree = bcAtomicOperation::bcAtomicLoad(mFree, bcMemoryOrder_SeqCst);
				lNode->mNextFree = lLocalFree;
				while (lLocalFree != bcNULL)
				{
					if (bcAtomicOperation::bcAtomicCompareExchangeStrong(
						mFree,
						&lNode->mNextFree,
						lNode,
						bcMemoryOrder_SeqCst,
						bcMemoryOrder_SeqCst))
					{
						bcAtomicOperation::bcAtomicFetchSub(mCount, 1U, bcMemoryOrder_SeqCst);
						return;
					}
					else
						lLocalFree = lNode->mNextFree;
				}

				// Block any thread in alloc function, so we can free our block safely
				if (lLocalFree == bcNULL)
				{
					bcLockGaurd lGaurd(mMutex);

					lLocalFree = bcAtomicOperation::bcAtomicLoad(mFree, bcMemoryOrder_Relaxed);
					if (lLocalFree == bcNULL)
					{
						lNode->mNextFree = lLocalFree;
						bcAtomicOperation::bcAtomicStore(mFree, lNode, bcMemoryOrder_Relaxed);

						bcAtomicOperation::bcAtomicFetchSub(mCount, 1U, bcMemoryOrder_Relaxed);
					}
				}
			}

			bcInline bcUINT32 count()
			{
				return bcAtomicOperation::bcAtomicLoad(mCount, bcMemoryOrder_SeqCst);
			}
		};

		//template< typename T >
		//class MemMngFreeList
		//{
		//private:
		//	struct FreeListChunk
		//	{
		//		void* mPointer;
		//		FreeListChunk* mNext;
		//	};

		//	struct MemMngFreeListItem
		//	{
		//		MemMngFreeListItem* mNextFree;
		//		T mData;
		//	};

		//protected:
		//	bcSIZE mAllocSize;
		//	FreeListChunk* mFirstBlock;
		//	bcAtomic< MemMngFreeListItem* > mFree;
		//	bcAtomic< bcUINT32 > mCount;

		//	//bcAtomic< bcBOOL > mAllocNewBlock;
		//	bcMutex mMutex;
		//	//bcConditionVariableAny mCondition;

		//	bcInline void _allocNewBlock()
		//	{
		//		const bcSIZE lAllocSize = mAllocSize;
		//		//const bcSIZE lAllocSize = 36;
		//		const bcSIZE lNumAllocPerBatch = lAllocSize / sizeof(MemMngFreeListItem);
		//		
		//		// Record allocated blocks to free on deconstructure
		//		FreeListChunk* lPrevLastBlock = NULL;
		//		FreeListChunk* lLastBlock = mFirstBlock;
		//		while(lLastBlock)
		//		{
		//			lPrevLastBlock = lLastBlock;
		//			lLastBlock = lLastBlock->mNext;
		//		}
		//		
		//		lLastBlock = (FreeListChunk*)std::malloc(sizeof(FreeListChunk));
		//		if (lLastBlock == bcNULL)
		//		{
		//			// TODO check for allocation fail here
		//			bcAssert(bcFALSE);
		//		}
		//		lLastBlock->mPointer = NULL;
		//		lLastBlock->mNext = NULL;
		//		
		//		if(lPrevLastBlock)
		//			lPrevLastBlock->mNext = lLastBlock;
		//		else
		//			mFirstBlock = lLastBlock;

		//		/*lLastBlock->mPointer = VirtualAlloc(NULL, lAllocSize, MEM_COMMIT, PAGE_READWRITE);*/
		//		lLastBlock->mPointer = std::malloc(lAllocSize);

		//		MemMngFreeListItem* lAllocBatch = (MemMngFreeListItem*)(lLastBlock->mPointer);
	 //
		//		// == Because we protect this method with mutex we can load mFree to a local variable == MRB ==
		//		MemMngFreeListItem* lLocalFree = bcAtomicOperation::bcAtomicLoad(mFree, bcMemoryOrder_Relaxed);
		//		for(bcUINT32 i = 0; i < lNumAllocPerBatch; ++i)
		//		{
		//			/*Free(&lAllocBatch->mData);*/
		//			
		//			lAllocBatch->mNextFree = lLocalFree;
		//			lLocalFree = lAllocBatch;
		//			++lAllocBatch;
		//		}
		//		bcAtomicOperation::bcAtomicStore(mFree, lLocalFree, bcMemoryOrder_Relaxed);
		//	}

		//public:
		//	MemMngFreeList() : 
		//		mFirstBlock(NULL)
		//	{
		//		bcAtomicOperation::bcAtomicStore(mFree, (MemMngFreeListItem*)bcNULL);
		//		bcAtomicOperation::bcAtomicStore(mCount, 0U);
		//		//bcAtomicOperation::bcAtomicInit(mAllocNewBlock, bcFALSE);
		//	};
		//	~MemMngFreeList()
		//	{
		//		FreeListChunk* lCurrent = mFirstBlock;
		//		while(lCurrent)
		//		{
		//			std::free(lCurrent->mPointer);
		//			lCurrent = lCurrent->mNext;
		//		}
		//	};

		//	// == Reserved space in byte ==
		//	void initialize(bcSIZE pAllocSize = 65536)
		//	{
		//		mAllocSize = pAllocSize;
		//		_allocNewBlock();
		//	}

		//	// Call object constructure after this function
		//	bcInline T* alloc()
		//	{
		//		MemMngFreeListItem* lLocalFree = bcAtomicOperation::bcAtomicLoad(mFree, bcMemoryOrder_SeqCst);
		//		while(lLocalFree != bcNULL && lLocalFree->mNextFree != bcNULL)
		//		{
		//			if(bcAtomicOperation::bcAtomicCompareExchangeStrong(
		//				mFree, 
		//				&lLocalFree, 
		//				lLocalFree->mNextFree,
		//				bcMemoryOrder_SeqCst,
		//				bcMemoryOrder_SeqCst))
		//			{
		//				//T* lResult = new(lLocalFree) T;
		//				lLocalFree->mNextFree = bcNULL;
		//				T* lResult = &lLocalFree->mData;

		//				bcAtomicOperation::bcAtomicFetchAdd(mCount, 1U, bcMemoryOrder_SeqCst);

		//				return lResult;
		//			}
		//		}
		//		// If next block is NUll, allocate a new chunk and connect current
		//		// block (first block in current chunk) to changed mFree that point
		//		// to last block to new chunk
		//		/*bcBOOL lExpected = bcFALSE;
		//		if(bcAtomicOperation::bcAtomicCompareExchangeStrong(
		//			mAllocNewBlock, 
		//			&lExpected,
		//			bcTRUE,
		//			bcMemoryOrder_Relaxed,
		//			bcMemoryOrder_Relaxed))*/
		//		if (lLocalFree == bcNULL || lLocalFree->mNextFree == bcNULL)
		//		{
		//			bcLockGaurd lGaurd(mMutex);

		//			MemMngFreeListItem* lFreePtr = bcAtomicOperation::bcAtomicLoad(mFree, bcMemoryOrder_Relaxed);

		//			if (lFreePtr == bcNULL || lFreePtr->mNextFree == bcNULL)
		//			{
		//				// Change mFree to NULL. This is for AllocNewBlock function, because 
		//				// in this function for every block we call Free function, so we 
		//				// changed mFree to NULL, so first block in new chunk will point to
		//				// NULL in its mNext pointer
		//				bcAtomicOperation::bcAtomicStore(mFree, (MemMngFreeListItem*)bcNULL, bcMemoryOrder_Relaxed);
		//				// This function will change mFree to last block in next allocated
		//				// chunk
		//				_allocNewBlock();
		//				// Connect current block (first block in current chunk) to mFree 
		//				// that point to last block in next chunk
		//				lFreePtr->mNextFree = bcAtomicOperation::bcAtomicLoad(mFree, bcMemoryOrder_Relaxed);;

		//				//T* lResult = new(lFreePtr)T;
		//				lFreePtr->mNextFree = bcNULL;
		//				T* lResult = &lFreePtr->mData;
		//				bcAtomicOperation::bcAtomicFetchAdd(mCount, 1U, bcMemoryOrder_Relaxed);

		//				/*bcAtomicOperation::bcAtomicStore(mAllocNewBlock, bcFALSE, bcMemoryOrder_Relaxed);
		//				mCondition.notifyAll();*/

		//				return lResult;
		//			}
		//			/*else
		//				return alloc();*/
		//		}
		//		/*else
		//		{
		//		bcUniqueLock lGaurd(mMutex);
		//		mCondition.wait(
		//		lGaurd,
		//		[&]()->bcBOOL { return !bcAtomicOperation::bcAtomicLoad(mAllocNewBlock, bcMemoryOrder_Relaxed); });

		//		return alloc();
		//		}*/

		//		return alloc();
		//	}
		//	// Call object destructure before this function
		//	bcInline void free(T* pPtr)
		//	{
		//		MemMngFreeListItem* lNode = (MemMngFreeListItem*)((bcUBYTE*)pPtr - sizeof(MemMngFreeListItem*));
		//		
		//		MemMngFreeListItem* lLocalFree = bcAtomicOperation::bcAtomicLoad(mFree, bcMemoryOrder_SeqCst);
		//		lNode->mNextFree = lLocalFree;
		//		while (lLocalFree != bcNULL && lLocalFree->mNextFree != bcNULL)
		//		{
		//			if (bcAtomicOperation::bcAtomicCompareExchangeStrong(
		//				mFree,
		//				&lNode->mNextFree,
		//				lNode,
		//				bcMemoryOrder_SeqCst,
		//				bcMemoryOrder_SeqCst))
		//			{
		//				bcAtomicOperation::bcAtomicFetchSub(mCount, 1U, bcMemoryOrder_SeqCst);
		//				return;
		//			}
		//			else
		//				lLocalFree = lNode->mNextFree;
		//		}

		//		// Block any any thread in alloc function, so we can free our block safely
		//		/*bcBOOL lExpected = bcFALSE;
		//		if (bcAtomicOperation::bcAtomicCompareExchangeStrong(
		//			mAllocNewBlock,
		//			&lExpected,
		//			bcTRUE,
		//			bcMemoryOrder_Relaxed,
		//			bcMemoryOrder_Relaxed))*/
		//		if (lLocalFree == bcNULL || lLocalFree->mNextFree == bcNULL)
		//		{
		//			bcLockGaurd lGaurd(mMutex);

		//			lLocalFree = bcAtomicOperation::bcAtomicLoad(mFree, bcMemoryOrder_Relaxed);
		//			if (lLocalFree == bcNULL || lLocalFree->mNextFree == bcNULL)
		//			{
		//				lNode->mNextFree = lLocalFree;
		//				bcAtomicOperation::bcAtomicStore(mFree, lNode, bcMemoryOrder_Relaxed);

		//				bcAtomicOperation::bcAtomicFetchSub(mCount, 1U, bcMemoryOrder_Relaxed);

		//				/*bcAtomicOperation::bcAtomicStore(mAllocNewBlock, bcFALSE, bcMemoryOrder_Relaxed);
		//				mCondition.notifyAll();*/
		//			}
		//			/*else
		//			{
		//				free(pPtr);
		//				return;
		//			}*/
		//		}
		//		/*else
		//		{
		//		bcUniqueLock lGaurd(mMutex);
		//		mCondition.wait(
		//		lGaurd,
		//		[&]()->bcBOOL { return !bcAtomicOperation::bcAtomicLoad(mAllocNewBlock, bcMemoryOrder_Relaxed); });

		//		free(pPtr);
		//		return;
		//		}*/

		//		//free(pPtr);

		//		//bcAtomicOperation::bcAtomicFetchSub(mCount, 1U, bcMemoryOrder_Relaxed);
		//	}

		//	bcInline bcUINT32 count()
		//	{
		//		return bcAtomicOperation::bcAtomicLoad(mCount, bcMemoryOrder_SeqCst);
		//	}
		//};

#endif
	}
}

#endif