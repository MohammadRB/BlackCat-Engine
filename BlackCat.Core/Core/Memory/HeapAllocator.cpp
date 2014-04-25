//  [8/29/2013 MRB]

#pragma once

#include "CorePCH.h"
#include "Allocators.h"
//#include "bcThreadManager.h"
#include "CoreExceptions.h"

namespace BlackCat
{
	namespace Core
	{
#ifdef BC_MEMORY_ENABLE

		bcHeapAllocator::bcHeapAllocator(bcUINT32 pNumThread) : mBlockList(pNumThread, 3), mRemainingFreeSpaceLimit(BC_MEMORY_MIN_ALIGN)
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

#ifdef BC_MEMORY_DEFRAG
			mPointerRefAllocator.initialize();
#endif
			
			mBlockList.pushFront(bcHeapAllocatorMemBlock(reinterpret_cast<bcSIZE>(mHeap), mSize, bcTRUE));
			// == In allocation, we use nexts block to set data address of blocks ==
			//mBlockList.end()->dataAddress(reinterpret_cast<bcSIZE>(mHeap) + mSize);
			mBlockList.pushBack(bcHeapAllocatorMemBlock(reinterpret_cast<bcSIZE>(mHeap)+mSize, 0, bcFALSE));

			bcAtomicOperation::bcAtomicStore(mNumFragmentation, 0U, bcMemoryOrder_SeqCst);

			bcAllocator::mTracer.initialize(pSize);
			bcAllocator::mTracer.acceptOverHead(sizeof(bcHeapAllocatorMemBlock));
			bcAllocator::tag(pTag);
		}

