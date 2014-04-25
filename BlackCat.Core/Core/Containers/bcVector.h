#pragma once

#ifndef bcVector_H
#define bcVector_H

#include "CorePCH.h"
#include "bcContainer.h"

namespace BlackCat
{
	namespace Core
	{
//		// == bcVector_ConstIterator ==============================================
//		template < class Vec >
//		class bcVector_ConstIterator : public bcIteratorBase
//		{
//		public:
//			typedef typename Vec::Value Value;
//			typedef typename Vec::Reference Reference;
//			typedef const Reference Const_Reference;
//			typedef typename Vec::Pointer Pointer;
//			typedef const Pointer Const_Pointer;
//			typedef typename Vec::RawPointer RawPointer;
//			typedef const RawPointer Const_RawPointer;
//		private:
//
//		protected:
//			Pointer mPtr;
//#ifdef BC_CONTAINER_DEBUG
//			bcUINT32 mIndex; // Index of current iterator in it's vector
//
//			bcInline bcUINT32 MyIndex() const
//			{
//				// Get index of iterator in it's vector
//				return ((bcUINT32)(RawPointer)((RP(Vec))mCont)->mFirst - (bcUINT32)(RawPointer)mPtr) / sizeof(Value);
//			};
//#endif
//			
//		public:
//			bcVector_ConstIterator() : bcIteratorBase(NULL), mPtr()
//			{
//			};
//			bcVector_ConstIterator(Pointer pPtr, RP(bcContainer_Base) pCont) : bcIteratorBase(pCont), mPtr(pPtr)
//			{
//#ifdef BC_CONTAINER_DEBUG
//				mIndex = MyIndex();
//#endif
//			};
//			bcVector_ConstIterator(const bcVector_ConstIterator<Vec>& pOther)
//			{
//				mPtr = pOther.mPtr;
//				mCont = pOther.mCont;
//#ifdef BC_CONTAINER_DEBUG
//				mIndex = pOther.mIndex;
//#endif
//			};
//			~bcVector_ConstIterator()
//			{
//			};
//
//			Pointer Get()
//			{
//				return mPtr;
//			};
//			bcBOOL Validate()
//			{
//				return (mPtr
//						&& mCont 
//						&& mPtr >= ((RP(Vec))mCont)->mFirst 
//						&& mPtr < ((RP(Vec))mCont)->mLast);
//			};
//
//			Const_Reference operator* () const
//			{
//#ifdef BC_CONTAINER_DEBUG
//				bcAssert(mPtr 
//						&& mCont 
//						&& mPtr >= ((RP(Vec))mCont)->mFirst 
//						&& mPtr < ((RP(Vec))mCont)->mLast);
//				// When we remove an element from vector or insert
//				// one in middle of vector, iterators that point to
//				// higher elements will point to another elemnts, we
//				// get current index of iterator and compaire it with
//				// it's orginal index
//				//bcAssert(MyIndex() == mIndex);
//#endif
//				return *mPtr;
//			};
//			Const_RawPointer operator-> () const
//			{
//#ifdef BC_CONTAINER_DEBUG
//				bcAssert(mPtr 
//						&& mCont 
//						&& mPtr >= ((RP(Vec))mCont)->mFirst 
//						&& mPtr < ((RP(Vec))mCont)->mLast);
//				// When we remove an element from vector or insert
//				// one in middle of vector, iterators that point to
//				// higher elements will point to another elemnts, we
//				// get current index of iterator and compaire it with
//				// it's orginal index
//				//bcAssert(MyIndex() == mIndex);
//#endif
//				return mPtr;
//			};
//			bcVector_ConstIterator<Vec>& operator= (bcVector_ConstIterator<Vec>& pOther)
//			{
//				mPtr = pOther.mPtr;
//				mCont = pOther.mCont;
//#ifdef BC_CONTAINER_DEBUG
//				mIndex = pOther.mIndex;
//#endif
//				return *this;
//			};
//			bcBOOL operator== (bcVector_ConstIterator<Vec>& pOther) const
//			{
//				if(mPtr == pOther.mPtr)
//					return bcTRUE;
//				else
//					return bcFALSE;
//			};
//			bcBOOL operator!= (bcVector_ConstIterator<Vec>& pOther) const
//			{
//				if(mPtr != pOther.mPtr)
//					return bcTRUE;
//				else
//					return bcFALSE;
//			};
//			bcBOOL operator> (bcVector_ConstIterator<Vec>& pOther) const
//			{
//				if(mPtr > pOther.mPtr)
//					return bcTRUE;
//				else
//					return bcFALSE;
//			};
//			bcBOOL operator>= (bcVector_ConstIterator<Vec>& pOther) const
//			{
//				if(mPtr >= pOther.mPtr)
//					return bcTRUE;
//				else
//					return bcFALSE;
//			};
//			bcBOOL operator< (bcVector_ConstIterator<Vec>& pOther) const
//			{
//				if(mPtr < pOther.mPtr)
//					return bcTRUE;
//				else
//					return bcFALSE;
//			};
//			bcBOOL operator<= (bcVector_ConstIterator<Vec>& pOther) const
//			{
//				if(mPtr <= pOther.mPtr)
//					return bcTRUE;
//				else
//					return bcFALSE;
//			};
//
//			bcVector_ConstIterator<Vec> operator+ (bcINT32 pNum) const
//			{
//				bcVector_ConstIterator<Vec> lTemp(*this);
//#ifdef BC_CONTAINER_DEBUG
//				bcAssert(lTemp.mPtr
//						&& lTemp.mCont 
//						&& lTemp.mPtr + pNum >= ((RP(Vec))mCont)->mFirst 
//						&& lTemp.mPtr + pNum <= ((RP(Vec))mCont)->mLast);
//				lTemp.mIndex += pNum;
//#endif
//				lTemp.mPtr += pNum;
//				return lTemp;
//			};
//			bcVector_ConstIterator<Vec>& operator+= (bcINT32 pNum)
//			{
//#ifdef BC_CONTAINER_DEBUG
//				bcAssert(mPtr
//						&& mCont 
//						&& mPtr + pNum >= ((RP(Vec))mCont)->mFirst 
//						&& mPtr + pNum <= ((RP(Vec))mCont)->mLast);
//				mIndex += pNum;
//#endif
//				mPtr += pNum;
//				return *this;
//			};
//			void operator++ ()
//			{
//#ifdef BC_CONTAINER_DEBUG
//				bcAssert(mPtr
//						&& mCont 
//						&& mPtr + 1 >= ((RP(Vec))mCont)->mFirst 
//						&& mPtr + 1 <= ((RP(Vec))mCont)->mLast);
//				++mIndex;
//#endif
//				++mPtr;
//			};
//			void operator++ (bcINT32)
//			{
//#ifdef BC_CONTAINER_DEBUG
//				bcAssert(mPtr
//						&& mCont 
//						&& mPtr + 1 >= ((RP(Vec))mCont)->mFirst 
//						&& mPtr + 1 <= ((RP(Vec))mCont)->mLast);
//				mIndex ++;
//#endif
//				mPtr++;
//			};
//			bcVector_ConstIterator<Vec> operator- (bcINT32 pNum) const
//			{
//				bcVector_ConstIterator<Vec> lTemp(*this);
//#ifdef BC_CONTAINER_DEBUG
//				bcAssert(lTemp.mPtr
//						&& lTemp.mCont 
//						&& lTemp.mPtr - pNum >= ((RP(Vec))mCont)->mFirst 
//						&& lTemp.mPtr - pNum <= ((RP(Vec))mCont)->mLast);
//				lTemp.mIndex += pNum;
//#endif
//				lTemp.mPtr -= pNum;
//				return lTemp;
//			};
//			bcVector_ConstIterator<Vec>& operator-= (bcINT32 pNum)
//			{
//#ifdef BC_CONTAINER_DEBUG
//				bcAssert(mPtr
//						&& mCont 
//						&& mPtr - pNum >= ((RP(Vec))mCont)->mFirst 
//						&& mPtr - pNum <= ((RP(Vec))mCont)->mLast);
//				mIndex += pNum;
//#endif
//				mPtr -= pNum;
//				return *this;
//			};
//			void operator-- ()
//			{
//#ifdef BC_CONTAINER_DEBUG
//				bcAssert(mPtr
//						&& mCont 
//						&& mPtr - 1 >= ((RP(Vec))mCont)->mFirst 
//						&& mPtr - 1 <= ((RP(Vec))mCont)->mLast);
//				--mIndex;
//#endif
//				--mPtr;
//			};
//			void operator-- (bcINT32)
//			{
//#ifdef BC_CONTAINER_DEBUG
//				bcAssert(mPtr
//						&& mCont 
//						&& mPtr - 1 >= ((RP(Vec))mCont)->mFirst 
//						&& mPtr - 1 <= ((RP(Vec))mCont)->mLast);
//				mIndex--;
//#endif
//				mPtr--;
//			};
//		};
//
//		// == bcVector_Iterator ==============================================
//		template < class Vec >
//		class bcVector_Iterator : public bcVector_ConstIterator< Vec >
//		{
//		private:
//
//		protected:
//			
//		public:
//			bcVector_Iterator() : bcVector_ConstIterator()
//			{
//			};
//			bcVector_Iterator(Pointer pPtr, RP(bcContainer_Base) pCont) : bcVector_ConstIterator(pPtr, pCont)
//			{
//			};
//			bcVector_Iterator(const bcVector_Iterator<Vec>& pOther)
//			{
//				mPtr = pOther.mPtr;
//				mCont = pOther.mCont;
//#ifdef BC_CONTAINER_DEBUG
//				mIndex = pOther.mIndex;
//#endif
//			};
//			~bcVector_Iterator()
//			{
//			};
//
//			Reference operator* () const
//			{
//#ifdef BC_CONTAINER_DEBUG
//				bcAssert(mPtr 
//						&& mCont 
//						&& mPtr >= ((RP(Vec))mCont)->mFirst 
//						&& mPtr < ((RP(Vec))mCont)->mLast);
//				// When we remove an element from vector or insert
//				// one in middle of vector, iterators that point to
//				// higher elements will point to another elemnts, we
//				// get current index of iterator and compaire it with
//				// it's orginal index
//				//bcAssert(MyIndex() == mIndex);
//#endif
//				return *mPtr;
//			};
//			RawPointer operator-> () const
//			{
//#ifdef BC_CONTAINER_DEBUG
//				bcAssert(mPtr 
//						&& mCont 
//						&& mPtr >= ((RP(Vec))mCont)->mFirst 
//						&& mPtr < ((RP(Vec))mCont)->mLast);
//				// When we remove an element from vector or insert
//				// one in middle of vector, iterators that point to
//				// higher elements will point to another elemnts, we
//				// get current index of iterator and compaire it with
//				// it's orginal index
//				//bcAssert(MyIndex() == mIndex);
//#endif
//				return mPtr;
//			};
//			bcVector_Iterator<Vec>& operator= (bcVector_Iterator<Vec>& pOther)
//			{
//				mPtr = pOther.mPtr;
//				mCont = pOther.mCont;
//#ifdef BC_CONTAINER_DEBUG
//				mIndex = pOther.mIndex;
//#endif
//				return *this;
//			};
//			/*bcBOOL operator== (bcVector_Iterator<T>& pOther) const;
//			bcBOOL operator!= (bcVector_Iterator<T>& pOther) const;
//			bcBOOL operator> (bcVector_Iterator<T>& pOther) const;
//			bcBOOL operator>= (bcVector_Iterator<T>& pOther) const;
//			bcBOOL operator< (bcVector_Iterator<T>& pOther) const;
//			bcBOOL operator<= (bcVector_Iterator<T>& pOther) const;*/
//			bcVector_Iterator<Vec> operator+ (bcINT32 pNum) const
//			{
//				bcVector_Iterator<Vec> lTemp(*this);
//#ifdef BC_CONTAINER_DEBUG
//				bcAssert(lTemp.mPtr
//						&& lTemp.mCont 
//						&& lTemp.mPtr + pNum >= ((RP(Vec))mCont)->mFirst 
//						&& lTemp.mPtr + pNum <= ((RP(Vec))mCont)->mLast);
//				lTemp.mIndex += pNum;
//#endif
//				lTemp.mPtr += pNum;
//				return lTemp;
//			};
//			//bcVector_Iterator<T>& operator+= (bcINT32 pNum);
//			//void operator++ ();
//			//void operator++ (bcINT);
//			bcVector_Iterator<Vec> operator- (bcINT32 pNum) const
//			{
//				bcVector_Iterator<Vec> lTemp(*this);
//#ifdef BC_CONTAINER_DEBUG
//				bcAssert(lTemp.mPtr
//						&& lTemp.mCont 
//						&& lTemp.mPtr - pNum >= ((RP(Vec))mCont)->mFirst 
//						&& lTemp.mPtr - pNum <= ((RP(Vec))mCont)->mLast);
//				lTemp.mIndex += pNum;
//#endif
//				lTemp.mPtr -= pNum;
//				return lTemp;
//			};
//			//bcVector_Iterator<T>& operator-= (bcINT32 pNum);
//			//void operator-- ();
//			//void operator-- (bcINT);
//		};

