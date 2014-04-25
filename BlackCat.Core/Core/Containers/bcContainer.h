//  [9/8/2013 MRB]

#pragma once

#ifndef bcContainer_H
#define bcContainer_H

#include "CorePCH.h"
#include "MemoryManagment.h"
#include "bcMemory.h"
#include "bcAtomic.h"
#include "bcIterator.h"

namespace BlackCat
{
	namespace Core
	{
		// == bcContainer ============================================================================= MRB ==
		class bcContainer_Base
		{
		private:

		protected:
			

		public:
			bcContainer_Base() {};
			~bcContainer_Base() {};
		};

		template < typename T, class Allocator >
		class bcContainer : public bcContainer_Base
		{
		public:
			typedef T ValueType;
			//typedef T& ReferenceType;
			//typedef T&& RValueReferenceType;
			typedef ValueType* PointerType;
			typedef ValueType* RawPointerType;
			typedef Allocator AllocType;
		private:

		protected:
			bcUINT32 mSize;
			AllocType mAllocator;

		public:
			bcContainer() { mSize = 0; };
			~bcContainer() {};

			bcUINT32 Size() const { return mSize; };
			bcBOOL Empty() const { return (Size() > 0) ? bcTRUE : bcFALSE; };
		};

		template< typename T, class Allocator >
		class bcConcurrentContainer : public bcContainer_Base
		{
		public:
			typedef T ValueType;
			//typedef T& ReferenceType;
			//typedef T&& RValueReferenceType;
			typedef ValueType* PointerType;
			typedef ValueType* RawPointerType;
			typedef Allocator AllocType;
		private:

		protected:
			bcAtomic<bcUINT32> mSize;
			AllocType mAllocator;

		public:
			bcConcurrentContainer() { bcAtomicOperation::bcAtomicStore(mSize, 0U); };
			~bcConcurrentContainer() {};

			bcUINT32 Size() const { return bcAtomicOperation::bcAtomicLoad(mSize, bcMemoryOrder_SeqCst); };
			bcBOOL Empty() const { return (Size() > 0) ? bcTRUE : bcFALSE; };
		};

#ifdef BC_MEMORY_ENABLE
		// == bcContainerAllocation =================================================================== MRB ==
		template < bcUINT32 mAlignment = BC_MEMORY_MIN_ALIGN, bcAlloc_Type mAllocType = Alloc_Unknown, bcUINT32 mCategory = 0 >
		class bcContainerAllocation
		{
		private:

		protected:

		public:
			bcInline void* alloc(bcUINT32 pSize)
			{
				if(mAlignment > BC_MEMORY_MIN_ALIGN)
					return bcAlignedAlloc(pSize, mAlignment, mAllocType, mCategory);
				return bcAlloc(pSize, mAllocType, mCategory);
			};
			bcInline void free(void* pMem)
			{
				if(mAlignment > BC_MEMORY_MIN_ALIGN)
				{
					bcAlignedFree(pMem);
					return;
				}
				bcFree(pMem);
			};
			bcInline void* reAlloc(void* pMem, bcUINT32 pNewSize)
			{
				if(mAlignment > BC_MEMORY_MIN_ALIGN)
					return bcAlignedReAlloc(pMem, pNewSize, mAlignment, mAllocType, mCategory);
				return bcReAlloc(pMem, pNewSize, mAllocType, mCategory);
			};
		};

#else
		template < bcUINT32 mAlignment = 8 >
		class bcContainerAllocation
		{
		private:

		protected:

		public:
			bcInline void* alloc(bcUINT32 pSize)
			{
				if(mAlignment > 8)
					return _aligned_malloc(pSize, mAlignment);
				return malloc(pSize);
			};
			bcInline void free(void* pMem)
			{
				if(mAlignment > 8)
				{
					_aligned_free(pMem);
					return;
				}
				free(pMem);
			};
			bcInline void* reAlloc(void* pMem, bcUINT32 pNewSize)
			{
				if(mAlignment > 8)
					return _aligned_realloc(pMem, pNewSize, mAlignment);
				return realloc(pMem, pNewSize);
			};
		};


#endif
		template < bcUINT32 mAlignment = 8 >
		class bcContainerCRTAllocation
		{
		private:

		protected:

		public:
			bcInline void* alloc(bcUINT32 pSize)
			{
				if(mAlignment > 8)
					return _aligned_malloc(pSize, mAlignment);
				return malloc(pSize);
			};
			bcInline void free(void* pMem)
			{
				if(mAlignment > 8)
				{
					_aligned_free(pMem);
					return;
				}
				free(pMem);
			};
			bcInline void* reAlloc(void* pMem, bcUINT32 pNewSize)
			{
				if(mAlignment > 8)
					return _aligned_realloc(pMem, pNewSize, mAlignment);
				return realloc(pMem, pNewSize);
			};
		};

		// == bcBineryOperations ====================================================================== MRB ==
		template < class Arg1, class Arg2, class Result >
		struct bcBinaryFunction
		{
			typedef Arg1 FirstArgumentType;
			typedef Arg2 SecondArgumentType;
			typedef Result ResultType;
		};

		template < class T >
		struct bcLess : public bcBinaryFunction< T, T, bcBOOL >
		{
			bcBOOL operator()(const T& pLeft, const T& pRight) const
			{
				return (pLeft < pRight);
			}
		};

		template <typename T1, typename T2>
		struct bcPair
		{
			typedef T1 FirstType;
			typedef T2 SecondType;
		public:
			T1 mFirst;
			T2 mSecond;
			
			bcPair() {};
			//bcPair(T1& pFirst, T2& pSecond) : mFirst(pFirst), mSecond(pSecond)
			//{
			//	/*mFirst = pFirst;
			//	mSecond = pSecond;*/
			//};
			template < typename T1, typename T2 >
			bcPair(T1&& pFirst, T2&& pSecond) : mFirst(std::forward<T1>(pFirst)), mSecond(std::forward<T2>(pSecond))
			{
				/*mFirst = pFirst;
				mSecond = pSecond;*/
			};

			// bcPair(const pair& p)
			// bcPair& operator=(const pair& p)
		};
	}
}

#endif