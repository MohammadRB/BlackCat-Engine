//  [9/16/2013 MRB]

#pragma once

#include "CorePCH.h"
#include "bcConcurrencyDefinition.h"
#include <chrono>
#include <mutex>
#include <condition_variable>
#include "bcAtomic.h"
#include "bcConditionVariable.h"

namespace BlackCat
{
	namespace Core
	{
		//typedef std::once_flag bcOnceFlag;

		//template< typename tFunc, typename ...tArgs >
		//void bcCallOnce(bcOnceFlag& pOnceFlag, tFunc&& pFunction, tArgs&&... pArguments)
		//{
		//	std::call_once(pOnceFlag, pFunction, std::forward<tArgs>(pArguments)...);
		//}

		//class BC_COREDLL_EXP bcLockable abstract
		//{
		//private:

		//protected:

		//public:
		//	bcInline virtual void lock() abstract;
		//	bcInline virtual void unlock() abstract;
		//	bcInline virtual bcBOOL tryLock() abstract;
		//};

		//class BC_COREDLL_EXP bcHybridLockable abstract : public bcLockable
		//{
		//private:

		//protected:

		//public:
		//	bcInline virtual void lock(bcLockOperation pLockOperation) abstract;
		//	//bcInline virtual void unlock() abstract;
		//	bcInline virtual bcBOOL tryLock(bcLockOperation pLockOperation) abstract;
		//};

		//class BC_COREDLL_EXP bcTimedLockable abstract : public bcLockable
		//{
		//private:

		//protected:

		//public:
		//	bcInline virtual bcBOOL tryLockFor(const bcUINT64 pNano) abstract;
		//};

		//template< bcConcurrencyLibrary CL >
		//class BC_COREDLL_EXP bcMutexBase
		//{
		//	// std::condition_variable_any in bcConditionVariableAny need access to std::mutex members
		//	friend class std::condition_variable_any;
		//private:
		//	typedef bcMutexBase< CL > ThisType;

		//protected:

		//public:
		//	bcMutexBase();
		//	~bcMutexBase();

		//	bcMutexBase(const ThisType&) = delete;
		//	ThisType& operator=(const ThisType&) = delete;

		//	bcInline void lock();
		//	bcInline void unlock();
		//	bcInline bcBOOL tryLock();
		//};

		//template< >
		//class BC_COREDLL_EXP bcMutexBase< bcConcurrencyLibrary_STD > : public bcLockable
		//{
		//private:
		//	typedef bcMutexBase< bcConcurrencyLibrary_STD > ThisType;

		//	std::mutex mMutex;

		//protected:

		//public:
		//	bcMutexBase() {};
		//	~bcMutexBase() {};

		//	bcMutexBase(const ThisType&) = delete;
		//	ThisType& operator=(const ThisType&) = delete;

		//	bcInline void lock() override { mMutex.lock(); };
		//	bcInline void unlock() override { mMutex.unlock(); };
		//	bcInline bcBOOL tryLock() override { return mMutex.try_lock(); };
		//};

		//template< bcConcurrencyLibrary CL >
		//class BC_COREDLL_EXP bcTimedMutexBase : public bcMutexBase< CL >
		//{
		//private:
		//	typedef bcTimedMutexBase< CL > ThisType;

		//protected:

		//public:
		//	bcTimedMutexBase();
		//	~bcTimedMutexBase();

		//	bcTimedMutexBase(const ThisType&) = delete;
		//	ThisType& operator=(const ThisType&) = delete;

		//	bcInline bcBOOL tryLockFor(const bcUINT64 pNano);
		//};

		//template< >
		//class BC_COREDLL_EXP bcTimedMutexBase< bcConcurrencyLibrary_STD > : bcTimedLockable
		//{
		//private:
		//	typedef bcTimedMutexBase< bcConcurrencyLibrary_STD > ThisType;

		//	std::timed_mutex mMutex;

		//protected:

		//public:
		//	bcTimedMutexBase() {};
		//	~bcTimedMutexBase() {};

		//	bcTimedMutexBase(const ThisType&) = delete;
		//	ThisType& operator=(const ThisType&) = delete;

		//	bcInline void lock() override { mMutex.lock(); };
		//	bcInline void unlock() override { mMutex.unlock(); };
		//	bcInline bcBOOL tryLock() override { return mMutex.try_lock(); };

		//	bcInline bcBOOL tryLockFor(const bcUINT64 pNano) override
		//	{
		//		return mMutex.try_lock_for(std::chrono::nanoseconds(pNano));
		//	};
		//};

		//template< bcConcurrencyLibrary CL >
		//class BC_COREDLL_EXP bcRecursiveMutexBase : public bcMutexBase< CL >
		//{
		//private:
		//	typedef bcRecursiveMutexBase< CL > ThisType;

		//protected:

		//public:
		//	bcRecursiveMutexBase();
		//	~bcRecursiveMutexBase();

		//	bcRecursiveMutexBase(const ThisType&) = delete;
		//	ThisType& operator=(const ThisType&) = delete;
		//};

		//template< >
		//class BC_COREDLL_EXP bcRecursiveMutexBase< bcConcurrencyLibrary_STD > : public bcLockable
		//{
		//private:
		//	typedef bcRecursiveMutexBase< bcConcurrencyLibrary_STD > ThisType;

