//  [9/23/2013 MRB]

#pragma  once

#include "CorePCH.h"
#include "MemBlock.h"
#include "bcMemMngList.h"
#include "bcMutex.h"
#include "bcConditionVariable.h"

namespace BlackCat
{
	namespace Core
	{
#ifdef BC_MEMORY_ENABLE

		template< typename T, bcSIZE ...tLevelSizes >
		class bcMemMngHashMap;

		template< typename T, bcSIZE ...tLevelSizes >
		class bcMemMngHashMap
		{
			typedef T Type;
			struct StorageItem;
			struct StorageChunk;
			typedef bcMemMngList< StorageItem > StorageItemListType;
			typedef typename StorageItemListType::IteratorType StorageItemIterator;
		private:

		protected:
			struct StorageItem
			{
			private:

			protected:

			public:
				Type mData;
				void* mKey;

				StorageItem()
				{
					mKey = bcNULL;
				}
				StorageItem(const void* pKey, const Type& pData)
				{
					mData = pData;
					mKey = const_cast<void*>(pKey);
				}
				~StorageItem() {}
			};

			struct StorageChunk
			{
				typedef typename StorageItemListType::Node Node;
			private:

			public:
				StorageChunk* mSubLevel;
				Node* mFirstItem;
				Node* mLastItem;

			public:
				StorageChunk()
				{
					mSubLevel = bcNULL;
					mFirstItem = bcNULL;
					mLastItem = bcNULL;
				}
				~StorageChunk() {}
			};

			const bcSIZE mMinAllocationSize;
			StorageChunk* mChunks;
			const bcSIZE mLevelCount;
			bcSIZE mLevelSizes[sizeof...(tLevelSizes)];
			StorageItemListType mItemList;
			bcAtomic<bcUINT32> mCount;

			bcMutex mMutext;
			bcAtomicFlag mFlag;

			void _freeChunk(StorageChunk* lChunk, bcSIZE pLevel)
			{
				// This function is used in deconstruction and won't be called concurrency, but for visibility of
				// mSubLevel for other threads(cache coherency) we use mFlag /
				bcAtomicFlag::bcAtomicFlagTestAndSet(mFlag, bcMemoryOrder_Acquire);
				
				for (bcUINT32 c = 0; c < mLevelSizes[pLevel]; ++c)
				{
					
					if (lChunk[c].mSubLevel)
					{
						_freeChunk(lChunk[c].mSubLevel, pLevel + 1);
					}
				}
				delete[] lChunk;
			}

			template< typename Func >
			void _iterateOver(Func pFunction, StorageChunk* lChunk, bcSIZE pLevel)
			{
				// This function is used only on app exit to report leaks and won't be called concurrency, but for 
				// visibility of mSubLevel for other threads(cache coherency) we use mFlag /
				bcAtomicFlag::bcAtomicFlagTestAndSet(mFlag, bcMemoryOrder_Acquire);

				for (bcUINT32 c = 0; c < mLevelSizes[pLevel]; ++c)
				{
					if (lChunk[c].mSubLevel)
					{
						_iterateOver(pFunction, lChunk[c].mSubLevel, pLevel + 1);
					}
					else if (lChunk[c].mFirstItem && lChunk[c].mLastItem)
					{
						StorageItemIterator lBegin = mItemList.createIterator(lChunk[c].mFirstItem);
						StorageItemIterator lEnd = mItemList.createIterator(lChunk[c].mLastItem);

						while (++lBegin != lEnd)
						{
							pFunction(&lBegin->mKey, &lBegin->mData);
						}
					}
				}
			}

			bcInline bcUINT32 _convertPointerToKey(const void* pPointer)
			{
				return reinterpret_cast<bcUINT32>(pPointer) / mMinAllocationSize;
			}

