#pragma once

#ifndef bcList_H
#define bcList_H

#include "CorePCH.h"
#include "bcContainer.h"
#include "CoreUtility.h"

namespace BlackCat
{
	namespace Core
	{
		// == bcList_ConstIterator =======================================
//		template < class Lst >
//		class bcList_ConstIterator : public bcIteratorBase
//		{
//		public:
//			typedef typename Lst::NodePtr NodePtr;
//			typedef typename Lst::Value Value;
//			typedef typename Lst::Reference Reference;
//			typedef const Reference Const_Reference;
//			typedef typename Lst::Pointer Pointer;
//			typedef const Pointer Const_Pointer;
//			typedef typename Lst::RawPointer RawPointer;
//			typedef const RawPointer Const_RawPointer;
//		private:
//
//		protected:
//			NodePtr mPtr;
//
//		public:
//			bcList_ConstIterator() : bcIteratorBase(NULL), mPtr()
//			{
//			};
//			bcList_ConstIterator(NodePtr pPtr, RP(bcContainer_Base) pCont) : bcIteratorBase(pCont), mPtr(pPtr)
//			{
//			};
//			bcList_ConstIterator(const bcList_ConstIterator<Lst>& pOther)
//			{
//				mPtr = pOther.mPtr;
//				mCont = pOther.mCont;
//			};
//			~bcList_ConstIterator()
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
//						&& mPtr != ((RP(Lst))mCont)->mHead 
//						/*|| mPtr != ((RP(Lst))mCont)->mTail*/);
//			};
//
//			Const_Reference operator* () const
//			{
//#ifdef BC_CONTAINER_DEBUG
//				bcAssert(mPtr 
//						&& mCont 
//						&& mPtr != ((RP(Lst))mCont)->mHead 
//						/*|| mPtr != ((RP(Lst))mCont)->mTail*/);
//#endif
//				return Lst::NodeValue(mPtr);
//			};
//			Const_RawPointer operator-> () const
//			{
//#ifdef BC_CONTAINER_DEBUG
//				bcAssert(mPtr 
//						&& mCont 
//						&& mPtr != ((RP(Lst))mCont)->mHead 
//						/*|| mPtr != ((RP(Lst))mCont)->mTail*/);
//#endif
//				return &Lst::NodeValue(mPtr);
//			};
//			bcList_ConstIterator<Lst>& operator= (bcList_ConstIterator<Lst>& pOther)
//			{
//				mPtr = pOther.mPtr;
//				mCont = pOther.mCont;
//
//				return *this;
//			};
//
//			bcBOOL operator== (bcList_ConstIterator<Lst>& pOther) const
//			{
//				if(mPtr == pOther.mPtr)
//					return bcTRUE;
//				else
//					return bcFALSE;
//			};
//			bcBOOL operator!= (bcList_ConstIterator<Lst>& pOther) const
//			{
//				if(mPtr != pOther.mPtr)
//					return bcTRUE;
//				else
//					return bcFALSE;
//			};
//			/*bcBOOL operator> (bcList_ConstIterator<Lst>& pOther) const
//			{
//				if(mPtr > pOther.mPtr)
//					return bcTRUE;
//				else
//					return bcFALSE;
//			};
//			bcBOOL operator>= (bcList_ConstIterator<Lst>& pOther) const
//			{
//				if(mPtr >= pOther.mPtr)
//					return bcTRUE;
//				else
//					return bcFALSE;
//			};
//			bcBOOL operator< (bcList_ConstIterator<Lst>& pOther) const
//			{
//				if(mPtr < pOther.mPtr)
//					return bcTRUE;
//				else
//					return bcFALSE;
//			};
//			bcBOOL operator<= (bcList_ConstIterator<Lst>& pOther) const
//			{
//				if(mPtr <= pOther.mPtr)
//					return bcTRUE;
//				else
//					return bcFALSE;
//			};*/
//
//			bcList_ConstIterator<Lst> operator+ (bcINT32 pNum) const
//			{
//				bcList_ConstIterator<Lst> lTemp(*this);
//#ifdef BC_CONTAINER_DEBUG
//				bcAssert(lTemp.mPtr
//						&& lTemp.mCont);
//#endif
//				for(bcUINT32 i = 0; i < pNum; ++i)
//				{
////#ifdef BC_CONTAINER_DEBUG
////					bcAssert(Lst::NodeNext(lTemp.mPtr) != /*((RP(Lst))mCont)->mTail*/ ((RP(Lst))mCont)->mHead);
////#endif
//					lTemp.mPtr = Lst::NodeNext(lTemp.mPtr);
//				}
//				return lTemp;
//			};
//			bcList_ConstIterator<Lst>& operator+= (bcINT32 pNum)
//			{
//#ifdef BC_CONTAINER_DEBUG
//				bcAssert(mPtr
//						&& mCont);
//#endif
//				for(bcUINT32 i = 0; i < pNum; ++i)
//				{
////#ifdef BC_CONTAINER_DEBUG
////					bcAssert(Lst::NodeNext(mPtr) != /*((RP(Lst))mCont)->mTail*/ ((RP(Lst))mCont)->mHead);
////#endif
//					mPtr = Lst::NodeNext(mPtr);
//				}
//				return *this;
//			};
//			void operator++ ()
//			{
//#ifdef BC_CONTAINER_DEBUG
//				bcAssert(mPtr
//						&& mCont 
//						/*&& Lst::NodeNext(mPtr) != ((RP(Lst))mCont)->mHead*/);
//#endif
//				mPtr = Lst::NodeNext(mPtr);
//			};
//			void operator++ (bcINT32)
//			{
//#ifdef BC_CONTAINER_DEBUG
//				bcAssert(mPtr
//						&& mCont 
//						/*&& Lst::NodeNext(mPtr) != ((RP(Lst))mCont)->mHead*/);
//#endif
//				mPtr = Lst::NodeNext(mPtr);
//			};
//			bcList_ConstIterator<Lst> operator- (bcINT32 pNum) const
//			{
//				bcList_ConstIterator<Lst> lTemp(*this);
//#ifdef BC_CONTAINER_DEBUG
//				bcAssert(lTemp.mPtr
//						&& lTemp.mCont);
//#endif
//				for(bcUINT32 i = 0; i < pNum; ++i)
//				{
////#ifdef BC_CONTAINER_DEBUG
////					bcAssert(Lst::NodePrev(lTemp.mPtr) != /*((RP(Lst))mCont)->mHead*/ ((RP(Lst))mCont)->mHead);
////#endif
//					lTemp.mPtr = Lst::NodePrev(lTemp.mPtr);
//				}
//				return lTemp;
//			};
//			bcList_ConstIterator<Lst>& operator-= (bcINT32 pNum)
//			{
//#ifdef BC_CONTAINER_DEBUG
//				bcAssert(mPtr
//						&& mCont);
//#endif
//				for(bcUINT32 i = 0; i < pNum; ++i)
//				{
////#ifdef BC_CONTAINER_DEBUG
////					bcAssert(Lst::NodePrev(mPtr) != /*((RP(Lst))mCont)->mHead*/ ((RP(Lst))mCont)->mHead);
////#endif
//					mPtr = Lst::NodePrev(mPtr);
//				}
//				return *this;
//			};
//			void operator-- ()
//			{
//#ifdef BC_CONTAINER_DEBUG
//				bcAssert(mPtr
//						&& mCont 
//						/*&& Lst::NodePrev(mPtr) != ((RP(Lst))mCont)->mHead*/);
//#endif
//				mPtr = Lst::NodePrev(mPtr);
//			};
//			void operator-- (bcINT32)
//			{
//#ifdef BC_CONTAINER_DEBUG
//				bcAssert(mPtr
//						&& mCont 
//						/*&& Lst::NodePrev(mPtr) != ((RP(Lst))mCont)->mHead*/);
//#endif
//				mPtr = Lst::NodePrev(mPtr);
//			};
//		};
//
//		// == bcList_Iterator ==============================================
//		template < class Lst >
//		class bcList_Iterator : public bcList_ConstIterator< Lst >
//		{
//		private:
//
//		protected:
//			
//		public:
//			bcList_Iterator() : bcList_ConstIterator()
//			{
//			};
//			bcList_Iterator(NodePtr pPtr, RP(bcContainer_Base) pCont) : bcList_ConstIterator(pPtr, pCont)
//			{
//			};
//			bcList_Iterator(const bcList_Iterator<Lst>& pOther)
//			{
//				mPtr = pOther.mPtr;
//				mCont = pOther.mCont;
//			};
//			~bcList_Iterator()
//			{
//			};
//
//			Reference operator* () const
//			{
//#ifdef BC_CONTAINER_DEBUG
//				bcAssert(mPtr 
//						&& mCont 
//						&& mPtr != ((RP(Lst))mCont)->mHead 
//						/*|| mPtr != ((RP(Lst))mCont)->mTail*/);
//#endif
//				return Lst::NodeValue(mPtr);
//			};
//			RawPointer operator-> () const
//			{
//#ifdef BC_CONTAINER_DEBUG
//				bcAssert(mPtr 
//						&& mCont 
//						&& mPtr != ((RP(Lst))mCont)->mHead 
//						/*|| mPtr != ((RP(Lst))mCont)->mTail*/);
//#endif
//				return &Lst::NodeValue(mPtr);
//			};
//			bcList_Iterator<Lst>& operator= (bcList_Iterator<Lst>& pOther)
//			{
//				mPtr = pOther.mPtr;
//				mCont = pOther.mCont;
//
//				return *this;
//			};
//
//			bcList_Iterator<Lst> operator+ (bcINT32 pNum) const
//			{
//				bcList_Iterator<Vec> lTemp(*this);
//#ifdef BC_CONTAINER_DEBUG
//				bcAssert(lTemp.mPtr
//						&& lTemp.mCont);
//#endif
//				for(bcUINT32 i = 0; i < pNum; ++i)
//				{
////#ifdef BC_CONTAINER_DEBUG
////					bcAssert(Lst::NodeNext(lTemp.mPtr) != /*((RP(Lst))mCont)->mTail*/ ((RP(Lst))mCont)->mHead);
////#endif
//					lTemp.mPtr = Lst::NodeNext(lTemp.mPtr);
//				}
//				return lTemp;
//			};
//			bcList_Iterator<Lst> operator- (bcINT32 pNum) const
//			{
//				bcList_Iterator<Lst> lTemp(*this);
//#ifdef BC_CONTAINER_DEBUG
//				bcAssert(lTemp.mPtr
//						&& lTemp.mCont);
//#endif
//				for(bcUINT32 i = 0; i < pNum; ++i)
//				{
////#ifdef BC_CONTAINER_DEBUG
////					bcAssert(Lst::NodePrev(lTemp.mPtr) != /*((RP(Lst))mCont)->mHead*/ ((RP(Lst))mCont)->mHead);
////#endif
//					lTemp.mPtr = Lst::NodePrev(lTemp.mPtr);
//				}
//				return lTemp;
//			};
//		};