		//	std::recursive_mutex mMutex;

		//protected:

		//public:
		//	bcRecursiveMutexBase() {};
		//	~bcRecursiveMutexBase() {};

		//	bcRecursiveMutexBase(const ThisType&) = delete;
		//	ThisType& operator=(const ThisType&) = delete;

		//	bcInline void lock() override { mMutex.lock(); };
		//	bcInline void unlock() override { mMutex.unlock(); };
		//	bcInline bcBOOL tryLock() override { return mMutex.try_lock(); };
		//};

		//template< bcConcurrencyLibrary CL >
		//class BC_COREDLL_EXP bcRecursiveTimedMutexBase : public bcTimedMutexBase< CL >
		//{
		//private:
		//	typedef bcRecursiveTimedMutexBase< CL > ThisType;

		//protected:

		//public:
		//	bcRecursiveTimedMutexBase();
		//	~bcRecursiveTimedMutexBase();

		//	bcRecursiveTimedMutexBase(const ThisType&) = delete;
		//	ThisType& operator=(const ThisType&) = delete;
		//};

		//template< >
		//class BC_COREDLL_EXP bcRecursiveTimedMutexBase< bcConcurrencyLibrary_STD > : public bcTimedLockable
		//{
		//private:
		//	typedef bcRecursiveTimedMutexBase< bcConcurrencyLibrary_STD > ThisType;

		//	std::recursive_timed_mutex mMutex;

		//protected:

		//public:
		//	bcRecursiveTimedMutexBase() {};
		//	~bcRecursiveTimedMutexBase() {};

		//	bcRecursiveTimedMutexBase(const ThisType&) = delete;
		//	ThisType& operator=(const ThisType&) = delete;

		//	bcInline void lock() override { mMutex.lock(); };
		//	bcInline void unlock() override { mMutex.unlock(); };
		//	bcInline bcBOOL tryLock() override { return mMutex.try_lock(); };

		//	bcInline bcBOOL tryLockFor(const bcUINT64 pNano) override
		//	{
		//		return mMutex.try_lock_for(std::chrono::nanoseconds(pNano));
		//	};
		//};

		//class BC_COREDLL_EXP bcMutex : public bcMutexBase< bcConcurrencyLibrary_STD >
		//{
		//private:

		//protected:

		//public:
		//	bcMutex() {};
		//	~bcMutex() {};

		//	bcMutex(const bcMutex&) = delete;
		//	bcMutex& operator=(const bcMutex&) = delete;
		//};

		//class BC_COREDLL_EXP bcTimedMutex : public bcTimedMutexBase< bcConcurrencyLibrary_STD >
		//{
		//private:

		//protected:

		//public:
		//	bcTimedMutex() {};
		//	~bcTimedMutex() {};

		//	bcTimedMutex(const bcTimedMutex&) = delete;
		//	bcTimedMutex& operator=(const bcTimedMutex&) = delete;
		//};

		//class BC_COREDLL_EXP bcRecursiveMutex : public bcRecursiveMutexBase< bcConcurrencyLibrary_STD >
		//{
		//private:

		//protected:

		//public:
		//	bcRecursiveMutex() {};
		//	~bcRecursiveMutex() {};

		//	bcRecursiveMutex(const bcRecursiveMutex&) = delete;
		//	bcRecursiveMutex& operator=(const bcRecursiveMutex&) = delete;
		//};

		//class BC_COREDLL_EXP bcRecursiveTimedMutex : public bcRecursiveTimedMutexBase< bcConcurrencyLibrary_STD >
		//{
		//private:

		//protected:

		//public:
		//	bcRecursiveTimedMutex() {};
		//	~bcRecursiveTimedMutex() {};

		//	bcRecursiveTimedMutex(const bcRecursiveTimedMutex&) = delete;
		//	bcRecursiveTimedMutex& operator=(const bcRecursiveTimedMutex&) = delete;
		//};

		//class BC_COREDLL_EXP bcLockGaurd
		//{
		//private:

		//protected:
		//	bcLockable* mLock;

		//public:
		//	bcLockGaurd(bcLockable& pLockable) : mLock(&pLockable)
		//	{
		//		mLock->lock();
		//	};
		//	bcLockGaurd(bcLockable& pLockable, bcLockProperty pLockProperties) : mLock(&pLockable)
		//	{
		//		bcAssert(pLockProperties == bcLock_Adapt);
		//	};
		//	bcLockGaurd(bcHybridLockable& pLockable, bcLockOperation pLockOperation)
		//	{
		//		mLock = static_cast<bcLockable*>(&pLockable);
		//		static_cast<bcHybridLockable*>(mLock)->lock(pLockOperation);
		//	}
		//	~bcLockGaurd()
		//	{
		//		mLock->unlock();
		//	};

		//	bcLockGaurd(const bcLockGaurd&) = delete;
		//	bcLockGaurd& operator=(const bcLockGaurd&) = delete;
		//};

		//class BC_COREDLL_EXP bcUniqueLock
		//{
		//private:

		//protected:
		//	bcLockable* mLock;
		//	bcBOOL mOwns;

		//	// For std calls we need this style coding
		//	friend class std::condition_variable_any;
		//	/*bcInline void lock()
		//	{
		//	mLock->lock();
		//	};
		//	bcInline void unlock()
		//	{
		//	mLock->UnLock();
		//	};*/