			bcInline StorageChunk* _findChunk(bcUINT32 pKey)
			{
				bcUINT32 lDepth = 1;
				StorageChunk* lTargetChunk = &mChunks[pKey % mLevelSizes[0]];

				for (bcUINT32 l = 1; l < mLevelCount; ++l)
				{
					++lDepth;
					if (!lTargetChunk->mSubLevel)
					{
						bcLockGaurd lGaurd(mMutext);

						if (!lTargetChunk->mSubLevel)
						{
							StorageChunk* lSubChunks = new StorageChunk[mLevelSizes[l]];
							if (lTargetChunk->mSubLevel == bcNULL)
							{
								// TODO check for allocation fail here
								bcAssert(bcFALSE);
							}
							if (lDepth == mLevelCount)
							{
								for (bcUINT32 i = 0; i < mLevelSizes[l]; ++i)
								{
									StorageChunk* lCurrentChunk = &lSubChunks->mSubLevel[i];
									StorageItem item;
									item.mKey = reinterpret_cast<void*>(i);
									lCurrentChunk->mSubLevel = bcNULL;
									lCurrentChunk->mFirstItem = mItemList.pushBack().getNode();
									lCurrentChunk->mLastItem = mItemList.pushBack().getNode();
								}
							}
							lTargetChunk->mSubLevel = lSubChunks;

							bcAtomicFlag::bcAtomicFlagTestAndSet(mFlag, bcMemoryOrder_AcqRel);
						}
					}
					
					lTargetChunk = &lTargetChunk->mSubLevel[pKey % mLevelSizes[l]];
				}

				return lTargetChunk;
			}

		public:
			bcMemMngHashMap(bcUINT32 pNumThread) :
				mLevelCount(sizeof...(tLevelSizes)),
				mMinAllocationSize(MemBlock::getRequiredSize(1, BC_MEMORY_MIN_ALIGN)),
				mItemList(pNumThread, 2)
			{
				bcAssert(mLevelCount > 0);
				//bcAssert(sizeof...(tLevelSizes) == tLevelCount);

				bcSIZE lTempLevelSizes[sizeof...(tLevelSizes)] = { tLevelSizes... };
				std::memcpy(&mLevelSizes[0], &lTempLevelSizes, mLevelCount * sizeof(bcSIZE));

				//bcAtomicOperation::bcAtomicStore(mNewChunkFlag, bcTRUE, bcMemoryOrder_SeqCst);

				mChunks = new StorageChunk[mLevelSizes[0]];
				if (mLevelCount == 1)
				{
					for (bcUINT32 i = 0; i < mLevelSizes[0]; ++i)
					{
						StorageChunk* lCurrentChunk = &mChunks[i];
						lCurrentChunk->mFirstItem = mItemList.pushBack().getNode();
						lCurrentChunk->mLastItem = mItemList.pushBack().getNode();
					}
				}
				bcAtomicFlag::bcAtomicFlagTestAndSet(mFlag, bcMemoryOrder_Release);

				bcAtomicOperation::bcAtomicStore(mCount, 0U, bcMemoryOrder_SeqCst);

				//bcAtomicOperation::bcAtomicStore(mNewChunkFlag, bcFALSE, bcMemoryOrder_SeqCst);
			}

			~bcMemMngHashMap()
			{
				_freeChunk(mChunks, 0);
			}

			Type* insert(const void* pPointer, const Type& pValue)
			{
				bcUINT32 lKey = _convertPointerToKey(pPointer);
				StorageChunk* lChunk = _findChunk(lKey);

				/*StorageItem* lItem = mItemAllocator.Alloc();

				if (!lChunk->mItems)
				{
					lChunk->mItems = lItem;
				}
				else
				{
					lItem->mNext = lChunk->mItems;
					lChunk->mItems = lItem;
				}

				lItem->mData = pValue;
				lItem->mKey = pPointer;

				return &lItem->mData;*/

				StorageItemIterator lBegin = mItemList.createIterator(lChunk->mFirstItem);
				StorageItemIterator lInsertedItem = mItemList.insertAfter(lBegin, StorageItem(pPointer, pValue));

				bcAtomicOperation::bcAtomicFetchAdd(mCount, 1U, bcMemoryOrder_SeqCst);

				return &lInsertedItem->mData;
			}

