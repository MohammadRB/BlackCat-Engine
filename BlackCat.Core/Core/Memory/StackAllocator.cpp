//  [8/24/2013 MRB]

#include "CorePCH.h"
#include "Allocators.h"
#include "CoreExceptions.h"

namespace BlackCat
{
	namespace Core
	{
#ifdef BC_MEMORY_ENABLE

		StackAllocator::StackAllocator()
		{
		}
		StackAllocator::~StackAllocator()
		{
			SAFE_ALIGNED_FREE(mHeap);
		}
		void StackAllocator::initialize(bcUINT32 pSize, const bcCHAR* pTag)
		{
			mSize = pSize;

			// == We alloc mHeap by min align defined in CorePCH because we
			// want all of our allocations have MIN_Align ========== MRB ==
			mHeap = (bcUBYTE*)_aligned_malloc(mSize * sizeof(bcUBYTE), BC_MEMORY_MIN_ALIGN);

			if (!mHeap)
				throw Exception(L"Memory Allocation Error");

			bcAtomicOperation::bcAtomicStore(mTop, mHeap, bcMemoryOrder_SeqCst);
			//mLastAllocation = bcNULL;

			bcAllocator::mTracer.initialize(mSize);
			bcAllocator::tag(pTag);
		}

		void* StackAllocator::push(MemBlock* pMemBlock)
		{
			void* lResult = bcNULL;
			bcSIZE lSize = pMemBlock->size();

			bcAssert(lSize > 0);

			bcUBYTE* lLocalTop = bcAtomicOperation::bcAtomicLoad(mTop, bcMemoryOrder_SeqCst);
			while (bcTRUE)
			{
				MemBlock* lLastMemBlock = bcNULL;
				bcSIZE lByteFree = 0;
				if (lLocalTop != mHeap)
				{
					lLastMemBlock = MemBlock::retrieveMemBlock(lLocalTop);
					lByteFree = (reinterpret_cast<bcSIZE>(mHeap)+mSize) - (reinterpret_cast<bcSIZE>(lLocalTop)-lLastMemBlock->offset() + lLastMemBlock->size());
				}
				else
				{
					lByteFree = mSize;
				}
				
				if (lSize <= lByteFree)
				{
					bcUINT32 lOffset;
					bcUBYTE* lNewTop;
					bcUBYTE* lAllocationTop;

					// == mTop must point to last allocation space plus it's offset ==
					if (lLocalTop != mHeap)
					{
						lAllocationTop = lLocalTop - lLastMemBlock->offset() + lLastMemBlock->size();
					}
					else
					{
						lAllocationTop = lLocalTop;
					}

					lOffset = MemBlock::getRequiredOffsetForData(lAllocationTop, pMemBlock->alignment());
					lNewTop = lAllocationTop + lOffset;

					if (bcAtomicOperation::bcAtomicCompareExchangeStrong(
						mTop,
						&lLocalTop,
						lNewTop,
						bcMemoryOrder_SeqCst,
						bcMemoryOrder_SeqCst))
					{
						lResult = lAllocationTop;

						// == MemBlock::mExtra must point to previous allocated data so we can extract it's MemBlock 
						// in pop function in order to free it if that block was free ==
						pMemBlock->allocatorsExtra(lLocalTop);

						//mTop += lSize;

						bcAllocator::mTracer.acceptAlloc(lSize);

						return lResult;
					}
					else
						continue;
				}
				else // == No more free memory  == MRB ==
				{
					bcAllocator::mTracer.rejectAlloc(lSize);

					return lResult;
				}
			}
		}
		void StackAllocator::pop(const void* pPointer, MemBlock* pMemBlock)
		{
			if (!isEmpty())
			{
				bcBOOL lBackTrace = bcFALSE;
				bcSIZE lFreeBlockSize = pMemBlock->size();
				// If block has offset, shift it's address to actual allocated address
				bcUBYTE lOffset = pMemBlock->offset();

				bcUBYTE* lNewTop;
				bcUBYTE* lLocalTop;
				while (bcTRUE)
				{
					lLocalTop = bcAtomicOperation::bcAtomicLoad(mTop, bcMemoryOrder_SeqCst);
					MemBlock* lTopMemBlock = MemBlock::retrieveMemBlock(lLocalTop);
					lNewTop = (bcUBYTE*)lTopMemBlock->allocatorsExtra();
					bcBOOL lIsFreeingTop = pPointer == lLocalTop;
					bcUBYTE* lPointerCopy = (bcUBYTE*)pPointer;

					// If we're not freeing the top of the stack mark block as free
					if (!lIsFreeingTop)
					{
						pMemBlock->free(bcTRUE, bcMemoryOrder_SeqCst);

						lBackTrace = bcAtomicOperation::bcAtomicCompareExchangeStrong(
							mTop,
							&lPointerCopy,
							lNewTop,
							bcMemoryOrder_SeqCst,
							bcMemoryOrder_SeqCst);

						if (lBackTrace)
							bcAllocator::mTracer.acceptFree(lFreeBlockSize);
						break;
					}
					else
					{
						if (bcAtomicOperation::bcAtomicCompareExchangeStrong(
							mTop,
							&lLocalTop,
							lNewTop,
							bcMemoryOrder_SeqCst,
							bcMemoryOrder_SeqCst))
						{
							bcAllocator::mTracer.acceptFree(lFreeBlockSize);

#ifdef BC_MEMORY_DEBUG
							std::memset(reinterpret_cast<void*>(reinterpret_cast<bcUINT32>(lLocalTop)), 0, lFreeBlockSize - lOffset);
							pMemBlock->free(bcTRUE, bcMemoryOrder_SeqCst);
#endif

							lBackTrace = bcTRUE;
							break;
						}
						else
							continue;
					}
				}

				// If Previous blocks are free, decrease mTop pointer
				if (lBackTrace)
				{
					// == MemBlock->mExtra point to previous allocated data so we can extract it's MemBlock to check
					// if it was free == 
					bcUBYTE* lPrevTop = (bcUBYTE*)pMemBlock->allocatorsExtra();
					if (lPrevTop != mHeap)
					{
						MemBlock* lPrevAllocationBlock = MemBlock::retrieveMemBlock(lPrevTop);
						bcUBYTE* lPrevBlockStart = lPrevTop - lPrevAllocationBlock->offset();

						bcUBYTE* lBackTraceNewTop = lNewTop;
						bcBOOL lIsPrevBlockFree = lPrevAllocationBlock->free(bcMemoryOrder_SeqCst);
						while (lIsPrevBlockFree)
						{
							lFreeBlockSize = lPrevAllocationBlock->size();
							lBackTraceNewTop = (bcUBYTE*)lPrevAllocationBlock->allocatorsExtra();

							if (!bcAtomicOperation::bcAtomicCompareExchangeStrong(
								mTop,
								&lNewTop,
								lBackTraceNewTop,
								bcMemoryOrder_SeqCst,
								bcMemoryOrder_SeqCst))
								break;

							bcAllocator::mTracer.acceptFree(lFreeBlockSize);

#ifdef BC_MEMORY_DEBUG
							std::memset(reinterpret_cast<void*>(reinterpret_cast<bcUINT32>(lNewTop)), 0, lFreeBlockSize - lPrevAllocationBlock->offset());
							pMemBlock->free(bcTRUE, bcMemoryOrder_SeqCst);
#endif

							lNewTop = lBackTraceNewTop;
							if (lNewTop == mHeap)
								break;
							MemBlock* lPrevPrevAllocationBlock = MemBlock::retrieveMemBlock(lPrevAllocationBlock->allocatorsExtra());
							lPrevBlockStart = (bcUBYTE*)lPrevAllocationBlock->allocatorsExtra() - lPrevPrevAllocationBlock->offset();
							lPrevAllocationBlock = lPrevPrevAllocationBlock;
							lIsPrevBlockFree = lPrevAllocationBlock->free(bcMemoryOrder_SeqCst);
						}
					}
				}
			}
			else
			{
				bcAssert(bcFALSE);
			}
		}