		// == bcList_Node ============================================================================= MRB ==
		template < typename T, class Allocator >
		class bcList_Node : public bcContainer< T, Allocator >
		{
		public:

		private:

		protected:
			struct Node : public bcContainerNode< ValueType >
			{
			public:
				Node* mNext;
				Node* mPrev;

				Node() : mNext(bcNULL), mPrev(bcNULL) {}
				Node(const ValueType& pValue) : bcContainerNode(pValue), mNext(bcNULL), mPrev(bcNULL) {}
				Node(ValueType&& pValue) : bcContainerNode(std::move(pValue)), mNext(bcNULL), mPrev(bcNULL) {}

				~Node() {}
			};
			typedef Node NodeType;

			bcInline NodeType* NewNode(NodeType* pNext, NodeType* pPrev)
			{
				NodeType* lP = (NodeType*)mAllocator.alloc(sizeof(NodeType));
				new (lP)NodeType;

				NodeNext(lP) = pNext;
				NodePrev(lP) = pPrev;

				NodePrev(pNext) = lP;
				NodeNext(pPrev) = lP;

				return lP;
			};
			bcInline NodeType* NewNode(NodeType* pNext, NodeType* pPrev, ValueType& pVal)
			{
				NodeType* lP = (NodeType*)mAllocator.alloc(sizeof(NodeType));
				new (lP)NodeType(pVal);

				NodeNext(lP) = pNext;
				NodePrev(lP) = pPrev;

				NodePrev(pNext) = lP;
				NodeNext(pPrev) = lP;

				return lP;
			};
			bcInline NodeType* NewNode(NodeType* pNext, NodeType* pPrev, ValueType&& pVal)
			{
				NodeType* lP = (NodeType*)mAllocator.alloc(sizeof(NodeType));
				new (lP) NodeType(std::forward<ValueType>(pVal));
				
				NodeNext(lP) = pNext;
				NodePrev(lP) = pPrev;

				NodePrev(pNext) = lP;
				NodeNext(pPrev) = lP;

				return lP;
			};
			bcInline void FreeNode(NodeType* pNode)
			{
				NodeNext(NodePrev(pNode)) = NodeNext(pNode);
				NodePrev(NodeNext(pNode)) = NodePrev(pNode);

				pNode->~Node();
				mAllocator.free(pNode);
			};
			bcInline void CopyNodes(NodeType*& pCpyHead, NodeType*& pOutHead)
			{
				NodeType* lCpyNode = NodeNext(pCpyHead);
				
				NodePrev(pOutHead) = pOutHead;
				NodeNext(pOutHead) = pOutHead;

				NodePtr lOutTail = pOutHead;

				NodeType* lNewNode;
				while(lCpyNode != pCpyHead)
				{
					lNewNode = (NodeType*)mAllocator.alloc(sizeof(NodeType));
					new (lNewNode)NodeType(std::move(NodeValue(lCpyNode)));

					NodePrev(lNewNode) = NodePrev(lOutTail);
					NodeNext(NodePrev(pOutTail)) = lNewNode;
					NodeNext(lNewNode) = pOutTail;
					NodePrev(pOutTail) = lNewNode;

					lCpyNode = NodeNext(lCpyNode);
				}
			};