			void remove(const void* pPointer)
			{
				bcUINT32 lPointer = reinterpret_cast<bcUINT32>(pPointer);
				bcUINT32 lKey = _convertPointerToKey(pPointer);
				StorageChunk* lChunk = _findChunk(lKey);

				StorageItemIterator lBegin = mItemList.createIterator(lChunk->mFirstItem);
				StorageItemIterator lEnd = mItemList.createIterator(lChunk->mLastItem);

				while (++lBegin != lEnd)
				{
					if (lBegin->mKey == pPointer)
						break;
				}

				if (lBegin != lEnd)
				{
					mItemList.erase(lBegin);
					bcAtomicOperation::bcAtomicFetchSub(mCount, 1U, bcMemoryOrder_SeqCst);
				}	
				else
					bcAssert(bcFALSE);

				/*StorageItem* lItem = lChunk->mItems;
				StorageItem* lPrevItem = bcNULL;
				while (lItem && lItem->mKey != pPointer)
				{
					lPrevItem = lItem;
					lItem = lItem->mNext;
				}

				if (lPrevItem && lItem)
				{
					lPrevItem->mNext = lItem->mNext;
				}
				else if (!lPrevItem && lItem)
				{
					lChunk->mItems = lItem->mNext;
				}
				else
				{
					bcAssert(bcFALSE);
				}

				mItemAllocator.Free(lItem);*/
			}

			bcUINT32 count()
			{
				return bcAtomicOperation::bcAtomicLoad(mCount, bcMemoryOrder_SeqCst);
			}

			// == Func(void**, T*) == MRB ==
			template< typename Func >
			void iterateOver(Func pFunction)
			{
				_iterateOver(pFunction, mChunks, 0);
			}
		};

		//template< typename T, bcSIZE ...tLevelSizes >
		//class bcMemMngHashMap;

		//template< typename T, bcSIZE ...tLevelSizes >
		//class bcMemMngHashMap
		//{
		//private:

		//protected:
		//	struct StorageItem
		//	{
		//	private:

		//	protected:

		//	public:
		//		T mData;
		//		void* mKey;
		//		StorageItem* mNext;

		//		StorageItem()
		//		{
		//			mKey = bcNULL;
		//			mNext = bcNULL;
		//		}
		//		StorageItem(const void* pKey, const T& pData)
		//		{
		//			mData = pData;
		//			mKey = pKey;
		//			mNext = bcNULL;
		//		}
		//		~StorageItem() {}
		//	};

		//	struct StorageChunk
		//	{
		//	private:

		//	public:
		//		StorageChunk* mSubLevel;
		//		StorageItem* mItems;

		//	public:
		//		StorageChunk()
		//		{
		//			mSubLevel = bcNULL;
		//			mItems = bcNULL;
		//		}
		//		~StorageChunk() {}
		//	};

		//	const bcSIZE mMinAllocationSize;
		//	StorageChunk* mChunks;
		//	const bcSIZE mLevelCount;
		//	bcSIZE mLevelSizes[sizeof...(tLevelSizes)];
		//	MemMngFreeList<StorageItem> mItemAllocator;

		//	void _freeChunk(StorageChunk* lChunk, bcSIZE pLevel)
		//	{
		//		for (bcUINT32 c = 0; c < mLevelSizes[pLevel]; ++c)
		//		{
		//			if(lChunk[c].mSubLevel)
		//			{
		//				_freeChunk(lChunk[c].mSubLevel, pLevel + 1);
		//			}
		//		}
		//		delete[] lChunk;
		//	}

		//	template< typename Func >
		//	void _iterateOver(Func pFunction, StorageChunk* lChunk, bcSIZE pLevel)
		//	{
		//		for (bcUINT32 c = 0; c < mLevelSizes[pLevel]; ++c)
		//		{
		//			if(lChunk[c].mSubLevel)
		//			{
		//				_iterateOver(pFunction, lChunk[c].mSubLevel, pLevel + 1);
		//			}
		//			else if(lChunk[c].mItems)
		//			{
		//				StorageItem* lItem = lChunk[c].mItems;
		//				while (lItem)
		//				{
		//					pFunction(&lItem->mKey, &lItem->mData);
		//					lItem = lItem->mNext;
		//				}
		//			}
		//		}
		//	}