		//public:

		//	bcUniqueLock() : mLock(nullptr), mOwns(bcFALSE) {};
		//	bcUniqueLock(bcLockable& pLockable) : mLock(&pLockable), mOwns(bcFALSE)
		//	{
		//		mLock->lock();
		//		mOwns = bcTRUE;
		//	};
		//	bcUniqueLock(bcLockable& pLockable, bcLockProperty pLockProperties)
		//	{
		//		mLock = &pLockable;
		//		switch (pLockProperties)
		//		{
		//		case bcLock_Adapt:
		//			mOwns = bcTRUE;
		//			break;
		//		case bcLock_Defer:
		//			mOwns = bcFALSE;
		//			break;
		//		case bcLock_Try:
		//			mOwns = mLock->tryLock();
		//			break;
		//		default:
		//			break;
		//		}
		//	};
		//	bcUniqueLock(bcTimedLockable& pLockable, const bcINT64 pNano)
		//	{
		//		mLock = static_cast<bcLockable*>(&pLockable);
		//		mOwns = static_cast<bcTimedLockable*>(mLock)->tryLockFor(pNano);
		//	};
		//	bcUniqueLock(bcHybridLockable& pLockable, bcLockOperation pLockOperation)
		//	{
		//		mLock = static_cast<bcLockable*>(&pLockable);
		//		static_cast<bcHybridLockable*>(mLock)->lock(pLockOperation);
		//		mOwns = bcTRUE;
		//	};
		//	bcUniqueLock(bcHybridLockable& pLockable, bcLockProperty pLockProperties, bcLockOperation pLockOperation)
		//	{
		//		mLock = static_cast<bcLockable*>(&pLockable);
		//		switch (pLockProperties)
		//		{
		//		case bcLock_Adapt:
		//			mOwns = bcTRUE;
		//			break;
		//		case bcLock_Defer:
		//			mOwns = bcFALSE;
		//			break;
		//		case bcLock_Try:
		//			mOwns = static_cast<bcHybridLockable*>(mLock)->tryLock(pLockOperation);
		//			break;
		//		default:
		//			break;
		//		}
		//	};
		//	~bcUniqueLock()
		//	{
		//		if (mOwns)
		//			mLock->unlock();
		//	};

		//	bcUniqueLock(const bcUniqueLock&) = delete;
		//	bcUniqueLock& operator=(const bcUniqueLock&) = delete;

		//	bcUniqueLock(bcUniqueLock&& pOther)
		//	{
		//		mLock = std::move(pOther.mLock);
		//		mOwns = pOther.mOwns;
		//		pOther.mLock = nullptr;
		//		pOther.mOwns = bcFALSE;
		//	};
		//	bcUniqueLock& operator=(bcUniqueLock&& pOther)
		//	{
		//		if (this != &pOther)
		//		{
		//			if (mOwns)
		//				mLock->unlock();
		//			mLock = std::move(pOther.mLock);
		//			mOwns = pOther.mOwns;
		//			pOther.mLock = nullptr;
		//			pOther.mOwns = bcFALSE;
		//		}
		//		return *this;
		//	};
		//	bcInline void swap(bcUniqueLock& pOther)
		//	{
		//		std::swap(mLock, pOther.mLock);
		//		std::swap(mOwns, pOther.mOwns);
		//	};

		//	bcInline void lock()
		//	{
		//		mLock->lock();
		//		mOwns = bcTRUE;
		//	};
		//	bcInline bcBOOL tryLock()
		//	{
		//		mOwns = mLock->tryLock();
		//		return mOwns;
		//	};
		//	bcInline void lock(bcLockOperation pLockOperation)
		//	{
		//		static_cast<bcHybridLockable*>(mLock)->tryLock(pLockOperation);
		//		mOwns = bcTRUE;
		//	};
		//	bcInline bcBOOL tryLock(bcLockOperation pLockOperation)
		//	{
		//		mOwns = static_cast<bcHybridLockable*>(mLock)->tryLock(pLockOperation);
		//		return mOwns;
		//	};
		//	bcInline bcBOOL tryLockFor(const bcINT64 pNano)
		//	{
		//		mOwns = static_cast<bcTimedLockable*>(mLock)->tryLockFor(pNano);
		//		return mOwns;
		//	};
		//	bcInline void unlock()
		//	{
		//		mLock->unlock();
		//		mOwns = bcFALSE;
		//	};
		//	bcInline explicit operator bool() const
		//	{
		//		return mOwns;
		//	};
		//	bcInline bool ownsLock() const
		//	{
		//		return mOwns;
		//	};
		//	bcInline bcLockable* lockable() const
		//	{
		//		return mLock;
		//	};
		//	bcInline bcLockable* release()
		//	{
		//		bcLockable* lLock = mLock;
		//		mLock = nullptr;
		//		mOwns = bcFALSE;
		//		return lLock;
		//	};
		//};

		//template< typename ...M >
		//bcBOOL BC_COREDLL_EXP bcTryLock(M&... pLocks);

		//template< typename ...M >
		//bcBOOL bcTryLock(M&... pLocks)
		//{
		//	const bcUINT32 lCount = sizeof...(pLocks);
		//	bcLockable* lLocks[lCount] = { &pLocks... };