		void* bcHeapAllocator::alloc(MemBlock* pMemBlock)
		{
			void* lReturnPointer = bcNULL;

			bcSIZE lRequireSize = pMemBlock->size();
			bcAssert(lRequireSize > 0);

			BlockListIterator lCurrBlock = mBlockList.begin();

			while (bcTRUE)
			{
				BlockListIterator lEnd = mBlockList.end();
				while (lCurrBlock != lEnd &&
					   (!(lCurrBlock->sizeAndFree()->free()) || lCurrBlock->sizeAndFree()->size() < lRequireSize))
				{
					++lCurrBlock;
				}
				if (lCurrBlock != mBlockList.end())
				{
					bcSIZE lCurrBlockSize = lCurrBlock->sizeAndFree()->size();
					// Check if we must divide our free block, do it
					if (lCurrBlockSize > lRequireSize && lCurrBlockSize - lRequireSize >= mRemainingFreeSpaceLimit)
					{
						if (lCurrBlock->sizeAndFree()->compareAndSwap(
							SizeAndFree(lCurrBlockSize, bcTRUE),
							SizeAndFree(lCurrBlockSize - lRequireSize, bcTRUE)))
						{
							BlockListIterator lInsertedBlock =
								mBlockList.insertAfter(
								lCurrBlock,
								bcHeapAllocatorMemBlock(0, lRequireSize, bcFALSE));

							BlockListIterator lNext = lInsertedBlock;
							bcSIZE lOffsetFromNext = lRequireSize;
							while (bcTRUE)
							{
								++lNext;

								// == TODO Blocking section ==
								if (lNext->sizeAndFree()->merge())
								{
									bcBOOL lContinueFlag = bcFALSE;
									while (bcTRUE)
									{
										if (!lNext->sizeAndFree()->merge())
										{
											lContinueFlag = bcTRUE;
											break;
										}
										if (!lNext.isValid())
											break;
									}
									if (!lContinueFlag)
										continue;
								}

								if (lNext->dataAddress() != 0U)
								{
									lInsertedBlock->dataAddress(lNext->dataAddress() - lOffsetFromNext);
									break;
								}

								lOffsetFromNext += lNext->sizeAndFree()->size();
							}

							pMemBlock->allocatorsExtra(lInsertedBlock.getNode());
							lReturnPointer = reinterpret_cast<void*>(lInsertedBlock->dataAddress());

							bcHeapAllocator::mTracer.acceptAlloc(lRequireSize);
							bcAllocator::mTracer.acceptOverHead(sizeof(bcHeapAllocatorMemBlock));

							break;
						}
					}
					else
					{
						if (lCurrBlock->sizeAndFree()->compareAndSwap(
							SizeAndFree(lCurrBlockSize, bcTRUE),
							SizeAndFree(lCurrBlockSize, bcFALSE)))
						{
							pMemBlock->allocatorsExtra(lCurrBlock.getNode());
							lReturnPointer = reinterpret_cast<void*>(lCurrBlock->dataAddress());

							bcHeapAllocator::mTracer.acceptAlloc(lRequireSize);

							break;
						}
					}
				}
				// no more free memory
				else
				{
					bcHeapAllocator::mTracer.rejectAlloc(lRequireSize);
					return lReturnPointer;
				}
			}

			return lReturnPointer;
		}
		void bcHeapAllocator::free(const void* pPointer, MemBlock* pMemBlock)
		{
			//bcHeapAllocatorMemBlock* lHeapBlock = reinterpret_cast<bcHeapAllocatorMemBlock*>(pMemBlock->allocatorsExtra());

			bcBOOL lNextFree = bcFALSE;
			bcBOOL lPrevFree = bcFALSE;

#ifdef BC_MEMORY_DEFRAG
			// == Free allocated pointers to this block == MRB ==
			bcHeapAllocatorMemBlock::PointerRefs* lHeapBlockPtrs = lHeapBlock->pointers();
			while (lHeapBlockPtrs)
			{
				bcHeapAllocatorMemBlock::PointerRefs* lNext = lHeapBlockPtrs->mNext;
				mPointerRefAllocator.free(lHeapBlockPtrs);
				lHeapBlockPtrs = lNext;

				bcAllocator::mTracer.acceptFreeOverHead(sizeof(bcHeapAllocatorMemBlock::PointerRefs));
			}
#endif

			pMemBlock->free(bcTRUE, bcMemoryOrder_SeqCst);
//#ifndef BC_MEMORY_DEFRAG
//			lHeapBlock->free(bcTRUE);
//#endif

			bcAllocator::mTracer.acceptFree(pMemBlock->size());

			BlockListIterator lNode = mBlockList.createIterator(
				reinterpret_cast<BlockListType::Node*>(pMemBlock->allocatorsExtra()));
			bcSIZE lNodeSize = lNode->sizeAndFree()->size();

			BlockListIterator lNext = lNode, lPrev = lNode;

			++lNext;
			if (lNext->sizeAndFree()->free())
			{
				while (bcTRUE)
				{
					bcSIZE lNextSize = lNext->sizeAndFree()->size();
					if (lNext->sizeAndFree()->compareAndSwap(SizeAndFree(lNextSize, bcTRUE), SizeAndFree(lNextSize, bcFALSE, bcTRUE)))
					{
						lNode->sizeAndFree(SizeAndFree(lNodeSize + lNextSize, bcTRUE));
						mBlockList.erase(lNext);
						bcAllocator::mTracer.acceptFreeOverHead(sizeof(bcHeapAllocatorMemBlock));
						lNextFree = bcTRUE;
						break;
					}
					// == If CAS fail due to size change ==
					else if(!lNext->sizeAndFree()->free())
					{
						lNode->sizeAndFree(SizeAndFree(lNodeSize, bcTRUE));
						break;
					}
				}
			}
			else
			{
				lNode->sizeAndFree(SizeAndFree(lNodeSize, bcTRUE));
			}

			--lPrev;
			if (lPrev->sizeAndFree()->free())
			{
				while (bcTRUE)
				{
					lNodeSize = lNode->sizeAndFree()->size();
					bcSIZE lPrevSize = lPrev->sizeAndFree()->size();
					// == If any thread reduce lPrev size, but still doesn't insert it's block in list ==
					if (lPrev->dataAddress() + lPrevSize != lNode->dataAddress())
						break;
					if (lNode->sizeAndFree()->compareAndSwap(SizeAndFree(lNodeSize, bcTRUE), SizeAndFree(lNodeSize, bcFALSE, bcTRUE)))
					{
						if (lPrev->sizeAndFree()->compareAndSwap(SizeAndFree(lPrevSize, bcTRUE), SizeAndFree(lPrevSize + lNodeSize, bcTRUE)))
						{
							mBlockList.erase(lNode);
							bcAllocator::mTracer.acceptFreeOverHead(sizeof(bcHeapAllocatorMemBlock));
							lPrevFree = bcTRUE;
							break;
						}
						// == If lPrev is merging with it's prev ==
						else if (lPrev->sizeAndFree()->merge())
						{
							lNode->sizeAndFree(SizeAndFree(lNodeSize, bcTRUE));
							continue;
						}
						else
						{
							lNode->sizeAndFree(SizeAndFree(lNodeSize, bcTRUE));
							break;
						}
					}
					// == If CAS fail due to size change ==
					else if (!lNode->sizeAndFree()->free())
					{
						break;
					}
				}
			}

			//// Merge free block to it's next, if next block is free
			//if(lHeapBlock->next() && lHeapBlock->next()->free() == bcTRUE)
			//{
			//	lNextFree = bcTRUE;

			//	// Check if next block is mTop, change mTop to point
			//	// to current block
			//	if(lHeapBlock->next() == mTop)
			//	{
			//		mTop = lHeapBlock;
			//	}

			//	lHeapBlock->size(lHeapBlock->size() + lHeapBlock->next()->size());
			//	if(lHeapBlock->next()->next())
			//		lHeapBlock->next()->next()->prev(lHeapBlock);
			//	bcHeapAllocatorMemBlock* lCurrNext = lHeapBlock->next();
			//	lHeapBlock->next(lHeapBlock->next()->next());
			//	// Clear next free block
			//	mBlockAllocator.Free(lCurrNext);

			//	bcAllocator::mTracer.acceptFreeOverHead(sizeof(bcHeapAllocatorMemBlock));
			//}
			//// Merge free block to it's prev, if prev block is free
			//if(lHeapBlock->prev() && lHeapBlock->prev()->free() == bcTRUE)
			//{
			//	lPrevFree = bcTRUE;

			//	lHeapBlock->size(lHeapBlock->size() + lHeapBlock->prev()->size());
			//	lHeapBlock->dataAddress(lHeapBlock->prev()->dataAddress());
			//	if(lHeapBlock->prev()->prev())
			//		lHeapBlock->prev()->prev()->next(lHeapBlock);
			//	bcHeapAllocatorMemBlock* lCurrPrev = lHeapBlock->prev();
			//	lHeapBlock->prev(lHeapBlock->prev()->prev());
			//	// Clear prev free block
			//	mBlockAllocator.Free(lCurrPrev);

			//	bcAllocator::mTracer.acceptFreeOverHead(sizeof(bcHeapAllocatorMemBlock));
			//}

			if (!lPrevFree && !lNextFree)
				bcAtomicOperation::bcAtomicFetchAdd(mNumFragmentation, 1U);
			else if((lPrevFree && lNextFree) /*|| (lPrevFree && mTop == lHeapBlock)*/)
				bcAtomicOperation::bcAtomicFetchSub(mNumFragmentation, 1U);

#ifdef BC_MEMORY_DEBUG
			std::memset(reinterpret_cast<void*>(reinterpret_cast<bcUINT32>(pPointer)), 0, pMemBlock->size() - pMemBlock->offset());
#endif		
		}

