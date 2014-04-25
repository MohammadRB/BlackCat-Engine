//  [10/28/2013 MRB]

#pragma once

#include "CorePCH.h"
#include "bcAtomic.h"
#include "CoreOS.h"
#include "CoreUtility.h"

using namespace BlackCat::Core::CoreOS;

namespace BlackCat
{
	namespace Core
	{
		//// Sequential index for threads that access this class. Only use with threads in a thread pool
		//// unless you can get non sequential indices
		//class bcThreadIndex
		//{
		//private:

		//protected:
		//	static BC_COREDLL_EXP bcAtomic<bcUINT32> mLastIndex;
		//	static BC_Thread_Local bcINT32 mMyIndex;

		//public:
		//	/*bcThreadIndex()
		//	{
		//		bcAtomicOperation::bcAtomicStore(mLastIndex, 0U);
		//		mMyIndex = -1;
		//	}*/

		//	static bcUINT32 getMyIndex()
		//	{
		//		if (mMyIndex == -1)
		//			mMyIndex = bcAtomicOperation::bcAtomicFetchAdd(mLastIndex, 1U, bcMemoryOrder_Relaxed);
		//		return static_cast<bcUINT32>(mMyIndex);
		//		//return bcAtomicOperation::bcAtomicLoad(mLastIndex);
		//	}
		//};

		// This class use bcThreadIndex, so you must use it with threads in thread pool
		template<typename T, typename ...A>
		class bcThreadLocal : public bcInitializable<bcUINT32, A...> // Another bcUINT32 for pNumThread
		{
			typedef T Type;
		private:

		protected:
			Type* mData;
			bcUINT32 mNumThread;

			static bcAtomic<bcUINT32> mLastIndex;
			static BC_Thread_Local bcINT32 mMyIndex;

			static bcUINT32 getMyIndex()
			{
				if (mMyIndex == -1)
					mMyIndex = bcAtomicOperation::bcAtomicFetchAdd(mLastIndex, 1U, bcMemoryOrder_Relaxed);
				return static_cast<bcUINT32>(mMyIndex);
				//return bcAtomicOperation::bcAtomicLoad(mLastIndex);
			}

		public:
			bcThreadLocal() {}
			~bcThreadLocal() {}

			void Initialize(bcUINT32 pNumThread, A... pArgs) override
			{
				mNumThread = pNumThread;

				void* lData = std::malloc(mNumThread * sizeof(T));
				mData = reinterpret_cast<T*>(lData);

				for (bcUINT32 i = 0; i < mNumThread; ++i)
					new (&mData[i])Type(pArgs...);
				
				mInitialized = bcTRUE;
			}
			void Destroy() override
			{
				for (bcUINT32 i = 0; i < mNumThread; ++i)
					mData[i].~Type();
				std::free(mData);

				mInitialized = bcFALSE;
			}

			const Type* operator ->() const { return &get(); }
			Type* operator ->() { return &get(); }
			const Type& operator *() const { return get(); }
			Type& operator *() { return get(); }

			T& get() { bcAssert(getMyIndex() < mNumThread); return mData[getMyIndex()]; }
		};

		template<typename T, typename ...A>
		bcAtomic<bcUINT32> bcThreadLocal<T, A...>::mLastIndex(0);
		template<typename T, typename ...A>
		bcINT32 bcThreadLocal<T, A...>::mMyIndex = -1;

		class bcBackOff
		{
		private:

		protected:
			struct LocalData
			{
				bcUINT64 mTime;
				bcINT32 mTick;

				LocalData() : mTime(0), mTick(0) { }
				~LocalData() {}
			};

			bcUINT64 mMinTime;
			bcUINT64 mMaxTime;
			bcINT32 mTickThreshold;
			bcThreadLocal<LocalData> mLocals;

		public:
			bcBackOff(
				bcUINT32 pNumThread,
				bcUINT64 pMinTime = std::chrono::nanoseconds(100).count(), 
				bcUINT64 pMaxTime = std::chrono::microseconds(100).count() * 1000,
				bcINT32 pTickThreshold = 5) : 
				mMinTime(pMinTime), 
				mMaxTime(pMaxTime),
				mTickThreshold(pTickThreshold)
				//mLocals(pNumThread)
				{ mLocals.Initialize(pNumThread); }
			~bcBackOff() { mLocals.Destroy(); }

			void inc(bcINT32 pTickCount = 1)
			{
				LocalData& lLocal = mLocals.get();

				lLocal.mTick += pTickCount;
				while (lLocal.mTick >= mTickThreshold)
				{
					lLocal.mTime = lLocal.mTime * 2 + 1;
					lLocal.mTick -= mTickThreshold;
				}
			}
			void dec(bcINT32 pTickCount = 1)
			{
				LocalData& lLocal = mLocals.get();

				lLocal.mTick -= pTickCount;
				while (lLocal.mTick <= -mTickThreshold)
				{
					lLocal.mTime = lLocal.mTime / 2;
					lLocal.mTick += mTickThreshold;
				}
			}

			bcUINT64 getCurrentWaitTime() { LocalData& lLocal = mLocals.get(); return lLocal.mTime; }

			void reset()
			{
				LocalData& lLocal = mLocals.get();

				lLocal.mTick = 0;
				lLocal.mTime = mMinTime;
			}

			void wait()
			{
				LocalData& lLocal = mLocals.get();
				auto start = std::chrono::steady_clock::now();

				while (bcTRUE)
				{
					if (std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - start).count() > lLocal.mTime)
						break;
				}

			}
		};
	}
}