//  [8/29/2013 MRB]

#include "CorePCH.h"
#include "Allocators.h"
#include "CoreExceptions.h"

namespace BlackCat
{
	namespace Core
	{
#ifdef BC_MEMORY_ENABLE

		bcHeapAllocator::bcHeapAllocator()
		{
		}
		bcHeapAllocator::~bcHeapAllocator()
		{
			SAFE_ALIGNED_FREE(mHeap);
		}
		void bcHeapAllocator::initialize(bcSIZE pSize, const bcCHAR* pTag)
		{
			mSize = pSize;

			// == We alloc mHeap by min align defined in CorePCH because we
			// want all of our allocations have MIN_Align == MRB ==
			mHeap = (bcUBYTE*)_aligned_malloc(mSize * sizeof(bcUBYTE), BC_MEMORY_MIN_ALIGN);

			if(!mHeap)
				throw Exception(L"Memory Allocation Error");

			mBlockAllocator.Initialize();

			mFirstMemBlock = mBlockAllocator.Alloc();
			mFirstMemBlock->dataAddress(reinterpret_cast<bcSIZE>(mHeap));
			mFirstMemBlock->size(mSize);

			mTop = mFirstMemBlock;

			mNumFragmentation = 0;

			bcAllocator::mTracer.initialize(pSize);
			bcAllocator::mTracer.acceptOverHead(sizeof(bcHeapAllocatorMemBlock));
			bcAllocator::tag(pTag);
		}

		void* bcHeapAllocator::alloc(MemBlock* pMemBlock)
		{
			void* lReturnPointer;
			bcHeapAllocatorMemBlock* lMemBlock = NULL;

			bcSIZE lRequireSize =  pMemBlock->size();

			bcAssert( lRequireSize > 0 );

			bcHeapAllocatorMemBlock* lPrevBlock = NULL;
			bcHeapAllocatorMemBlock* lCurrBlock = NULL;
			if( mTop != NULL && mTop->size() >= lRequireSize )
				lCurrBlock = mTop;
			else
			{
				lCurrBlock = mFirstMemBlock;
				while(lCurrBlock && ( !(lCurrBlock->free()) || lCurrBlock->size() < lRequireSize ))
				{
					//lPrevBlock = lCurrBlock;
					lCurrBlock = lCurrBlock->next();
				}
			}
			if( lCurrBlock && lCurrBlock->free() )
			{
				lPrevBlock = lCurrBlock->prev();
				// Check if we must divide our free block, do it
				if( lCurrBlock->size() > lRequireSize )
				{
					lMemBlock = mBlockAllocator.Alloc();
					
					/*if(!lMemBlock)
						throw Exception(L"Error in HeapAllocator.CPP: Memory Allocation Error");*/
					lMemBlock->dataAddress(lCurrBlock->dataAddress());
					//lMemBlock->free(bcFALSE);
					lMemBlock->size(lRequireSize);
					lMemBlock->prev(lPrevBlock);
					lMemBlock->next(lCurrBlock);
					
					pMemBlock->allocatorsExtra(lMemBlock);

					if(lPrevBlock)
						lPrevBlock->next(lMemBlock);

					lCurrBlock->dataAddress(lCurrBlock->dataAddress() + lRequireSize);
					lCurrBlock->size(lCurrBlock->size() - lRequireSize);
					lCurrBlock->prev(lMemBlock);

					// Change mFirstMemBlock
					if(mFirstMemBlock == lCurrBlock)
					{
						mFirstMemBlock = lMemBlock;
					}

					lReturnPointer = reinterpret_cast<void*>(lMemBlock->dataAddress());

					bcHeapAllocator::mTracer.acceptAlloc(lRequireSize);
					bcAllocator::mTracer.acceptOverHead(sizeof(bcHeapAllocatorMemBlock));
				}
				else
				{
					lReturnPointer = reinterpret_cast<void*>(lCurrBlock->dataAddress());

					pMemBlock->allocatorsExtra(lCurrBlock);

					bcHeapAllocator::mTracer.acceptAlloc(lRequireSize);

					// If mTop is full, clear it
					if(mTop == lCurrBlock)
					{
						//mTop->mDataAddress = 0;
						mTop = NULL;
					}
				}
				return lReturnPointer;
			}
			// no more free memory
			else
			{
				bcHeapAllocator::mTracer.rejectAlloc(lRequireSize);
				return lReturnPointer;
			}
		}
		void bcHeapAllocator::free(const void* pPointer, MemBlock* pMemBlock)
		{
			bcHeapAllocatorMemBlock* lHeapBlock = reinterpret_cast<bcHeapAllocatorMemBlock*>(pMemBlock->extra());

			bcBOOL lNextFree = bcFALSE;
			bcBOOL lPrevFree = bcFALSE;

			// == Free allocated pointers to this block == MRB ==
			bcHeapAllocatorMemBlock::PointerRefs* lHeapBlockPtrs = lHeapBlock->pointers();
			while (lHeapBlockPtrs)
			{
				bcHeapAllocatorMemBlock::PointerRefs* lNext = lHeapBlockPtrs->mNext;
				mPointerRefAllocator.Free(lHeapBlockPtrs);
				lHeapBlockPtrs = lNext;

				bcAllocator::mTracer.acceptFreeOverHead(sizeof(bcHeapAllocatorMemBlock::PointerRefs));
			}

			pMemBlock->free(bcTRUE);

			bcAllocator::mTracer.acceptFree(pMemBlock->size());

			// Merge free block to it's next, if next block is free
			if(lHeapBlock->next() && lHeapBlock->next()->free() == bcTRUE)
			{
				lNextFree = bcTRUE;

				// Check if next block is mTop, change mTop to point
				// to current block
				if(lHeapBlock->next() == mTop)
				{
					mTop = lHeapBlock;
				}

				lHeapBlock->size(lHeapBlock->size() + lHeapBlock->next()->size());
				if(lHeapBlock->next()->next())
					lHeapBlock->next()->next()->prev(lHeapBlock);
				bcHeapAllocatorMemBlock* lCurrNext = lHeapBlock->next();
				lHeapBlock->next(lHeapBlock->next()->next());
				// Clear next free block
				mBlockAllocator.Free(lCurrNext);

				bcAllocator::mTracer.acceptFreeOverHead(sizeof(bcHeapAllocatorMemBlock));
			}
			// Merge free block to it's prev, if prev block is free
			if(lHeapBlock->prev() && lHeapBlock->prev()->free() == bcTRUE)
			{
				lPrevFree = bcTRUE;

				lHeapBlock->size(lHeapBlock->size() + lHeapBlock->prev()->size());
				lHeapBlock->dataAddress(lHeapBlock->prev()->dataAddress());
				if(lHeapBlock->prev()->prev())
					lHeapBlock->prev()->prev()->next(lHeapBlock);
				bcHeapAllocatorMemBlock* lCurrPrev = lHeapBlock->prev();
				lHeapBlock->prev(lHeapBlock->prev()->prev());
				// Clear prev free block
				mBlockAllocator.Free(lCurrPrev);

				bcAllocator::mTracer.acceptFreeOverHead(sizeof(bcHeapAllocatorMemBlock));
			}

			if(!lPrevFree && !lNextFree)
				++mNumFragmentation;
			else if((lPrevFree && lNextFree) || (lPrevFree && mTop == lHeapBlock))
				--mNumFragmentation;
				
		}

