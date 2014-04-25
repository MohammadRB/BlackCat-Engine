//  [9/2/2013 MRB]

#include "CorePCH.h"
#include "MemoryManagment.h"
//#include "Utility.h"
#include "CoreMathUtility.h"
#include "CoreExceptions.h"

//void* operator new(bcSIZE pSize, BlackCat::Core::MemBlock** pOutMemBlock, bcUINT32 pAllocType, bcUINT32 pCategory, const bcCHAR* pFile, bcUINT32 pLine)
//{
//	BlackCat::Core::MemBlock* lMemBlock = BlackCat::Core::MemMng::Get()->_Alloc(pSize, (BlackCat::Core::Alloc_Type)pAllocType, pCategory, pFile, pLine).mBlock;
//	*pOutMemBlock = lMemBlock;
//	return (void*)lMemBlock->mDataAddress;
//	//return malloc(pSize);
//};
//// This Function will used instead of New[]. for array we
//// allocate a large enough block and call this function for
//// each array item. this function job is to call objects
//// Constructures
////void* operator new(bcSIZE pSize, void* pPointerToReturn)
////{
////	return pPointerToReturn;
////};
//void* operator new[](bcSIZE pSize, BlackCat::Core::MemBlock** pOutMemBlock, bcUINT32 pAllocType, bcUINT32 pCategory, const bcCHAR* pFile, bcUINT32 pLine)
//{
//	BlackCat::Core::MemBlock* lMemBlock = BlackCat::Core::MemMng::Get()->_Alloc(pSize, (BlackCat::Core::Alloc_Type)pAllocType, pCategory, pFile, pLine).mBlock;
//	*pOutMemBlock = lMemBlock;
//	return (void*)lMemBlock->mDataAddress;
//	//return malloc(pSize);
//};

void* operator new(bcSIZE pSize, BlackCat::Core::bcAlloc_Type pAllocType, bcUINT32 pCategory, const bcCHAR* pFile, bcUINT32 pLine)
{
	return BlackCat::Core::bcMemMng::get()->alloc(pSize, pAllocType, pCategory, pFile, pLine);
};
BC_COREDLL_EXP void* operator new[](bcSIZE pSize, BlackCat::Core::bcAlloc_Type pAllocType, bcUINT32 pCategory, const bcCHAR* pFile, bcUINT32 pLine)
{
	return BlackCat::Core::bcMemMng::get()->alloc(pSize, pAllocType, pCategory, pFile, pLine);
}
BC_COREDLL_EXP void* operator new(bcSIZE pSize, bcSIZE pAlignment, BlackCat::Core::bcAlloc_Type pAllocType, bcUINT32 pCategory, const bcCHAR* pFile, bcUINT32 pLine)
{
	return BlackCat::Core::bcMemMng::get()->alignedAlloc(pSize, pAlignment, pAllocType, pCategory, pFile, pLine);
}
BC_COREDLL_EXP void* operator new[](bcSIZE pSize, bcSIZE pAlignment, BlackCat::Core::bcAlloc_Type pAllocType, bcUINT32 pCategory, const bcCHAR* pFile, bcUINT32 pLine)
{
	return BlackCat::Core::bcMemMng::get()->alignedAlloc(pSize, pAlignment, pAllocType, pCategory, pFile, pLine);
}

namespace BlackCat
{
	namespace Core
	{
#ifdef BC_MEMORY_ENABLE

		

		bcMemMng bcMemMng::mInstance;

		bcMemMng::bcMemMng()
		{};

		bcMemMng::~bcMemMng()
		{
			
		};

		//template< typename T >
		/*Ptr< void > MemMng::_New(bcSIZE pSize, Alloc_Type pAllocType, bcINT32 pCategory, const bcCHAR* pFile, bcUINT32 pLine)
		{
			MemBlock* lMemBlock;
			void* lP = (new(&lMemBlock, pAllocType, pCategory, pFile, pLine) FixedSizeAllocator());
			return lMemBlock;
		};*/
								