		//	bcINT32 lBreak = -1;
		//	for (bcUINT32 i = 0; i < lCount; ++i)
		//	{
		//		if (!lLocks[i]->tryLock())
		//		{
		//			lBreak = i;
		//			break;
		//		}
		//	}
		//	if (lBreak != -1)
		//	{
		//		for (bcUINT32 i = 0; i < lBreak; ++i)
		//		{
		//			lLocks[i]->unlock();
		//		}
		//		return bcFALSE;
		//	}
		//	return bcTRUE;
		//};

		//template< typename ...M >
		//bcBOOL BC_COREDLL_EXP bcTryLock(M&... pLocks, bcLockOperation pLockOperation);

		//template< typename ...M >
		//bcBOOL bcTryLock(M&... pLocks, bcLockOperation pLockOperation)
		//{
		//	const bcUINT32 lCount = sizeof...(pLocks);
		//	bcLockable* lLocks[lCount] = { &pLocks... };

		//	bcINT32 lBreak = -1;
		//	for (bcUINT32 i = 0; i < lCount; ++i)
		//	{
		//		if (!lLocks[i]->tryLock(pLockOperation))
		//		{
		//			lBreak = i;
		//			break;
		//		}
		//	}
		//	if (lBreak != -1)
		//	{
		//		for (bcUINT32 i = 0; i < lBreak; ++i)
		//		{
		//			lLocks[i]->unlock();
		//		}
		//		return bcFALSE;
		//	}
		//	return bcTRUE;
		//};

		//template< typename ...M >
		//bcBOOL BC_COREDLL_EXP bcLock(M&... pLocks);

		//template< typename ...M >
		//bcBOOL bcLock(M&... pLocks)
		//{
		//	while (!bcTryLock(pLocks...))
		//	{

		//	}
		//}

		//template< typename ...M >
		//bcBOOL BC_COREDLL_EXP bcLock(M&... pLocks, bcLockOperation pLockOperation);

		//template< typename ...M >
		//bcBOOL bcLock(M&... pLocks, bcLockOperation pLockOperation)
		//{
		//	while (!bcTryLock(pLocks..., pLockOperation))
		//	{

		//	}
		//}

		//template< typename ...M >
		//void BC_COREDLL_EXP bcUnLock(M&... pLocks);

		//template< typename ...M >
		//void bcUnLock(M&... pLocks)
		//{
		//	const bcUINT32 lCount = sizeof...(pLocks);
		//	bcLockable* lLocks[lCount] = { &pLocks... };

		//	for (bcUINT32 i = 0; i < lCount; ++i)
		//	{
		//		lLocks[i]->unlock();
		//	}
		//};

		//class BC_COREDLL_EXP bcHybridMutex : public bcHybridLockable
		//{
		//private:

		//protected:
		//	bcAtomic<bcINT32> mFlag;
		//	bcMutex mMutex;
		//	bcConditionVariableAny mCondition;

		//public:
		//	bcHybridMutex() { bcAtomicOperation::bcAtomicInit(mFlag, 0); };
		//	~bcHybridMutex() {};

		//	bcHybridMutex(const bcHybridMutex& pOther) = delete;
		//	bcHybridMutex& operator=(const bcHybridMutex& pOther) = delete;

		//	bcInline void lock(bcLockOperation pLockOperation) override
		//	{
		//		int lNewLoopCount = static_cast<int>(pLockOperation);
		//		int lLoopCounter = 0;
		//		int lExpected = 0;
		//		int lLoopCount = bcAtomicOperation::bcAtomicLoad(mFlag, bcMemoryOrder_SeqCst);

		//		while (true)
		//		{
		//			while (bcAtomicOperation::bcAtomicLoad(mFlag, bcMemoryOrder_SeqCst) != 0 &&
		//				lLoopCounter != lLoopCount)
		//				++lLoopCounter;
		//			bcAtomicOperation::bcAtomicCompareExchangeStrong(
		//				mFlag,
		//				&lExpected,
		//				lNewLoopCount,
		//				bcMemoryOrder_SeqCst,
		//				bcMemoryOrder_SeqCst);
		//			if (lExpected == 0)
		//			{
		//				return;
		//			}
		//			else if (lLoopCounter == lLoopCount)
		//			{
		//				lExpected = 0;
		//				bcUniqueLock lGuard(mMutex);
		//				mCondition.wait(
		//					lGuard,
		//					[&]()->bool { return bcAtomicOperation::bcAtomicLoad(mFlag, bcMemoryOrder_SeqCst) == 0; });
		//			}
		//			else
		//			{
		//				lExpected = 0;
		//				continue;
		//			}
		//		}
		//	};
		//	bcInline void lock() override
		//	{
		//		lock(bcLockOperation_Medium);
		//	};
		//	bcInline void unlock() override
		//	{
		//		bcLockGaurd lGuard(mMutex);
		//		bcAtomicOperation::bcAtomicStore(mFlag, 0, bcMemoryOrder_SeqCst);
		//		mCondition.notifyOne();
		//	};

		//	bcInline bcBOOL tryLock(bcLockOperation pLockOperation) override
		//	{
		//		int lNewLoopCount = static_cast<int>(pLockOperation);
		//		int lLoopCounter = 0;
		//		int lExpected = 0;
		//		int lLoopCount = bcAtomicOperation::bcAtomicLoad(mFlag, bcMemoryOrder_SeqCst);

