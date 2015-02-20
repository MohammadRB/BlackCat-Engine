//  [9/23/2013 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcConditionVariableProvider.h"
#include "CorePlatformImp/Concurrency/bcMutexProvider.h"
#include "Core/CorePCH.h"
#include "Core/Memory/bcMemBlock.h"

namespace black_cat
{
	namespace core
	{
#ifdef BC_MEMORY_ENABLE

		template< typename T, bcUINT32 ...tLevelSizes >
		class bc_memmng_hashmap;

		template< typename T, bcUINT32 ...tLevelSizes >
		class bc_memmng_hashmap
		{
		public:
			bc_memmng_hashmap() 
				: m_min_allocation_size(bc_memblock::get_required_size(1, BC_MEMORY_MIN_ALIGN)),
				m_level_count(sizeof...(tLevelSizes))
			{
				bcAssert(m_level_count > 0);
				//bcAssert(sizeof...(tLevelSizes) == tLevelCount);

				bcUINT32 l_temp_level_sizes[sizeof...(tLevelSizes)] = { tLevelSizes... };
				std::memcpy(&m_level_sizes, &l_temp_level_sizes, m_level_count * sizeof(bcUINT32));

				m_chunks = new storage_chunk[m_level_sizes[0]];
				m_item_allocator.initialize();
			}

			~bc_memmng_hashmap()
			{
				_free_chunk(m_chunks, 0);
			}

			T* insert(void* p_pointer, const T& p_value)
			{
				bcUINT32 l_key = _convert_pointer_to_key(p_pointer);
				storage_item* l_item = m_item_allocator.alloc();
				new (l_item)storage_item();

				storage_chunk* l_chunk = _find_chunk(l_key);

				if (!l_chunk->m_items)
				{
					l_chunk->m_items = l_item;
				}
				else
				{
					l_item->m_next = l_chunk->m_items;
					l_chunk->m_items = l_item;
				}

				l_chunk->unlock();

				l_item->m_key = p_pointer;
				l_item->m_data = p_value;

				return &l_item->m_data;
			}

			void remove(const void* p_pointer)
			{
				bcUINT32 l_key = _convert_pointer_to_key(p_pointer);

				storage_chunk* l_chunk = _find_chunk(l_key);

				storage_item* l_item = l_chunk->m_items;
				storage_item* l_prev_item = nullptr;
				while (l_item && l_item->m_key != p_pointer)
				{
					l_prev_item = l_item;
					l_item = l_item->m_next;
				}

				if (l_prev_item && l_item)
				{
					l_prev_item->m_next = l_item->m_next;
				}
				else if (!l_prev_item && l_item)
				{
					l_chunk->m_items = l_item->m_next;
				}
				else
				{
					bcAssert(false);
				}

				l_chunk->unlock();

				l_item->~storage_item();
				m_item_allocator.free(l_item);
			}

			bcUINT32 count()
			{
				return m_item_allocator.count();
			}

			// Func(void*, T&)
			template<typename TFunc>
			void iterate_over(TFunc p_function)
			{
				_iterate_over(p_function, m_chunks, 0);
			}

		protected:

		private:
			struct storage_item
			{
			public:
				T m_data;
				void* m_key;
				storage_item* m_next;

				storage_item()
				{
					m_key = nullptr;
					m_next = nullptr;
				}
				storage_item(void* pKey, const T& pData)
				{
					m_data = pData;
					m_key = pKey;
					m_next = nullptr;
				}
				~storage_item() {}
			};

			struct storage_chunk
			{
			public:
				storage_chunk* m_sub_levels;
				storage_item* m_items;
				core_platform::bc_hybrid_mutex m_mutext;

			public:
				storage_chunk()
				{
					m_sub_levels = nullptr;
					m_items = nullptr;
				}

				~storage_chunk() {}

				void lock(core_platform::bc_lock_operation p_operation)
				{
					m_mutext.lock(p_operation);
				}

				bool try_lock(core_platform::bc_lock_operation p_operation)
				{
					return m_mutext.try_lock(p_operation);
				}

				void unlock()
				{
					m_mutext.unlock();
				}
			};

			bcInline bcUINT32 _convert_pointer_to_key(const void* p_pointer)
			{
				return std::hash<bcUINTPTR>()(reinterpret_cast<bcUINTPTR>(p_pointer));
			}