		void bcMemMng::initialize(bcUINT32 pNumSystemThread,
								bcUINT32 pNumFSAs, 
								bcUINT32 pFSAsStartSize, 
								bcUINT32 pFSAsNumAllocations,
								bcUINT32 pPerProgHeapSize, 
								bcUINT32 pPerLvlHeapSize, 
								bcUINT32 pPerFrmHeapSize, 
								bcUINT32 pSuperHeapSize)
		{
			// Look at bcThreadManager
			bcUINT32 lNumThread = pNumSystemThread == 1 ? 2 : pNumSystemThread;

			mNumFSAllocators = pNumFSAs;
			mFSAllocatorsStartSize = pFSAsStartSize;
			mFSAllocatorsStartIndex = Log2(pFSAsStartSize);
			mFSAllocators = new bcFixedSizeAllocator[mNumFSAllocators];
			
			for(bcUINT32 i = 0; i < pNumFSAs; i++)
			{
				bcUINT32 lBlockSize = pow(2.0f, (int)(mFSAllocatorsStartIndex + i));
				mFSAllocators[i].initialize(pFSAsNumAllocations, lBlockSize, (std::string("FSA") + std::to_string(i)).c_str());
			}
			
			mPerProgramStack = new StackAllocator();
			mPerProgramStack->initialize(pPerProgHeapSize, "PerProgramStack");
			mPerLevelStack = new StackAllocator();
			mPerLevelStack->initialize(pPerLvlHeapSize, "PerLevelStack");
			mPerFrameStack = new StackAllocator();
			mPerFrameStack->initialize(pPerFrmHeapSize, "PerFrameStack");

			mSuperHeap = new bcHeapAllocator(lNumThread);
			mSuperHeap->initialize(pSuperHeapSize, "SuperHeap");

			mCrtAllocator = new bcCrtAllocator();

#ifdef BC_MEMORY_LEAK_DETECTION
			mLeakAllocator = new bcMemMngHashMap<MemBlockLeakInformation, 20, 10>(lNumThread);
#endif
		};
		void bcMemMng::destroy()
		{
			SAFE_DELETE_ARRAY(mFSAllocators);
			SAFE_DELETE(mPerProgramStack);
			SAFE_DELETE(mPerLevelStack);
			SAFE_DELETE(mPerFrameStack);
			SAFE_DELETE(mSuperHeap);
			SAFE_DELETE(mCrtAllocator);
#ifdef BC_MEMORY_LEAK_DETECTION
			SAFE_DELETE(mLeakAllocator);
#endif
		}
		void bcMemMng::startUp(	bcUINT32 pNumSystemThread,
								bcUINT32 pNumFSAs, 
								bcUINT32 pFSAsStartSize, 
								bcUINT32 pFSAsNumAllocations,
								bcUINT32 pPerPrgHeapSize, 
								bcUINT32 pPerLvlHeapSize, 
								bcUINT32 pPerFrmHeapSize, 
								bcUINT32 pSuperHeapSize)
		{		
						
			bcMemMng::mInstance.initialize(	pNumSystemThread,
											pNumFSAs,
											pFSAsStartSize,
											pFSAsNumAllocations,
											pPerPrgHeapSize,
											pPerLvlHeapSize,
											pPerFrmHeapSize,
											pSuperHeapSize);
		};
		
		void bcMemMng::close()
		{
			//MemMng::mInstance.~MemMng();
			bcMemMng::mInstance.destroy();
		};
		
		void* bcMemMng::alloc(bcSIZE pSize, bcAlloc_Type pAllocType, bcUINT32 pCategory, const bcCHAR* pFile, bcUINT32 pLine)
		{
			void* lResult = bcNULL;
			bcAllocator* lAllocator = bcNULL;
			MemBlock lBlock; 
			MemBlock::initializeMemBlockForAlllocation(pSize, BC_MEMORY_MIN_ALIGN, &lBlock);
			bcSIZE lSize = lBlock.size();

			switch(pAllocType)
			{
case Alloc_FRM:
				lResult = mPerFrameStack->alloc(&lBlock);
				lAllocator = mPerFrameStack;
				if(!lResult)
				{
					lResult = mSuperHeap->alloc(&lBlock);
					lAllocator = mSuperHeap;
				}
				break;
case Alloc_Unknown:
				// Check can Request handled by FSAllocators
				if(lSize <= _FSAIndexMaxSize(mNumFSAllocators - 1))
				{
					lResult = (mFSAllocators + getFSAIndex(lSize))->alloc(&lBlock);
					lAllocator = (mFSAllocators + getFSAIndex(lSize));
					if(!lResult)
					{
						lResult = mSuperHeap->alloc(&lBlock);
						lAllocator = mSuperHeap;
					}
				}
				else
				{
					lResult = mSuperHeap->alloc(&lBlock);
					lAllocator = mSuperHeap;
				}
				break;
case Alloc_LVL:
				lResult = mPerLevelStack->alloc(&lBlock);
				lAllocator = mPerLevelStack;
				if(!lResult)
				{
					lResult = mSuperHeap->alloc(&lBlock);
					lAllocator = mSuperHeap;
				}
				break;
case Alloc_PRG:
				lResult = mPerProgramStack->alloc(&lBlock);
				lAllocator = mPerProgramStack;
				if(!lResult)
				{
					lResult = mSuperHeap->alloc(&lBlock);
					lAllocator = mSuperHeap;
				}
				break;
default:
				lResult = mCrtAllocator->alloc(&lBlock);
				lAllocator = mCrtAllocator;
			};

			if(!lResult)
			{
				lResult = mCrtAllocator->alloc(&lBlock);
				lAllocator = mCrtAllocator;
			}

			MemBlock::inititalizeMemBlockAfterAllocation(&lResult, (lAllocator == mSuperHeap), lAllocator, &lBlock);

#ifdef BC_MEMORY_LEAK_DETECTION
			mLeakAllocator->insert(lResult, MemBlockLeakInformation(pLine, pFile));
#endif

			return (lResult);
		};
		