		template < typename T, class Allocator >
		class bcVector_Base : public bcContainer< T, Allocator >, public bcIteratorProvider< T >
		{
		private:

		protected:
			bcUINT32 mCapacity;

			struct Node : public bcContainerNode<ValueType>
			{
				Node() {}
				Node(const ValueType& pValue) : bcContainerNode(pValue) {}
				Node(ValueType&& pValue) : bcContainerNode(std::move(pValue)) {}
			};
			typedef Node NodeType;

		public:
			NodeType* mFirst;
			NodeType* mLast;
			NodeType* mEnd;

			bcVector_Base() 
			{
				mCapacity = 0;
				mFirst = NULL;
				mLast = NULL;
				mEnd = NULL;
			};
			~bcVector_Base() {};

			bcUINT32 Capacity() const { return mCapacity; };

			ValueType& nodeValue(NodeBaseType* pNode) override
			{
				return pNode->mValue;
			}
			NodeBaseType* moveForward(NodeBaseType* pNode) override
			{
				NodeType* lNode = static_cast<NodeType*>(pNode);
				++lNode;
				return (pNode);
			}
			NodeBaseType* moveForward(NodeBaseType* pNode, bcUINT32 pCount) override
			{
				NodeType* lNode = static_cast<NodeType*>(pNode);
				for (bcUINT32 i = 0; i < pCount; ++i)
				{
					++pNode;
				}
				return lNode;
			}
			NodeBaseType* moveBackward(NodeBaseType* pNode) override
			{
				NodeType* lNode = static_cast<NodeType*>(pNode);
				--lNode;
				return pNode;
			}
			NodeBaseType* moveBackward(NodeBaseType* pNode, bcUINT32 pCount) override
			{
				NodeType* lNode = static_cast<NodeType*>(pNode);
				for (bcUINT32 i = 0; i < pCount; ++i)
				{
					--pNode;
				}
				return lNode;
			}
			bcBOOL isValid(const NodeBaseType* pNode) const override
			{ return (pNode && pNode >= mFirst && pNode < mLast); }
		};

