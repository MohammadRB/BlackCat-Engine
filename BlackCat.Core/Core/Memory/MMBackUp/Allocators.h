//  [8/23/2013 MRB]

#pragma once

#ifndef Allocators_H
#define Allocators_H

#include "CorePCH.h"
#include "MemBlock.h"
#include "Ptr.h"
#include "MemMngFreeList.h"

namespace BlackCat
{
	namespace Core
	{
#ifdef BC_MEMORY_ENABLE

		/*bcInline bcSIZE Align16Size( bcSIZE pSize )
		{
			if( pSize % 16 == 0 )
				return pSize;
			else
				return ( ( pSize / 16 ) + 1 ) * 16;
		};*/

		// == bcAllocatorTracer ======================================================================= MRB ==
		class bcAllocatorTracer
		{
		private:

		protected:
#ifdef BC_MEMORY_TRACING
			bcUINT32 mTotalSize;
			bcUINT32 mUsedSize;
			bcUINT32 mOverheadSize;
			bcUINT32 mMaxUsedSize;
			bcUINT32 mAllocCount;
			bcUINT32 mRejectedAllocSize;
			bcUINT32 mRejectedAllocCount;
#endif	

		public:
			bcAllocatorTracer()
			{
#ifdef BC_MEMORY_TRACING
				mTotalSize = 0;
				mUsedSize = 0;
				mOverheadSize = 0;
				mMaxUsedSize = 0;
				mAllocCount = 0;
				mRejectedAllocSize = 0;
				mRejectedAllocCount = 0;
#endif
			}

			void initialize(bcUINT32 pTotalSize)
			{
#ifdef BC_MEMORY_TRACING
				mTotalSize = pTotalSize;
#endif
			}

			bcUINT32 totalSize() const 
			{ 
#ifdef BC_MEMORY_TRACING
				return mTotalSize; 
#else
				return 0;
#endif
			}
			void totalSize(bcUINT32 val) 
			{ 
#ifdef BC_MEMORY_TRACING
				mTotalSize = val; 
#endif
			}
			bcUINT32 usedSize() const 
			{ 
#ifdef BC_MEMORY_TRACING
				return mUsedSize; 
#else
				return 0;
#endif
			}
			void usedSize(bcUINT32 val) 
			{ 
#ifdef BC_MEMORY_TRACING
				mUsedSize = val; 
#endif 
			}
			bcUINT32 overheadSize() const 
			{ 
#ifdef BC_MEMORY_TRACING
				return mOverheadSize; 
#else
				return 0;
#endif
			}
			void overheadSize(bcUINT32 val) 
			{ 
#ifdef BC_MEMORY_TRACING
				mOverheadSize = val; 
#endif
			}
			bcUINT32 maxUsedSize() const 
			{ 
#ifdef BC_MEMORY_TRACING
				return mMaxUsedSize; 
#else
				return 0;
#endif
			}
			void maxUsedSize(bcUINT32 val) 
			{ 
#ifdef BC_MEMORY_TRACING
				mMaxUsedSize = val; 
#endif 
			}
			bcUINT32 allocCount() const 
			{ 
#ifdef BC_MEMORY_TRACING
				return mAllocCount; 
#else
				return 0;
#endif
			}
			void allocCount(bcUINT32 val) 
			{ 
#ifdef BC_MEMORY_TRACING
				mAllocCount = val; 
#endif
			}
			bcUINT32 rejectedAllocSize() const 
			{ 
#ifdef BC_MEMORY_TRACING
				return mRejectedAllocSize; 
#else
				return 0;
#endif
			}
			void rejectedAllocSize(bcUINT32 val) 
			{ 
#ifdef BC_MEMORY_TRACING
				mRejectedAllocSize = val; 
#endif 
			}
			bcUINT32 rejectedAllocCount() const 
			{ 
#ifdef BC_MEMORY_TRACING
				return mRejectedAllocCount; 
#else
				return 0;
#endif
			}
			void rejectedAllocCount(bcUINT32 val) 
			{ 
#ifdef BC_MEMORY_TRACING
				mRejectedAllocCount = val; 
#endif 
			}

			bcInline void acceptAlloc(bcUINT32 pSize)
			{
#ifdef BC_MEMORY_TRACING
				mUsedSize += pSize;
				mMaxUsedSize = std::max<bcUINT32>(mUsedSize, mMaxUsedSize);
				++mAllocCount;
#endif
			}
			bcInline void acceptFree(bcUINT32 pSize)
			{
#ifdef BC_MEMORY_TRACING
				mUsedSize -= pSize;
				--mAllocCount;
#endif
			}
			bcInline void acceptOverHead(bcUINT32 pSize)
			{
#ifdef BC_MEMORY_TRACING
				mOverheadSize += pSize;
#endif
			}
			bcInline void acceptFreeOverHead(bcUINT32 pSize)
			{
#ifdef BC_MEMORY_TRACING
				mOverheadSize -= pSize;
#endif
			}
			bcInline void rejectAlloc(bcUINT32 pSize)
			{
#ifdef BC_MEMORY_TRACING
				mRejectedAllocSize += pSize;
				++mRejectedAllocCount;
#endif
			}