			bcInline static NodeType*& NodeNext(const NodeType* pNode)
			{
				return pNode->mNext;
			};
			bcInline static NodeType*& NodePrev(const NodeType* pNode)
			{
				return return pNode->mPrev;
			};
			bcInline static ValueType& NodeValue(const NodeType* pNode)
			{
				return pNode->mValue;
			};

		public:
			bcList_Node() {};
			~bcList_Node() {};
		};

		// == bcList_Base ============================================================================= MRB ==
		template < typename T, class Allocator >
		class bcList_Base : public bcList_Node < T, Allocator >, public bcIteratorProvider< T >
		{
		private:

		protected:
			NodeType* mHead;
			//NodePtr mTail;

		public:
			bcList_Base() 
			{
				mHead = (NodeType*)mAllocator.alloc(sizeof(NodeType));
				new (mHead)NodeType;

				/*mTail = mAllocator.Alloc(sizeof(Node));
				new ((RawPointer)mTail) Value;

				NodeNext(mHead) = mTail;
				NodePrev(mHead) = NULL;

				NodePrev(mTail) = mHead;
				NodeNext(mTail) = NULL;*/

				NodeNext(mHead) = mHead;
				NodePrev(mHead) = mHead;
			};
			~bcList_Base() {};

			ValueType& nodeValue(NodeBaseType* pNode) override
			{
				return NodeValue(static_cast<NodeType*>(pNode));
			}
			NodeBaseType* moveForward(NodeBaseType* pNode) override
			{
				return NodeNext(static_cast<NodeType*>(pNode));
			}
			NodeBaseType* moveForward(NodeBaseType* pNode, bcUINT32 pCount) override
			{
				NodeType* lNode = static_cast<NodeType*>(pNode);
				for (bcUINT32 i = 0; i < pCount; ++i)
				{
					lNode = NodeNext(lNode);
				}
				return lNode;
			}
			NodeBaseType* moveBackward(NodeBaseType* pNode) override
			{
				return NodePrev(static_cast<NodeType*>(pNode));
			}
			NodeBaseType* moveBackward(NodeBaseType* pNode, bcUINT32 pCount) override
			{
				NodeType* lNode = static_cast<NodeType*>(pNode);
				for (bcUINT32 i = 0; i < pCount; ++i)
				{
					lNode = NodePrev(lNode);
				}
				return lNode;
			}
			bcBOOL isValid(const NodeBaseType* pNode) const override
			{ return (pNode && pNode != mHead); }
		};

