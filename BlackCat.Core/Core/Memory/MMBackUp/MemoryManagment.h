//  [8/30/2013 MRB]

#pragma once

#ifndef MemoryManagment_H
#define MemoryManagment_H

#include "CorePCH.h"
#include "Allocators.h"
#include "MemMngFreeList.h"
#include "Ptr.h"

namespace BlackCat
{
	namespace Core
	{
#ifdef BC_MEMORY_ENABLE

		/*class FixedSizeAllocator;
		class StackAllocator;
		class HeapAllocator;*/
		//template<typename T> class Ptr;
		//template<> class Ptr<void>;
		class bcMemMng;

#define KB 1024
#define MB 1048576

		enum bcHeap_Category
		{
			Heap_Unknown,
			Heap_MemoryManagment
		};

		enum bcAlloc_Type
		{
			Alloc_Unknown,
			Alloc_PRG,
			Alloc_LVL,
			Alloc_FRM
		};
		
		// == MemMng ================================================================================== MRB ==
#pragma region MemMng
		class BC_COREDLL_EXP bcMemMng
		{
		private:

		protected:
			// We don't use static pointer because it is possible after
			// we free pointer on exit, some call to free be called
			static bcMemMng mInstance;

			bcUINT32 mNumFSAllocators;
			bcUINT32 mFSAllocatorsStartSize;
			bcUINT32 mFSAllocatorsStartIndex;
			bcFixedSizeAllocator* mFSAllocators;
			StackAllocator mPerProgramStack;
			StackAllocator mPerLevelStack;
			StackAllocator mPerFrameStack;
			bcHeapAllocator mSuperHeap;
			bcCrtAllocator mCrtAllocator;

			bcInline bcUINT32 _FSAIndexMaxSize(bcUINT32 pIndex)
			{
				return 1 << (pIndex + mFSAllocatorsStartIndex);
			};
			bcInline bcUINT32 getFSAIndex(bcSIZE pSize)
			{
				bcUINT32 lIndex = 0;
				while(bcTRUE)
				{
					if(pSize <= _FSAIndexMaxSize(lIndex))
						return lIndex;
					++lIndex;
				}
			};
		public:
			bcMemMng();
			~bcMemMng();
			void initialize(bcUINT32 pNumFSAs, 
							bcUINT32 pFSAsStartSize, 
							bcUINT32 pFSAsNumAllocations,
							bcUINT32 pPerPrgHeapSize, 
							bcUINT32 pPerLvlHeapSize, 
							bcUINT32 pPerFrmHeapSize, 
							bcUINT32 pSuperHeapSize);					
			static void startUp(bcUINT32 pNumFSAs, 
								bcUINT32 pFSAsStartSize, 
								bcUINT32 pFSAsNumAllocations,
								bcUINT32 pPerPrgHeapSize, 
								bcUINT32 pPerLvlHeapSize, 
								bcUINT32 pPerFrmHeapSize, 
								bcUINT32 pSuperHeapSize);
			static void close();
			bcInline static bcMemMng* get()
			{
				return &bcMemMng::mInstance;
			};

			Ptr<void> alloc(bcSIZE pSize, bcAlloc_Type pAllocType, bcUINT32 pCategory, const bcCHAR* pFile, bcUINT32 pLine);
			void free(const Ptr<void>& pPointer);
			Ptr<void> reAlloc(Ptr<void>& pPointer, bcSIZE pNewSize, bcAlloc_Type pAllocType);
			Ptr<void> alignedAlloc(bcSIZE pSize, bcINT32 pAlignment, bcAlloc_Type pAllocType, bcUINT32 pCategory, const bcCHAR* pFile, bcUINT32 pLine);
			void alignedFree(const Ptr<void>& pPointer);
			Ptr<void> alignedReAlloc(Ptr<void>& pPointer, bcSIZE pNewSize, bcINT32 pAlignment, bcAlloc_Type pAllocType);

			template< typename T >
			Ptr< T > new_(const T&& pT, bcAlloc_Type pAllocType, bcINT32 pCategory, const bcCHAR* pFile, bcUINT32 pLine)
			{
				Ptr<T> lReturnPointer;
				lReturnPointer = alloc(sizeof(T), pAllocType, pCategory, pFile, pLine);
				new((void*)lReturnPointer->get()) T(pT);
				return lReturnPointer;
			};

