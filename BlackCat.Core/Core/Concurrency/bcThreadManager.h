//  [11/22/2013 MRB]

#pragma once

#include "CorePCH.h"
#include "bcSingleton.h"
#include "bcMemory.h"
#include "MemoryManagment.h"
#include "Ptr.h"
#include "bcFuture.h"
#include "bcAtomic.h"
#include "bcVector.h"
#include "bcThread.h"
#include "bcConcurrentQueue.h"
#include "bcDelegate.h"
#include "CoreUtility.h"
#include "bcConcurrencyUtility.h"
#include "bcConcurrencyDefinition.h"
#include "bcList.h"
#include "bcMutex.h"
#include "bcConditionVariable.h"

using namespace BlackCat::Core::CoreOS;

namespace BlackCat
{
	namespace Core
	{
//#define BC_NUM_THREAD 1

		template<typename T>
		class bcTaskLink;

		class bcThreadManager;

		template<typename T>
		class bcTask
		{
			typedef T Type;
			typedef bcTask<Type> ThisType;
			friend class bcTaskLink<T>;
			
		private:
			
		protected:
			bcFuture<Type> mFuture;
			// This future will set by bcTaskLink when bcTaskLink pick up with one thread in thread manager
			bcFuture<bcUINT32> mThreadIdFuture;

		public:
			bcTask() {}
			bcTask(const ThisType&) = delete;
			bcTask(ThisType&& pOther) : mFuture(std::move(pOther.mFuture)), mThreadIdFuture(std::move(pOther.mThreadIdFuture)) {}
			~bcTask() {}

			ThisType& operator =(const ThisType&) = delete;
			ThisType& operator =(ThisType&& pOther) { mFuture = std::move(pOther.mFuture); mThreadIdFuture = std::move(pOther.mThreadIdFuture); }

			Type get() { return mFuture.get(); }
			bcFutureStatus waitFor(const bcUINT64 pNano) const { return mFuture.waitFor(std::chrono.nanoseconds(pNano)); }
			// When executer thread see this interrupt, it will throw an exception
			void interruptExecuterThread() const
			{
				bcUINT32 lExcecuterThreadId = mThreadIdFuture.get();
				bcThreadManager::Get()->interruptThread(lExcecuterThreadId);
			}

			static ThisType startNew(bcDelegate< Type(void) > pDelegate)
			{
				return bcThreadManager::Get()->doTask(pDelegate);
			}
			static ThisType startNew(Type(*pFunc)(void))
			{
				return bcThreadManager::Get()->doTask(bcDelegate< Type(void) >(pFunc));
			}
		};

		template<typename T>
		class bcTaskLink
		{
			typedef T Type;
			typedef bcDelegate< Type(void) > DelegateType;
			typedef bcTaskLink<Type> ThisType;
		private:

		protected:
			DelegateType mDel;
			bcPromise<Type> mPromise;
			bcPromise<bcUINT32> mThreadIdPromise;

			bcInline bcFuture<Type> _getFuture() { return mPromise.getFuture(); }
			bcInline bcFuture<bcUINT32> _getThreadIdFuture() { return mThreadIdPromise.getFuture(); }

		public:
			bcTaskLink(DelegateType& pDel) : mDel(pDel) {}
			bcTaskLink(const ThisType&) = delete;
			bcTaskLink(ThisType&& pOther)
			{
				mDel = pOther.mDel;
				mPromise = std::move(pOther.mPromise);
				mThreadIdPromise = std::move(pOther.mThreadIdPromise);
			}
			~bcTaskLink() {}

			ThisType& operator =(const ThisType&) = delete;
			ThisType& operator =(ThisType&& pOther)
			{
				mDel = pOther.mDel;
				mPromise = std::move(pOther.mPromise);
				mThreadIdPromise = std::move(pOther.mThreadIdPromise);
			}

			void operator ()()
			{
				try
				{
					// Set executer thread id so in bcTask we can access to this id
					mThreadIdPromise.setValue(bcThreadManager::Get()->currentThreadId());
					Type lResult = mDel();
					mPromise.setValue(lResult);
				}
				catch (...)
				{
					mPromise.setException(std::current_exception());
				}
			}

			bcTask<Type> getTask()
			{
				bcTask<Type> lTask;
				lTask.mFuture = std::move(_getFuture());
				lTask.mThreadIdFuture = std::move(_getThreadIdFuture());
				return lTask;
			}
		};

		class bcThreadManager : public bcSingleton< bcThreadManager(bcUINT32) >
		{
		private:

		protected:
			class FunctionWrapper
			{
			private:

			protected:
				class ImpBase
				{
				public:
					virtual ~ImpBase() {}
					virtual void call() abstract;
				};

				template<typename F>
				class Imp : public ImpBase
				{
				public:
					typedef F FunctionType;

					FunctionType mFunc;

					Imp(FunctionType&& pFunc) : mFunc(std::move(pFunc)) {}
					~Imp() {}

					void call() override { mFunc(); }
				};

				bcUniquePtr<ImpBase> mPtr;

			public:
				FunctionWrapper() = default;
				FunctionWrapper(const FunctionWrapper&) = delete;
				FunctionWrapper(FunctionWrapper&& pOther) { mPtr = std::move(pOther.mPtr); }
				template<typename F>
				FunctionWrapper(F&& pFunc) : mPtr(bcNew(Alloc_FRM, 0) Imp<F>(std::move(pFunc))) {} // TODO check allocation type

				FunctionWrapper& operator =(const FunctionWrapper&) = delete;
				FunctionWrapper& operator =(FunctionWrapper&& pOther) { mPtr = std::move(pOther.mPtr); return *this; }

				void operator ()() { mPtr->call(); }
			};

			struct InterruptFlag
			{
			public:
				bcAtomic<bcBOOL> mFlag;

				InterruptFlag() : mFlag(bcFALSE) {}
				InterruptFlag(bcBOOL pFlag) : mFlag(pFlag) {}
				InterruptFlag(const InterruptFlag& pOther) 
				{ 
					bcAtomicOperation::bcAtomicStore(
						mFlag, 
						bcAtomicOperation::bcAtomicLoad(pOther.mFlag, bcMemoryOrder_SeqCst), 
						bcMemoryOrder_SeqCst); 
				}

				void set()
				{
					bcAtomicOperation::bcAtomicStore(mFlag, bcTRUE, bcMemoryOrder_SeqCst);
				}
				void unset()
				{
					bcAtomicOperation::bcAtomicStore(mFlag, bcFALSE, bcMemoryOrder_SeqCst);
				}
				bcBOOL compareExchangeFlag(bcBOOL* pExpected, bcBOOL pNew)
				{
					return bcAtomicOperation::bcAtomicCompareExchangeStrong(
							mFlag,
							pExpected,
							pNew,
							bcMemoryOrder_SeqCst);
				}

				bcBOOL isSet() { bcAtomicOperation::bcAtomicLoad(mFlag, bcMemoryOrder_SeqCst); }
			};

			void joinWorkers()
			{
				for (bcIterator<bcThread> lBegin = mThreads.Begin(); lBegin != mThreads.End(); ++lBegin)
				{
					if (lBegin->joinable()) lBegin->join();
				}
			}

			void workerSpin()
			{
				//// Get an index for this thread
				//bcThreadIndex::getMyIndex();

				bcNullable<FunctionWrapper> lTask;
				while (!bcAtomicOperation::bcAtomicLoad(mDone))
				{
					bcBOOL lDone = bcFALSE;
					bcUINT32 lQueueSize = mQueues.Size();
					for (bcUINT32 i = 0; i < lQueueSize; ++i)
					{
						if ((*mQueues[*mLastQueueIndex])->popFront(&lTask))
						{
							bcAtomicOperation::bcAtomicFetchSub(mWorkCount, 1U, bcMemoryOrder_SeqCst);

							lDone = bcTRUE;
							(*lTask)();
							*mLastQueueIndex = ++*mLastQueueIndex % lQueueSize;
							break;
						}
						*mLastQueueIndex = ++*mLastQueueIndex % lQueueSize;
					}

					if (!lDone)
					{
						bcUniqueLock lGaurd(mMutext);
						mCond.wait(
							lGaurd,
							[&]()->bcBOOL { return bcAtomicOperation::bcAtomicLoad(mWorkCount, bcMemoryOrder_SeqCst) != 0; });
						//currentThreadYield();
					}
				}
			}

			bcUINT32 mNumThread;
			bcAtomic<bcBOOL> mDone;
			bcVector<bcThread, 4> mThreads;
			bcVector<InterruptFlag, 4> mThreadInterruptFlags;
			bcVector<bcUniquePtr<bcConcurrentQueue<FunctionWrapper>>, 2> mQueues;
			// An index for looping around queues
			bcThreadLocal<bcUINT32, bcUINT32> mLastQueueIndex;

			bcAtomic<bcUINT32> mWorkCount;
			bcMutex mMutext;
			bcConditionVariableAny mCond;

		public:
			bcThreadManager() {}
			~bcThreadManager() {}

