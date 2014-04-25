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
			mHeap = (bcUBYTE*)_aligned_malloc( mSize * sizeof(bcUBYTE), BC_MEMORY_MIN_ALIGN );

			if(!mHeap)
				throw Exception(L"Memory Allocation Error");

			mTop = mHeap;
			mLastAllocation = bcNULL;

			bcAllocator::mTracer.initialize(mSize);
			bcAllocator::tag(pTag);
		}

		void* StackAllocator::push(MemBlock* pMemBlock)
		{
			void* lResult = bcNULL;
			bcSIZE lSize = pMemBlock->size();

			bcAssert( lSize > 0 );

			bcSIZE lByteFree = (reinterpret_cast<bcSIZE>(mHeap) + mSize) - reinterpret_cast<bcSIZE>(mTop);
			if(lSize <= lByteFree)
			{
				lResult = mTop;
				mLastAllocation = mTop + MemBlock::getRequiredOffsetForData(mTop, pMemBlock->alignment());
				// == MemBlock::mExtra must point to previous allocated data so we 
				// can extract it's MemBlock in pop function in order to free it if 
				// that block was free == MRB ==
				if(!isEmpty())
					pMemBlock->allocatorsExtra(mLastAllocation);
				else
					pMemBlock->allocatorsExtra(bcNULL);

				mTop += lSize;

				bcAllocator::mTracer.acceptAlloc(lSize);

				return lResult;
			}
			else // == No more free memory  == MRB ==
			{
				bcAllocator::mTracer.rejectAlloc(lSize);

				return lResult;
			}
		}
		void StackAllocator::pop(MemBlock* pMemBlock)
		{
			if(!isEmpty())
			{
				bcSIZE lFreeBlockSize = pMemBlock->size();
				mTop -= lFreeBlockSize;

				pMemBlock->free(bcTRUE);

				bcAllocator::mTracer.acceptFree(lFreeBlockSize);

				// If Previous blocks are free, decrease mTop pointer
				if(!isEmpty())
				{
					// == MemBlock->mExtra point to previous allocated data so we can
					// extract it's MemBlock to check if it was free == MRB ==
					MemBlock* lPrevAllocationBlock = MemBlock::retrieveMemBlock(pMemBlock->allocatorsExtra());
					bcBOOL lIsPrevBlockFree = lPrevAllocationBlock->free();
					while(lIsPrevBlockFree)
					{
						lFreeBlockSize = lPrevAllocationBlock->size();
						mTop -= lFreeBlockSize;

						bcAllocator::mTracer.acceptFree(lFreeBlockSize);

						if(isEmpty())
							break;

						lPrevAllocationBlock = MemBlock::retrieveMemBlock(lPrevAllocationBlock->allocatorsExtra());
						lIsPrevBlockFree = lPrevAllocationBlock->free();
					}
				}
			}
		}

		void* StackAllocator::alloc(MemBlock* pMemBlock)
		{
			return push(pMemBlock);
		}
		void StackAllocator::free(const void* pPointer, MemBlock* pMemBlock)
		{
			MemBlock* lMemBlock = const_cast<MemBlock*>(pMemBlock);
			bcUBYTE* lTopPointer = mTop - lMemBlock->size();

			// If block has offset, shift it's address to actual allocated address
			bcUBYTE lOffset = lMemBlock->offset();
			bcUBYTE* lBlockRealPointer = reinterpret_cast<bcUBYTE*>(reinterpret_cast<bcSIZE>(pPointer) - 
																	lOffset);
			
			// If we're not freeing the top of the stack mark block as free
			if( lBlockRealPointer != lTopPointer )
				lMemBlock->free(bcTRUE);
			else
				pop(lMemBlock);
		}

		bcBOOL StackAllocator::containPointer(const void* pPointer)
		{
			return (pPointer >= mHeap && pPointer < mHeap + mSize)? bcTRUE: bcFALSE;
		}
		bcBOOL StackAllocator::isEmpty()
		{
			return mHeap == mTop;
		}
		void StackAllocator::clear()
		{
			mTop = mHeap;

			bcAllocator::mTracer.acceptClear();
		}
#endif
	}
}