		void bcMemMng::free(const void* pPointer)
		{
			const void* lPointer = pPointer;
			MemBlock* lBlock = MemBlock::retrieveMemBlock(lPointer);

			bcAssert(lBlock);

			reinterpret_cast<bcAllocator*>(lBlock->extra())->free(lPointer, lBlock);

			MemBlock::freeMemBlock(lPointer);

#ifdef BC_MEMORY_LEAK_DETECTION
			mLeakAllocator->remove(pPointer);
#endif

			//// First check per frame stack
			//if(mPerFrameStack.containPointer(lPointer))
			//{
			//	mPerFrameStack.free(lPointer, lBlock);
			//	return;
			//}
			//// Then FSAllocators
			//// Check if size is smaller than max FSA
			//if(lBlock->size() <= FSAIndexMaxSize(mNumFSAllocators - 1))
			//{
			//	bcFixedSizeAllocator* lFSA = (mFSAllocators + GetFSAIndex(lBlock->size()));
			//	if(lFSA->containPointer(lPointer))
			//	{
			//		lFSA->free(lPointer, lBlock);
			//		return;
			//	}
			//}
			//// If pMemory is not for FSAllocators check SuperHeap
			//if(mSuperHeap.containPointer(lPointer))
			//{
			//	mSuperHeap.free(lPointer, lBlock);
			//	return;
			//}
			//// If pMemory is not belong to any previous allocators check perLevel and perProgram stacks
			//if(mPerLevelStack.containPointer(lPointer))
			//{
			//	mPerLevelStack.free(lPointer, lBlock);
			//	return;
			//}
			//if(mPerProgramStack.containPointer(lPointer))
			//{
			//	mPerProgramStack.free(lPointer, lBlock);
			//	return;
			//}
			// 
			//// == If non of them then use crt allocator == MRB ==
			//mCrtAllocator.free(lPointer, lBlock);
		};
		
		void* bcMemMng::reAlloc(const void* pPointer, bcSIZE pNewSize, bcAlloc_Type pAllocType, bcUINT32 pCategory, const bcCHAR* pFile, bcUINT32 pLine)
		{
			MemBlock* lBlock = MemBlock::retrieveMemBlock(pPointer);
			void* lNewPointer;
			MemBlock* lNewBlock;

			lNewPointer = alloc(pNewSize, pAllocType, 0, pFile, pLine);

			lNewBlock = MemBlock::retrieveMemBlock(lNewPointer);

			bcSIZE lMinSize = std::min<bcSIZE>(lBlock->size() - lBlock->offset(), lNewBlock->size() - lNewBlock->offset());

			std::memcpy(
				reinterpret_cast<void*>(reinterpret_cast<bcSIZE>(lNewPointer)), 
				reinterpret_cast<void*>(reinterpret_cast<bcSIZE>(pPointer)),
				lMinSize);

			free(pPointer);

			return lNewPointer;
		};
		