		//	bcInline bcUINT32 _convertPointerToKey(void* pPointer)
		//	{
		//		return reinterpret_cast<bcUINT32>(pPointer) / mMinAllocationSize;
		//	}

		//	bcInline StorageChunk* _findChunk(bcUINT32 pKey)
		//	{
		//		StorageChunk* lTargetChunk = &mChunks[pKey % mLevelSizes[0]];

		//		for(bcUINT32 l = 1; l < mLevelCount; ++l)
		//		{
		//			if(!lTargetChunk->mSubLevel)
		//				lTargetChunk->mSubLevel = new StorageChunk[mLevelSizes[l]];

		//			lTargetChunk = &lTargetChunk->mSubLevel[pKey % mLevelSizes[l]];
		//		}

		//		return lTargetChunk;
		//	}

		//public:
		//	bcMemMngHashMap() : 
		//		mLevelCount(sizeof...(tLevelSizes)), 
		//		mMinAllocationSize(MemBlock::getRequiredSize(1, BC_MEMORY_MIN_ALIGN))
		//	{
		//		bcAssert(mLevelCount > 0);
		//		//bcAssert(sizeof...(tLevelSizes) == tLevelCount);

		//		bcSIZE lTempLevelSizes[sizeof...(tLevelSizes)] = { tLevelSizes... };
		//		std::memcpy(&mLevelSizes, &lTempLevelSizes, mLevelCount * sizeof(bcSIZE));

		//		mChunks = new StorageChunk[mLevelSizes[0]];
		//		mItemAllocator.initialize();
		//	}

		//	~bcMemMngHashMap()
		//	{
		//		_freeChunk(mChunks, 0);
		//	}

		//	T* insert(void* pPointer, const T& pValue)
		//	{
		//		bcUINT32 lKey = _convertPointerToKey(pPointer);
		//		StorageChunk* lChunk = _findChunk(lKey);

		//		StorageItem* lItem = mItemAllocator.alloc();
		//		
		//		if(!lChunk->mItems)
		//		{
		//			lChunk->mItems = lItem;
		//		}
		//		else
		//		{
		//			lItem->mNext = lChunk->mItems;
		//			lChunk->mItems = lItem;
		//		}

		//		lItem->mData = pValue;
		//		lItem->mKey = pPointer;

		//		return &lItem->mData;
		//	}

		//	void remove(void* pPointer)
		//	{
		//		bcUINT32 lPointer = reinterpret_cast<bcUINT32>(pPointer);
		//		bcUINT32 lKey = _convertPointerToKey(pPointer);
		//		StorageChunk* lChunk = _findChunk(lKey);

		//		StorageItem* lItem = lChunk->mItems;
		//		StorageItem* lPrevItem = bcNULL;
		//		while (lItem && lItem->mKey != pPointer)
		//		{
		//			lPrevItem = lItem;
		//			lItem = lItem->mNext;
		//		}

		//		if(lPrevItem && lItem)
		//		{
		//			lPrevItem->mNext = lItem->mNext;
		//		}
		//		else if(!lPrevItem && lItem)
		//		{
		//			lChunk->mItems = lItem->mNext;
		//		}
		//		else
		//		{
		//			bcAssert(bcFALSE);
		//		}

		//		mItemAllocator.free(lItem);
		//	}

		//	bcUINT32 count()
		//	{
		//		return mItemAllocator.count();
		//	}

		//	// == Func(void**, T*) == MRB ==
		//	template< typename Func >
		//	void iterateOver(Func pFunction)
		//	{
		//		_iterateOver(pFunction, mChunks, 0);
		//	}
		//};

#endif // BC_MEMORY_ENABLE
	}
}