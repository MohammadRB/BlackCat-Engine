//  [9/18/2013 MRB]

#pragma once

#include "CorePCH.h"
#include "bcConcurrencyDefinition.h"
#include <condition_variable>
#include "bcMutex.h"

namespace BlackCat
{
	namespace Core
	{
		template< typename Concrete >
		class bcIConditionVariableAnyBase
		{
		private:
			typedef Concrete ConcreteType;
			typedef bcIConditionVariableAnyBase< ConcreteType > ThisType;

		protected:

		public:
			bcIConditionVariableAnyBase() {};
			~bcIConditionVariableAnyBase() {};

			bcIConditionVariableAnyBase(const ThisType&) = delete;
			bcIConditionVariableAnyBase& operator=(const ThisType&) = delete;
			/*bcConditionVariableAnyBase(ThisType&&);
			bcConditionVariableAnyBase& operator=(ThisType&&);*/

			bcInline virtual void notifyOne() abstract;
			bcInline virtual void notifyAll() abstract;
			template< typename Lockable >
			bcInline void wait(Lockable& pLock);
			template < typename Lockable, typename Predicate >
			bcInline void wait(Lockable& pLock, Predicate pPred);
			template < typename Lockable >
			bcInline bcBOOL waitFor(Lockable& pLock, const bcUINT64 pNano);
			template < typename Lockable, typename Predicate >
			bcInline bcBOOL waitFor(Lockable& pLock, const bcUINT64 pNano, Predicate pPred);
		};

		template<bcConcurrencyLibrary CL >
		class bcConditionVariableAnyBase;

		template< >
		class bcConditionVariableAnyBase< bcConcurrencyLibrary_STD > : bcIConditionVariableAnyBase<bcConditionVariableAnyBase< bcConcurrencyLibrary_STD >>
		{
		private:
			typedef bcConditionVariableAnyBase< bcConcurrencyLibrary_STD > ThisType;

			std::condition_variable_any mConVar;

		protected:

		public:
			bcConditionVariableAnyBase() {};
			~bcConditionVariableAnyBase() {};

			bcConditionVariableAnyBase(const ThisType&) = delete;
			bcConditionVariableAnyBase& operator=(const ThisType&) = delete;
			/*bcConditionVariableAnyBase(ThisType&&);
			bcConditionVariableAnyBase& operator=(ThisType&&);*/

			bcInline void notifyOne() override { mConVar.notify_one(); };
			bcInline void notifyAll() override { mConVar.notify_all(); };
			template< typename Lockable >
			bcInline void wait(Lockable& pLock)
			{
				mConVar.wait(pLock);
			};
			template < typename Lockable, typename Predicate >
			bcInline void wait(Lockable& pLock, Predicate pPred)
			{
				mConVar.wait(pLock, pPred);
			};
			template < typename Lockable >
			bcInline bcBOOL waitFor(Lockable& pLock, const bcUINT64 pNano)
			{
				return (mConVar.wait_for(pLock, std::chrono::nanoseconds(pNano)));
			};
			template < typename Lockable, typename Predicate >
			bcInline bcBOOL waitFor(Lockable& pLock, const bcUINT64 pNano, Predicate pPred)
			{
				return mConVar.wait_for(pLock, std::chrono::nanoseconds(pNano), pPred);
			};
		};

		class BC_COREDLL_EXP bcConditionVariableAny : public bcConditionVariableAnyBase< bcConcurrencyLibrary_STD >
		{
		private:

		protected:

		public:
		};

		//template< bcConcurrencyLibrary CL >
		//class bcConditionVariableAnyBase
		//{
		//private:
		//	typedef bcConditionVariableAnyBase< CL > ThisType;

		//	bcConditionVariableAnyBase(const ThisType&);
		//	bcConditionVariableAnyBase& operator=(const ThisType&);
		//	bcConditionVariableAnyBase(ThisType&&);
		//	bcConditionVariableAnyBase& operator=(ThisType&&);

		//protected:

		//public:
		//	bcConditionVariableAnyBase() {};
		//	~bcConditionVariableAnyBase() {};

		//	/*bcConditionVariableAny(const bcConditionVariableAny&) = delete;
		//	bcConditionVariableAny& operator=(const bcConditionVariableAny&) = delete;*/

		//	bcInline virtual void notifyOne() abstract;
		//	bcInline virtual void notifyAll() abstract;
		//	template< typename Lockable >
		//	bcInline void wait(Lockable& pLock);
		//	template < typename Lockable, typename Predicate >
		//	bcInline void wait(Lockable& pLock, Predicate pPred);
		//	template < typename Lockable >
		//	bcInline bcBOOL waitFor(Lockable& pLock, const bcUINT64 pNano);
		//	template < typename Lockable, typename Predicate >
		//	bcInline bcBOOL waitFor(Lockable& pLock, const bcUINT64 pNano, Predicate pPred);
		//};

		//template< >
		//class bcConditionVariableAnyBase< bcConcurrencyLibrary_STD > : public bcConditionVariableAnyBase< bcConcurrencyLibrary_None >
		//{
		//private:
		//	typedef bcConditionVariableAnyBase< bcConcurrencyLibrary_STD > ThisType;

		//	std::condition_variable_any mConVar;

		//	bcConditionVariableAnyBase(const ThisType&);
		//	bcConditionVariableAnyBase& operator=(const ThisType&);
		//	bcConditionVariableAnyBase(ThisType&&);
		//	bcConditionVariableAnyBase& operator=(ThisType&&);

		//protected:

		//public:
		//	bcConditionVariableAnyBase() {};
		//	~bcConditionVariableAnyBase() {};

		//	/*bcConditionVariableAny(const bcConditionVariableAny&) = delete;
		//	bcConditionVariableAny& operator=(const bcConditionVariableAny&) = delete;*/

		//	bcInline void notifyOne() override { mConVar.notify_one(); };
		//	bcInline void notifyAll() override { mConVar.notify_all(); };
		//	template< typename Lockable >
		//	bcInline void wait(Lockable& pLock)
		//	{
		//		mConVar.wait(pLock);
		//	};
		//	template < typename Lockable, typename Predicate >
		//	bcInline void wait(Lockable& pLock, Predicate pPred)
		//	{
		//		mConVar.wait(pLock, pPred);
		//	};
		//	template < typename Lockable >
		//	bcInline bcBOOL waitFor(Lockable& pLock, const bcUINT64 pNano)
		//	{
		//		return (mConVar.wait_for(pLock, std::chrono::nanoseconds(pNano)));
		//	};
		//	template < typename Lockable, typename Predicate >
		//	bcInline bcBOOL waitFor(Lockable& pLock, const bcUINT64 pNano, Predicate pPred)
		//	{
		//		return mConVar.wait_for(pLock, std::chrono::nanoseconds(pNano), pPred);
		//	};
		//};

		//class BC_COREDLL_EXP bcConditionVariableAny : public bcConditionVariableAnyBase< bcConcurrencyLibrary_STD >
		//{
		//private:

		//protected:

		//public:
		//};
	}
}