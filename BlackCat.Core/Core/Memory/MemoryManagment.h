//  [8/30/2013 MRB]

#pragma once

#ifndef MemoryManagment_H
#define MemoryManagment_H

#include "CorePCH.h"
#include "Allocators.h"
#include "bcMemMngFreeList.h"
#include "bcMemMngHashMap.h"
//#include "Ptr.h"

namespace BlackCat
{
	namespace Core
	{
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

#ifdef BC_MEMORY_ENABLE

		/*class FixedSizeAllocator;
		class StackAllocator;
		class HeapAllocator;*/
		//template<typename T> class Ptr;
		//template<> class Ptr<void>;
		class bcMemMng;

#pragma region bcPtrBase

		template< typename T >
		class bcPtrBase
		{
		private:

		protected:

			bcInline bcBOOL _isOnStack(void* pPointer) const
			{
				return bcCoreOS::isPointerOnStack(pPointer);
			}

#ifdef BC_MEMORY_ENABLE

			bcInline MemBlock* _getMemBlock(void* pPointer) const
			{
				return MemBlock::retrieveMemBlock(pPointer);
			}

			bcInline void _baseIncReferenceCount(void* pPointer)
			{
				MemBlock* lMemBlock = _getMemBlock(pPointer);
				if (lMemBlock != bcNULL)
					lMemBlock->refrenceCount(lMemBlock->refrenceCount(bcMemoryOrder_Relaxed) + 1, bcMemoryOrder_Relaxed);
			}

			bcInline bcBOOL _baseDecReferenceCount(void* pPointer)
			{
				MemBlock* lMemBlock = _getMemBlock(pPointer);
				if (lMemBlock != bcNULL)
				{
					lMemBlock->refrenceCount(lMemBlock->refrenceCount(bcMemoryOrder_Relaxed) - 1, bcMemoryOrder_Relaxed);

					if (lMemBlock->refrenceCount() > 0)
						return bcTRUE;
					else
						return bcFALSE;
				}

				return bcTRUE;
			}

#endif
			bcInline void _baseConstruct(void* pPointer)
			{
#ifdef BC_MEMORY_ENABLE
#ifdef BC_MEMORY_DEFRAG
				MemBlock* lMemBlock = _getMemBlock(pPointer);
				if(lMemBlock != bcNULL)
				{
					if(lMemBlock->dynamicPointer())
					{
						// TODO : directly call this method without retrieving MemBlock, instead
						// do checking in MemMng and remove mOffset from bcPtr
						bcMemMng::get()->registerPointerInDynamicAllocators(this);
					}
				}
#endif
#endif
			}
			bcInline void _baseDeconstruct(void* pPointer)
			{
#ifdef BC_MEMORY_ENABLE
#ifdef BC_MEMORY_DEFRAG
				MemBlock* lMemBlock = _getMemBlock(pPointer);
				if(lMemBlock != bcNULL)
				{
					if(lMemBlock->dynamicPointer())
					{
						bcMemMng::get()->unRegisterPointerInDynamicAllocators(this);
					}
				}
#endif
#endif
			}

		public:

			bcPtrBase() {}
			~bcPtrBase() {}

			/*bcInline virtual operator T*() const abstract;
			bcInline virtual bcBOOL operator !() const abstract;*/
		};

#pragma endregion

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
			StackAllocator* mPerProgramStack;
			StackAllocator* mPerLevelStack;
			StackAllocator* mPerFrameStack;
			bcHeapAllocator* mSuperHeap;
			bcCrtAllocator* mCrtAllocator;
#ifdef BC_MEMORY_LEAK_DETECTION
			bcMemMngHashMap<MemBlockLeakInformation, 20, 10>* mLeakAllocator;
#endif