		bcBOOL bcHeapAllocator::containPointer(const void* pMemory)
		{
			return (pMemory >= mHeap && pMemory < mHeap + mSize)? bcTRUE: bcFALSE;
		}
		bcBOOL bcHeapAllocator::isEmpty()
		{
			bcHeapAllocatorMemBlock* lFirstMemBlock = mFirstMemBlock;
			while (lFirstMemBlock)
			{
				if(!lFirstMemBlock->free())
					return bcFALSE;
				lFirstMemBlock = lFirstMemBlock->next();
			}

			return bcTRUE;
		}
		void bcHeapAllocator::clear()
		{
			// == Free all blocks and theirs pointer references == MRB ==
			bcHeapAllocatorMemBlock* lFirstMemBlock = mFirstMemBlock;
			while(lFirstMemBlock)
			{
				mFirstMemBlock = mFirstMemBlock->next();

				// == Free allocated pointers to this block == MRB ==
				bcHeapAllocatorMemBlock::PointerRefs* lHeapBlockPtrs = lFirstMemBlock->pointers();
				while (lHeapBlockPtrs)
				{
					bcHeapAllocatorMemBlock::PointerRefs* lNext = lHeapBlockPtrs->mNext;
					mPointerRefAllocator.Free(lHeapBlockPtrs);
					lHeapBlockPtrs = lNext;
				}

				mBlockAllocator.Free(lFirstMemBlock);
				lFirstMemBlock = mFirstMemBlock;
			}

			mFirstMemBlock = mBlockAllocator.Alloc();
			mFirstMemBlock->dataAddress(reinterpret_cast<bcSIZE>(mHeap));

			mFirstMemBlock->size(mSize);
			mFirstMemBlock->next(bcNULL);
			mFirstMemBlock->prev(bcNULL);

			mTop = mFirstMemBlock;

			bcAllocator::mTracer.acceptClear();
			bcAllocator::mTracer.acceptOverHead(sizeof(bcHeapAllocatorMemBlock));

//#ifdef BC_MEMORY_TRACING
//			mWastedSize = sizeof(MemBlock);
//			mTotalSize = mSize + mWastedSize;
//			mUsedSize = 0;
//#endif
		}