		//		while (true)
		//		{
		//			while (bcAtomicOperation::bcAtomicLoad(mFlag, bcMemoryOrder_SeqCst) != 0 &&
		//				lLoopCounter != lLoopCount)
		//				++lLoopCounter;
		//			bcAtomicOperation::bcAtomicCompareExchangeStrong(
		//				mFlag,
		//				&lExpected,
		//				lNewLoopCount,
		//				bcMemoryOrder_SeqCst,
		//				bcMemoryOrder_SeqCst);
		//			if (lExpected == 0)
		//			{
		//				return bcTRUE;
		//			}
		//			else if (lLoopCounter == lLoopCount)
		//			{
		//				return bcFALSE;
		//			}
		//			else
		//			{
		//				lExpected = 0;
		//				continue;
		//			}
		//		}
		//	}
		//	bcInline bcBOOL tryLock() override
		//	{
		//		return tryLock(bcLockOperation_Medium);
		//	}
		//};


		typedef std::once_flag bcOnceFlag;

		template< typename tFunc, typename ...tArgs >
		void bcCallOnce(bcOnceFlag& pOnceFlag, tFunc&& pFunction, tArgs&&... pArguments)
		{
			std::call_once(pOnceFlag, pFunction, std::forward<tArgs>(pArguments)...);
		}

		class BC_COREDLL_EXP bcLockable
		{
		private:

		protected:

		public:
			bcInline virtual void lock() abstract;
			bcInline virtual void unlock() abstract;
			bcInline virtual bcBOOL tryLock() abstract;
		};

		class BC_COREDLL_EXP bcHybridLockable : public bcLockable
		{
		private:

		protected:

		public:
			bcInline virtual void lock(bcLockOperation pLockOperation) abstract;
			//bcInline virtual void unlock() abstract;
			bcInline virtual bcBOOL tryLock(bcLockOperation pLockOperation) abstract;
		};

		class BC_COREDLL_EXP bcTimedLockable : public bcLockable
		{
		private:

		protected:

		public:
			virtual bcBOOL tryLockFor(const bcUINT64 pNano) abstract;
		};

		template< typename Concrete >
		class BC_COREDLL_EXP bcIMutexBase : public bcLockable
		{
			//// std::condition_variable_any in bcConditionVariableAny need access to std::mutex members
			//friend class std::condition_variable_any;
		private:
			typedef Concrete ConcreteType;
			typedef bcIMutexBase< ConcreteType > ThisType;

		protected:

		public:
			bcIMutexBase() {};
			~bcIMutexBase() {};

			ThisType(const ThisType&) = delete;
			ThisType& operator=(const ThisType&) = delete;
		};

		template<bcConcurrencyLibrary CL>
		class bcMutexBase
		{};

		template< >
		class BC_COREDLL_EXP bcMutexBase< bcConcurrencyLibrary_STD > : public bcIMutexBase< bcMutexBase< bcConcurrencyLibrary_STD > >
		{
		private:
			typedef bcMutexBase< bcConcurrencyLibrary_STD > ThisType;

			std::mutex mMutex;

		protected:

		public:
			bcMutexBase() {};
			~bcMutexBase() {};

			bcMutexBase(const ThisType&) = delete;
			ThisType& operator=(const ThisType&) = delete;

			bcInline void lock() override { mMutex.lock(); };
			bcInline void unlock() override { mMutex.unlock(); };
			bcInline bcBOOL tryLock() override { return mMutex.try_lock(); };
		};

		template< typename Concrete >
		class BC_COREDLL_EXP bcITimedMutexBase : public bcIMutexBase< Concrete >, public bcTimedLockable
		{
		private:
			typedef Concrete ConcreteType;
			typedef bcITimedMutexBase< ConcreteType > ThisType;

		protected:

		public:
			bcITimedMutexBase() {};
			~bcITimedMutexBase() {};

			bcITimedMutexBase(const ThisType&) = delete;
			ThisType& operator=(const ThisType&) = delete;
		};

		template<bcConcurrencyLibrary CL>
		class bcTimedMutexBase
		{};

		template< >
		class BC_COREDLL_EXP bcTimedMutexBase< bcConcurrencyLibrary_STD > : public bcITimedMutexBase< bcTimedMutexBase< bcConcurrencyLibrary_STD > >
		{
		private:
			typedef bcTimedMutexBase< bcConcurrencyLibrary_STD > ThisType;

			std::timed_mutex mMutex;

		protected:

		public:
			bcTimedMutexBase() {};
			~bcTimedMutexBase() {};

			bcTimedMutexBase(const ThisType&) = delete;
			ThisType& operator=(const ThisType&) = delete;

			bcInline void lock() override { mMutex.lock(); };
			bcInline void unlock() override { mMutex.unlock(); };
			bcInline bcBOOL tryLock() override { return mMutex.try_lock(); };

			bcInline bcBOOL tryLockFor(const bcUINT64 pNano) override
			{
				return mMutex.try_lock_for(std::chrono::nanoseconds(pNano));
			};
		};

		template< typename Concrete >
		class BC_COREDLL_EXP bcIRecursiveMutexBase : public bcIMutexBase< Concrete >
		{
		private:
			typedef Concrete ConcreteType;
			typedef bcIRecursiveMutexBase< ConcreteType > ThisType;