		void* StackAllocator::alloc(MemBlock* pMemBlock)
		{
			return push(pMemBlock);
		}
		void StackAllocator::free(const void* pPointer, MemBlock* pMemBlock)
		{
			pop(pPointer, pMemBlock);
		}

		bcBOOL StackAllocator::containPointer(const void* pPointer)
		{
			return (pPointer >= mHeap && pPointer < mHeap + mSize) ? bcTRUE : bcFALSE;
		}
		bcBOOL StackAllocator::isEmpty()
		{
			return mHeap == bcAtomicOperation::bcAtomicLoad(mTop, bcMemoryOrder_SeqCst);
		}
		void StackAllocator::clear()
		{
			bcAtomicOperation::bcAtomicStore(mTop, mHeap, bcMemoryOrder_SeqCst);

			bcAllocator::mTracer.acceptClear();
		}

//		StackAllocator::StackAllocator()
//		{
//		}
//		StackAllocator::~StackAllocator()
//		{
//			SAFE_ALIGNED_FREE(mHeap);
//		}
//		void StackAllocator::initialize(bcUINT32 pSize, const bcCHAR* pTag)
//		{
//			mSize = pSize;
//
//			// == We alloc mHeap by min align defined in CorePCH because we
//			// want all of our allocations have MIN_Align ========== MRB ==
//			mHeap = (bcUBYTE*)_aligned_malloc(mSize * sizeof(bcUBYTE), BC_MEMORY_MIN_ALIGN);
//
//			if (!mHeap)
//				throw Exception(L"Memory Allocation Error");
//
//			bcAtomicOperation::bcAtomicStore(mTop, mHeap, bcMemoryOrder_SeqCst);
//			//mLastAllocation = bcNULL;
//
//			bcAllocator::mTracer.initialize(mSize);
//			bcAllocator::tag(pTag);
//		}
//
//		void* StackAllocator::push(MemBlock* pMemBlock)
//		{
//			void* lResult = bcNULL;
//			bcSIZE lSize = pMemBlock->size();
//
//			bcAssert(lSize > 0);
//
//			bcUBYTE* lLocalTop = bcAtomicOperation::bcAtomicLoad(mTop, bcMemoryOrder_SeqCst);
//			while (bcTRUE)
//			{
//				bcSIZE lByteFree = (reinterpret_cast<bcSIZE>(mHeap)+mSize) - reinterpret_cast<bcSIZE>(lLocalTop);
//				if (lSize <= lByteFree)
//				{
//					if (bcAtomicOperation::bcAtomicCompareExchangeStrong(
//						mTop,
//						&lLocalTop,
//						lLocalTop + lSize,
//						bcMemoryOrder_SeqCst,
//						bcMemoryOrder_SeqCst))
//					{
//						lResult = lLocalTop;
//						
//						// == MemBlock::mExtra must point to previous allocated data so we 
//						// can extract it's MemBlock in pop function in order to free it if 
//						// that block was free ==
//						if (lLocalTop != mHeap)
//						{
//							void* lLastAllocation = lLocalTop + MemBlock::getRequiredOffsetForData(lLocalTop, pMemBlock->alignment());
//							pMemBlock->allocatorsExtra(lLastAllocation);
//						}
//						else
//							pMemBlock->allocatorsExtra(bcNULL);
//
//						//mTop += lSize;
//
//						bcAllocator::mTracer.acceptAlloc(lSize);
//
//						return lResult;
//					}
//					else
//						continue;
//				}
//				else // == No more free memory  == MRB ==
//				{
//					bcAllocator::mTracer.rejectAlloc(lSize);
//
//					return lResult;
//				}
//			}
//		}
//		void StackAllocator::pop(const void* pPointer, MemBlock* pMemBlock)
//		{
//			if (!isEmpty())
//			{
//				bcBOOL lBackTrace = bcFALSE;
//				bcSIZE lFreeBlockSize = pMemBlock->size();
//				// If block has offset, shift it's address to actual allocated address
//				bcUBYTE lOffset = pMemBlock->offset();
//				bcUBYTE* lBlockTopPointer = reinterpret_cast<bcUBYTE*>(reinterpret_cast<bcSIZE>(pPointer)-
//					lOffset +
//					lFreeBlockSize);
//				bcUBYTE* lBlockTopPointerCopy = lBlockTopPointer;
//				bcUBYTE* lNewTop = lBlockTopPointer - lFreeBlockSize;
//
//				bcUBYTE* lLocalTop;
//				while (bcTRUE)
//				{
//					lLocalTop = bcAtomicOperation::bcAtomicLoad(mTop, bcMemoryOrder_SeqCst);
//
//					// If we're not freeing the top of the stack mark block as free
//					if (lBlockTopPointer != lLocalTop)
//					{
//						pMemBlock->free(bcTRUE, bcMemoryOrder_SeqCst);
//
//						lBackTrace = bcAtomicOperation::bcAtomicCompareExchangeStrong(
//							mTop,
//							&lBlockTopPointerCopy,
//							lNewTop,
//							bcMemoryOrder_SeqCst,
//							bcMemoryOrder_SeqCst);
//
//						if (lBackTrace)
//							bcAllocator::mTracer.acceptFree(lFreeBlockSize);
//						break;
//					}
//					else
//						//pop(pPointer, pMemBlock);
//					{
//						if (bcAtomicOperation::bcAtomicCompareExchangeStrong(
//							mTop,
//							&lLocalTop,
//							lNewTop,
//							bcMemoryOrder_SeqCst,
//							bcMemoryOrder_SeqCst))
//						{
//							bcAllocator::mTracer.acceptFree(lFreeBlockSize);
//
//#ifdef BC_MEMORY_DEBUG
//							std::memset(reinterpret_cast<void*>(reinterpret_cast<bcUINT32>(lNewTop + lOffset)), 0, lFreeBlockSize - lOffset);
//							pMemBlock->free(bcTRUE, bcMemoryOrder_SeqCst);
//#endif
//
//							lBackTrace = bcTRUE;
//							break;
//						}
//						else
//							continue;
//						//mTop -= lFreeBlockSize;
//					}
//				}
//
//				// If Previous blocks are free, decrease mTop pointer
//				if (lBackTrace)
//				{
//					// == MemBlock->mExtra point to previous allocated data so we can extract it's MemBlock to check
//					// if it was free == 
//					MemBlock* lPrevAllocationBlock = MemBlock::retrieveMemBlock(pMemBlock->allocatorsExtra());
//					if (lPrevAllocationBlock != bcNULL)
//					{
//						bcUBYTE* lBackTraceNewTop = lNewTop;
//						bcBOOL lIsPrevBlockFree = lPrevAllocationBlock->free(bcMemoryOrder_SeqCst);
//						while (lIsPrevBlockFree)
//						{
//							lFreeBlockSize = lPrevAllocationBlock->size();
//							lBackTraceNewTop -= lFreeBlockSize;
//
//							if (!bcAtomicOperation::bcAtomicCompareExchangeStrong(
//								mTop,
//								&lNewTop,
//								lBackTraceNewTop,
//								bcMemoryOrder_SeqCst,
//								bcMemoryOrder_SeqCst))
//								break;
//
//							lNewTop = lBackTraceNewTop;
//
//							bcAllocator::mTracer.acceptFree(lFreeBlockSize);
//
//#ifdef BC_MEMORY_DEBUG
//							std::memset(reinterpret_cast<void*>(reinterpret_cast<bcUINT32>(lNewTop + lPrevAllocationBlock->offset())), 0, lFreeBlockSize - lPrevAllocationBlock->offset());
//							pMemBlock->free(bcTRUE, bcMemoryOrder_SeqCst);
//#endif
//
//							/*if(isEmpty())
//								break;*/
//
//							lPrevAllocationBlock = MemBlock::retrieveMemBlock(lPrevAllocationBlock->allocatorsExtra());
//							if (lPrevAllocationBlock == bcNULL)
//								break;
//							lIsPrevBlockFree = lPrevAllocationBlock->free(bcMemoryOrder_SeqCst);
//						}
//					}
//				}
//			}
//			else
//			{
//				bcAssert(bcFALSE);
//			}
//		}
//
//		void* StackAllocator::alloc(MemBlock* pMemBlock)
//		{
//			return push(pMemBlock);
//		}
//		void StackAllocator::free(const void* pPointer, MemBlock* pMemBlock)
//		{
//			//MemBlock* lMemBlock = pMemBlock;
//			//bcUBYTE* lTopPointer = mTop - lMemBlock->size();
//
//			//// If block has offset, shift it's address to actual allocated address
//			//bcUBYTE lOffset = lMemBlock->offset();
//			//bcUBYTE* lBlockRealPointer = reinterpret_cast<bcUBYTE*>(reinterpret_cast<bcSIZE>(pPointer) - 
//			//														lOffset);
//			//
//			//// If we're not freeing the top of the stack mark block as free
//			//if( lBlockRealPointer != lTopPointer )
//			//	lMemBlock->free(bcTRUE);
//			//else
//			//	pop(lMemBlock);
//			pop(pPointer, pMemBlock);
//		}
//
//		bcBOOL StackAllocator::containPointer(const void* pPointer)
//		{
//			return (pPointer >= mHeap && pPointer < mHeap + mSize) ? bcTRUE : bcFALSE;
//		}
//		bcBOOL StackAllocator::isEmpty()
//		{
//			return mHeap == bcAtomicOperation::bcAtomicLoad(mTop, bcMemoryOrder_SeqCst);
//		}
//		void StackAllocator::clear()
//		{
//			bcAtomicOperation::bcAtomicStore(mTop, mHeap, bcMemoryOrder_SeqCst);
//
//			bcAllocator::mTracer.acceptClear();
//		}
#endif
	}
}