		// == bcList =====================================================
		template < typename T, class Allocator = bcContainerAllocation<> >
		class bcList : public bcList_Base< T, Allocator >
		{
		public:
			typedef bcList < T, Allocator > ThisType;
			//typedef bcList_Iterator< bcList_Base< T, Allocator, PT > > Iterator;
			//typedef bcList_ConstIterator< bcList_Base< T, Allocator, PT > > Const_Iterator;
			typedef bcIterator< T > Iterator;
			typedef bcConstIterator< T > ConstIterator;
			typedef bcReference< T > Reference;
			typedef bcConstReference< T > ConstReference;
		private:

		protected:

		public:
			bcList();
			bcList(const ThisType& pOther);
			bcList(ThisType&& pOther);
			~bcList();

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

			Reference      At(bcUINT32 n);
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

		template < typename T, class Allocator >
		bcList< T, Allocator >::bcList()
		{
		};
		template < typename T, class Allocator >
		bcList< T, Allocator >::bcList(const ThisType& pOther)
		{
			Clear();
			mSize = pOther.mSize;
			mAllocator = pOther.mAllocator;
			CopyNodes(pOther.mHead, mHead)
		};
		template < typename T, class Allocator >
		bcList< T, Allocator >::bcList(ThisType&& pOther)
		{
			Clear();

			mSize = pOther.mSize;
			mAllocator = pOther.mAllocator;
			
			mHead = pOther.mHead;

			pOther.mHead = nullptr;
			
			pOther.Clear();
		};
		template < typename T, class Allocator >
		bcList< T, Allocator >::~bcList()
		{
			Clear();
			mAllocator.free(mHead);
			/*mAllocator.Free(mTail);*/
		};