			bcIRecursiveMutexBase(const ThisType&);
			ThisType& operator=(const ThisType&);

		protected:

		public:
			bcIRecursiveMutexBase() {};
			~bcIRecursiveMutexBase() {};
		};

		template<bcConcurrencyLibrary CL>
		class bcRecursiveMutexBase
		{};

		template< >
		class BC_COREDLL_EXP bcRecursiveMutexBase< bcConcurrencyLibrary_STD > : public bcIRecursiveMutexBase< bcRecursiveMutexBase< bcConcurrencyLibrary_STD > >
		{
		private:
			typedef bcRecursiveMutexBase< bcConcurrencyLibrary_STD > ThisType;

			std::recursive_mutex mMutex;

		protected:

		public:
			bcRecursiveMutexBase() {};
			~bcRecursiveMutexBase() {};

			bcRecursiveMutexBase(const ThisType&) = delete;
			ThisType& operator=(const ThisType&) = delete;

			bcInline void lock() override { mMutex.lock(); };
			bcInline void unlock() override { mMutex.unlock(); };
			bcInline bcBOOL tryLock() override { return mMutex.try_lock(); };
		};

		template< typename Concrete >
		class BC_COREDLL_EXP bcIRecursiveTimedMutexBase : public bcITimedMutexBase< Concrete >
		{
		private:
			typedef Concrete ConcreteType;
			typedef bcIRecursiveTimedMutexBase< ConcreteType > ThisType;

		protected:

		public:
			bcIRecursiveTimedMutexBase() {};
			~bcIRecursiveTimedMutexBase() {};

			bcIRecursiveTimedMutexBase(const ThisType&) = delete;
			ThisType& operator=(const ThisType&) = delete;
		};

		template<bcConcurrencyLibrary CL>
		class bcRecursiveTimedMutexBase
		{};

		template< >
		class BC_COREDLL_EXP bcRecursiveTimedMutexBase< bcConcurrencyLibrary_STD > : public bcIRecursiveTimedMutexBase< bcRecursiveTimedMutexBase< bcConcurrencyLibrary_STD > >
		{
		private:
			typedef bcRecursiveTimedMutexBase< bcConcurrencyLibrary_STD > ThisType;

			std::recursive_timed_mutex mMutex;

		protected:

		public:
			bcRecursiveTimedMutexBase() {};
			~bcRecursiveTimedMutexBase() {};

			bcRecursiveTimedMutexBase(const ThisType&) = delete;
			ThisType& operator=(const ThisType&) = delete;

			bcInline void lock() override { mMutex.lock(); };
			bcInline void unlock() override { mMutex.unlock(); };
			bcInline bcBOOL tryLock() override { return mMutex.try_lock(); };

			bcInline bcBOOL tryLockFor(const bcUINT64 pNano) override
			{
				return mMutex.try_lock_for(std::chrono::nanoseconds(pNano));
			};
		};

		class BC_COREDLL_EXP bcMutex : public bcMutexBase< bcConcurrencyLibrary_STD >
		{
		private:

		protected:

		public:
			bcMutex() {};
			~bcMutex() {};

			bcMutex(const bcMutex&) = delete;
			bcMutex& operator=(const bcMutex&) = delete;
		};

		class BC_COREDLL_EXP bcTimedMutex : public bcTimedMutexBase< bcConcurrencyLibrary_STD >
		{
		private:

		protected:

		public:
			bcTimedMutex() {};
			~bcTimedMutex() {};

			bcTimedMutex(const bcTimedMutex&) = delete;
			bcTimedMutex& operator=(const bcTimedMutex&) = delete;
		};

		class BC_COREDLL_EXP bcRecursiveMutex : public bcRecursiveMutexBase< bcConcurrencyLibrary_STD >
		{
		private:
			
		protected:

		public:
			bcRecursiveMutex() {};
			~bcRecursiveMutex() {};

			bcRecursiveMutex(const bcRecursiveMutex&) = delete;
			bcRecursiveMutex& operator=(const bcRecursiveMutex&) = delete;
		};

		class BC_COREDLL_EXP bcRecursiveTimedMutex : public bcRecursiveTimedMutexBase< bcConcurrencyLibrary_STD >
		{
		private:

		protected:

		public:
			bcRecursiveTimedMutex() {};
			~bcRecursiveTimedMutex() {};

			bcRecursiveTimedMutex(const bcRecursiveTimedMutex&) = delete;
			bcRecursiveTimedMutex& operator=(const bcRecursiveTimedMutex&) = delete;
		};

		class BC_COREDLL_EXP bcLockGaurd
		{
		private:

		protected:
			bcLockable* mLock;

		public:
			bcLockGaurd(bcLockable& pLockable) : mLock(&pLockable)
			{
				mLock->lock();
			};
			bcLockGaurd(bcLockable& pLockable, bcLockProperty pLockProperties) : mLock(&pLockable)
			{
				bcAssert(pLockProperties == bcLock_Adapt);
			};
			bcLockGaurd(bcHybridLockable& pLockable, bcLockOperation pLockOperation)
			{
				mLock = static_cast<bcLockable*>(&pLockable);
				static_cast<bcHybridLockable*>(mLock)->lock(pLockOperation);
			}
			~bcLockGaurd()
			{
				mLock->unlock();
			};