			bcInline storage_chunk* _find_chunk(bcUINT32 p_key)
			{
				storage_chunk* l_target_chunk = &m_chunks[p_key % m_level_sizes[0]];
				core_platform::bc_unique_lock<storage_chunk> l_gaurd(*l_target_chunk, core_platform::bc_lock_operation::light);

				for (bcUINT32 l = 1; l < m_level_count; ++l)
				{
					if (!l_target_chunk->m_sub_levels)
						l_target_chunk->m_sub_levels = new storage_chunk[m_level_sizes[l]];

					storage_chunk* l_sub_level = &l_target_chunk->m_sub_levels[p_key % m_level_sizes[l]];

					core_platform::bc_unique_lock<storage_chunk> l_gaurd_sub_level(*l_sub_level, core_platform::bc_lock_operation::light);
					l_gaurd.unlock();

					l_target_chunk = l_sub_level;
					l_gaurd = std::move(l_gaurd_sub_level);
				}

				return l_gaurd.release();
			}

			void _free_chunk(storage_chunk* l_chunk, bcUINT32 p_level)
			{
				// TODO is there any need to synchronization?
				for (bcUINT32 c = 0; c < m_level_sizes[p_level]; ++c)
				{
					if (l_chunk[c].m_sub_levels)
					{
						_free_chunk(l_chunk[c].m_sub_levels, p_level + 1);
					}
				}
				delete[] l_chunk;
			}

			template<typename TFunc>
			void _iterate_over(TFunc p_function, storage_chunk* p_chunks, bcUINT32 p_level)
			{
				for (bcUINT32 c = 0; c < m_level_sizes[p_level]; ++c)
				{
					storage_chunk* l_chunk = &p_chunks[c];
					core_platform::bc_unique_lock<storage_chunk> l_gaurd(*l_chunk, core_platform::bc_lock_operation::medium);
					storage_chunk* l_sub_chunks = l_chunk->m_sub_levels;

					if (l_sub_chunks)
					{
						bcAssert(!l_chunk->m_items);

						l_gaurd.unlock();

						_iterate_over(p_function, l_sub_chunks, p_level + 1);
					}
					else
					{
						storage_item* l_item = l_chunk->m_items;
						while (l_item)
						{
							p_function(l_item->m_key, l_item->m_data);
							l_item = l_item->m_next;
						}
					}
				}
			}

			const bcUINT32 m_min_allocation_size;
			const bcUINT32 m_level_count;
			storage_chunk* m_chunks;
			bcUINT32 m_level_sizes[sizeof...(tLevelSizes)];
			bc_memmng_freelist<storage_item> m_item_allocator;

		};

		//template< typename T, bcSIZE ...tLevelSizes >
		//class bcMemMngHashMap;

		//template< typename T, bcSIZE ...tLevelSizes >
		//class bcMemMngHashMap
		//{
		//	typedef T Type;
		//	struct StorageItem;
		//	struct StorageChunk;
		//	typedef bcMemMngList< StorageItem > StorageItemListType;
		//	typedef typename StorageItemListType::IteratorType StorageItemIterator;
		//private:

		//protected:
		//	struct StorageItem
		//	{
		//	private:

		//	protected:

		//	public:
		//		Type mData;
		//		void* mKey;

		//		StorageItem()
		//		{
		//			mKey = nullptr;
		//		}
		//		StorageItem(const void* pKey, const Type& pData)
		//		{
		//			mData = pData;
		//			mKey = const_cast<void*>(pKey);
		//		}
		//		~StorageItem() {}
		//	};

		//	struct StorageChunk
		//	{
		//		typedef typename StorageItemListType::Node Node;
		//	private:

		//	public:
		//		StorageChunk* mSubLevel;
		//		Node* mFirstItem;
		//		Node* mLastItem;

		//	public:
		//		StorageChunk()
		//		{
		//			mSubLevel = nullptr;
		//			mFirstItem = nullptr;
		//			mLastItem = nullptr;
		//		}
		//		~StorageChunk() {}
		//	};