		// == bcVector ===================================================
		template < typename T, bcUINT32 mStepSize = 10, class Allocator = bcContainerAllocation<> >
		class bcVector : public bcVector_Base< T, Allocator >
		{
		public:
			typedef bcVector < T, mStepSize, Allocator > ThisType;
			//typedef bcVector_Iterator< bcVector_Base< T, Allocator, PT > > Iterator;
			//typedef bcVector_ConstIterator< bcVector_Base< T, Allocator, PT> > Const_Iterator;
			typedef bcIterator< T > Iterator;
			typedef bcConstIterator< T > ConstIterator;
			typedef bcReference< T > Reference;
			typedef bcConstReference< T > ConstReference;
		private:

		protected:
			bcInline void CheckHeap()
			{
				// Increase heap
				if(mCapacity == mSize)
				{
					NodeType* lHeap = (NodeType*)mAllocator.alloc((mCapacity + mStepSize) * sizeof(NodeType));
					mCapacity += mStepSize;

					if(mFirst)
					{
						std::memcpy(lHeap, mFirst, sizeof(NodeType)* mSize);

						mAllocator.free(mFirst);
					}

					mFirst = lHeap;
					mLast = lHeap;
					mLast += mSize;
					mEnd = lHeap;
					mEnd += mCapacity;
				}
				// Decrease heap
				else if(mCapacity == mSize + mStepSize)
				{
					NodeType* lHeap = nullptr;
					if(mCapacity - mStepSize > 0)
						lHeap = (NodeType*)mAllocator.alloc((mCapacity - mStepSize) * sizeof(NodeType));
					mCapacity -= mStepSize;

					if(mFirst)
					{
						if(lHeap)
							std::memcpy(lHeap, mFirst, sizeof(NodeType)* mSize);
					
						mAllocator.free(mFirst);
					}

					mFirst = lHeap;
					mLast = lHeap;
					mLast += mSize;
					mEnd = lHeap;
					mEnd += mCapacity;
				}
			};
			
