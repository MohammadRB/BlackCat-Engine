//  [12/17/2013 MRB]

#pragma once

#include "CorePCH.h"

namespace BlackCat
{
	namespace Core
	{
		template< typename T >
		struct bcContainerNode
		{
		public:
			T mValue;

			bcContainerNode() {}
			bcContainerNode(const T& pValue) : mValue(pValue) {}
			bcContainerNode(T&& pValue) : mValue(std::move(pValue)) {}
		};

		/*template< typename T >
		class bcIteratorBase;
		template< typename T >
		class bcConstIterator;
		template< typename T >
		class bcIterator;
		template< typename T >
		class bcConstConcurrentIterator;
		template< typename T >
		class bcConcurrentIterator;
		template< typename T >
		bcConstReference;
		template< typename T >
		bcReference;*/

		// == bcReference ============================================================================= MRB ==

		template < typename T >
		class bcConstReference
		{
		public:
			typedef bcConstReference< T > ThisType;
			typedef T ValueType;
			typedef ValueType* PointerType;
			typedef ValueType* RawPointerType;
			typedef bcContainerNode< ValueType > NodeBaseType;

		private:

		protected:
			NodeBaseType* mNodePtr;

			bcInline ThisType& _assign(ThisType& pOther)
			{
				mNodePtr = pOther.mNodePtr;
				return *this;
			}
			bcInline bcINT32 _compare(ThisType& pOther)
			{
				return (mNodePtr == pOther.mPtr) ? 0 : (mNodePtr > pOther.mPtr) ? 1 : -1;
			}

		public:
			bcConstReference() { mNodePtr = bcNULL; }
			bcConstReference(bcNULLT pNull) { mNodePtr = bcNULL; }
			bcConstReference(NodeBaseType* pPointer) { mNodePtr = pPointer; }
			bcConstReference(ThisType& pOther)
			{
				_assign(pOther);
			}
			~bcConstReference() {}

			const ValueType* operator->() const
			{
#ifdef BC_CONTAINER_DEBUG
				bcAssert(mNodePtr);
#endif
				return &mNodePtr->mValue;
			}
			const ValueType& operator*() const
			{
#ifdef BC_CONTAINER_DEBUG
				bcAssert(mNodePtr);
#endif
				return mNodePtr->mValue;
			}

			ThisType& operator= (ThisType& pOther)
			{
				return _assign(pOther);
			}
			ThisType& operator= (bcNULLT pNull)
			{
				return mNodePtr = bcNULL;
			}

			bcBOOL operator== (ThisType& pOther) const
			{
				return (_compare(pOther) == 0);
			}
			bcBOOL operator== (bcNULLT pNull) const
			{
				return (mNodePtr == bcNULL);
			}
			bcBOOL operator!= (ThisType& pOther) const
			{
				return (_compare(pOther) != 0);
			}
			bcBOOL operator!= (bcNULLT pNull) const
			{
				return (mNodePtr == bcNULL);
			}

			NodeBaseType* getNode() { return mNodePtr; }
			RawPointerType get() { return mNodePtr.mValue; }
		};

		template < typename T >
		class bcReference : bcConstReference< T >
		{
		private:

		protected:
			bcInline ThisType& _assign(ThisType& pOther)
			{
				mNodePtr = pOther.mNodePtr;
				return *this;
			}

		public:
			bcReference() : bcConstReference(bcNULL) {}
			bcReference(bcNULLT pNull) : bcConstReference(bcNULL) {}
			bcReference(NodeBaseType* pPointer) : bcConstReference(pPointer) {}
			bcReference(ThisType& pOther) : bcConstReference(pOther) {}
			~bcReference() {}

			ValueType* operator->()
			{
#ifdef BC_CONTAINER_DEBUG
				bcAssert(mNodePtr);
#endif
				return &mNodePtr->mValue;
			}
			ValueType& operator*()
			{
#ifdef BC_CONTAINER_DEBUG
				bcAssert(mNodePtr);
#endif
				return mNodePtr->mValue;
			}

			ThisType& operator= (ThisType& pOther)
			{
				return _assign(pOther);
			}
			ThisType& operator= (bcNULLT pNull)
			{
				return mNodePtr = bcNULL;
			}
		};

		// == bcIterator ============================================================================== MRB ==
		template< typename T >
		class bcIteratorBase
		{
		public:
			typedef T ValueType;
			typedef ValueType* PointerType;
			typedef ValueType* RawPointerType;
			typedef bcContainerNode<ValueType> NodeBaseType;