		//	const bcSIZE mMinAllocationSize;
		//	StorageChunk* mChunks;
		//	const bcSIZE mLevelCount;
		//	bcSIZE mLevelSizes[sizeof...(tLevelSizes)];
		//	StorageItemListType mItemList;
		//	core_platform::bc_atomic<bcUINT32> mCount;

		//	core_platform::bc_mutex mMutext;
		//	core_platform::bc_atomic_flag mFlag;

		//	void _freeChunk(StorageChunk* lChunk, bcSIZE pLevel)
		//	{
		//		// This function is used in deconstruction and won't be called concurrency, but for visibility of
		//		// mSubLevel for other threads(cache coherency) we use mFlag /
		//		mFlag.test_and_set(core_platform::bc_memory_order::acquire);
		//		
		//		for (bcUINT32 c = 0; c < mLevelSizes[pLevel]; ++c)
		//		{
		//			
		//			if (lChunk[c].mSubLevel)
		//			{
		//				_freeChunk(lChunk[c].mSubLevel, pLevel + 1);
		//			}
		//		}
		//		delete[] lChunk;
		//	}

		//	template< typename Func >
		//	void _iterateOver(Func pFunction, StorageChunk* lChunk, bcSIZE pLevel)
		//	{
		//		// This function is used only on app exit to report leaks and won't be called concurrency, but for 
		//		// visibility of mSubLevel for other threads(cache coherency) we use mFlag /
		//		mFlag.test_and_set(core_platform::bc_memory_order::acquire);

		//		for (bcUINT32 c = 0; c < mLevelSizes[pLevel]; ++c)
		//		{
		//			if (lChunk[c].mSubLevel)
		//			{
		//				_iterateOver(pFunction, lChunk[c].mSubLevel, pLevel + 1);
		//			}
		//			else if (lChunk[c].mFirstItem && lChunk[c].mLastItem)
		//			{
		//				StorageItemIterator lBegin = mItemList.createIterator(lChunk[c].mFirstItem);
		//				StorageItemIterator lEnd = mItemList.createIterator(lChunk[c].mLastItem);

		//				while (++lBegin != lEnd)
		//				{
		//					pFunction(&lBegin->mKey, &lBegin->mData);
		//				}
		//			}
		//		}
		//	}

		//	bcInline bcUINT32 _convertPointerToKey(const void* pPointer)
		//	{
		//		return reinterpret_cast<bcUINT32>(pPointer) / mMinAllocationSize;
		//	}

		//	bcInline StorageChunk* _findChunk(bcUINT32 pKey)
		//	{
		//		bcUINT32 lDepth = 1;
		//		StorageChunk* lTargetChunk = &mChunks[pKey % mLevelSizes[0]];

		//		for (bcUINT32 l = 1; l < mLevelCount; ++l)
		//		{
		//			++lDepth;
		//			if (!lTargetChunk->mSubLevel)
		//			{
		//				core_platform::bc_lock_guard<core_platform::bc_mutex> lGaurd(mMutext);

		//				if (!lTargetChunk->mSubLevel)
		//				{
		//					StorageChunk* lSubChunks = new StorageChunk[mLevelSizes[l]];
		//					if (lTargetChunk->mSubLevel == nullptr)
		//					{
		//						// TODO check for allocation fail here
		//						bcAssert(false);
		//					}
		//					if (lDepth == mLevelCount)
		//					{
		//						for (bcUINT32 i = 0; i < mLevelSizes[l]; ++i)
		//						{
		//							StorageChunk* lCurrentChunk = &lSubChunks->mSubLevel[i];
		//							StorageItem item;
		//							item.mKey = reinterpret_cast<void*>(i);
		//							lCurrentChunk->mSubLevel = nullptr;
		//							lCurrentChunk->mFirstItem = mItemList.pushBack().getNode();
		//							lCurrentChunk->mLastItem = mItemList.pushBack().getNode();
		//						}
		//					}
		//					lTargetChunk->mSubLevel = lSubChunks;

		//					mFlag.test_and_set(core_platform::bc_memory_order::acqrel);
		//				}
		//			}
		//			
		//			lTargetChunk = &lTargetChunk->mSubLevel[pKey % mLevelSizes[l]];
		//		}

		//		return lTargetChunk;
		//	}