			bcInline NodeType* NewNode(const NodeType* pPosition, ValueType&& pValue)
			{
#ifdef BC_CONTAINER_DEBUG
				bcAssert(pPosition >= mFirst && pPosition <= mLast);
#endif
				// Because in CheckHeap method mFirst may change we store
				// mFirst in a variable
				NodeType* lFirst = mFirst;

				CheckHeap();

				// May be CheckHeap method change pointers, so if it occur
				// we correct pPosition
				NodeType* lPosition = mFirst + (pPosition - lFirst);

				if(lPosition == mLast)
				{
					new (mLast)NodeType(std::forward<ValueType>(pValue));
					++mLast;
				}
				else
				{
					memmove(lPosition + 1, lPosition, (mLast - lPosition) * sizeof(NodeType));

					new (lPosition)NodeType(std::forward<ValueType>(pValue));
					++mLast;
				}

				++mSize;

				return lPosition;
			};
			bcInline NodeType* NewNode(const NodeType* pPosition, const ValueType& pValue)
			{
#ifdef BC_CONTAINER_DEBUG
				bcAssert(pPosition >= mFirst && pPosition <= mLast);
#endif
				// Because in CheckHeap method mFirst may change we store
				// mFirst in a variable
				NodeType* lFirst = mFirst;

				CheckHeap();

				// May be CheckHeap method change pointers, so if it occur
				// we correct pPosition
				NodeType* lPosition = mFirst + (pPosition - lFirst);

				if (lPosition == mLast)
				{
					new (mLast)NodeType(pValue);
					++mLast;
				}
				else
				{
					memmove(lPosition + 1, lPosition, (mLast - lPosition) * sizeof(NodeType));

					new (lPosition)NodeType(pValue);
					++mLast;
				}

				++mSize;

				return lPosition;
			};
			bcInline NodeType* NewNode(const NodeType* pPosition)
			{
#ifdef BC_CONTAINER_DEBUG
				bcAssert(pPosition >= mFirst && pPosition <= mLast);
#endif
				// Because in CheckHeap method mFirst may change we store
				// mFirst in a variable
				NodeType* lFirst = mFirst;

				CheckHeap();

				// May be CheckHeap method change pointers, so if it occur
				// we correct pPosition
				NodeType* lPosition = mFirst + (pPosition - lFirst);

				if(lPosition == mLast)
				{
					new (mLast)NodeType;
					++mLast;
				}
				else
				{
					memmove(lPosition + 1, lPosition, (mLast - lPosition) * sizeof(NodeType));

					new (lPosition)NodeType;
					++mLast;
				}

				++mSize;

				return lPosition;
			};
			bcInline void FreeNode(const NodeType* pPosition)
			{
#ifdef BC_CONTAINER_DEBUG
				bcAssert(pPosition >= mFirst && pPosition <= mLast);
#endif
				if(pPosition == mLast - 1)
				{
					(pPosition)->~Node();
					--mLast;
				}
				else
				{
					(pPosition)->~Node();

					memmove(const_cast<NodeType*>(pPosition), pPosition + 1, (mLast - (pPosition + 1)) * sizeof(NodeType));

					--mLast;
				}

				--mSize;

				CheckHeap();
			};

