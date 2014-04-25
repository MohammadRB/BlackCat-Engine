//  [9/20/2013 MRB]

#pragma once

#include "CorePCH.h"
#include "bcConcurrencyDefinition.h"
#include <thread>

namespace BlackCat
{
	namespace Core
	{
		template< typename Concrete >
		class bcIThreadBase
		{
		private:
			typedef Concrete ConcreteType;
			typedef bcIThreadBase<ConcreteType> ThisType;

		protected:

		public:
			bcIThreadBase() throw() {};
			~bcIThreadBase() {};

			bcIThreadBase(const ThisType&) = delete;
			bcIThreadBase(ThisType&&) throw() {};

			/*template< typename Callable, typename ...Args >
			explicit bcIThreadBase(Callable&& pFunc, Args&&... pArgs);*/

			ThisType& operator=(const ThisType&) = delete;
			ThisType& operator=(ThisType&&) throw() {};

			bcInline virtual void swap(ConcreteType&) throw() abstract;
			bcInline virtual void join() abstract;
			bcInline virtual void detach() abstract;
			bcInline virtual bcBOOL joinable() const throw() abstract;
			bcInline virtual bcUINT32 getId() const throw() abstract;
		};

		template< bcConcurrencyLibrary CL >
		class bcThreadBase
		{};

		template< >
		class bcThreadBase< bcConcurrencyLibrary_STD > : bcIThreadBase<bcThreadBase< bcConcurrencyLibrary_STD >>
		{
		private:
			typedef bcThreadBase< bcConcurrencyLibrary_STD > ThisType;

			std::thread mThread;
		protected:

		public:
			bcThreadBase() throw() : mThread() {}
			~bcThreadBase() {}
			
			bcThreadBase(const ThisType&) = delete;
			bcThreadBase(ThisType&& pOther) throw()
			{
				mThread = std::move(pOther.mThread);
			}
			
			template< typename Callable, typename ...Args >
			explicit bcThreadBase(Callable&& pFunc, Args&&... pArgs) : mThread(std::forward<Callable>(pFunc), std::forward<Args>(pArgs)...) {}

			bcThreadBase& operator=(const ThisType&) = delete;
			ThisType& operator=(ThisType&& pOther) throw()
			{
				mThread = std::move(pOther.mThread);

				return *this;
			}

			bcInline void swap(ThisType& pOther) throw() override
			{
				mThread.swap(pOther.mThread);
			}
			bcInline void join() override
			{
				mThread.join();
			}
			bcInline void detach() override
			{
				mThread.detach();
			}
			bcInline bcBOOL joinable() const throw() override
			{
				return mThread.joinable();
			}
			bcInline bcUINT32 getId() const throw() override
			{
				return mThread.get_id().hash();
			}
		};

		class bcThread : public bcThreadBase< bcConcurrencyLibrary_STD >
		{
		private:
			typedef bcThread ThisType;

		protected:

		public:
			bcThread() throw() : bcThreadBase() {}
			~bcThread() {}

			bcThread(const ThisType&) = delete;
			bcThread(ThisType&& pOther) throw() : bcThreadBase(std::forward<bcThreadBase< bcConcurrencyLibrary_STD >>(pOther)) {}

			template< typename Callable, typename ...Args >
			explicit bcThread(Callable&& pFunc, Args&&... pArgs) : bcThreadBase(std::forward<Callable>(pFunc), std::forward<Args>(pArgs)...) {}

			bcThread& operator=(const ThisType&) = delete;
			ThisType& operator=(ThisType&& pOther) throw()
			{
				return static_cast<ThisType&>(static_cast<bcThreadBase< bcConcurrencyLibrary_STD >&>(*this) = std::move(pOther));
			}

			bcInline static bcUINT32 hardwareConcurrency() throw() { return std::thread::hardware_concurrency(); }
			bcInline static bcUINT32 currentThreadId() { return std::this_thread::get_id().hash(); }
			bcInline static void currentThreadSleepFor(bcUINT64 pNano) { std::this_thread::sleep_for(std::chrono::nanoseconds(pNano)); }
			bcInline static void currentThreadYield() { std::this_thread::yield(); }
		};
	}
}