			bcLockGaurd(const bcLockGaurd&) = delete;
			bcLockGaurd& operator=(const bcLockGaurd&) = delete;
		};

		class BC_COREDLL_EXP bcUniqueLock
		{
		private:

		protected:
			bcLockable* mLock;
			bcBOOL mOwns;

			// For std calls we need this style coding
			friend class std::condition_variable_any;
			/*bcInline void lock()
			{
			mLock->lock();
			};
			bcInline void unlock()
			{
			mLock->UnLock();
			};*/

		public:

			bcUniqueLock() : mLock(nullptr), mOwns(bcFALSE) {};
			bcUniqueLock(bcLockable& pLockable) : mLock(&pLockable), mOwns(bcFALSE)
			{
				mLock->lock();
				mOwns = bcTRUE;
			};
			bcUniqueLock(bcLockable& pLockable, bcLockProperty pLockProperties)
			{
				mLock = &pLockable;
				switch (pLockProperties)
				{
				case bcLock_Adapt:
					mOwns = bcTRUE;
					break;
				case bcLock_Defer:
					mOwns = bcFALSE;
					break;
				case bcLock_Try:
					mOwns = mLock->tryLock();
					break;
				default:
					break;
				}
			};
			bcUniqueLock(bcTimedLockable& pLockable, const bcINT64 pNano)
			{
				mLock = static_cast<bcLockable*>(&pLockable);
				mOwns = static_cast<bcTimedLockable*>(mLock)->tryLockFor(pNano);
			};
			bcUniqueLock(bcHybridLockable& pLockable, bcLockOperation pLockOperation)
			{
				mLock =  static_cast<bcLockable*>(&pLockable);
				static_cast<bcHybridLockable*>(mLock)->lock(pLockOperation);
				mOwns = bcTRUE;
			};
			bcUniqueLock(bcHybridLockable& pLockable, bcLockProperty pLockProperties, bcLockOperation pLockOperation)
			{
				mLock =  static_cast<bcLockable*>(&pLockable);
				switch (pLockProperties)
				{
				case bcLock_Adapt:
					mOwns = bcTRUE;
					break;
				case bcLock_Defer:
					mOwns = bcFALSE;
					break;
				case bcLock_Try:
					mOwns = static_cast<bcHybridLockable*>(mLock)->tryLock(pLockOperation);
					break;
				default:
					break;
				}
			};
			~bcUniqueLock()
			{
				if(mOwns)
					mLock->unlock();
			};

			bcUniqueLock(const bcUniqueLock&) = delete;
			bcUniqueLock& operator=(const bcUniqueLock&) = delete;

			bcUniqueLock(bcUniqueLock&& pOther)
			{
				mLock = pOther.mLock;
				mOwns = pOther.mOwns;
				pOther.mLock = nullptr;
				pOther.mOwns = bcFALSE;
			};
			bcUniqueLock& operator=(bcUniqueLock&& pOther)
			{
				if(this != &pOther)
				{
					if(mOwns)
						mLock->unlock();
					mLock = pOther.mLock;
					mOwns = pOther.mOwns;
					pOther.mLock = nullptr;
					pOther.mOwns = bcFALSE;
				}
				return *this;
			};
			bcInline void swap(bcUniqueLock& pOther)
			{
				std::swap(mLock, pOther.mLock);
				std::swap(mOwns, pOther.mOwns);
			};

			bcInline void lock() 
			{
				mLock->lock();
				mOwns = bcTRUE;
			};
			bcInline bcBOOL tryLock() 
			{
				mOwns = mLock->tryLock();
				return mOwns;
			};
			bcInline void lock(bcLockOperation pLockOperation) 
			{
				static_cast<bcHybridLockable*>(mLock)->lock(pLockOperation);
				mOwns = bcTRUE;
			};
			bcInline bcBOOL tryLock(bcLockOperation pLockOperation) 
			{
				mOwns = static_cast<bcHybridLockable*>(mLock)->tryLock(pLockOperation);
				return mOwns;
			};
			bcInline bcBOOL tryLockFor(const bcINT64 pNano)
			{
				mOwns = static_cast<bcTimedLockable*>(mLock)->tryLockFor(pNano);
				return mOwns;
			};
			bcInline void unlock() 
			{
				mLock->unlock();
				mOwns = bcFALSE;
			};
			bcInline explicit operator bool() const
			{
				return mOwns;
			};
			bcInline bool ownsLock() const
			{
				return mOwns;
			};
			bcInline bcLockable* lockable() const
			{
				return mLock;
			};
			bcInline bcLockable* release()
			{
				bcLockable* lLock = mLock;
				mLock = nullptr;
				mOwns = bcFALSE;
				return lLock;
			};
		};

		template< typename ...M >
		bcBOOL BC_COREDLL_EXP bcTryLock(M&... pLocks);

		template< typename ...M >
		bcBOOL bcTryLock(M&... pLocks)
		{
			const bcUINT32 lCount = sizeof...(pLocks);
			bcLockable* lLocks[lCount] = { &pLocks... };

			bcINT32 lBreak = -1;
			for(bcUINT32 i = 0; i < lCount; ++i)
			{
				if(!lLocks[i]->tryLock())
				{
					lBreak = i;
					break;
				}
			}
			if(lBreak != -1)
			{
				for(bcUINT32 i = 0; i < lBreak; ++i)
				{
					lLocks[i]->unlock();
				}
				return bcFALSE;
			}
			return bcTRUE;
		};