		public:
			bcVector();
			bcVector(const ThisType& pOther);
			bcVector(ThisType&& pOther);
			~bcVector();

			ThisType& operator=(const ThisType& pOther);
			void Swap(ThisType& pOther);

			void Assign(bcUINT32 n, const ValueType& pValue);

			Iterator       Begin();
			ConstIterator  Begin() const;
			
			Iterator       RBegin();
			ConstIterator  RBegin() const;

			Iterator       End();
			ConstIterator  End() const;

			Iterator       REnd();
			ConstIterator  REnd() const;

			PointerType       Data();
			const PointerType Data() const;

			Reference      operator[](bcUINT32 n);
			ConstReference operator[](bcUINT32 n) const;

			Reference       At(bcUINT32 n);
			ConstReference At(bcUINT32 n) const;

			Iterator       Find(ValueType& pValue);
			ConstIterator  Find(ValueType& pValue) const;

			//void PushBack(const ReferenceType pValue);
			Iterator PushBack(const ValueType& pValue);
			Iterator PushBack(ValueType&& pValue);
			//void PushBack();
			Iterator PushBack();

			void PopBack();

			//void Insert(Iterator pPosition, const Reference pValue);
			Iterator Insert(Iterator pPosition, const ValueType& pValue);
			Iterator Insert(Iterator pPosition, ValueType&& pValue);