			bcInline void acceptClear()
			{
#ifdef BC_MEMORY_TRACING
				//mTotalSize = 0;
				mUsedSize = 0;
				mOverheadSize = 0;
				//mMaxUsedSize = 0;
				mAllocCount = 0;
				//mRejectedAllocSize = 0;
				//mRejectedAllocCount = 0;
#endif
			}
		};

		// == bcAllocator ============================================================================= MRB ==
#pragma region bcAllocator
		class BC_COREDLL_EXP bcAllocator abstract
		{
		private:

		protected:
			bcCHAR mTag[30];

			bcAllocatorTracer mTracer;

		public:
			bcAllocator() {}			
			virtual ~bcAllocator() {}

			void tag(const bcCHAR* pTag)
			{
				std::memcpy(mTag, pTag, std::min<bcSIZE>(bcSIZE(29), strlen(pTag)));
				*(bcAllocator::mTag + std::min<bcSIZE>(bcSIZE(29), strlen(pTag))) = '\0';
			}
			bcCHAR* tag() { return mTag; };

			bcAllocatorTracer tracer() const { return mTracer; }
			
			virtual void* alloc(MemBlock* pMemBlock) abstract;
			virtual void free(const void* pPointer, MemBlock* pMemBlock) abstract;

			virtual bcBOOL containPointer(const void* pMemory) abstract; 
			virtual bcBOOL isEmpty() abstract;
			virtual void clear() abstract;
		};
#pragma endregion

		// == bcMovableAllocator ====================================================================== MRB ==
		class bcMovableAllocator abstract
		{
		private:

		protected:

		public:
			virtual void registerPointer(const void* pPointer) abstract;
			virtual void unRegisterPointer(const void* pPointer) abstract;

			virtual void defragment(bcINT32 pNumDefrag) abstract;
		};

		// == FixedSizeAllocator ====================================================================== MRB ==
#pragma region FixedSizeAllocator
		class BC_COREDLL_EXP bcFixedSizeAllocator : public bcAllocator
		{
		private:
#define FIXEDSIZE_INDEX_TYPE bcUINT32
#define FIXEDSIZE_INDEX_MASK 0xffffffff
#define FIXEDSIZE_INDEX_SIZE 32
			
		protected:
			bcUINT32 mBlockSize;
			bcUINT32 mNumBlock;
			bcUINT32 mNumBitBlocks;
			// == An index that searching for free block will continue from this place == MRB ==
			bcUINT32 mAllocatedBlock;
			// == bit-vector indicating if a block is allocated or not == MRB ==
			FIXEDSIZE_INDEX_TYPE* mBlocks;
			// == array of block data == MRB ==
			bcUBYTE* mHeap;
		
		public:
			bcFixedSizeAllocator();
			~bcFixedSizeAllocator();
			void initialize(bcUINT32 pNumBlock,
							bcUINT32 pBlockSize, 
							const bcCHAR* pTag);

			bcUINT32 blockSize() { return mBlockSize; };
			bcUINT32 numBlock() { return mNumBlock; };

			virtual void* alloc(MemBlock* pMemBlock) override;
			virtual void free(const void* pPointer, MemBlock* pMemBlock) override;

			virtual bcBOOL containPointer(const void* pPointer) override;
			virtual bcBOOL isEmpty() override;
			virtual void clear() override;
		};
#pragma endregion

		// == StackAllocator ========================================================================== MRB ==
#pragma region StackAllocator
		class BC_COREDLL_EXP StackAllocator : public bcAllocator
		{
		private:

		protected:
			bcUINT32 mSize;
			bcUBYTE* mTop;
			bcUBYTE* mHeap;
			// == Pointer to actual(include offset of block) data of last allocation == MRB ==
			bcUBYTE* mLastAllocation;

		public:
			StackAllocator();
			~StackAllocator();
			void initialize(bcUINT32 pSize, const bcCHAR* pTag);

			bcUINT32 size() { return mSize; };

			void* push(MemBlock* pMemBlock);
			void pop(MemBlock* pMemBlock);

			virtual void* alloc(MemBlock* pMemBlock) override;
			virtual void free(const void* pPointer, MemBlock* pMemBlock) override;

			virtual bcBOOL containPointer(const void* pPointer) override;
			virtual bcBOOL isEmpty() override;
			virtual void clear() override;
		};
#pragma endregion

		// == HeapAllocator =========================================================================== MRB ==
#pragma region HeapAllocator
		class BC_COREDLL_EXP bcHeapAllocator : public bcAllocator, bcMovableAllocator
		{
		private:
			 struct bcHeapAllocatorMemBlock
			 {
				 struct PointerRefs
				 {
					 void** mPointerReference;
					 PointerRefs* mNext;
				 };
			 private:

			 protected:
				 bcSIZE mDataAddress;
				 bcSIZE mSize;
				 bcHeapAllocatorMemBlock* mNext;
				 bcHeapAllocatorMemBlock* mPrev;
				 PointerRefs* mPointers;

			 public:

