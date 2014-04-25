//  [8/24/2013 MRB]

#pragma  once

#include "CorePCH.h"
#include "CoreOS.h"

namespace BlackCat
{
	namespace Core
	{
#ifdef BC_MEMORY_ENABLE

		// == Represent a block of memory == MRB ==
		struct MemBlock
		{
		private:

		protected:
			// == Total size of block == MRB ==
			bcSIZE mSize;
			bcUBYTE mRefrenceCount;
			bcUBYTE mAlignment;
			// == Offset from begin of block where actual data is located == MRB ==
			bcUBYTE mOffset;
			//bcBOOL mFree;	
			// == Used for data that can be represented via a bit == MRB ==
			// == [0] : Free | [1] : DynamicPointer == MRB ==
			bcBYTE m8Bit;
			void* mExtra;
			void* mAllocatorsExtra;
			//bcBYTE mReserved[4];

		public:
			MemBlock()
			{
				mSize = 0;
				mRefrenceCount = 0;
				mAlignment = 16;
				m8Bit = 0;
				mOffset = 0;
				mExtra = bcNULL;
				mAllocatorsExtra = bcNULL;
			}

			bcSIZE size() const { return mSize; }
			void size(bcSIZE val) { mSize = val; }

			bcUBYTE refrenceCount() const { return mRefrenceCount; }
			void refrenceCount(bcUBYTE val) { mRefrenceCount = val; }

			bcUBYTE alignment() const { return mAlignment; }
			void alignment(bcUBYTE val) { mAlignment = val; }

			bcUBYTE offset() const { return mOffset; }
			void offset(bcUBYTE val) { mOffset = val; }

			bcBOOL free() const { return m8Bit & (bcBYTE(1) >> 0); }
			void free(bcBOOL val) { m8Bit = (val)? m8Bit | (bcBYTE(1) >> 0) : m8Bit & !(bcBYTE(1) >> 0); }

			bcBOOL dynamicPointer() const { return m8Bit & (bcBYTE(1) >> 1); }
			void dynamicPointer(bcBOOL val) { m8Bit = (val)? m8Bit | (bcBYTE(1) >> 1) : m8Bit & !(bcBYTE(1) >> 1); }

			void* extra() const { return mExtra; }
			void extra(void* val) { mExtra = val; }

			void* allocatorsExtra() const { return mAllocatorsExtra; }
			void allocatorsExtra(void* val) { mAllocatorsExtra = val; }

			bcInline static bcSIZE getAlignedSize(bcSIZE pSize, bcSIZE pAlignment)
			{
				bcSIZE lMisAlignment = pSize % pAlignment;
				return (lMisAlignment == 0)? pSize : (pSize - lMisAlignment) + pAlignment;
			}

			// == Store MemBlock for pointer that point to actual location of data == MRB ==
			bcInline static void storeMemBlock(const void* pPointer, const MemBlock* pBlock)
			{
				*(reinterpret_cast<MemBlock*>((reinterpret_cast<bcSIZE>(pPointer) - sizeof(MemBlock)))) = *pBlock;
			}

			// == Retrieve MemBlock for pointer that point to actual location of data == MRB ==
			bcInline static MemBlock* retrieveMemBlock(const void* pPointer)
			{
				if(pPointer != bcNULL && !bcCoreOS::isPointerOnStack(pPointer))
					return (MemBlock*)(reinterpret_cast<bcUINT32>(pPointer) - sizeof(MemBlock));
				else
					return bcNULL;
			}

			bcInline static bcSIZE getRequiredSize(bcSIZE pDataSize, bcSIZE pAlignment)
			{
				return 
					getAlignedSize(sizeof(MemBlock), BC_MEMORY_MIN_ALIGN) +
					(pAlignment - BC_MEMORY_MIN_ALIGN) +
					getAlignedSize(pDataSize, BC_MEMORY_MIN_ALIGN);
					
			}

			// == Return required offset from begging of block where actual
			// data must store == MRB ==
			bcInline static bcSIZE getRequiredOffsetForData(const void* pPointer, bcSIZE pAlignment)
			{
				bcSIZE lMemBlockOffset = reinterpret_cast<bcSIZE>(pPointer) + 
										 getAlignedSize(sizeof(MemBlock), BC_MEMORY_MIN_ALIGN) +
										 (pAlignment - BC_MEMORY_MIN_ALIGN);

				return pAlignment - (lMemBlockOffset % pAlignment);
			}

			bcInline static bcSIZE getRequiredOffsetForMemBlock(const void* pPointer, bcSIZE pAlignment)
			{
				return 
					(getRequiredOffsetForData(pPointer, pAlignment) - sizeof(MemBlock)) - 
					reinterpret_cast<bcSIZE>(pPointer);
			}

			bcInline static void initializeMemBlockForAlllocation(bcSIZE pSize, bcSIZE pAlignment, MemBlock* pBlock)
			{
				pBlock->size(getRequiredSize(pSize, pAlignment));
				pBlock->refrenceCount(0);
				pBlock->alignment(pAlignment);
				pBlock->offset(0);
				pBlock->free(bcFALSE);
				pBlock->dynamicPointer(bcFALSE);
				pBlock->extra(bcNULL);
				pBlock->allocatorsExtra(bcNULL);
			}

			// == Fill MemBlock with data and shift pPointer with requeried offset for
			// actual data == MRB ==
			bcInline static void inititalizeMemBlockAfterAllocation(void** pPointer, 
																	bcBOOL pDynamic, 
																	void* pExtra,
																	MemBlock* pBlock)
			{
				pBlock->offset(MemBlock::getRequiredOffsetForData(*pPointer, pBlock->alignment()));
				pBlock->dynamicPointer(pDynamic);
				pBlock->extra(pExtra);

				*pPointer = reinterpret_cast<void*>((reinterpret_cast<bcSIZE>(*pPointer) + pBlock->offset()));

				MemBlock::storeMemBlock(*pPointer, pBlock);
			}
		};

#endif
	}
}