			void Erase(Iterator pPosition);
			void Erase(Iterator pFirst, Iterator pLast);

			void Clear();
		};

		template < typename T, bcUINT32 mStepSize, class Allocator >
		bcVector<T, mStepSize, Allocator>::bcVector()
		{
			bcAssert(mStepSize > 0);
		};
		template < typename T, bcUINT32 mStepSize, class Allocator >
		bcVector<T, mStepSize, Allocator>::bcVector(const ThisType& pOther)
		{
			Clear();

			mSize = pOther.mSize;
			mAllocator = pOther.mAllocator;
			mCapacity = pOther.mCapacity;

			mFirst = (NodeType*)mAllocator.alloc(mCapacity * sizeof(NodeType));
			mLast = mFirst + mSize;
			mEnd = mFirst + mCapacity;

			//memcpy(pOther.mFirst, mFirst, mSize * sizeof(Value));
			for(bcUINT32 i = 0; i < mSize; ++i)
			{
				new (mFirst + i) NodeType(std::move(*(pOther.mFirst + i)));
			}
		};
		template < typename T, bcUINT32 mStepSize, class Allocator >
		bcVector<T, mStepSize, Allocator>::bcVector(ThisType&& pOther)
		{
			Clear();

			mSize = pOther.mSize;
			mAllocator = pOther.mAllocator;
			mCapacity = pOther.mCapacity;

			mFirst = pOther.mFirst;
			mLast = pOther.mLast;
			mEnd = pOther.mEnd;

			pOther.mFirst = nullptr;
			pOther.mLast = nullptr;
			pOther.mEnd = nullptr;

			pOther.Clear();
		};
		template < typename T, bcUINT32 mStepSize, class Allocator >
		bcVector<T, mStepSize, Allocator>::~bcVector()
		{
			if(mFirst)
				mAllocator.free(mFirst);
		};

		template < typename T, bcUINT32 mStepSize, class Allocator >
		typename bcVector<T, mStepSize, Allocator>::ThisType&
		bcVector<T, mStepSize, Allocator>::operator=(const ThisType& pOther)
		{
			if(this != &pOther)
			{
				Clear();

				mSize = pOther.mSize;
				mAllocator = pOther.mAllocator;
				mCapacity = pOther.mCapacity;

				mFirst = (NodeType*)mAllocator.alloc(mCapacity * sizeof(NodeType));
				mLast = mFirst + mSize;
				mEnd = mFirst + mCapacity;

				//memcpy(pOther.mFirst, mFirst, mSize * sizeof(Value));
				for(bcUINT32 i = 0; i < mSize; ++i)
				{
					new (mFirst + i) NodeType(std::move(*(pOther.mFirst + i)));
				}
			}
			return *this;
		};
		template < typename T, bcUINT32 mStepSize, class Allocator >
		void bcVector<T, mStepSize, Allocator>::Swap(ThisType& pOther)
		{
			std::swap(mSize, pOther.mSize);
			std::swap(mAllocator, pOther.mAllocator);
			std::swap(mCapacity, pOther.mCapacity);
			std::swap(mFirst, pOther.mFirst);
			std::swap(mLast, pOther.mLast);
			std::swap(mEnd, pOther.mEnd);
		};

		template < typename T, bcUINT32 mStepSize, class Allocator >
		void bcVector<T, mStepSize, Allocator>::Assign(bcUINT32 n, const ValueType& pValue)
		{
			for(bcUINT32 i = 0; i < n; ++i)
				PushBack(pValue);
		};