		bcBOOL bcHeapAllocator::containPointer(const void* pMemory)
		{
			return (pMemory >= mHeap && pMemory < mHeap + mSize)? bcTRUE: bcFALSE;
		}
		bcBOOL bcHeapAllocator::isEmpty()
		{
			BlockListIterator lFirstMemBlock = mBlockList.begin();
			BlockListIterator lEnd = mBlockList.end();
			while (lFirstMemBlock != lEnd)
			{
				if(!lFirstMemBlock->sizeAndFree()->free())
					return bcFALSE;
				++lFirstMemBlock;
			}

			return bcTRUE;
		}
		void bcHeapAllocator::clear()
		{
			// == Free all blocks and theirs pointer references == MRB ==
			BlockListIterator lCurrentBlock = mBlockList.begin();
			BlockListIterator lEnd = mBlockList.end();
			while (lCurrentBlock != lEnd)
			{
#ifdef BC_MEMORY_DEFRAG
				// == Free allocated pointers to this block == MRB ==
				bcHeapAllocatorMemBlock::PointerRefs* lHeapBlockPtrs = lCurrentBlock->pointers();
				while (lHeapBlockPtrs)
				{
					bcHeapAllocatorMemBlock::PointerRefs* lNext = lHeapBlockPtrs->mNext;
					mPointerRefAllocator.free(lHeapBlockPtrs);
					lHeapBlockPtrs = lNext;
				}
#endif

				mBlockList.erase(lCurrentBlock);
			}

			mBlockList.pushFront(bcHeapAllocatorMemBlock(reinterpret_cast<bcSIZE>(mHeap), mSize, bcTRUE));
			// == In allocation, we use nexts block to set data address of blocks ==
			//mBlockList.end()->dataAddress(reinterpret_cast<bcSIZE>(mHeap)+mSize);
			mBlockList.pushBack(bcHeapAllocatorMemBlock(reinterpret_cast<bcSIZE>(mHeap)+mSize, 0, bcFALSE));

			bcAtomicOperation::bcAtomicInit(mNumFragmentation, 0U);

			bcAllocator::mTracer.acceptClear();
			bcAllocator::mTracer.acceptOverHead(sizeof(bcHeapAllocatorMemBlock));

//#ifdef BC_MEMORY_TRACING
//			mWastedSize = sizeof(MemBlock);
//			mTotalSize = mSize + mWastedSize;
//			mUsedSize = 0;
//#endif
		}

#ifdef BC_MEMORY_DEFRAG
		void bcHeapAllocator::registerPointer(const void* pPointer)
		{
			bcHeapAllocatorMemBlock* lMemBlock = 
				reinterpret_cast<bcHeapAllocatorMemBlock*>(MemBlock::retrieveMemBlock(pPointer)->allocatorsExtra());

			lMemBlock->registerNewPointer(mPointerRefAllocator.alloc(), pPointer);

			bcAllocator::mTracer.acceptOverHead(sizeof(bcHeapAllocatorMemBlock::PointerRefs));
		}
		void bcHeapAllocator::unRegisterPointer(const void* pPointer)
		{
			bcHeapAllocatorMemBlock* lMemBlock = 
				reinterpret_cast<bcHeapAllocatorMemBlock*>(MemBlock::retrieveMemBlock(pPointer)->allocatorsExtra());

			mPointerRefAllocator.free(lMemBlock->unRegisterPointer(pPointer));

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
					mBlockAllocator.free(lFreeBlock);
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
#endif

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