		template < typename T, class Allocator >
		typename bcList< T, Allocator >::ThisType& 
		bcList< T, Allocator >::operator=(const ThisType& pOther)
		{
			if(this != pOther)
			{
				Clear();

				mSize = pOther.mSize;
				mAllocator = pOther.mAllocator;

				CopyNodes(pOther.mHead, mHead);
			}
		};
		template < typename T, class Allocator >
		void bcList< T, Allocator >::Swap(ThisType& pOther)
		{
			std::swap(mSize, pOther.mSize);
			std::swap(mAllocator, pOther.mAllocator);
			std::swap(mHead, pOther.mHead);
			/*Sawp(mTail, pOther.mTail);*/
		};

		template < typename T, class Allocator >
		void bcList< T, Allocator >::Assign(bcUINT32 n, const ValueType& pValue)
		{
			for(bcUINT32 i = 0; i < n; ++i)
				PushBack(pValue);
		};

		template < typename T, class Allocator >
		typename bcList< T, Allocator >::Iterator 
		bcList< T, Allocator >::Begin()
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(mSize > 0);
#endif
			return Iterator(NodeNext(mHead), this);
		};
		template < typename T, class Allocator >
		typename bcList< T, Allocator >::ConstIterator 
		bcList< T, Allocator >::Begin() const
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(mSize > 0);
#endif
			return ConstIterator(NodeNext(mHead), this);
		};