			void Initialize(bcUINT32 pNumSystemThread) override
			{
				//// Get an index for main thread
				//bcThreadIndex::getMyIndex();

				bcAtomicOperation::bcAtomicStore(mDone, bcFALSE);
				bcAtomicOperation::bcAtomicStore(mWorkCount, 0U, bcMemoryOrder_SeqCst);

				// If we pass 1 to mNumThread, no thread will be pushed to mThreads vector, so any task in this
				// thread pool won't execute, also in systems with 1 thread, we want one addition thread for
				// works that we don't want block main thread
				mNumThread = pNumSystemThread == 1 ? 2 : pNumSystemThread;
				mLastQueueIndex.Initialize(mNumThread, 0);

				try
				{
					bcUINT32 lNumQeueus = std::log2(mNumThread);
					for (bcUINT32 i = 0; i < lNumQeueus; ++i)
					{
						/*mQueues.PushBack(
						bcUniquePtr<bcConcurrentQueue<FunctionWrapper>>(
						bcNew(bcConcurrentQueue<FunctionWrapper>(mNumThread), Alloc_PRG, 0)));*/
						mQueues.PushBack(
							bcUniquePtr<bcConcurrentQueue<FunctionWrapper>>(
							bcNew(Alloc_PRG, 0) bcConcurrentQueue<FunctionWrapper>(mNumThread)));
					}

					// mNumThread - 1 because we have one main thread that doesn't exist in this thread pool
					for (bcUINT32 i = 0; i < mNumThread - 1; ++i)
					{
						mThreads.PushBack(bcThread(&bcThreadManager::workerSpin, this));
						mThreadInterruptFlags.PushBack(InterruptFlag(bcFALSE));
					}			

					
				}
				catch (...)
				{
					Destroy();
					throw;
				}

				mInitialized = bcTRUE;
			}
			void Destroy() override
			{
				bcAtomicOperation::bcAtomicStore(mDone, bcTRUE);
				{
					bcLockGaurd lGaurd(mMutext);
					// == Add a work count, so threads waiting in conditaion variable can wake up ==
					bcAtomicOperation::bcAtomicFetchAdd(mWorkCount, 1U, bcMemoryOrder_SeqCst);
					mCond.notifyAll();
				}
				joinWorkers();

				mThreads.Clear();
				mQueues.Clear();
				mThreadInterruptFlags.Clear();

				mLastQueueIndex.Destroy();

				mInitialized = bcFALSE;
			}

			template<typename T>
			bcTask<T> doTask(bcDelegate< T(void) >& pDelegate)
			{
				bcTaskLink<T> lTaskLink(pDelegate);
				bcTask<T> lTask = lTaskLink.getTask();

				bcAtomicOperation::bcAtomicFetchAdd(mWorkCount, 1U, bcMemoryOrder_SeqCst);

				(*mQueues[*mLastQueueIndex])->pushBack(std::move(lTaskLink));
				*mLastQueueIndex = ++*mLastQueueIndex % mQueues.Size();

				{
					bcLockGaurd lGuard(mMutext);
					mCond.notifyOne();
				}

				return lTask;
			}

			bcUINT32 currentThreadId() { return bcThread::currentThreadId(); }
			/*bcUINT32 currentThreadIndex() { return bcThreadIndex::getMyIndex(); }*/
			void currentThreadSleepFor(bcUINT64 pNano) { bcThread::currentThreadSleepFor(pNano); }
			void currentThreadYield() { bcThread::currentThreadYield(); }

			void interruptThread(bcUINT32 pThreadId)
			{
				bcINT32 lThreadPos = -1;
				for (bcINT32 i = 0; i < mThreads.Size(); ++i)
				{
					if (mThreads[i]->getId() == pThreadId)
					{
						lThreadPos = i;
						break;
					}
				}

				bcAssert(lThreadPos != -1);

				mThreadInterruptFlags[lThreadPos]->set();
			}
			void checkForInterruption()
			{
				bcUINT32 lThreadId = currentThreadId();
				bcINT32 lThreadPos = -1;
				for (bcINT32 i = 0; i < mThreads.Size(); ++i)
				{
					if (mThreads[i]->getId() == lThreadId)
					{
						lThreadPos = i;
						break;
					}
				}

				bcAssert(lThreadPos != -1);

				bcBOOL lExpected = bcTRUE;
				if (mThreadInterruptFlags[lThreadPos]->compareExchangeFlag(&lExpected, bcFALSE) == bcTRUE)
				{
					throw; // TODO create and throw InterruptException
				}
					
			}

			bcUINT32 numThread() { return mNumThread; }
		};

		void checkForInterruption();
	}
}