		private:

		protected:

		public:
			bcIteratorBase() {};
			~bcIteratorBase() {};

			/*virtual ValueType* operator->() const abstract;
			virtual ValueType& operator*() const abstract;*/

			virtual NodeBaseType* getNode() abstract;
			// == Manually set node pointer, in concurrent iterators new node must have reference ==
			virtual void setNode(NodeBaseType* pNode) abstract;
			virtual const RawPointerType get() const abstract;
			virtual bcBOOL validate() const abstract;

			virtual bcReference<ValueType> toReference() abstract;
		};

		template< typename T >
		class bcConcurrentIteratorBase : public bcIteratorBase< T >
		{
		private:

		protected:

		public:
			bcConcurrentIteratorBase() {};
			~bcConcurrentIteratorBase() {};

			// == Release iterator reference and make pointers null ==
			virtual void releaseReference() abstract;
		};

		template< typename T >
		class bcIteratorProvider
		{
		public:
			typedef T Type;
			typedef bcIteratorProvider< Type > ThisType;
			typedef bcContainerNode< Type > NodeBaseType;

		private:

		protected:

		public:
			// == We use bcContainerNode in containers so any pointer to values in containers will point 
			// to container nodes == MRB ==

			virtual Type& nodeValue(NodeBaseType* pNode) abstract;
			virtual NodeBaseType* moveForward(NodeBaseType* pNode) abstract;
			virtual NodeBaseType* moveForward(NodeBaseType* pNode, bcUINT32 pCount) abstract;
			virtual NodeBaseType* moveBackward(NodeBaseType* pNode) abstract;
			virtual NodeBaseType* moveBackward(NodeBaseType* pNode, bcUINT32 pCount) abstract;
			virtual bcBOOL isValid(const NodeBaseType* pNode) const abstract;
		};

		template< typename T >
		class bcConcurrentIteratorProvider : public bcIteratorProvider<T>
		{
		private:

		protected:

		public:
			virtual void referenceIterator(bcConcurrentIteratorBase<Type>* pIte) abstract;
			virtual void releaseIterator(bcConcurrentIteratorBase<Type>* pIte) abstract;
		};

		template< typename T >
		class bcConstIterator : public bcIteratorBase< T >
		{
		public:
			typedef bcIteratorProvider< ValueType > ProviderType;
			typedef bcConstIterator< T > ThisType;

		private:

		protected:
			ProviderType* mProvider;
			NodeBaseType* mNodePtr;

			bcInline ThisType& _assign(NodeBaseType* pNode, ProviderType* pProvider)
			{
				mNodePtr = pNode;
				mProvider = pProvider;

				return *this;
			}
			bcInline ThisType& _assign(const ThisType& pOther)
			{
				mNodePtr = pOther.mNodePtr;
				mProvider = pOther.mProvider;

				return *this;
			}
			bcInline void _unAssign()
			{
				mNodePtr = bcNULL;
				mProvider = bcNULL;
			}
			bcInline bcINT32 _compare(ThisType& pOther) const
			{
				return (mNodePtr == pOther.mNodePtr) ? 0 : (mNodePtr > pOther.mNodePtr) ? 1 : -1;
			}

		public:
			bcConstIterator() : bcIteratorBase()
			{
				_assign(bcNULL, bcNULL);
			}

			bcConstIterator(NodeBaseType* pNode, ProviderType* pProvider)
				: bcIteratorBase()
			{
				_assign(pNode, pProvider);
			}

			bcConstIterator(const ThisType& pOther) : bcIteratorBase()
			{
				_assign(pOther);
			}

			~bcConstIterator() { _unAssign(); }

			const ValueType* operator->() const
			{
#ifdef BC_CONTAINER_DEBUG
				bcAssert(mNodePtr &&
					//mCont && 
					mProvider &&
					validate());
#endif
				return &(mProvider->nodeValue(mNodePtr));
			}
			const ValueType& operator*() const
			{
#ifdef BC_CONTAINER_DEBUG
				bcAssert(mNodePtr &&
					//mCont && 
					mProvider &&
					validate());
#endif
				return mProvider->nodeValue(mNodePtr);
			}

			ThisType& operator= (ThisType& pOther)
			{
				return _assign(pOther);
			}