		template < typename T, class Allocator >
		typename bcList< T, Allocator >::Iterator 
		bcList< T, Allocator >::RBegin()
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(mSize > 0);
#endif
			return Iterator(NodePrev(mHead), this);
		};
		template < typename T, class Allocator >
		typename bcList< T, Allocator >::ConstIterator 
		bcList< T, Allocator >::RBegin() const
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(mSize > 0);
#endif
			return ConstIterator(NodePrev(mHead), this);
		};
        
		template < typename T, class Allocator >
		typename bcList< T, Allocator >::Iterator 
		bcList< T, Allocator >::End()
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(mSize > 0);
#endif
			/*return Iterator(NodePrev(mTail), this);*/
			return Iterator(mHead, this);
		};
		template < typename T, class Allocator >
		typename bcList< T, Allocator >::ConstIterator 
		bcList< T, Allocator >::End() const
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(mSize > 0);
#endif
			/*return Iterator(NodePrev(mTail), this);*/
			return ConstIterator(mHead, this);
		};

		template < typename T, class Allocator >
		typename bcList< T, Allocator >::Iterator 
		bcList< T, Allocator >::REnd()
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(mSize > 0);
#endif
			/*return Iterator(NodePrev(mTail), this);*/
			return Iterator(mHead, this);
		};
		template < typename T, class Allocator >
		typename bcList< T, Allocator >::ConstIterator 
		bcList< T, Allocator >::REnd() const
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(mSize > 0);
#endif
			/*return Iterator(NodePrev(mTail), this);*/
			return ConstIterator(mHead, this);
		};

		template < typename T, class Allocator >
		typename bcList< T, Allocator >::PointerType 
		bcList< T, Allocator >::Data()
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(mSize > 0);
#endif
			PointerType lData = (PointerType)mAllocator.alloc(sizeof(ValueType) * mSize);
			NodePtr lCurrNode = NodeNext(mHead);
			
			for(bcUINT32 i = 0; i < mSize; ++i, ++lCurrNode)
				new (lData + i) ValueType(std::move(NodeValue(lCurrNode)));

			return lData;
		};
		template < typename T, class Allocator >
		const typename bcList< T, Allocator >::PointerType 
		bcList< T, Allocator >::Data() const
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(mSize > 0);
#endif
			PointerType lData = (PointerType)mAllocator.alloc(sizeof(ValueType) * mSize);
			NodePtr lCurrNode = NodeNext(mHead);
			
			for(bcUINT32 i = 0; i < mSize; ++i, ++lCurrNode)
				new (lData + i) ValueType(std::move(NodeValue(lCurrNode)));

			return lData;

		};

		template < typename T, class Allocator >
		typename bcList< T, Allocator >::Reference 
		bcList< T, Allocator >::operator[](bcUINT32 n)
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(mSize > 0 && n <= mSize);
#endif
			NodeType* lNode;

			if(n <= mSize / 2)
			{
				lNode = mHead;
				for(bcUINT32 i = 0; i <= n; ++i)
					lNode = NodeNext(lNode);
			}
			else
			{
				/*lNode = mTail;*/
				lNode = mHead;
				for(bcUINT32 i = mSize; i >= n; ++i)
					lNode = NodePrev(lNode);
			}

			return Reference(lNode);
		};
		template < typename T, class Allocator >
		typename bcList< T, Allocator >::ConstReference 
		bcList< T, Allocator >::operator[](bcUINT32 n) const
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(mSize > 0 && n <= mSize);
#endif
			NodeType* lNode;

			if(n <= mSize / 2)
			{
				lNode = mHead;
				for(bcUINT32 i = 0; i <= n; ++i)
					lNode = NodeNext(lNode);
			}
			else
			{
				/*lNode = mTail;*/
				lNode = mHead;
				for(bcUINT32 i = mSize; i >= n; ++i)
					lNode = NodePrev(lNode);
			}

			return Reference(lNode);
		};

		template < typename T, class Allocator >
		typename bcList< T, Allocator >::Reference 
		bcList< T, Allocator >::At(bcUINT32 n)
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(mSize > 0 && n <= mSize);
#endif
			NodeType* lNode;

			if(n <= mSize / 2)
			{
				lNode = mHead;
				for(bcUINT32 i = 0; i <= n; ++i)
					lNode = NodeNext(lNode);
			}
			else
			{
				/*lNode = mTail;*/
				lNode = mHead;
				for(bcUINT32 i = mSize; i >= n; ++i)
					lNode = NodePrev(lNode);
			}

			return Reference(lNode);
		};
		template < typename T, class Allocator >
		typename bcList< T, Allocator >::ConstReference
		bcList< T, Allocator >::At(bcUINT32 n) const
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(mSize > 0 && n <= mSize);
#endif
			NodeType* lNode;

			if(n <= mSize / 2)
			{
				lNode = mHead;
				for(bcUINT32 i = 0; i <= n; ++i)
					lNode = NodeNext(lNode);
			}
			else
			{
				/*lNode = mTail;*/
				lNode = mHead;
				for(bcUINT32 i = mSize; i >= n; ++i)
					lNode = NodePrev(lNode);
			}

			return Reference(lNode);
		};

		template < typename T, class Allocator >
		typename bcList< T, Allocator >::Iterator 
		bcList< T, Allocator >::Find(ValueType& pValue)
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(mSize > 0);
#endif
			NodeType* lCurr = NodeNext(mHead);
			while(lCurr != mHead)
			{
				if(NodeValue(lCurr) == pValue)
					return Iterator(lCurr, this); 
				else
					lCurr = NodeNext(lCurr);
			}
			return Iterator(NULL, this);
		};
		template < typename T, class Allocator >
		typename bcList< T, Allocator >::ConstIterator
		bcList< T, Allocator >::Find(ValueType& pValue) const
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(mSize > 0);
#endif
			NodeType* lCurr = NodeNext(mHead);
			while(lCurr != mHead)
			{
				if(NodeValue(lCurr) == pValue)
					return Const_Iterator(lCurr, this); 
				else
					lCurr = NodeNext(lCurr);
			}
			return ConstIterator(NULL, this);
		};

		template < typename T, class Allocator >
		typename bcList< T, Allocator >::Iterator 
		bcList< T, Allocator >::PushBack(const ValueType& pValue)
		{
			++mSize;
			/*return Iterator(NewNode(mTail, NodePrev(mTail), pValue), this);*/
			return Iterator(NewNode(mHead, NodePrev(mHead), pValue), this);
		};
		template < typename T, class Allocator >
		typename bcList< T, Allocator >::Iterator 
		bcList< T, Allocator >::PushBack(ValueType&& pValue)
		{
			++mSize;
			/*return Iterator(NewNode(mTail, NodePrev(mTail), pValue), this);*/
			return Iterator(NewNode(mHead, NodePrev(mHead), std::forward<ValueType>(pValue)), this);
		};
		template < typename T, class Allocator >
		typename bcList< T, Allocator >::Iterator 
		bcList< T, Allocator >::PushBack()
		{
			++mSize;
			/*return Iterator(NewNode(mTail, NodePrev(mTail), pValue), this);*/
			return Iterator(NewNode(mHead, NodePrev(mHead), pValue), this);
		};

		template < typename T, class Allocator >
		void bcList< T, Allocator >::PopBack()
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(mSize > 0);
#endif
			--mSize;
			/*FreeNode(NodePrev(mTail));*/
			FreeNode(NodePrev(mHead));
		};

		template < typename T, class Allocator >
		typename bcList< T, Allocator >::Iterator 
		bcList< T, Allocator >::Insert(Iterator pPosition, const ValueType& pValue)
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(pPosition.Validate());
#endif
			++mSize;
			const NodeType* lP = static_cast<const NodeType*>(pPosition.getNode());
			return Iterator(NewNode(NodeNext(lP), lP, pValue), this);
		};
		template < typename T, class Allocator >
		typename bcList< T, Allocator >::Iterator 
		bcList< T, Allocator >::Insert(Iterator pPosition, ValueType&& pValue)
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(pPosition.Validate());
#endif
			++mSize;
			const NodeType* lP = static_cast<const NodeType*>(pPosition.getNode());
			return Iterator(NewNode(NodeNext(lP), lP, std::forward<ValueType>(pValue)), this);
		};

		template < typename T, class Allocator >
		void bcList< T, Allocator >::Erase(Iterator pPosition)
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(pPosition.Validate() && mSize > 0);
#endif
			--mSize;
			const NodeType* lP = static_cast<const NodeType*>(pPosition.getNode());
			FreeNode(lP);
		};
		template < typename T, class Allocator >
		void bcList< T, Allocator >::Erase(Iterator pFirst, Iterator pLast)
		{
#ifdef BC_CONTAINER_DEBUG
			bcAssert(pFirst.Validate() && pLast.Validate() && mSize > 0);
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

		template < typename T, class Allocator >
		void bcList< T, Allocator >::Clear()
		{
			if (mHead != bcNULL)
			{
				NodeType* lCurrNode = NodeNext(mHead);
				NodeType* lNextNode;

				while (lCurrNode && lCurrNode != mHead)
				{
					lNextNode = NodeNext(lCurrNode);
					(lCurrNode)->~Node();
					mAllocator.free(lCurrNode);
					lCurrNode = lNextNode;
				}
			}

			mSize = 0;
		};
	}
}

#endif