			bcInline bcUINT32 _FSAIndexMaxSize(bcUINT32 pIndex)
			{
				return 1 << (pIndex + mFSAllocatorsStartIndex);
			};
			bcInline bcUINT32 getFSAIndex(bcSIZE pSize)
			{
				bcUINT32 lIndex = 0;
				while (bcTRUE)
				{
					if (pSize <= _FSAIndexMaxSize(lIndex))
						return lIndex;
					++lIndex;
				}
			};
		public:
			bcMemMng();
			~bcMemMng();
			void initialize(bcUINT32 pNumSystemThread,
				bcUINT32 pNumFSAs,
				bcUINT32 pFSAsStartSize,
				bcUINT32 pFSAsNumAllocations,
				bcUINT32 pPerPrgHeapSize,
				bcUINT32 pPerLvlHeapSize,
				bcUINT32 pPerFrmHeapSize,
				bcUINT32 pSuperHeapSize);
			void destroy();
			static void startUp(bcUINT32 pNumSystemThread,
				bcUINT32 pNumFSAs,
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

			void* alloc(bcSIZE pSize, bcAlloc_Type pAllocType, bcUINT32 pCategory, const bcCHAR* pFile, bcUINT32 pLine);
			void free(const void* pPointer);
			void* reAlloc(const void* pPointer, bcSIZE pNewSize, bcAlloc_Type pAllocType, bcUINT32 pCategory, const bcCHAR* pFile, bcUINT32 pLine);
			void* alignedAlloc(bcSIZE pSize, bcINT32 pAlignment, bcAlloc_Type pAllocType, bcUINT32 pCategory, const bcCHAR* pFile, bcUINT32 pLine);
			void alignedFree(const void* pPointer);
			void* alignedReAlloc(const void* pPointer, bcSIZE pNewSize, bcINT32 pAlignment, bcAlloc_Type pAllocType, bcUINT32 pCategory, const bcCHAR* pFile, bcUINT32 pLine);

			template< typename T >
			T* new_(const T&& pT, bcAlloc_Type pAllocType, bcINT32 pCategory, const bcCHAR* pFile, bcUINT32 pLine)
			{
				T* lReturnPointer;
				lReturnPointer = reinterpret_cast<T*>(alloc(sizeof(T), pAllocType, pCategory, pFile, pLine));
				new(lReturnPointer)T(pT);
				return lReturnPointer;
			};

			template< typename T >
			T* newArray(const T&& pT, bcUINT32 pArrayLength, bcAlloc_Type pAllocType, bcINT32 pCategory, const bcCHAR* pFile, bcUINT32 pLine)
			{
				T* lReturnPointer;
				// Alloc 4 byte more for array length, it will be
				// used in last 4 byte of allocated block
				bcUINT32 lSize = sizeof(T)* pArrayLength + sizeof(bcUINT32);
				lReturnPointer = reinterpret_cast<T*>(alloc(lSize, pAllocType, pCategory, pFile, pLine));
				// Store array length
				MemBlock* lBlock = MemBlock::retrieveMemBlock(lReturnPointer)

					*(reinterpret_cast<bcUINT32*>((reinterpret_cast<bcSIZE>(lReturnPointer)-lBlock->offset() + lBlock->size() - sizeof(bcUINT32)))) = pArrayLength;

				T* lFirst = lReturnPointer;
				for (register bcUINT32 i = 0; i < pArrayLength; ++i, ++lFirst)
					new(lFirst)T(pT);

				return lReturnPointer;
			};

			template< typename T >
			void delete_(const T* pPointer)
			{
				pPointer->~T();
				free(pPointer);
			};

			template< typename T >
			void deleteArray(const T* pPointer)
			{
				T* lFirst = pPointer;
				// Retrieve array length
				MemBlock* lBlock = MemBlock::retrieveMemBlock(lFirst);
				bcUINT32 lCount = *(reinterpret_cast<bcUINT32*>(reinterpret_cast<bcSIZE>(pPointer)-lBlock->offset() + lBlock->size() - sizeof(bcUINT32)));

				for (register bcUINT32 i = 0; i < lCount; ++i, ++lFirst)
					lFirst->~T();

				free(pPointer);
			};

			template< typename T >
			T* alignedNew(const T&& pT, bcUINT32 pAlignment, bcAlloc_Type pAllocType, bcINT32 pCategory, const bcCHAR* pFile, bcUINT32 pLine)
			{
				T* lReturnPointer;
				lReturnPointer = reinterpret_cast<T*>(alignedAlloc(sizeof(T), pAlignment, pAllocType, pCategory, pFile, pLine));
				new(lReturnPointer)T(pT);
				return lReturnPointer;
			};

			template< typename T >
			T* alignedNewArray(const T&& pT, bcUINT32 pAlignment, bcUINT32 pArrayLength, bcAlloc_Type pAllocType, bcINT32 pCategory, const bcCHAR* pFile, bcUINT32 pLine)
			{
				T* lReturnPointer;
				// Alloc 4 byte more for array length, it will be
				// used in last 4 byte of allocated block
				bcUINT32 lSize = sizeof(T)* pArrayLength + sizeof(bcUINT32);
				lReturnPointer = reinterpret_cast<T*>(alignedAlloc(lSize, pAlignment, pAllocType, pCategory, pFile, pLine));
				// Store array length
				MemBlock* lBlock = MemBlock::retrieveMemBlock(lReturnPointer);
				*(reinterpret_cast<bcUINT32*>((reinterpret_cast<bcSIZE>(lReturnPointer)-lBlock->offset() + lBlock->size() - sizeof(bcUINT32)))) = pArrayLength;

				T* lFirst = lReturnPointer;
				for (register bcUINT32 i = 0; i < pArrayLength; ++i, ++lFirst)
					new(lFirst)T(pT);

				return lReturnPointer;
			};

			template< typename T >
			void alignedDelete(const T* pPointer)
			{
				pPointer->~T();
				alignedFree(pPointer);
			};

			template< typename T >
			void alignedDeleteArray(const T* pPointer)
			{
				T* lFirst = pPointer;
				// Retrieve array length
				MemBlock* lBlock = MemBlock::retrieveMemBlock(lFirst);
				bcUINT32 lCount = *(reinterpret_cast<bcUINT32*>(reinterpret_cast<bcSIZE>(pPointer)-lBlock->offset() + lBlock->size() - sizeof(bcUINT32)));

				for (register bcUINT32 i = 0; i < lCount; ++i, ++lFirst)
					lFirst->~T();

				alignedFree(pPointer);
			};

#ifdef BC_MEMORY_DEFRAG
			template< typename T >
			void registerPointerInDynamicAllocators(const T* pPointer)
			{
				mSuperHeap.registerPointer(pPointer);
			};

			template< typename T >
			void unRegisterPointerInDynamicAllocators(const T* pPointer)
			{
				mSuperHeap.unRegisterPointer(pPointer);
			};
#endif

			void endOfFrame();

			//bcSIZE GetUsedSizeByCategory(bcUINT32 pCategory);

			bcSIZE getTotalSize();
			bcSIZE getUsedSize();
			bcSIZE getOverheadSize();
			bcSIZE getMaxUsedSize();

#ifdef BC_MEMORY_LEAK_DETECTION
			bcUINT32 reportMemoryLeaks();
#endif
		};

//#define P(T) bcPtr<T>
//#define RP(T) T*
//
//#define bcAlloc(pSize, pAllocType, pCategory)									bcMemMng::get()->alloc(pSize, pAllocType, pCategory, __FILE__, __LINE__)
//#define bcFree(pPointer)														/*if(pPointer)*/ { bcMemMng::get()->free(pPointer); pPointer = bcNULL; }
//#define bcReAlloc(pPointer, pNewSize, pAllocType, pCategory)					bcMemMng::get()->reAlloc(pPointer, pNewSize, pAllocType, pCategory, __FILE__, __LINE__)
//#define bcAlignedAlloc(pSize, pAlignment, pAllocType, pCategory)				bcMemMng::get()->alignedAlloc(pSize, pAlignment, pAllocType, pCategory, __FILE__, __LINE__)
//#define bcAlignedFree(pPointer)													/*if(pPointer)*/ { bcMemMng::get()->alignedFree(pPointer); pPointer = bcNULL; }
//#define bcAlignedReAlloc(pPointer, pNewSize, pAlignment, pAllocType, pCategory)	bcMemMng::get()->alignedReAlloc(pPointer, pNewSize, pAlignment, pAllocType, pCategory, __FILE__, __LINE__)
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

BC_COREDLL_EXP void* operator new(bcSIZE pSize, BlackCat::Core::bcAlloc_Type pAllocType, bcUINT32 pCategory, const bcCHAR* pFile, bcUINT32 pLine);
BC_COREDLL_EXP void* operator new[](bcSIZE pSize, BlackCat::Core::bcAlloc_Type pAllocType, bcUINT32 pCategory, const bcCHAR* pFile, bcUINT32 pLine);
BC_COREDLL_EXP void* operator new(bcSIZE pSize, bcSIZE pAlignment, BlackCat::Core::bcAlloc_Type pAllocType, bcUINT32 pCategory, const bcCHAR* pFile, bcUINT32 pLine);
BC_COREDLL_EXP void* operator new[](bcSIZE pSize, bcSIZE pAlignment, BlackCat::Core::bcAlloc_Type pAllocType, bcUINT32 pCategory, const bcCHAR* pFile, bcUINT32 pLine);

//void* operator new(bcSIZE pSize, BlackCat::Core::MemBlock** pOutMemBlock, bcUINT32 pAllocType, bcUINT32 pCategory, const bcCHAR* pFile, bcUINT32 pLine);
////void* operator new(bcSIZE pSize, void* pPointerToReturn);
//void* operator new[](bcSIZE pSize, BlackCat::Core::MemBlock** pOutMemBlock, bcUINT32 pAllocType, bcUINT32 pCategory, const bcCHAR* pFile, bcUINT32 pLine);

#endif