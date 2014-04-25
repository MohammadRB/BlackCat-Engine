//  [9/4/2013 MRB]

#pragma once

#include "CorePCH.h"
#include "MemoryManagment.h"

namespace BlackCat
{
	namespace Core
	{

#ifdef BC_MEMORY_ENABLE

#define P(T) Ptr<T>
#define RP(T) T*

#define bcAlloc(pSize, pAllocType, pCategory)									bcMemMng::get()->alloc(pSize, pAllocType, pCategory, __FILE__, __LINE__)
#define bcFree(pPointer)														/*if(pPointer)*/ { bcMemMng::get()->free(pPointer); pPointer = bcNULL; }
#define bcReAlloc(pPointer, pNewSize, pAllocType)								bcMemMng::get()->reAlloc(pPointer, pNewSize, pAllocType)
#define bcAlignedAlloc(pSize, pAlignment, pAllocType, pCategory)				bcMemMng::get()->alignedAlloc(pSize, pAlignment, pAllocType, pCategory, __FILE__, __LINE__)
#define bcAlignedFree(pPointer)													/*if(pPointer)*/ { bcMemMng::get()->alignedFree(pPointer); pPointer = bcNULL; }
#define bcAlignedReAlloc(pPointer, pNewSize, pAlignment, pAllocType)			bcMemMng::get()->alignedReAlloc(pPointer, pNewSize, pAlignment, pAllocType)

#define bcNew(pT, pAllocType, pCategory)										bcMemMng::get()->new_(pT, pAllocType, pCategory, __FILE__, __LINE__)
#define bcNewArray(pT, pArrayLenth, pAllocType, pCategory)						bcMemMng::get()->newArray(pT, pArrayLenth, pAllocType, pCategory, __FILE__, __LINE__)
#define bcDelete(pT)															/*if(pT)*/ { bcMemMng::get()->delete_(pT); pT = bcNULL; }
#define bcDeleteArray(pT)														/*if(pT)*/ { bcMemMng::get()->deleteArray(pT); pT = bcNULL; }
#define bcAlignedNew(pT, pAlignment, pAllocType, pCategory)						bcMemMng::get()->alignedNew(pT, pAlignment, pAllocType, pCategory, __FILE__, __LINE__)
#define bcAlignedNewArray(pT, pAlignment, pArrayLenth, pAllocType, pCategory)	bcMemMng::get()->alignedNewArray(pT, pAlignment, pArrayLenth, pAllocType, pCategory, __FILE__, __LINE__)
#define bcAlignedDelete(pT)														/*if(pT)*/ { bcMemMng::get()->alignedDelete(pT); pT = bcNULL; }
#define bcAlignedDeleteArray(pT)												/*if(pT)*/ { bcMemMng::get()->alignedDeleteArray(pT); pT = bcNULL; }

#else

#define P(T) T*
#define RP(T) T*

#define bcAlloc(pSize, pAllocType, pCategory)						malloc(pSize)
#define bcFree(pMemory)												if(pMemory) { free(pMemory); pMemory = NULL; }
#define bcReAlloc(pMemory, pNewSize, pAllocType)					realloc(pMemory, pNewSize)
#define bcAlignedAlloc(pSize, pAlignment, pAllocType, pCategory)	_aligned_malloc(pSize, pAlignment)
#define bcAlignedFree(pMemory)										_aligned_free(pMemory)
#define bcAlignedReAlloc(pMemory, pNewSize, pAlignment, pAllocType)	_aligned_realloc(pMemory, pNewSize, pAlignment)

#define bcNew(pT, pAllocType, pCategory)							new pT
#define bcNewArray(pT, pArrayLenth, pAllocType, pCategory)			new pT[pArrayLenth]
#define bcDelete(pT)												if(pMemory) { delete(pT); pMemory = NULL; }
#define bcDeleteArray(pT)											if(pMemory) { delete[](pT); pMemory = NULL; }

#endif
	}
}