		template < typename T, bcUINT32 mStepSize, class Allocator >
		typename bcVector<T, mStepSize, Allocator>::Iterator 
		bcVector<T, mStepSize, Allocator>::Begin()
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(mSize > 0);
#endif
			return Iterator(mFirst, this);
		};
		template < typename T, bcUINT32 mStepSize, class Allocator >
		typename bcVector<T, mStepSize, Allocator>::ConstIterator 
		bcVector<T, mStepSize, Allocator>::Begin() const
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(mSize > 0);
#endif
			return ConstIterator(mFirst, this);
		};

		template < typename T, bcUINT32 mStepSize, class Allocator >
		typename bcVector<T, mStepSize, Allocator>::Iterator 
		bcVector<T, mStepSize, Allocator>::RBegin()
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(mSize > 0);
#endif
			return Iterator(mLast - 1, this);
		};
		template < typename T, bcUINT32 mStepSize, class Allocator >
		typename bcVector<T, mStepSize, Allocator>::ConstIterator 
		bcVector<T, mStepSize, Allocator>::RBegin() const
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(mSize > 0);
#endif
			return ConstIterator(mLast - 1, this);
		};
        
		template < typename T, bcUINT32 mStepSize, class Allocator >
		typename bcVector<T, mStepSize, Allocator>::Iterator 
		bcVector<T, mStepSize, Allocator>::End()
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(mSize > 0);
#endif
			return Iterator(mLast, this);
		};
		template < typename T, bcUINT32 mStepSize, class Allocator >
		typename bcVector<T, mStepSize, Allocator>::ConstIterator 
		bcVector<T, mStepSize, Allocator>::End() const
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(mSize > 0);
#endif
			return ConstIterator(mLast, this);
		};

		template < typename T, bcUINT32 mStepSize, class Allocator >
		typename bcVector<T, mStepSize, Allocator>::Iterator 
		bcVector<T, mStepSize, Allocator>::REnd()
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(mSize > 0);
#endif
			return Iterator(mFirst - 1, this);
		};
		template < typename T, bcUINT32 mStepSize, class Allocator >
		typename bcVector<T, mStepSize, Allocator>::ConstIterator 
		bcVector<T, mStepSize, Allocator>::REnd() const
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(mSize > 0);
#endif
			return ConstIterator(mFirst - 1, this);
		};

		template < typename T, bcUINT32 mStepSize, class Allocator >
		typename bcVector<T, mStepSize, Allocator>::PointerType
		bcVector<T, mStepSize, Allocator>::Data()
		{
			return mFirst;
		};
		template < typename T, bcUINT32 mStepSize, class Allocator >
		const typename bcVector<T, mStepSize, Allocator>::PointerType
		bcVector<T, mStepSize, Allocator>::Data() const
		{
			return mLast;
		};

		template < typename T, bcUINT32 mStepSize, class Allocator >
		typename bcVector<T, mStepSize, Allocator>::Reference 
		bcVector<T, mStepSize, Allocator>::operator[](bcUINT32 n)
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(mSize > 0 && n <= mSize);
#endif
			return Reference((mFirst + n));
		};
		template < typename T, bcUINT32 mStepSize, class Allocator >
		typename bcVector<T, mStepSize, Allocator>::ConstReference 
		bcVector<T, mStepSize, Allocator>::operator[](bcUINT32 n) const
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(mSize > 0 && n <= mSize);
#endif
			return ConstReference((mFirst + n));
		};

		template < typename T, bcUINT32 mStepSize, class Allocator >
		typename bcVector<T, mStepSize, Allocator>::Reference 
		bcVector<T, mStepSize, Allocator>::At(bcUINT32 n)
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(mSize > 0 && n <= mSize);
#endif
			return Reference((mFirst + n));
		};
		template < typename T, bcUINT32 mStepSize, class Allocator >
		typename bcVector<T, mStepSize, Allocator>::ConstReference
		bcVector<T, mStepSize, Allocator>::At(bcUINT32 n) const
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(mSize > 0 && n <= mSize);
#endif
			return ConstReference((mFirst + n));
		};

		template < typename T, bcUINT32 mStepSize, class Allocator >
		typename bcVector<T, mStepSize, Allocator>::Iterator 
		bcVector<T, mStepSize, Allocator>::Find(ValueType& pValue)
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(mSize > 0);
#endif
			for(register bcUINT32 i = 0; i < mSize; ++i)
			{
				if(*(mFirst + n) == pValue)
					return Iterator(mFirst + n, this); 
			}
			return Iterator(NULL, this);
		};
		template < typename T, bcUINT32 mStepSize, class Allocator >
		typename bcVector<T, mStepSize, Allocator>::ConstIterator 
		bcVector<T, mStepSize, Allocator>::Find(ValueType& pValue) const
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(mSize > 0);
#endif
			for(register bcUINT32 i = 0; i < mSize; ++i)
			{
				if (nodeValue(mFirst + n) == pValue)
					return Const_Iterator(mFirst + n, this); 
			}
			return Const_Iterator(NULL, this);
		};

		template < typename T, bcUINT32 mStepSize, class Allocator >
		typename bcVector<T, mStepSize, Allocator>::Iterator 
		//void
		bcVector<T, mStepSize, Allocator>::PushBack(const ValueType& pValue)
		{
			return Iterator(NewNode(mLast, pValue), this);
			//NewNode(mLast, pValue);
		};
		template < typename T, bcUINT32 mStepSize, class Allocator >
		typename bcVector<T, mStepSize, Allocator>::Iterator 
		//void
		bcVector<T, mStepSize, Allocator>::PushBack(ValueType&& pValue)
		{
			return Iterator(NewNode(mLast, std::forward<ValueType>(pValue)), this);
			//NewNode(mLast, pValue);
		};
		template < typename T, bcUINT32 mStepSize, class Allocator >
		typename bcVector<T, mStepSize, Allocator>::Iterator 
		//void
		bcVector<T, mStepSize, Allocator>::PushBack()
		{
			return Iterator(NewNode(mLast), this);
			//NewNode(mLast, pValue);
		};

		template < typename T, bcUINT32 mStepSize, class Allocator >
		void bcVector<T, mStepSize, Allocator>::PopBack()
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(mSize > 0);
#endif
			FreeNode(mLast - 1);
		};

		template < typename T, bcUINT32 mStepSize, class Allocator >
		typename bcVector<T, mStepSize, Allocator>::Iterator 
		bcVector<T, mStepSize, Allocator>::Insert(Iterator pPosition, const ValueType& pValue)
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(pPosition.Validate());
#endif
			NodeType* lP = static_cast<NodeType*>(pPosition.getNode());
			return Iterator(NewNode(lP + 1, pValue), this);
		};
		template < typename T, bcUINT32 mStepSize, class Allocator >
		typename bcVector<T, mStepSize, Allocator>::Iterator 
		bcVector<T, mStepSize, Allocator>::Insert(Iterator pPosition, ValueType&& pValue)
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(pPosition.validate());
#endif
			NodeType* lP = static_cast<NodeType*>(pPosition.getNode());
			return Iterator(NewNode(lP + 1, std::forward<ValueType>(pValue)), this);
		};

		template < typename T, bcUINT32 mStepSize, class Allocator >
		void bcVector<T, mStepSize, Allocator>::Erase(Iterator pPosition)
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(pPosition.validate());
#endif
			const NodeType* lP = static_cast<const NodeType*>(pPosition.getNode());
			FreeNode(lP);
		};
		template < typename T, bcUINT32 mStepSize, class Allocator >
		void bcVector<T, mStepSize, Allocator>::Erase(Iterator pFirst, Iterator pLast)
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(pFirst.validate() && pLast.validate() && mSize > 0);
			// Check first key is lower than last
			bcAssert(pFirst <= pLast);
#endif
			Iterator lDel;
			while(pFirst != pLast)
			{
				lDel = pFirst;
				++pFirst;
				Erase(lDel);
			}
			Erase(pLast);
		};

		template < typename T, bcUINT32 mStepSize, class Allocator >
		void bcVector<T, mStepSize, Allocator>::Clear()
		{
			if (mFirst)
			{
				NodeType* lFrist = mFirst;
				while (lFrist != mLast)
				{
					(lFrist)->~Node();
					++lFrist;
				}

				mAllocator.free(mFirst);
			}	

			mSize = 0;
			mCapacity = 0;
			mFirst = NULL;
			mLast = NULL;
			mEnd = NULL;
		};
	}
}

#endif