		void* bcMemMng::alignedAlloc(bcSIZE pSize, bcINT32 pAlignment, bcAlloc_Type pAllocType, bcUINT32 pCategory, const bcCHAR* pFile, bcUINT32 pLine)
		{
			bcAssert(pAlignment >= 16);

			void* lResult = bcNULL;
			bcAllocator* lAllocator = bcNULL;
			MemBlock lBlock; 
			MemBlock::initializeMemBlockForAlllocation(pSize, pAlignment, &lBlock);
			bcSIZE lSize = lBlock.size();

			switch(pAllocType)
			{
			case Alloc_FRM:
				lResult = mPerFrameStack->alloc(&lBlock);
				lAllocator = mPerFrameStack;
				if(!lResult)
				{
					lResult = mSuperHeap->alloc(&lBlock);
					lAllocator = mSuperHeap;
				}
				break;
			case Alloc_Unknown:
				// Check can Request handled by FSAllocators
				if(lSize <= _FSAIndexMaxSize(mNumFSAllocators - 1))
				{
					lResult = (mFSAllocators + getFSAIndex(lSize))->alloc(&lBlock);
					lAllocator = (mFSAllocators + getFSAIndex(lSize));
					if(!lResult)
					{
						lResult = mSuperHeap->alloc(&lBlock);
						lAllocator = mSuperHeap;
					}
				}
				else
				{
					lResult = mSuperHeap->alloc(&lBlock);
					lAllocator = mSuperHeap;
				}
				break;
			case Alloc_LVL:
				lResult = mPerLevelStack->alloc(&lBlock);
				lAllocator = mPerLevelStack;
				if(!lResult)
				{
					lResult = mSuperHeap->alloc(&lBlock);
					lAllocator = mSuperHeap;
				}
				break;
			case Alloc_PRG:
				lResult = mPerProgramStack->alloc(&lBlock);
				lAllocator = mPerProgramStack;
				if(!lResult)
				{
					lResult = mSuperHeap->alloc(&lBlock);
					lAllocator = mSuperHeap;
				}
				break;
			default:
				lResult = mCrtAllocator->alloc(&lBlock);
				lAllocator = mCrtAllocator;
			};

			if(!lResult)
			{
				lResult = mCrtAllocator->alloc(&lBlock);
				lAllocator = mCrtAllocator;
			}

			MemBlock::inititalizeMemBlockAfterAllocation(&lResult, (lAllocator == mSuperHeap), lAllocator, &lBlock);

#ifdef BC_MEMORY_LEAK_DETECTION
			mLeakAllocator->insert(lResult, MemBlockLeakInformation(pLine, pFile));
#endif

			return (lResult);
		};
		
		void bcMemMng::alignedFree(const void* pPointer)
		{
			const void* lPointer = pPointer;
			MemBlock* lBlock = MemBlock::retrieveMemBlock(lPointer);

			bcAssert(lBlock);
			bcAssert(lBlock->alignment() >= 16);

			reinterpret_cast<bcAllocator*>(lBlock->extra())->free(lPointer, lBlock);

			MemBlock::freeMemBlock(lPointer);

#ifdef BC_MEMORY_LEAK_DETECTION
			mLeakAllocator->remove(pPointer);
#endif

			//// First check per frame stack
			//if(mPerFrameStack.containPointer(lPointer))
			//{
			//	mPerFrameStack.free(lPointer, lBlock);
			//	return;
			//}
			//// Then FSAllocators
			//// Check if size is smaller than max FSA
			//if(lBlock->size() <= FSAIndexMaxSize(mNumFSAllocators - 1))
			//{
			//	bcFixedSizeAllocator* lFSA = (mFSAllocators + GetFSAIndex(lBlock->size()));
			//	if(lFSA->containPointer(lPointer))
			//	{
			//		lFSA->free(lPointer, lBlock);
			//		return;
			//	}
			//}
			//// If pMemory is not for FSAllocators check SuperHeap
			//if(mSuperHeap.containPointer(lPointer))
			//{
			//	mSuperHeap.free(lPointer, lBlock);
			//	return;
			//}
			//// If pMemory is not belong to any previous allocators check perLevel and perProgram stacks
			//if(mPerLevelStack.containPointer(lPointer))
			//{
			//	mPerLevelStack.free(lPointer, lBlock);
			//	return;
			//}
			//if(mPerProgramStack.containPointer(lPointer))
			//{
			//	mPerProgramStack.free(lPointer, lBlock);
			//	return;
			//}
			// 
			//// == If non of them then use crt allocator == MRB ==
			//mCrtAllocator.free(lPointer, lBlock);
		};
		
		void* bcMemMng::alignedReAlloc(const void* pPointer, bcSIZE pNewSize, bcINT32 pAlignment, bcAlloc_Type pAllocType, bcUINT32 pCategory, const bcCHAR* pFile, bcUINT32 pLine)
		{
			MemBlock* lBlock = MemBlock::retrieveMemBlock(pPointer);
			void* lNewPointer;
			MemBlock* lNewBlock;

			lNewPointer = alignedAlloc(pNewSize, pAlignment, pAllocType, 0, pFile, pLine);

			lNewBlock = MemBlock::retrieveMemBlock(lNewPointer);

			bcSIZE lMinSize = std::min<bcSIZE>(lBlock->size() - lBlock->offset(), lNewBlock->size() - lNewBlock->offset());

			std::memcpy(
				reinterpret_cast<void*>(reinterpret_cast<bcSIZE>(lNewPointer)), 
				reinterpret_cast<void*>(reinterpret_cast<bcSIZE>(pPointer)),
				lMinSize);

			free(pPointer);

			return lNewPointer;
		};