				 bcHeapAllocatorMemBlock()
				 {
					 mDataAddress = 0;
					 mSize = 0;
					 mNext = bcNULL;
					 mPrev = bcNULL;
					 mPointers = bcNULL;
				 }

				 bcSIZE dataAddress() const { return mDataAddress; }
				 void dataAddress(bcSIZE val) { mDataAddress = val; }
				 bcSIZE size() const { return mSize; }
				 void size(bcSIZE val) { mSize = val; }
				 // == If there is any pointer reference in this block return true
				 // else return false == MRB ==
				 bcBOOL free() const { return (mPointers)? bcFALSE : bcTRUE; }
				 bcHeapAllocatorMemBlock* next() const { return mNext; }
				 void next(bcHeapAllocatorMemBlock* val) { mNext = val; }
				 bcHeapAllocatorMemBlock* prev() const { return mPrev; }
				 void prev(bcHeapAllocatorMemBlock* val) { mPrev = val; }
				 PointerRefs* pointers() const { return mPointers; }
				 void pointers(PointerRefs* val) { mPointers = val; }

				 // == If any pointer exist in this block or this block is not free
				 // return MemBlock of one of it's pointers else return null that
				 // indicate this block is free == MRB ==
				 bcInline MemBlock* getMemBlock()
				 {
					 if(mPointers)
						 return MemBlock::retrieveMemBlock(*(mPointers->mPointerReference));
					 else
						 return bcNULL;
				 }

				 bcInline void registerNewPointer(PointerRefs* pPointerRef, const void* pPointer)
				 {
					 pPointerRef->mPointerReference = const_cast<void**>(&pPointer);

					 if(mPointers)
					 {
						 pPointerRef->mNext = mPointers->mNext;
						 mPointers = pPointerRef;
					 }
					 else
						 mPointers = pPointerRef;
				 }

				 bcInline PointerRefs* unRegisterPointer(const void* pPointer)
				 {
					 PointerRefs* lPrevPointerRef = bcNULL;
					 PointerRefs* lPointerRef = mPointers;
					 while (lPointerRef && *(lPointerRef->mPointerReference) != pPointer)
					 {
						 lPrevPointerRef = lPointerRef;
						 lPointerRef = lPointerRef->mNext;
					 }

					 if(lPrevPointerRef && lPointerRef)
					 {
						 lPrevPointerRef->mNext = lPointerRef->mNext;
					 }
					 else if(!lPrevPointerRef && lPointerRef)
					 {
						 mPointers = lPointerRef->mNext;
					 }
					 else
					 {
						bcAssert(bcFALSE);	
					 }

					 return lPointerRef;
				 }
			 };

		protected:
			MemMngFreeList<bcHeapAllocatorMemBlock> mBlockAllocator;
			MemMngFreeList<bcHeapAllocatorMemBlock::PointerRefs> mPointerRefAllocator;

			bcHeapAllocatorMemBlock* mFirstMemBlock;
			bcHeapAllocatorMemBlock* mTop;
			bcSIZE mSize;
			bcUBYTE* mHeap;
			bcUINT32 mNumFragmentation;

		public:
			bcHeapAllocator();
			~bcHeapAllocator();
			void initialize(bcSIZE pSize, const bcCHAR* pTag);

			bcUINT32 size() { return mSize; };
			bcUINT32 numFragmentation() { return mNumFragmentation; };

			virtual void* alloc(MemBlock* pMemBlock) override;
			virtual void free(const void* pPointer, MemBlock* pMemBlock) override;
			
			virtual bcBOOL containPointer(const void* pMemory) override;
			virtual bcBOOL isEmpty() override;
			virtual void clear() override;

			virtual void registerPointer(const void* pPointer) override;
			virtual void unRegisterPointer(const void* pPointer) override;

			virtual void defragment(bcINT32 pNumDefrag) override;
			//void reSize(bcSIZE pNewSize);
		};
#pragma endregion

		// == CrtAllocator ============================================================================ MRB ==
		class BC_COREDLL_EXP bcCrtAllocator : public bcAllocator
		{
		private:

		protected:

		public:
			bcCrtAllocator() {};
			~bcCrtAllocator() {};
			void initialize(const bcCHAR* pTag)
			{
				bcAllocator::tag(pTag);
			}

			virtual void* alloc(MemBlock* pMemBlock) override
			{
				void* result = _aligned_malloc(pMemBlock->size(), BC_MEMORY_MIN_ALIGN);
				if(result)
					bcAllocator::mTracer.acceptAlloc(pMemBlock->size());
				else
					bcAllocator::mTracer.rejectAlloc(pMemBlock->size());

				return result;
			}
			virtual void free(const void* pPointer, MemBlock* pMemBlock) override
			{
				_aligned_free(const_cast<void*>(pPointer));
				pMemBlock->free(bcTRUE);
				bcAllocator::mTracer.acceptFree(pMemBlock->size());
			}

			virtual bcBOOL containPointer(const void* pPointer) override
			{
				return bcTRUE;
			}
			virtual bcBOOL isEmpty() override
			{
				return bcFALSE;
			}
			virtual void clear() override
			{

			}
		};

#endif
	}
}

#endif