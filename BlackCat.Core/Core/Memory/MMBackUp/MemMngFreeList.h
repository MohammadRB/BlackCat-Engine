
#ifndef MemMngFreeList_H
#define MemMngFreeList_H

#include "CorePCH.h"

namespace BlackCat
{
	namespace Core
	{
#ifdef BC_MEMORY_ENABLE

		template< typename T >
		class MemMngFreeList
		{
		public:
			struct MemMngFreeListItem
			{
				MemMngFreeListItem* mNextFree;
				T mData;
				MemMngFreeListItem* mNext;
				MemMngFreeListItem* mPrev;
			};

		private:
			struct FreeListChunk
			{
				void* mPointer;
				FreeListChunk* mNext;
			};

		protected:
			bcSIZE mAllocSize;
			FreeListChunk* mFirstBlock;
			MemMngFreeListItem* mFree;
			// Two node for accessing other nodes, this two nodes doesn't contain any data
			MemMngFreeListItem mHead;
			MemMngFreeListItem mTail;
			bcUINT32 mCount;

			bcInline void _allocNewBlock()
			{
				const bcSIZE lAllocSize = mAllocSize;
				//const bcSIZE lAllocSize = 36;
				const bcSIZE lNumAllocPerBatch = lAllocSize / sizeof(MemMngFreeListItem);
				
				// Record allocated blocks to free on deconstructure
				FreeListChunk* lPrevLastBlock = NULL;
				FreeListChunk* lLastBlock = mFirstBlock;
				while(lLastBlock)
				{
					lPrevLastBlock = lLastBlock;
					lLastBlock = lLastBlock->mNext;
				}
				
				lLastBlock = (FreeListChunk*)malloc(sizeof(FreeListChunk));
				lLastBlock->mPointer = NULL;
				lLastBlock->mNext = NULL;
				
				if(lPrevLastBlock)
					lPrevLastBlock->mNext = lLastBlock;
				else
					mFirstBlock = lLastBlock;

				/*lLastBlock->mPointer = VirtualAlloc(NULL, lAllocSize, MEM_COMMIT, PAGE_READWRITE);*/
				lLastBlock->mPointer = malloc(lAllocSize);

				MemMngFreeListItem* lAllocBatch = (MemMngFreeListItem*)(lLastBlock->mPointer);
	 
				for(bcUINT32 i = 0; i < lNumAllocPerBatch; ++i)
				{
					/*Free(&lAllocBatch->mData);*/
					
					lAllocBatch->mNextFree = mFree;
					mFree = lAllocBatch;
					++lAllocBatch;
				}
			}

		public:
			MemMngFreeList() : mFirstBlock(NULL), mFree(NULL), mCount(0)
			{
				mHead.mPrev = NULL;
				mHead.mNext = &mTail;
				mTail.mPrev = &mHead;
				mTail.mNext = NULL;
			};
			~MemMngFreeList()
			{
				FreeListChunk* lCurrent = mFirstBlock;
				while(lCurrent)
				{
					free(lCurrent->mPointer);
					lCurrent = lCurrent->mNext;
				}
			};
			void Initialize(bcSIZE pAllocSize = 65536)
			{
				/*mFirstBlock = (FreeListBlock*)malloc(sizeof(FreeListBlock));
				mFirstBlock->mPointer = NULL;
				mFirstBlock->mNext = NULL;*/
				
				mAllocSize = pAllocSize;
				_allocNewBlock();
			}

			bcInline T* Alloc()
			{
				/*T* Result = Free;
				Free = *((T**)Free);
				return Result;*/

				++mCount;

				if(mFree->mNextFree)
				{
					mTail.mPrev->mNext = mFree;
					mFree->mPrev = mTail.mPrev;
					mFree->mNext = &mTail;
					mTail.mPrev = mFree;

					T* lResult = new(&mFree->mData) T;
					mFree = mFree->mNextFree;
					return lResult;
				}
				// If next block is NUll, allocate a new chunk and connect current
				// block (first block in current chunk) to changed mFree that point
				// to last block to new chunk
				else
				{
					MemMngFreeListItem* lFreePtr = mFree;
					// Change mFree to NULL. This is for AllocNewBlock function, because 
					// in this function for every block we call Free function, so we 
					// changed mFree to NULL, so first block in new chunk will point to
					// NULL in its mNext pointer
					mFree = NULL;
					// This function will change mFree to last block in next allocated
					// chunk
					_allocNewBlock();
					// Connect current block (first block in current chunk) to mFree 
					// that point to last block in next chunk
					lFreePtr->mNextFree = mFree;

					mTail.mPrev->mNext = lFreePtr;
					lFreePtr->mPrev = mTail.mPrev;
					lFreePtr->mNext = &mTail;
					mTail.mPrev = lFreePtr;
					
					T* lResult = &lFreePtr->mData;
					return lResult;
				}
			}
			bcInline void Free(T* pPtr)
			{
				/**(T**)(pPtr - 1) = Free;
				Free = Ptr;*/

				--mCount;

				MemMngFreeListItem* lNode = (MemMngFreeListItem*)((bcUBYTE*)pPtr - sizeof(MemMngFreeListItem*));
				lNode->mNextFree = mFree;
				mFree = lNode;

				lNode->mPrev->mNext = lNode->mNext;
				lNode->mNext->mPrev = lNode->mPrev;
			}

			bcInline bcUINT32 Count()
			{
				return mCount;
			}

			bcInline MemMngFreeListItem* First()
			{
				if(mCount != 0)
					return mHead.mNext;
				else
					return NULL;
			}

			bcInline MemMngFreeListItem* Tail()
			{
				return &mTail;
			}
		};

#endif
	}
}

#endif