		void bcMemMng::endOfFrame()
		{
#ifdef BC_MEMORY_DEFRAG
			bcUINT32 lNumDef = mSuperHeap.numFragmentation();
			lNumDef = (lNumDef == 0)? 0: lNumDef / 3 + 1;
			mSuperHeap.defragment(lNumDef);
#endif

			mPerFrameStack->clear();
		};

		/*bcSIZE MemMng::GetUsedSizeByCategory(bcUINT32 pCategory)
		{
#ifdef BC_MEMORY_TRACING
			bcSIZE lCatSize = 0;
			MemMngFreeList<MemBlock>::MemMngFreeListItem* lMemBlock = mAllocations.First();
			for(bcUINT32 i = 0; i < mAllocations.Count(); i++)
			{
				if(lMemBlock->mData.mCategory == pCategory)
					lCatSize += lMemBlock->mData.mSize;
				lMemBlock = lMemBlock->mNext;
			}
			return lCatSize;
#endif
			return 0;
		};*/

		bcSIZE bcMemMng::getTotalSize()
		{
#ifdef BC_MEMORY_TRACING
			bcSIZE lTotalSize = 0;

			for(bcUINT32 i = 0; i < mNumFSAllocators; i++)
				lTotalSize += mFSAllocators[i].tracer().totalSize();

			lTotalSize += mPerProgramStack->tracer().totalSize();
			lTotalSize += mPerLevelStack->tracer().totalSize();
			lTotalSize += mPerFrameStack->tracer().totalSize();
			lTotalSize += mSuperHeap->tracer().totalSize();

			return lTotalSize;
#endif
			return 0;
		};
		
		bcSIZE bcMemMng::getUsedSize()
		{
#ifdef BC_MEMORY_TRACING
			bcSIZE lUsedSize = 0;

			for(bcUINT32 i = 0; i < mNumFSAllocators; i++)
				lUsedSize += mFSAllocators[i].tracer().usedSize();

			lUsedSize += mPerProgramStack->tracer().usedSize();
			lUsedSize += mPerLevelStack->tracer().usedSize();
			lUsedSize += mPerFrameStack->tracer().usedSize();
			lUsedSize += mSuperHeap->tracer().usedSize();

			return lUsedSize;
#endif
			return 0;
		};
		
		bcSIZE bcMemMng::getOverheadSize()
		{
#ifdef BC_MEMORY_TRACING
			bcSIZE WastedSize = 0;

			for(bcUINT32 i = 0; i < mNumFSAllocators; i++)
				WastedSize += mFSAllocators[i].tracer().overheadSize();

			WastedSize += mPerProgramStack->tracer().overheadSize();
			WastedSize += mPerLevelStack->tracer().overheadSize();
			WastedSize += mPerFrameStack->tracer().overheadSize();
			WastedSize += mSuperHeap->tracer().overheadSize();

			return WastedSize;
#endif
			return 0;
		};
		
		bcSIZE bcMemMng::getMaxUsedSize()
		{
#ifdef BC_MEMORY_TRACING
			bcSIZE MaxUsedSize = 0;

			for(bcUINT32 i = 0; i < mNumFSAllocators; i++)
				MaxUsedSize += mFSAllocators[i].tracer().maxUsedSize();

			MaxUsedSize += mPerProgramStack->tracer().maxUsedSize();
			MaxUsedSize += mPerLevelStack->tracer().maxUsedSize();
			MaxUsedSize += mPerFrameStack->tracer().maxUsedSize();
			MaxUsedSize += mSuperHeap->tracer().maxUsedSize();

			return MaxUsedSize;
#endif
			return 0;
		};

#ifdef BC_MEMORY_LEAK_DETECTION
		bcUINT32 bcMemMng::reportMemoryLeaks()
		{
			bcSIZE lLeakCount = mLeakAllocator->count();
			
			mLeakAllocator->iterateOver([](void** pKey, MemBlockLeakInformation* pValue)->void
				{
					
				});

			return lLeakCount;
		};
#endif

#endif
	}
}