			bcBOOL operator== (ThisType& pOther) const
			{
				return (_compare(pOther) == 0);
			}

			bcBOOL operator!= (ThisType& pOther) const
			{
				return (_compare(pOther) != 0);
			}

			ThisType operator+ (bcINT32 pNum) const
			{
				ThisType lTemp(*this);
#ifdef BC_CONTAINER_DEBUG
				bcAssert(lTemp.mNodePtr &&
					//lTemp.mCont &&
					lTemp.mProvider);
#endif
				lTemp.mNodePtr = mProvider->moveForward(lTemp.mNodePtr, pNum);

				return lTemp;
			};
			ThisType& operator+= (bcINT32 pNum)
			{
#ifdef BC_CONTAINER_DEBUG
				bcAssert(mNodePtr &&
					//mCont &&
					mProvider);
#endif
				mNodePtr = mProvider->moveForward(mNodePtr, pNum);

				return *this;
			};
			void operator++ ()
			{
#ifdef BC_CONTAINER_DEBUG
				bcAssert(mNodePtr &&
					//mCont &&
					mProvider);
#endif
				mNodePtr = mProvider->moveForward(mNodePtr);
			};
			void operator++ (bcINT32)
			{
#ifdef BC_CONTAINER_DEBUG
				bcAssert(mNodePtr &&
					//mCont &&
					mProvider);
#endif
				mNodePtr = mProvider->moveForward(mNodePtr);
			};
			ThisType operator- (bcINT32 pNum) const
			{
				ThisType lTemp(*this);
#ifdef BC_CONTAINER_DEBUG
				bcAssert(lTemp.mNodePtr &&
					//lTemp.mCont &&
					lTemp.mProvider);
#endif
				lTemp.mNodePtr = mProvider->moveBackward(lTemp.mNodePtr, pNum);

				return lTemp;
			};
			ThisType& operator-= (bcINT32 pNum)
			{
#ifdef BC_CONTAINER_DEBUG
				bcAssert(mNodePtr &&
					//mCont &&
					mProvider);
#endif
				mNodePtr = mProvider->moveBackward(mNodePtr, pNum);

				return *this;
			};
			void operator-- ()
			{
#ifdef BC_CONTAINER_DEBUG
				bcAssert(mNodePtr &&
					//mCont &&
					mProvider);
#endif
				mNodePtr = mProvider->moveBackward(mNodePtr);
			};
			void operator-- (bcINT32)
			{
#ifdef BC_CONTAINER_DEBUG
				bcAssert(mNodePtr &&
					//mCont &&
					mProvider);
#endif
				mNodePtr = mProvider->moveBackward(mNodePtr);
			};

			NodeBaseType* getNode() override
			{
				return mNodePtr;
			}
			void setNode(NodeBaseType* pNode) override
			{
				mNodePtr = pNode;
			}
			const RawPointerType get() const override
			{
				return &(mProvider->nodeValue(mNodePtr));
			}
			bcBOOL validate() const override
			{
				return mProvider->isValid(mNodePtr);
			}

			bcReference<ValueType> toReference() override { return bcReference<ValueType>(mNodePtr); }
		};

		template< typename T >
		class bcIterator : public bcConstIterator< T >
		{
		public:
			typedef bcIterator< T > ThisType;

		private:

		protected:
			bcInline ThisType& _assign(ThisType& pOther)
			{
				mNodePtr = pOther.mNodePtr;
				mProvider = pOther.mProvider

					return *this;
			}

		public:
			bcIterator() : bcConstIterator(bcNULL, bcNULL)
			{}

			bcIterator(NodeBaseType* pNode, ProviderType* pProvider)
				: bcConstIterator(pNode, pProvider)
			{}

			bcIterator(const ThisType& pOther) : bcConstIterator(pOther)
			{}

			~bcIterator() {}

			ValueType* operator->()
			{
#ifdef BC_CONTAINER_DEBUG
				bcAssert(mNodePtr &&
					//mCont && 
					mProvider &&
					validate());
#endif
				return &(mProvider->nodeValue(mNodePtr));
			}
			ValueType& operator*()
			{
#ifdef BC_CONTAINER_DEBUG
				bcAssert(mNodePtr &&
					//mCont && 
					mProvider &&
					validate());
#endif
				return mProvider->nodeValue(mNodePtr);
			}

			ThisType& operator= (ThisType& pOther)
			{
				return _assign(pOther);
			}

