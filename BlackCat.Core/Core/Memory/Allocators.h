//  [8/23/2013 MRB]

#pragma once

#ifndef Allocators_H
#define Allocators_H

#include "CorePCH.h"
#include "MemBlock.h"
//#include "Ptr.h"
#include "bcMemMngFreeList.h"
#include "bcAtomic.h"
#include "bcMemMngList.h"

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
			bcAtomic< bcUINT32 > mTotalSize;
			bcAtomic< bcUINT32 > mUsedSize;
			bcAtomic< bcUINT32 > mOverheadSize;
			bcAtomic< bcUINT32 > mMaxUsedSize;
			bcAtomic< bcUINT32 > mAllocCount;
			bcAtomic< bcUINT32 > mRejectedAllocSize;
			bcAtomic< bcUINT32 > mRejectedAllocCount;
#endif	

		public:
			bcAllocatorTracer()
			{
#ifdef BC_MEMORY_TRACING
				bcAtomicOperation::bcAtomicInit(mTotalSize, 0U);
				bcAtomicOperation::bcAtomicInit(mUsedSize, 0U);
				bcAtomicOperation::bcAtomicInit(mOverheadSize, 0U);
				bcAtomicOperation::bcAtomicInit(mMaxUsedSize, 0U);
				bcAtomicOperation::bcAtomicInit(mAllocCount, 0U);
				bcAtomicOperation::bcAtomicInit(mRejectedAllocSize, 0U);
				bcAtomicOperation::bcAtomicInit(mRejectedAllocCount, 0U);
#endif
			}

			void initialize(bcUINT32 pTotalSize)
			{
#ifdef BC_MEMORY_TRACING
				bcAtomicOperation::bcAtomicStore(mTotalSize, pTotalSize, bcMemoryOrder_SeqCst);
#endif
			}

			bcUINT32 totalSize() const 
			{ 
#ifdef BC_MEMORY_TRACING
				return bcAtomicOperation::bcAtomicLoad(mTotalSize, bcMemoryOrder_Relaxed); 
#else
				return 0;
#endif
			}
			void totalSize(bcUINT32 val) 
			{ 
#ifdef BC_MEMORY_TRACING
				bcAtomicOperation::bcAtomicStore(mTotalSize, val, bcMemoryOrder_Relaxed); 
#endif
			}
			bcUINT32 usedSize() const 
			{ 
#ifdef BC_MEMORY_TRACING
				return bcAtomicOperation::bcAtomicLoad(mUsedSize, bcMemoryOrder_Relaxed); 
#else
				return 0;
#endif
			}
			void usedSize(bcUINT32 val) 
			{ 
#ifdef BC_MEMORY_TRACING
				bcAtomicOperation::bcAtomicStore(mUsedSize, val, bcMemoryOrder_Relaxed); 
#endif 
			}
			bcUINT32 overheadSize() const 
			{ 
#ifdef BC_MEMORY_TRACING
				return bcAtomicOperation::bcAtomicLoad(mOverheadSize, bcMemoryOrder_Relaxed); 
#else
				return 0;
#endif
			}
			void overheadSize(bcUINT32 val) 
			{ 
#ifdef BC_MEMORY_TRACING
				bcAtomicOperation::bcAtomicStore(mOverheadSize, val, bcMemoryOrder_Relaxed); 
#endif
			}
			bcUINT32 maxUsedSize() const 
			{ 
#ifdef BC_MEMORY_TRACING
				return bcAtomicOperation::bcAtomicLoad(mMaxUsedSize, bcMemoryOrder_Relaxed); 
#else
				return 0;
#endif
			}
			void maxUsedSize(bcUINT32 val) 
			{ 
#ifdef BC_MEMORY_TRACING
				bcAtomicOperation::bcAtomicStore(mMaxUsedSize, val, bcMemoryOrder_Relaxed); 
#endif 
			}
			bcUINT32 allocCount() const 
			{ 
#ifdef BC_MEMORY_TRACING
				return bcAtomicOperation::bcAtomicLoad(mAllocCount, bcMemoryOrder_Relaxed); 
#else
				return 0;
#endif
			}
			void allocCount(bcUINT32 val) 
			{ 
#ifdef BC_MEMORY_TRACING
				bcAtomicOperation::bcAtomicStore(mAllocCount, val, bcMemoryOrder_Relaxed); 
#endif
			}
			bcUINT32 rejectedAllocSize() const 
			{ 
#ifdef BC_MEMORY_TRACING
				return bcAtomicOperation::bcAtomicLoad(mRejectedAllocSize, bcMemoryOrder_Relaxed); 
#else
				return 0;
#endif
			}
			void rejectedAllocSize(bcUINT32 val) 
			{ 
#ifdef BC_MEMORY_TRACING
				bcAtomicOperation::bcAtomicStore(mRejectedAllocSize, val, bcMemoryOrder_Relaxed); 
#endif 
			}
			bcUINT32 rejectedAllocCount() const 
			{ 
#ifdef BC_MEMORY_TRACING
				return bcAtomicOperation::bcAtomicLoad(mRejectedAllocCount, bcMemoryOrder_Relaxed); 
#else
				return 0;
#endif
			}
			void rejectedAllocCount(bcUINT32 val) 
			{ 
#ifdef BC_MEMORY_TRACING
				bcAtomicOperation::bcAtomicStore(mRejectedAllocCount, val, bcMemoryOrder_Relaxed); 
#endif 
			}

			bcInline void acceptAlloc(bcUINT32 pSize)
			{
#ifdef BC_MEMORY_TRACING
				bcAtomicOperation::bcAtomicFetchAdd(mUsedSize, pSize, bcMemoryOrder_Relaxed);

				while(true)
				{
					bcUINT32 lLocalMaxUsedSize = bcAtomicOperation::bcAtomicLoad(mMaxUsedSize, bcMemoryOrder_Relaxed);
					bcUINT32 lLocalUsedSize = bcAtomicOperation::bcAtomicLoad(mUsedSize, bcMemoryOrder_Relaxed);
					bcUINT32 lNewMaxUsedSize = std::max<bcUINT32>(lLocalUsedSize, lLocalMaxUsedSize);

					if(bcAtomicOperation::bcAtomicCompareExchangeStrong(
						mMaxUsedSize,
						&lLocalMaxUsedSize,
						lNewMaxUsedSize,
						bcMemoryOrder_Relaxed,
						bcMemoryOrder_Relaxed))
					{
						break;
					}
				}
				
				bcAtomicOperation::bcAtomicFetchAdd(mAllocCount, 1U, bcMemoryOrder_Relaxed);
#endif
			}
			bcInline void acceptFree(bcUINT32 pSize)
			{
#ifdef BC_MEMORY_TRACING
				bcAtomicOperation::bcAtomicFetchSub(mUsedSize, pSize, bcMemoryOrder_Relaxed);
				bcAtomicOperation::bcAtomicFetchSub(mAllocCount, 1U, bcMemoryOrder_Relaxed);
#endif
			}
			bcInline void acceptOverHead(bcUINT32 pSize)
			{
#ifdef BC_MEMORY_TRACING
				bcAtomicOperation::bcAtomicFetchAdd(mOverheadSize, pSize, bcMemoryOrder_Relaxed);
#endif
			}
			bcInline void acceptFreeOverHead(bcUINT32 pSize)
			{
#ifdef BC_MEMORY_TRACING
				bcAtomicOperation::bcAtomicFetchSub(mOverheadSize, pSize, bcMemoryOrder_Relaxed);
#endif
			}
			bcInline void rejectAlloc(bcUINT32 pSize)
			{
#ifdef BC_MEMORY_TRACING
				bcAtomicOperation::bcAtomicFetchAdd(mRejectedAllocSize, pSize, bcMemoryOrder_Relaxed);
				bcAtomicOperation::bcAtomicFetchAdd(mRejectedAllocCount, 1U, bcMemoryOrder_Relaxed);
#endif
			}

			bcInline void acceptClear()
			{
#ifdef BC_MEMORY_TRACING
				//mTotalSize = 0;
				bcAtomicOperation::bcAtomicStore(mUsedSize, 0U, bcMemoryOrder_Relaxed);
				bcAtomicOperation::bcAtomicStore(mOverheadSize, 0U, bcMemoryOrder_Relaxed);
				//mMaxUsedSize = 0;
				bcAtomicOperation::bcAtomicStore(mAllocCount, 0U, bcMemoryOrder_Relaxed);
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

			// == In allocators we call this function only in theirs Initialize method, so it is safe == MRB ==
			void tag(const bcCHAR* pTag)
			{
				std::memcpy(mTag, pTag, std::min<bcSIZE>(bcSIZE(29), strlen(pTag)));
				*(bcAllocator::mTag + std::min<bcSIZE>(bcSIZE(29), strlen(pTag))) = '\0';
			}
			bcCHAR* tag() { return mTag; };

			const bcAllocatorTracer& tracer() const { return mTracer; }
			
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
#ifdef BC_MEMORY_DEFRAG
			virtual void registerPointer(const void* pPointer) abstract;
			virtual void unRegisterPointer(const void* pPointer) abstract;

			virtual void defragment(bcINT32 pNumDefrag) abstract;
#endif
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
			bcAtomic< bcUINT32 > mAllocatedBlock;
			// == bit-vector indicating if a block is allocated or not == MRB ==
			bcAtomic < FIXEDSIZE_INDEX_TYPE >* mBlocks;
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
			bcAtomic< bcUBYTE* > mTop;
			bcUBYTE* mHeap;
			//// == Pointer to actual(include offset of block) data of last allocation == MRB ==
			//bcUBYTE* mLastAllocation;

		public:
			StackAllocator();
			~StackAllocator();
			void initialize(bcUINT32 pSize, const bcCHAR* pTag);

			bcUINT32 size() { return mSize; };

			void* push(MemBlock* pMemBlock);
			void pop(const void* pPointer, MemBlock* pMemBlock);

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
#ifdef BC_MEMORY_DEFRAG
				 struct PointerRefs
				 {
					 void** mPointerReference;
					 PointerRefs* mNext;
				 };
#endif
			 public:
				 struct SizeAndFree
				 {
					 friend class bcHeapAllocatorMemBlock;
				 private:
					 static const bcINT32 mergeMask = 1;
					 static const bcINT32 freeMask = 2;
					 static const bcINT32 sizeMask = ~(freeMask + mergeMask);

				 protected:
					 bcAtomic< bcUINT32 > mData;

				 public:
					 SizeAndFree()
					 {
						 bcAtomicOperation::bcAtomicStore(mData, 0U);
					 }
					 SizeAndFree(bcUINT32 pSize, bcBOOL pFree = bcFALSE, bcBOOL pMergeFlag = bcFALSE)
					 {
						 bcUINT32 lSize = pSize & sizeMask; // Reset flags
						 bcAssert((pSize & (freeMask + mergeMask)) == 0);
						 bcAtomicOperation::bcAtomicStore(
							 mData, 
							 pSize | (static_cast<bcUINT32>(pFree) << 1) | static_cast<bcUINT32>(pMergeFlag));
					 }
					 
					 bcInline bcUINT32 size() { return bcAtomicOperation::bcAtomicLoad(mData, bcMemoryOrder_SeqCst) & sizeMask; }
					 bcInline bcBOOL free() { return bcAtomicOperation::bcAtomicLoad(mData, bcMemoryOrder_SeqCst) & freeMask; }
					 bcInline bcBOOL merge() { return bcAtomicOperation::bcAtomicLoad(mData, bcMemoryOrder_SeqCst) & mergeMask; }

					 bcInline bcBOOL compareAndSwap(const SizeAndFree& pExpected, const SizeAndFree& pNew)
					 {
						 bcUINT32 lExpected = bcAtomicOperation::bcAtomicLoad(pExpected.mData, bcMemoryOrder_SeqCst);
						 bcUINT32 lNew = bcAtomicOperation::bcAtomicLoad(pNew.mData, bcMemoryOrder_SeqCst);

						 if (bcAtomicOperation::bcAtomicCompareExchangeStrong(
							 mData,
							 &lExpected,
							 lNew,
							 bcMemoryOrder_SeqCst,
							 bcMemoryOrder_SeqCst))
						 {
							 bcUINT32 lNewExpected = bcAtomicOperation::bcAtomicLoad(pExpected.mData, bcMemoryOrder_SeqCst);
							 bcUINT32 lNewNew = bcAtomicOperation::bcAtomicLoad(pNew.mData, bcMemoryOrder_SeqCst);

							 bcAssert(lExpected == lNewExpected && lNew == lNewNew);

							 return bcTRUE;
						 }

						 return bcFALSE;
					 }
					 bcInline void store(const SizeAndFree& pValue)
					 {
						 bcUINT32 lValue = bcAtomicOperation::bcAtomicLoad(pValue.mData, bcMemoryOrder_SeqCst);
						 bcAtomicOperation::bcAtomicStore(mData, lValue, bcMemoryOrder_SeqCst);
					 }
				 };

			 private:

			 protected:
				 bcAtomic< bcSIZE > mDataAddress;
				 SizeAndFree mSizeAndFree;
				 //bcSIZE mSize;

#ifdef BC_MEMORY_DEFRAG
				 PointerRefs* mPointers;
#else
				 //bcBOOL mFree;
#endif

			 public:
				 bcHeapAllocatorMemBlock() : mSizeAndFree(0U, bcFALSE)
				 {
					 bcAtomicOperation::bcAtomicStore(mDataAddress, 0U);
					 //mDataAddress = 0;
					 //mSize = 0;
					 //mNext = bcNULL;
					 //mPrev = bcNULL;
#ifdef BC_MEMORY_DEFRAG
					 mPointers = bcNULL;
#else
					 //mFree = bcTRUE;
#endif
				 }
				 bcHeapAllocatorMemBlock(bcSIZE pDataAddress, bcSIZE pSize, bcBOOL pFree) : mSizeAndFree(pSize, pFree)
				 {
					 bcAtomicOperation::bcAtomicStore(mDataAddress, pDataAddress);
				 }
				 bcHeapAllocatorMemBlock(const bcHeapAllocatorMemBlock& pOther)
				 {
					 bcAtomicOperation::bcAtomicStore(
						 mDataAddress, 
						 bcAtomicOperation::bcAtomicLoad(pOther.mDataAddress, bcMemoryOrder_SeqCst),
						 bcMemoryOrder_SeqCst);
					 bcAtomicOperation::bcAtomicStore(
						 mSizeAndFree.mData,
						 bcAtomicOperation::bcAtomicLoad(pOther.mSizeAndFree.mData, bcMemoryOrder_SeqCst),
						 bcMemoryOrder_SeqCst);
				 };
				 bcHeapAllocatorMemBlock& operator =(const bcHeapAllocatorMemBlock& pOther)
				 {
					 bcAtomicOperation::bcAtomicStore(
						 mDataAddress,
						 bcAtomicOperation::bcAtomicLoad(pOther.mDataAddress, bcMemoryOrder_SeqCst),
						 bcMemoryOrder_SeqCst);
					 bcAtomicOperation::bcAtomicStore(
						 mSizeAndFree.mData,
						 bcAtomicOperation::bcAtomicLoad(pOther.mSizeAndFree.mData, bcMemoryOrder_SeqCst),
						 bcMemoryOrder_SeqCst);

					 return *this;
				 }

				 bcInline bcSIZE dataAddress() const { return bcAtomicOperation::bcAtomicLoad(mDataAddress, bcMemoryOrder_SeqCst); }
				 bcInline void dataAddress(bcSIZE val) { bcAtomicOperation::bcAtomicStore(mDataAddress, val, bcMemoryOrder_SeqCst); }
				 bcInline SizeAndFree* sizeAndFree() { return &mSizeAndFree; }
				 bcInline void sizeAndFree(const SizeAndFree& pVal) 
				 {
					 mSizeAndFree.store(pVal);
				 }
				 /*bcSIZE size() const { return mSize; }
				 void size(bcSIZE val) { mSize = val; }*/
#ifdef BC_MEMORY_DEFRAG
				 // == If there is any pointer reference in this block return true
				 // else return false == MRB ==
				 bcBOOL free() const { return (mPointers)? bcFALSE : bcTRUE; }
#else
				 /*bcBOOL free() const { return mFree; }
				 void free(bcBOOL val) { mFree = val; }*/
#endif

#ifdef BC_MEMORY_DEFRAG
				 PointerRefs* pointers() const { return mPointers; }
				 void pointers(PointerRefs* val) { mPointers = val; }
#endif

#ifdef BC_MEMORY_DEFRAG
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
#endif
			 };
			 typedef bcHeapAllocatorMemBlock::SizeAndFree SizeAndFree;

		protected:
			typedef bcMemMngList< bcHeapAllocatorMemBlock > BlockListType;
			BlockListType mBlockList;
			typedef BlockListType::IteratorType BlockListIterator;