		void bcHeapAllocator::registerPointer(const void* pPointer)
		{
			bcHeapAllocatorMemBlock* lMemBlock = 
				reinterpret_cast<bcHeapAllocatorMemBlock*>(MemBlock::retrieveMemBlock(pPointer)->allocatorsExtra());

			lMemBlock->registerNewPointer(mPointerRefAllocator.Alloc(), pPointer);

			bcAllocator::mTracer.acceptOverHead(sizeof(bcHeapAllocatorMemBlock::PointerRefs));
		}
		void bcHeapAllocator::unRegisterPointer(const void* pPointer)
		{
			bcHeapAllocatorMemBlock* lMemBlock = 
				reinterpret_cast<bcHeapAllocatorMemBlock*>(MemBlock::retrieveMemBlock(pPointer)->allocatorsExtra());

			mPointerRefAllocator.Free(lMemBlock->unRegisterPointer(pPointer));

			bcAllocator::mTracer.acceptFreeOverHead(sizeof(bcHeapAllocatorMemBlock::PointerRefs));
		}

		void bcHeapAllocator::defragment(bcINT32 pNumDefrag)
		{
			bcHeapAllocatorMemBlock* lFirstBlock = mFirstMemBlock;
			bcHeapAllocatorMemBlock* lPrevFreeBlock = NULL;
			bcHeapAllocatorMemBlock* lFreeBlock = NULL;
			bcHeapAllocatorMemBlock* lUsedBlock = NULL;
			bcHeapAllocatorMemBlock* lLastUsedBlock = NULL;
			bcHeapAllocatorMemBlock* lAlignedBlock = NULL;
			bcUINT32 lNumMovedBlocks = 0;

			while(--pNumDefrag >= 0)
			{
				// Find first free block
				while(lFirstBlock->next() && !lFirstBlock->free())
				{
					//lPrevFreeBlock = lFirstBlock;
					lFirstBlock = lFirstBlock->next();
				}
				// If we have found a free block
				if(lFirstBlock->free())
				{
					lFreeBlock = lFirstBlock;
					lPrevFreeBlock = lFirstBlock->prev();
				}
				else
					return;

				// After we have moved a aligned block we return to this point
				// for checking next blocks
ReturnFromAlignedBlockDefrag :

				lUsedBlock = lFreeBlock->next();
				if(!lUsedBlock)
					return;
				lLastUsedBlock = lUsedBlock;
				++lNumMovedBlocks;
				// We must move aligned block individually
				while(	lLastUsedBlock->next() && 
						!lLastUsedBlock->next()->free() && 
						lLastUsedBlock->next()->getMemBlock()->alignment() == BC_MEMORY_MIN_ALIGN)
				{
					lLastUsedBlock = lLastUsedBlock->next();
					++lNumMovedBlocks;
				}
				bcSIZE lNumByteMove =	(lLastUsedBlock->dataAddress() + lLastUsedBlock->size()) - 
										lUsedBlock->dataAddress();

				std::memmove(
					reinterpret_cast<void*>(lFreeBlock->dataAddress()), 
					reinterpret_cast<void*>(lUsedBlock->dataAddress()), 
					lNumByteMove);
				
				// Correct MemBlock's mDataAddress
				bcSIZE lMoveOffset = lUsedBlock->dataAddress() - lFreeBlock->dataAddress();
				bcHeapAllocatorMemBlock* lUsedBlocks = lUsedBlock;
				for(; lNumMovedBlocks > 0; --lNumMovedBlocks, lUsedBlocks = lUsedBlocks->next())
				{
					lUsedBlocks->dataAddress(lUsedBlocks->dataAddress() - lMoveOffset);
					bcHeapAllocatorMemBlock::PointerRefs* lBlockPointers = lUsedBlocks->pointers();
					while (lBlockPointers)
					{
						*lBlockPointers->mPointerReference = 
							reinterpret_cast<void*>(
							reinterpret_cast<bcSIZE>(*lBlockPointers->mPointerReference) - lMoveOffset);
						lBlockPointers = lBlockPointers->mNext;
					}					
				}
				lFreeBlock->dataAddress(lFreeBlock->dataAddress() + lNumByteMove);

				// Correct MemBlock Pointers
				if(lPrevFreeBlock)
					lPrevFreeBlock->next(lUsedBlock);
				lUsedBlock->prev(lPrevFreeBlock);
				//if(lLastUsedBlock)
				lFreeBlock->next(lLastUsedBlock->next());
				if(lLastUsedBlock->next())
					lLastUsedBlock->next()->prev(lFreeBlock);
				//if(lLastUsedBlock)
				lLastUsedBlock->next(lFreeBlock);
				lFreeBlock->prev(lLastUsedBlock);

				// If Loop was ended for an aligned block, so we move it or theirs
				while(	lFreeBlock->next() && 
						!lFreeBlock->next()->free() && 
						lFreeBlock->next()->getMemBlock()->alignment() > BC_MEMORY_MIN_ALIGN)
				{
					lAlignedBlock = lFreeBlock->next();

					// == Retrive MemBlock for this block, so we can store it again == MRB ==
					MemBlock lAlignedMemBlock = *lAlignedBlock->getMemBlock();

					/*bcUINT32 lAlignment = lAlignedBlock->mAlignment;
					bcUINT32 lMask = lAlignment - 1;
					bcUINT32 lMisalignment = (lFreeBlock->mDataAddress & lMask);
					bcUINT32 lAdjustment = lAlignment - lMisalignment;*/
					bcUINT32 lAdjustment = MemBlock::getRequiredOffsetForData(
						reinterpret_cast<void*>(lFreeBlock->dataAddress()), lAlignedMemBlock.alignment());

					std::memmove(
						reinterpret_cast<void*>(lFreeBlock->dataAddress() + lAdjustment), 
						reinterpret_cast<void*>(lAlignedBlock->dataAddress() + lAlignedMemBlock.offset()), 
						lAlignedBlock->size() - lAlignedMemBlock.offset());

					bcHeapAllocatorMemBlock::PointerRefs* lBlockPointers = lAlignedBlock->pointers();
					while (lBlockPointers)
					{
						*lBlockPointers->mPointerReference = 
							reinterpret_cast<void*>(lFreeBlock->dataAddress() + lAdjustment);
						lBlockPointers = lBlockPointers->mNext;
					}

					// == Store MemBlock for moved block == MRB ==
					MemBlock::storeMemBlock(
						reinterpret_cast<void*>(lFreeBlock->dataAddress() + lAdjustment),
						&lAlignedMemBlock);
					
					// Correct MemBlock's mDataAddress
					lAlignedBlock->dataAddress(lFreeBlock->dataAddress());
					lFreeBlock->dataAddress(lFreeBlock->dataAddress() + lAlignedBlock->size());
					
					//// Store lAdjustment in first byte before aligned address
					//*((bcUBYTE*)lAlignedBlock->mDataAddress - 1) = lAdjustment;

					// Correct MemBlock Pointers
					lLastUsedBlock->next(lAlignedBlock);
					lAlignedBlock->prev(lLastUsedBlock);
					lFreeBlock->next(lAlignedBlock->next());
					if(lAlignedBlock->next())
						lAlignedBlock->next()->prev(lFreeBlock);
					lAlignedBlock->next(lFreeBlock);
					lFreeBlock->prev(lAlignedBlock);

					lLastUsedBlock = lAlignedBlock;
				}

				// Correct lPrevFreeBlock to point to lLastUsedBlock for next loop
				lPrevFreeBlock = lLastUsedBlock;

				// Correct mFirstMemBlock
				if(mFirstMemBlock == lFreeBlock)
					//mFirstMemBlock.mOffsetAddress = (bcUINT32)lUsedBlock - *mFirstMemBlock.mBaseAddress;
					mFirstMemBlock = lUsedBlock;

				// Check if next blocks are still used,swap them with
				// correct free block
				if(lFreeBlock->next() && !lFreeBlock->next()->free())
					goto ReturnFromAlignedBlockDefrag;

				// If we haven't reach to end of heap, merge two free MemBlock
				// and delete lFreeBlock block
				if(lFreeBlock->next() && lFreeBlock->next()->free())
				{
					// Correct lFirstBlock for next defrag loops
					lFirstBlock = lFreeBlock->next();

					lPrevFreeBlock->next(lFreeBlock->next());
					lFreeBlock->next()->prev(lPrevFreeBlock);
					lFreeBlock->next()->size(lFreeBlock->next()->size() + lFreeBlock->size());
					lFreeBlock->next()->dataAddress(lFreeBlock->dataAddress());
					mBlockAllocator.Free(lFreeBlock);
				}
				// If lFreeBlock->mNext point to NULL, it is last block
				// in the heap, so mTop most point to it
				else if(!lFreeBlock->next())
				{
					mTop = lFreeBlock;
				}

				--mNumFragmentation;
			}	
		}

//		void bcHeapAllocator::reSize(bcSIZE pNewSize)
//		{
//			bcAssert(pNewSize != mSize);
//
//			// If we want to reduce size, check can we do it
//			if(mTop != NULL)
//			{
//				bcSIZE lUsedSize = mTop->mDataAddress - (bcUINT32)mHeap;
//				if(pNewSize < lUsedSize)
//					throw Exception(L"Error in HeapAllocator.CPP: You want to dawnSize HeapAllocator, but your new size will delete data");
//			}
//			else if(pNewSize < mSize)
//					throw Exception(L"Error in HeapAllocator.CPP: You want to dawnSize HeapAllocator, but your new size will delete data");
//
//			bcUBYTE* lNewHeap;
//			bcSIZE lMinSize = std::min<bcSIZE>(pNewSize, mSize);
//
//#ifdef BC_MEMORY_ALIGN16
//			pNewSize = Align16Size(pNewSize);
//#else
//			pNewSize = pNewSize;
//#endif
//
//#ifdef BC_MEMORY_ALIGN16
//			lNewHeap = (bcUBYTE*)_aligned_malloc(pNewSize * sizeof(bcUBYTE), BC_MEMORY_MAX_ALIGN);
//#else
//			lNewHeap = (bcUBYTE*)_aligned_malloc(pNewSize * sizeof(bcUBYTE), BC_MEMORY_MAX_ALIGN);
//#endif
//			if(!lNewHeap)
//				throw Exception(L"Error in HeapAllocator.CPP: Memory resize Error");
//
//			if(mTop != NULL)
//			{
//				if(pNewSize > mSize)
//					mTop->mSize += (pNewSize - mSize);
//				else
//					mTop->mSize -= (mSize - pNewSize);
//			}
//
//#ifdef BC_MEMORY_TRACING
//			mWastedSize = mWastedSize;
//			mTotalSize = pNewSize + mWastedSize;
//			if(pNewSize > mSize)
//				mUsedSize = mUsedSize;
//			else if(mTop != NULL)
//			{
//				bcSIZE lUsedSize = mTop->mDataAddress - (bcUINT32)mHeap;
//				if(pNewSize > lUsedSize)
//					mUsedSize = mUsedSize;
//			}
//
//			bcUINT32 lAddedSize = (pNewSize - mSize);
//			mCurrReSizeThreshold = (lAddedSize >= mCurrReSizeThreshold)? 0 : (mCurrReSizeThreshold - lAddedSize);
//#endif
//			memcpy(lNewHeap, mHeap, lMinSize);
//
//			// Correct MemBlock's mDataAddress
//			MemBlock* lBlocks = mFirstMemBlock;
//			while(lBlocks)
//			{
//				lBlocks->mDataAddress = (lBlocks->mDataAddress - (bcUINT32)mHeap) + (bcUINT32)lNewHeap;
//
//#ifndef BC_MEMORY_HANDLE_Ptr
//				// Find all pointers that point to this block and
//				// correct theirs address;
//				Ptr<void>* lPtr = lBlocks->mPtrHeads.mNext;
//				while(lPtr)
//				{
//					lPtr->mData = (void*)(((bcUINT32)lPtr->mData - (bcUINT32)mHeap) + (bcUINT32)lNewHeap);
//					lPtr = lPtr->mNext;
//				}
//#endif
//
//				lBlocks = lBlocks->mNext;
//			}
//
//#ifdef BC_MEMORY_ALIGN16
//			SAFE_ALIGNED_FREE(mHeap);
//#else
//			SAFE_ALIGNED_FREE(mHeap);
//#endif
//
//			mHeap = lNewHeap;
//			mSize = pNewSize;
//
//			// Store base address in a UINT, for use os Ptrs
//			bcAllocator::mBaseAddress = (bcUINT32)mHeap;
//		}

#endif
	}
}