			ThisType operator+ (bcINT32 pNum) const
			{
				ThisType lTemp(*this);
#ifdef BC_CONTAINER_DEBUG
				bcAssert(lTemp.mNodePtr &&
					//lTemp.mCont &&
					lTemp.mProvider);
#endif
				lTemp.mNodePtr = mProvider->moveForward(lTemp.mNodePtr, pNum);

				return lTemp;
			};
			ThisType operator- (bcINT32 pNum) const
			{
				ThisType lTemp(*this);
#ifdef BC_CONTAINER_DEBUG
				bcAssert(lTemp.mNodePtr &&
					//lTemp.mCont &&
					lTemp.mProvider);
#endif
				lTemp.mNodePtr = mProvider->moveBackward(lTemp.mNodePtr, pNum);

				return lTemp;
			};
		};

		// == bcConcurrentIterator ==================================================================== MRB ==

		template< typename T >
		class bcConstConcurrentIterator : public bcConcurrentIteratorBase< T >
		{
		public:
			typedef bcConcurrentIteratorProvider< ValueType > ProviderType;
			typedef bcConstConcurrentIterator< T > ThisType;

		private:

		protected:
			NodeBaseType* mNodePtr;
			ProviderType* mProvider;

			bcInline ThisType& _assign(NodeBaseType* pNode, ProviderType* pProvider)
			{
				mProvider = pProvider;
				mNodePtr = pNode;

				if (mNodePtr)
					mProvider->referenceIterator(this);

				return *this;
			}
			bcInline ThisType& _assign(const ThisType& pOther)
			{
				_unAssign();
				return _assign(pOther.mNodePtr, pOther.mProvider);
			}
			bcInline void _unAssign()
			{
				if (mNodePtr)
					mProvider->releaseIterator(this);

				mNodePtr = bcNULL;
				mProvider = bcNULL;
			}
			bcInline bcINT32 _compare(ThisType& pOther) const
			{
				return (mNodePtr == pOther.mNodePtr) ? 0 : (mNodePtr > pOther.mNodePtr) ? 1 : -1;
			}

		public:
			bcConstConcurrentIterator() { _assign(bcNULL, bcNULL); }

			bcConstConcurrentIterator(NodeBaseType* pNodePtr, ProviderType* pProvider) { _assign(pNodePtr, pProvider); }

			bcConstConcurrentIterator(const ThisType& pOther) { _assign(pOther); }

			~bcConstConcurrentIterator() { _unAssign(); }

			const ValueType* operator->() const
			{
#ifdef BC_CONTAINER_DEBUG
				bcAssert(mNodePtr &&
					//mCont && 
					mProvider &&
					validate());
#endif
				return &(mProvider->nodeValue(mNodePtr));
			}
			const ValueType& operator*() const
			{
#ifdef BC_CONTAINER_DEBUG
				bcAssert(mNodePtr &&
					//mCont && 
					mProvider &&
					validate());
#endif
				return mProvider->nodeValue(mNodePtr);
			}

			ThisType& operator= (ThisType& pOther)
			{
				return _assign(pOther);
			}

			bcBOOL operator== (ThisType& pOther) const
			{
				return (_compare(pOther) == 0);
			}

			bcBOOL operator!= (ThisType& pOther) const
			{
				return (_compare(pOther) != 0);
			}