#ifdef BC_MEMORY_DEFRAG
			bcMemMngFreeList<bcHeapAllocatorMemBlock::PointerRefs> mPointerRefAllocator;
#endif

			bcSIZE mSize;
			bcUBYTE* mHeap;
			bcAtomic< bcUINT32 > mNumFragmentation;
			
			const bcSIZE mRemainingFreeSpaceLimit;

		public:
			bcHeapAllocator(bcUINT32 pNumThread);
			~bcHeapAllocator();
			void initialize(bcSIZE pSize, const bcCHAR* pTag);

			bcInline bcUINT32 size() { return mSize; };
			bcInline bcUINT32 numFragmentation() { return bcAtomicOperation::bcAtomicLoad(mNumFragmentation, bcMemoryOrder_Relaxed); };

			virtual void* alloc(MemBlock* pMemBlock) override;
			virtual void free(const void* pPointer, MemBlock* pMemBlock) override;
			
			virtual bcBOOL containPointer(const void* pMemory) override;
			virtual bcBOOL isEmpty() override;
			virtual void clear() override;

#ifdef BC_MEMORY_DEFRAG
			virtual void registerPointer(const void* pPointer) override;
			virtual void unRegisterPointer(const void* pPointer) override;

			virtual void defragment(bcINT32 pNumDefrag) override;
#endif
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
				pMemBlock->free(bcTRUE, bcMemoryOrder_SeqCst);
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