			template< typename T >
			Ptr< T > newArray(const T&& pT, bcUINT32 pArrayLength, bcAlloc_Type pAllocType, bcINT32 pCategory, const bcCHAR* pFile, bcUINT32 pLine)
			{
				Ptr<T> lReturnPointer;
				// Alloc 4 byte more for array length, it will be
				// used in last 4 byte of allocated block
				bcUINT32 lSize = sizeof(T) * pArrayLength + sizeof(bcUINT32);
				lReturnPointer = alloc(lSize, pAllocType, pCategory, pFile, pLine);
				// Store array length
				MemBlock* lBlock = MemBlock::retrieveMemBlock(lReturnPointer->get())

				*(reinterpret_cast<bcUINT32*>((reinterpret_cast<bcSIZE>(lReturnPointer->get()) - lBlock->offset() + lBlock->size() - sizeof(bcUINT32)))) = pArrayLength;
				
				T* lFirst = lReturnPointer.get();
				for(register bcUINT32 i = 0; i < pArrayLength; ++i, ++lFirst)
					new(lFirst) T(pT);

				return lReturnPointer;
			};
			
			template< typename T >
			void delete_(const Ptr<T>& pPointer)
			{
				pPointer.get()->~T();
				free(pPointer);
			};
			
			template< typename T >
			void deleteArray(const Ptr<T>& pPointer)
			{
				T* lFirst = pPointer.get();
				// Retrieve array length
				MemBlock* lBlock = MemBlock::retrieveMemBlock(lFirst);
				bcUINT32 lCount = *(reinterpret_cast<bcUINT32*>(reinterpret_cast<bcSIZE>(pPointer.get()) - lBlock->offset() + lBlock->size() - sizeof(bcUINT32)));
				
				for(register bcUINT32 i = 0; i < lCount; ++i, ++lFirst)
					lFirst->~T();

				free(pPointer);
			};
			
			template< typename T >
			Ptr< T > alignedNew(const T&& pT, bcUINT32 pAlignment, bcAlloc_Type pAllocType, bcINT32 pCategory, const bcCHAR* pFile, bcUINT32 pLine)
			{
				Ptr<T> lReturnPointer;
				lReturnPointer = alignedAlloc(sizeof(T), pAlignment, pAllocType, pCategory, pFile, pLine);
				new(reinterpret_cast<void*>(lReturnPointer->get())) T(pT);
				return lReturnPointer;
			};
			
			template< typename T >
			Ptr< T > alignedNewArray(const T&& pT, bcUINT32 pAlignment, bcUINT32 pArrayLength, bcAlloc_Type pAllocType, bcINT32 pCategory, const bcCHAR* pFile, bcUINT32 pLine)
			{
				Ptr<T> lReturnPointer;
				// Alloc 4 byte more for array length, it will be
				// used in last 4 byte of allocated block
				bcUINT32 lSize = sizeof(T) * pArrayLength + sizeof(bcUINT32);
				lReturnPointer = alignedAlloc(lSize, pAlignment, pAllocType, pCategory, pFile, pLine);
				// Store array length
				MemBlock* lBlock = MemBlock::retrieveMemBlock(lReturnPointer->get());
				*(reinterpret_cast<bcUINT32*>((reinterpret_cast<bcSIZE>(lReturnPointer->get()) - lBlock->offset() + lBlock->size() - sizeof(bcUINT32)))) = pArrayLength;
				
				T* lFirst = lReturnPointer.get();
				for(register bcUINT32 i = 0; i < pArrayLength; ++i, ++lFirst)
					new(lFirst) T(pT);

				return lReturnPointer;
			};
			
			template< typename T >
			void alignedDelete(const Ptr<T>& pPointer)
			{
				pPointer.get()->~T();
				alignedFree(pPointer);
			};
			
			template< typename T >
			void alignedDeleteArray(const Ptr<T>& pPointer)
			{
				T* lFirst = pPointer.get();
				// Retrieve array length
				MemBlock* lBlock = MemBlock::retrieveMemBlock(lFirst);
				bcUINT32 lCount = *(reinterpret_cast<bcUINT32*>(reinterpret_cast<bcSIZE>(pPointer.get()) - lBlock->offset() + lBlock->size() - sizeof(bcUINT32)));

				for(register bcUINT32 i = 0; i < lCount; ++i, ++lFirst)
					lFirst->~T();

				alignedFree(pPointer);
			};

			template< typename T >
			void registerPointerInDynamicAllocators(const Ptr<T>& pPointer)
			{
				mSuperHeap.registerPointer(pPointer.get());
			};

			template< typename T >
			void unRegisterPointerInDynamicAllocators(const Ptr<T>& pPointer)
			{
				mSuperHeap.unRegisterPointer(pPointer.get());
			};

			void endOfFrame();