			ThisType operator+ (bcINT32 pNum) const
			{
				ThisType lTemp(*this);
#ifdef BC_CONTAINER_DEBUG
				bcAssert(lTemp.mNodePtr &&
					//lTemp.mCont &&
					lTemp.mProvider);
#endif
				lTemp.mNodePtr = mProvider->moveForward(lTemp.mNodePtr, pNum);

				return lTemp;
			};
			ThisType& operator+= (bcINT32 pNum)
			{
#ifdef BC_CONTAINER_DEBUG
				bcAssert(mNodePtr &&
					//mCont &&
					mProvider);
#endif
				mNodePtr = mProvider->moveForward(mNodePtr, pNum);

				return *this;
			};
			ThisType& operator++ ()
			{
#ifdef BC_CONTAINER_DEBUG
				bcAssert(mNodePtr &&
					//mCont &&
					mProvider);
#endif
				mNodePtr = mProvider->moveForward(mNodePtr);

				return *this;
			};
			ThisType& operator++ (bcINT32)
			{
#ifdef BC_CONTAINER_DEBUG
				bcAssert(mNodePtr &&
					//mCont &&
					mProvider);
#endif
				mNodePtr = mProvider->moveForward(mNodePtr);

				return *this;
			};
			ThisType operator- (bcINT32 pNum) const
			{
				ThisType lTemp(*this);
#ifdef BC_CONTAINER_DEBUG
				bcAssert(lTemp.mNodePtr &&
					//lTemp.mCont &&
					lTemp.mProvider);
#endif
				lTemp.mNodePtr = mProvider->moveBackward(lTemp.mNodePtr, pNum);

				return lTemp;
			};
			ThisType& operator-= (bcINT32 pNum)
			{
#ifdef BC_CONTAINER_DEBUG
				bcAssert(mNodePtr &&
					//mCont &&
					mProvider);
#endif
				mNodePtr = mProvider->moveBackward(mNodePtr, pNum);

				return *this;
			};
			ThisType& operator-- ()
			{
#ifdef BC_CONTAINER_DEBUG
				bcAssert(mNodePtr &&
					//mCont &&
					mProvider);
#endif
				mNodePtr = mProvider->moveBackward(mNodePtr);

				return *this;
			};
			ThisType& operator-- (bcINT32)
			{
#ifdef BC_CONTAINER_DEBUG
				bcAssert(mNodePtr &&
					//mCont &&
					mProvider);
#endif
				mNodePtr = mProvider->moveBackward(mNodePtr);

				return *this;
			};

			NodeBaseType* getNode() override
			{
				return mNodePtr;
			}
			void setNode(NodeBaseType* pNode) override
			{
				mNodePtr = pNode;
			}
			const RawPointerType get() const override
			{
				return &(mProvider->nodeValue(mNodePtr));
			}
			bcBOOL validate() const override
			{
				return mProvider->isValid(mNodePtr);
			}

			bcReference<ValueType> toReference() override { return bcReference<ValueType>(mNodePtr); }

			void releaseReference() override 
			{
#ifdef BC_CONTAINER_DEBUG
				bcAssert(mNodePtr != bcNULL);
#endif
				_unAssign();

			}
		};

		template< typename T >
		class bcConcurrentIterator : public bcConstConcurrentIterator< T >
		{
		public:
			typedef bcConcurrentIterator< T > ThisType;

		private:

		protected:
			bcInline ThisType& _assign(ThisType& pOther)
			{
				_unAssign();
				_assign(pOther->mNodePtr, pOther->mProvider);

				return *this;
			}

		public:
			bcConcurrentIterator() : bcConstConcurrentIterator()
			{}

			bcConcurrentIterator(NodeBaseType* pNodePtr, ProviderType* pProvider)
				: bcConstConcurrentIterator(pNodePtr, pProvider)
			{}

			bcConcurrentIterator(const ThisType& pOther) : bcConstConcurrentIterator(pOther) {}

			~bcConcurrentIterator() {}

			ValueType* operator->()
			{
#ifdef BC_CONTAINER_DEBUG
				bcAssert(mNodePtr &&
					//mCont && 
					mProvider &&
					validate());
#endif
				return &(mProvider->nodeValue(mNodePtr));
			}
			ValueType& operator*()
			{
#ifdef BC_CONTAINER_DEBUG
				bcAssert(mNodePtr &&
					//mCont && 
					mProvider &&
					validate());
#endif
				return mProvider->nodeValue(mNodePtr);
			}

			ThisType& operator= (ThisType& pOther)
			{
				return _assign(pOther);
			}

			ThisType operator+ (bcINT32 pNum) const
			{
				ThisType lTemp(*this);
#ifdef BC_CONTAINER_DEBUG
				bcAssert(lTemp.mNodePtr &&
					//lTemp.mCont &&
					lTemp.mProvider);
#endif
				lTemp.mNodePtr = mProvider->moveForward(lTemp.mNodePtr, pNum);

				return lTemp;
			};
			ThisType operator- (bcINT32 pNum) const
			{
				ThisType lTemp(*this);
#ifdef BC_CONTAINER_DEBUG
				bcAssert(lTemp.mNodePtr &&
					//lTemp.mCont &&
					lTemp.mProvider);
#endif
				lTemp.mNodePtr = mProvider->moveBackward(lTemp.mNodePtr, pNum);

				return lTemp;
			};
		};
	}
}