		template< typename ...M >
		bcBOOL BC_COREDLL_EXP bcTryLock(M&... pLocks, bcLockOperation pLockOperation);

		template< typename ...M >
		bcBOOL bcTryLock(M&... pLocks, bcLockOperation pLockOperation)
		{
			const bcUINT32 lCount = sizeof...(pLocks);
			bcLockable* lLocks[lCount] = { &pLocks... };

			bcINT32 lBreak = -1;
			for(bcUINT32 i = 0; i < lCount; ++i)
			{
				if(!lLocks[i]->tryLock(pLockOperation))
				{
					lBreak = i;
					break;
				}
			}
			if(lBreak != -1)
			{
				for(bcUINT32 i = 0; i < lBreak; ++i)
				{
					lLocks[i]->unlock();
				}
				return bcFALSE;
			}
			return bcTRUE;
		};

		template< typename ...M >
		bcBOOL BC_COREDLL_EXP bcLock(M&... pLocks);

		template< typename ...M >
		bcBOOL bcLock(M&... pLocks)
		{
			while (!bcTryLock(pLocks...))
			{

			}
		}

		template< typename ...M >
		bcBOOL BC_COREDLL_EXP bcLock(M&... pLocks, bcLockOperation pLockOperation);

		template< typename ...M >
		bcBOOL bcLock(M&... pLocks, bcLockOperation pLockOperation)
		{
			while (!bcTryLock(pLocks..., pLockOperation))
			{

			}
		}

		template< typename ...M >
		void BC_COREDLL_EXP bcUnLock(M&... pLocks);

		template< typename ...M >
		void bcUnLock(M&... pLocks)
		{
			const bcUINT32 lCount = sizeof...(pLocks);
			bcLockable* lLocks[lCount] = { &pLocks... };

			for(bcUINT32 i = 0; i < lCount; ++i)
			{
				lLocks[i]->unlock();
			}
		};

		class BC_COREDLL_EXP bcHybridMutex : public bcHybridLockable
		{
		private:

		protected:
			bcAtomic<bcINT32> mFlag;
			bcMutex mMutex;
			bcConditionVariableAny mCondition;
			
		public:
			bcHybridMutex() { bcAtomicOperation::bcAtomicInit(mFlag, 0); };
			~bcHybridMutex() {};

			bcHybridMutex(const bcHybridMutex& pOther) = delete;
			bcHybridMutex& operator=(const bcHybridMutex& pOther) = delete;

			bcInline void lock(bcLockOperation pLockOperation) override  
			{
				int lNewLoopCount = static_cast<int>(pLockOperation);
				int lLoopCounter = 0;
				int lExpected = 0;
				int lLoopCount = bcAtomicOperation::bcAtomicLoad(mFlag, bcMemoryOrder_SeqCst);

				while (true)
				{
					while (bcAtomicOperation::bcAtomicLoad(mFlag, bcMemoryOrder_SeqCst) != 0 &&
						lLoopCounter != lLoopCount)
						++lLoopCounter;
					bcAtomicOperation::bcAtomicCompareExchangeStrong(
						mFlag, 
						&lExpected,
						lNewLoopCount,
						bcMemoryOrder_SeqCst,
						bcMemoryOrder_SeqCst);
					if(lExpected == 0)
					{
						return;
					}
					else if(lLoopCounter == lLoopCount)
					{
						lExpected = 0;
						bcUniqueLock lGuard(mMutex);
						mCondition.wait(
							lGuard, 
							[&]()->bool { return bcAtomicOperation::bcAtomicLoad(mFlag, bcMemoryOrder_SeqCst) == 0; });
					}
					else
					{
						lExpected = 0;
						continue;
					}
				}
			};
			bcInline void lock() override  
			{
				lock(bcLockOperation_Medium);
			};
			bcInline void unlock() override  
			{ 
				bcLockGaurd lGuard(mMutex);
				bcAtomicOperation::bcAtomicStore(mFlag, 0, bcMemoryOrder_SeqCst);
				mCondition.notifyOne();
			};

			bcInline bcBOOL tryLock(bcLockOperation pLockOperation) override  
			{
				int lNewLoopCount = static_cast<int>(pLockOperation);
				int lLoopCounter = 0;
				int lExpected = 0;
				int lLoopCount = bcAtomicOperation::bcAtomicLoad(mFlag, bcMemoryOrder_SeqCst);

				while (true)
				{
					while (bcAtomicOperation::bcAtomicLoad(mFlag, bcMemoryOrder_SeqCst) != 0 &&
						lLoopCounter != lLoopCount)
						++lLoopCounter;
					bcAtomicOperation::bcAtomicCompareExchangeStrong(
						mFlag, 
						&lExpected,
						lNewLoopCount,
						bcMemoryOrder_SeqCst,
						bcMemoryOrder_SeqCst);
					if(lExpected == 0)
					{
						return bcTRUE;
					}
					else if(lLoopCounter == lLoopCount)
					{
						return bcFALSE;
					}
					else
					{
						lExpected = 0;
						continue;
					}
				}
			}
			bcInline bcBOOL tryLock() override  
			{
				return tryLock(bcLockOperation_Medium);
			}
		};
	}
}