			//bcSIZE GetUsedSizeByCategory(bcUINT32 pCategory);

			bcSIZE getTotalSize();
			bcSIZE getUsedSize();
			bcSIZE getOverheadSize();
			bcSIZE getMaxUsedSize();

			//bcUINT32 ReportMemoryLeaks();
		};

//#define P(T) Ptr<T>
//#define RP(T) T*
//
//#define bcAlloc(pSize, pAllocType, pCategory)									bcMemMng::get()->alloc(pSize, pAllocType, pCategory, __FILE__, __LINE__)
//#define bcFree(pPointer)														/*if(pPointer)*/ { bcMemMng::get()->free(pPointer); pPointer = bcNULL; }
//#define bcReAlloc(pPointer, pNewSize, pAllocType)								bcMemMng::get()->reAlloc(pPointer, pNewSize, pAllocType)
//#define bcAlignedAlloc(pSize, pAlignment, pAllocType, pCategory)				bcMemMng::get()->alignedAlloc(pSize, pAlignment, pAllocType, pCategory, __FILE__, __LINE__)
//#define bcAlignedFree(pPointer)													/*if(pPointer)*/ { bcMemMng::get()->alignedFree(pPointer); pPointer = bcNULL; }
//#define bcAlignedReAlloc(pPointer, pNewSize, pAlignment, pAllocType)			bcMemMng::get()->alignedReAlloc(pPointer, pNewSize, pAlignment, pAllocType)
//
//#define bcNew(pT, pAllocType, pCategory)										bcMemMng::get()->new_(pT, pAllocType, pCategory, __FILE__, __LINE__)
//#define bcNewArray(pT, pArrayLenth, pAllocType, pCategory)						bcMemMng::get()->newArray(pT, pArrayLenth, pAllocType, pCategory, __FILE__, __LINE__)
//#define bcDelete(pT)															/*if(pT)*/ { bcMemMng::get()->delete_(pT); pT = bcNULL; }
//#define bcDeleteArray(pT)														/*if(pT)*/ { bcMemMng::get()->deleteArray(pT); pT = bcNULL; }
//#define bcAlignedNew(pT, pAlignment, pAllocType, pCategory)						bcMemMng::get()->alignedNew(pT, pAlignment, pAllocType, pCategory, __FILE__, __LINE__)
//#define bcAlignedNewArray(pT, pAlignment, pArrayLenth, pAllocType, pCategory)	bcMemMng::get()->alignedNewArray(pT, pAlignment, pArrayLenth, pAllocType, pCategory, __FILE__, __LINE__)
//#define bcAlignedDelete(pT)														/*if(pT)*/ { bcMemMng::get()->alignedDelete(pT); pT = bcNULL; }
//#define bcAlignedDeleteArray(pT)												/*if(pT)*/ { bcMemMng::get()->alignedDeleteArray(pT); pT = bcNULL; }
//
//#else
//
//#define P(T) T*
//#define RP(T) T*
//
//#define bcAlloc(pSize, pAllocType, pCategory)						malloc(pSize)
//#define bcFree(pMemory)												if(pMemory) { free(pMemory); pMemory = NULL; }
//#define bcReAlloc(pMemory, pNewSize, pAllocType)					realloc(pMemory, pNewSize)
//#define bcAlignedAlloc(pSize, pAlignment, pAllocType, pCategory)	_aligned_malloc(pSize, pAlignment)
//#define bcAlignedFree(pMemory)										_aligned_free(pMemory)
//#define bcAlignedReAlloc(pMemory, pNewSize, pAlignment, pAllocType)	_aligned_realloc(pMemory, pNewSize, pAlignment)
//
//#define bcNew(pT, pAllocType, pCategory)							new pT
//#define bcNewArray(pT, pArrayLenth, pAllocType, pCategory)			new pT[pArrayLenth]
//#define bcDelete(pT)												if(pMemory) { delete(pT); pMemory = NULL; }
//#define bcDeleteArray(pT)											if(pMemory) { delete[](pT); pMemory = NULL; }

#endif

#pragma endregion

	}
}

//void* operator new(bcSIZE pSize, BlackCat::Core::MemBlock** pOutMemBlock, bcUINT32 pAllocType, bcUINT32 pCategory, const bcCHAR* pFile, bcUINT32 pLine);
////void* operator new(bcSIZE pSize, void* pPointerToReturn);
//void* operator new[](bcSIZE pSize, BlackCat::Core::MemBlock** pOutMemBlock, bcUINT32 pAllocType, bcUINT32 pCategory, const bcCHAR* pFile, bcUINT32 pLine);

#endif