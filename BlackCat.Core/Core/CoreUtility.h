#pragma once

#ifndef CoreUtility_H
#define CoreUtility_H

#include "CorePCH.h"
//#include "CoreExceptions.h"

namespace BlackCat
{
	namespace Core
	{
		/*template < typename T >
		bcInline BC_COREDLL_EXP void Sawp(T& pFirst, T& pSecond)
		{
			T lTemp = pFirst;
			pFirst = pSecond;
			pSecond = pFirst;
		};*/

		/*template< typename T >
		class bcInitializable;*/

		template< typename ...A >
		class /*BC_COREDLL_EXP*/ bcInitializable
		{
		private:

		protected:
			bcBOOL mInitialized;

		public:
			bcInitializable() : mInitialized(bcFALSE) { };
			~bcInitializable() { bcAssert(!mInitialized); };

			virtual void Initialize(A... pArgs) abstract;
			virtual void Destroy() abstract;
		};

		template< typename T >
		class bcNullable
		{
			typedef T Type;
			typedef bcNullable< Type > ThisType;
		private:

		protected:
			bcBOOL mIsSet;
			Type mValue;
			
			void _set(const Type& pValue) { mValue = pValue; mIsSet = bcTRUE; }
			void _set(Type&& pValue) { mValue = std::move(pValue); mIsSet = bcTRUE; }
			bcBOOL _equal(ThisType& pOther) const
			{
				if (!mIsSet)
					return !pOther.mIsSet;

				return mValue == pOther.mValue;
			}
			bcBOOL _equal(nullptr_t n) const { return !mIsSet; }

		public:
			bcNullable() : mIsSet(bcFALSE) {}
			bcNullable(const Type& pValue) { _set(pValue); }
			bcNullable(Type&& pValue) { _set(std::forward<Type>(pValue)); }
			bcNullable(bcNULLT pNUll) throw() { mIsSet = bcFALSE; }
			~bcNullable() throw() {}

			Type& operator =(const Type& pValue) { _set(pValue); return mValue; }
			Type& operator =(Type&& pValue) { _set(std::forward<Type>(pValue)); return mValue; }
			void operator =(bcNULLT pNull) throw() { mIsSet = bcFALSE; mValue.~Type(); }

			bcBOOL operator ==(ThisType& pOther) const
			{
				return _equal(pOther);
			}
			bcBOOL operator ==(bcNULLT n) const { return _equal(n); }

			bcBOOL operator !=(ThisType& pOther) const
			{
				return !_equal(pOther);
			}
			bcBOOL operator !=(bcNULLT n) const { return !_equal(n); }

			Type* operator ->()
			{
				return &mValue;
			}
			Type& operator *()
			{
				return mValue;
			}

			Type get() const
			{
				if (mIsSet)
					return mValue;
				bcAssert(bcFALSE);
				//throw Exception("Value is null");
			}

			bcBOOL isSet() const throw() { return mIsSet; }

			void reset() throw() { mIsSet = bcFALSE; }
		};
	}
}

#endif