		//public:
		//	bcMemMngHashMap(bcUINT32 pNumThread) :
		//		mLevelCount(sizeof...(tLevelSizes)),
		//		mMinAllocationSize(bc_memblock::get_required_size(1, BC_MEMORY_MIN_ALIGN)),
		//		mItemList(pNumThread, 2)
		//	{
		//		bcAssert(mLevelCount > 0);
		//		//bcAssert(sizeof...(tLevelSizes) == tLevelCount);

		//		bcSIZE lTempLevelSizes[sizeof...(tLevelSizes)] = { tLevelSizes... };
		//		std::memcpy(&mLevelSizes[0], &lTempLevelSizes, mLevelCount * sizeof(bcSIZE));

		//		//bcAtomicOperation::bcAtomicStore(mNewChunkFlag, true, core_platform::bc_memory_order::seqcst);

		//		mChunks = new StorageChunk[mLevelSizes[0]];
		//		if (mLevelCount == 1)
		//		{
		//			for (bcUINT32 i = 0; i < mLevelSizes[0]; ++i)
		//			{
		//				StorageChunk* lCurrentChunk = &mChunks[i];
		//				lCurrentChunk->mFirstItem = mItemList.pushBack().getNode();
		//				lCurrentChunk->mLastItem = mItemList.pushBack().getNode();
		//			}
		//		}
		//		mFlag.test_and_set(core_platform::bc_memory_order::release);

		//		mCount.store(0U, core_platform::bc_memory_order::seqcst);

		//		//bcAtomicOperation::bcAtomicStore(mNewChunkFlag, false, core_platform::bc_memory_order::seqcst);
		//	}

		//	~bcMemMngHashMap()
		//	{
		//		_freeChunk(mChunks, 0);
		//	}

		//	Type* insert(const void* pPointer, const Type& pValue)
		//	{
		//		bcUINT32 lKey = _convertPointerToKey(pPointer);
		//		StorageChunk* lChunk = _findChunk(lKey);

		//		/*StorageItem* lItem = mItemAllocator.Alloc();

		//		if (!lChunk->mItems)
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

		//		return &lItem->mData;*/

		//		StorageItemIterator lBegin = mItemList.createIterator(lChunk->mFirstItem);
		//		StorageItemIterator lInsertedItem = mItemList.insertAfter(lBegin, StorageItem(pPointer, pValue));

		//		mCount.fetch_add(1U, core_platform::bc_memory_order::seqcst);

		//		return &lInsertedItem->mData;
		//	}

		//	void remove(const void* pPointer)
		//	{
		//		bcUINT32 lPointer = reinterpret_cast<bcUINT32>(pPointer);
		//		bcUINT32 lKey = _convertPointerToKey(pPointer);
		//		StorageChunk* lChunk = _findChunk(lKey);

		//		StorageItemIterator lBegin = mItemList.createIterator(lChunk->mFirstItem);
		//		StorageItemIterator lEnd = mItemList.createIterator(lChunk->mLastItem);

		//		while (++lBegin != lEnd)
		//		{
		//			if (lBegin->mKey == pPointer)
		//				break;
		//		}

		//		if (lBegin != lEnd)
		//		{
		//			mItemList.erase(lBegin);
		//			mCount.fetch_sub(1U, core_platform::bc_memory_order::seqcst);
		//		}	
		//		else
		//			bcAssert(false);

		//		/*StorageItem* lItem = lChunk->mItems;
		//		StorageItem* lPrevItem = nullptr;
		//		while (lItem && lItem->mKey != pPointer)
		//		{
		//			lPrevItem = lItem;
		//			lItem = lItem->mNext;
		//		}

		//		if (lPrevItem && lItem)
		//		{
		//			lPrevItem->mNext = lItem->mNext;
		//		}
		//		else if (!lPrevItem && lItem)
		//		{
		//			lChunk->mItems = lItem->mNext;
		//		}
		//		else
		//		{
		//			bcAssert(false);
		//		}

		//		mItemAllocator.Free(lItem);*/
		//	}

		//	bcUINT32 count()
		//	{
		//		return mCount.load(core_platform::bc_memory_order::seqcst);
		//	}

		//	// Func(void**, T*) /
		//	template< typename Func >
		//	void iterateOver(Func pFunction)
		//	{
		//		_iterateOver(pFunction, mChunks, 0);
		//	}
		//};
#endif
	}
}