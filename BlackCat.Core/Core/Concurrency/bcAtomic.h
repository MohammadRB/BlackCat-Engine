//  [9/19/2013 MRB]

#pragma once

#include "CorePCH.h"
#include "bcConcurrencyDefinition.h"
#include <atomic>

namespace BlackCat
{
	namespace Core
	{
		//template< typename T >
		//struct bcAtomic;

		//template< bcConcurrencyLibrary CL, typename T >
		//struct bcAtomicBase
		//{
		//private:
		//	typedef bcAtomicBase< CL, T > ThisType;

		//	bcAtomicBase(const ThisType&);
		//	bcAtomicBase& operator=(const ThisType&);
		//	bcAtomicBase& operator=(const ThisType&) volatile;

		//protected:

		//public:
		//	bcAtomicBase() {}
		//	~bcAtomicBase() {}

		//	constexpr bcAtomicBase(T) {};
		//	/*T operator=(T) volatile {};
		//	T operator=(T) {};*/
		//};

		//template< typename T >
		//struct bcAtomicBase< bcConcurrencyLibrary_STD, T > : public bcAtomicBase< bcConcurrencyLibrary_None, T >
		//{
		//private:
		//	typedef bcAtomicBase< bcConcurrencyLibrary_STD, T > ThisType;

		//	std::atomic<T> mAtomic;

		//	bcAtomicBase(const ThisType&);
		//	bcAtomicBase& operator=(const ThisType&);
		//	bcAtomicBase& operator=(const ThisType&) volatile;

		//protected:

		//public:
		//	bcAtomicBase() {}
		//	~bcAtomicBase() {}

		//	constexpr bcAtomicBase(T pValue) : mAtomic(pValue)
		//	{
		//	}
		//	/*T operator=(T pValue) volatile
		//	{
		//		mAtomic = pValue;

		//		return pValue;
		//	}
		//	T operator=(T pValue)
		//	{
		//		mAtomic = pValue;

		//		return pValue;
		//	}*/
		//};


		struct bcAtomicFlag
		{
		private:
			typedef bcAtomicFlag ThisType;

			std::atomic_flag mFlag;

		protected:

		public:
			bcAtomicFlag() {}

			bcAtomicFlag(const ThisType&) = delete;
			bcAtomicFlag& operator=(const ThisType&) = delete;
			bcAtomicFlag& operator=(const ThisType&) volatile = delete;

			static bcInline bcBOOL bcAtomicFlagTestAndSet(volatile bcAtomicFlag& pFlag) throw()
			{
				return std::atomic_flag_test_and_set(&pFlag.mFlag);
			}
			static bcInline bcBOOL bcAtomicFlagTestAndSet(bcAtomicFlag& pFlag) throw()
			{
				return std::atomic_flag_test_and_set(&pFlag.mFlag);
			}
			static bcInline bcBOOL bcAtomicFlagTestAndSet(volatile bcAtomicFlag& pFlag, bcMemoryOrder pOrder) throw()
			{
				return std::atomic_flag_test_and_set_explicit(&pFlag.mFlag, static_cast<std::memory_order>(pOrder));
			}
			static bcInline bcBOOL bcAtomicFlagTestAndSet(bcAtomicFlag& pFlag, bcMemoryOrder pOrder) throw()
			{
				return std::atomic_flag_test_and_set_explicit(&pFlag.mFlag, static_cast<std::memory_order>(pOrder));
			}
			static bcInline void bcAtomicFlagClear(volatile bcAtomicFlag& pFlag) throw()
			{
				std::atomic_flag_clear(&pFlag.mFlag);
			}
			static bcInline void bcAtomicFlagClear(bcAtomicFlag& pFlag) throw()
			{
				std::atomic_flag_clear(&pFlag.mFlag);
			}
			static bcInline void bcAtomicFlagClear(volatile bcAtomicFlag& pFlag, bcMemoryOrder pOrder) throw()
			{
				std::atomic_flag_clear_explicit(&pFlag.mFlag, static_cast<std::memory_order>(pOrder));
			}
			static bcInline void bcAtomicFlagClear(bcAtomicFlag& pFlag, bcMemoryOrder pOrder) throw()
			{
				std::atomic_flag_clear_explicit(&pFlag.mFlag, static_cast<std::memory_order>(pOrder));
			}
		};

		template< typename T >
		struct bcAtomic /*: public bcAtomicBase< bcConcurrencyLibrary_STD, T >*/
		{
			friend class bcAtomicOperation;
		private:
			typedef bcAtomic< T > ThisType;

			std::atomic< T > mAtomic;

		protected:

		public:
			bcAtomic() {}
			~bcAtomic() {}

			bcAtomic(T pValue) : mAtomic(pValue)
			{
			}

			bcAtomic(const ThisType&) = delete;
			bcAtomic& operator=(const ThisType&) = delete;
			bcAtomic& operator=(const ThisType&) volatile = delete;
		};

		static class bcAtomicOperation
		{
		private:

		protected:

		public:
			template<typename T>
			static bcInline bcBOOL bcAtomicIsLockFree(volatile const bcAtomic<T>& pAtomic) throw()
			{
				return std::atomic_is_lock_free(&pAtomic.mAtomic);
			}
			template<typename T>
			static bcInline bcBOOL bcAtomicIsLockFree(const bcAtomic<T>& pAtomic) throw()
			{
				return std::atomic_is_lock_free(&pAtomic.mAtomic);
			}

			template<typename T>
			static bcInline void bcAtomicInit(volatile bcAtomic<T>& pAtomic, T pValue) throw()
			{
				std::atomic_init(&pAtomic.mAtomic, pValue);
			}
			template<typename T>
			static bcInline void bcAtomicInit(bcAtomic<T>& pAtomic, T pValue) throw()
			{
				std::atomic_init(&pAtomic.mAtomic, pValue);
			}

			template<typename T>
			static bcInline T bcAtomicExchange(volatile bcAtomic<T>& pAtomic, T pValue) throw()
			{
				return std::atomic_exchange(&pAtomic.mAtomic, pValue);
			}
			template<typename T>
			static bcInline T bcAtomicExchange(bcAtomic<T>& pAtomic, T pValue) throw()
			{
				return std::atomic_exchange(&pAtomic.mAtomic, pValue);
			}
			template<typename T>
			static bcInline T bcAtomicExchange(
				volatile bcAtomic<T>& pAtomic,
				T pValue,
				bcMemoryOrder pOrder) throw()
			{
				return std::atomic_exchange_explicit(&pAtomic.mAtomic, pValue, static_cast<std::memory_order>(pOrder));
			}
			template<typename T>
			static bcInline T bcAtomicExchange(bcAtomic<T>& pAtomic, T pValue, bcMemoryOrder pOrder) throw()
			{
				return std::atomic_exchange_explicit(&pAtomic.mAtomic, pValue, static_cast<std::memory_order>(pOrder));
			}

			template<typename T>
			static bcInline void bcAtomicStore(volatile bcAtomic<T>& pAtomic, T pValue) throw()
			{
				std::atomic_store(&pAtomic.mAtomic, pValue);
			}
			template<typename T>
			static bcInline void bcAtomicStore(bcAtomic<T>& pAtomic, T pValue) throw()
			{
				std::atomic_store(&pAtomic.mAtomic, pValue);
			}
			template<typename T>
			static bcInline void bcAtomicStore(
				volatile bcAtomic<T>& pAtomic,
				T pValue,
				bcMemoryOrder pOrder) throw()
			{
				std::atomic_store_explicit(&pAtomic.mAtomic, pValue, static_cast<std::memory_order>(pOrder));
			}
			template<typename T>
			static bcInline void bcAtomicStore(bcAtomic<T>& pAtomic, T pValue, bcMemoryOrder pOrder) throw()
			{
				std::atomic_store_explicit(&pAtomic.mAtomic, pValue, static_cast<std::memory_order>(pOrder));
			}

			template<typename T>
			static bcInline T bcAtomicLoad(volatile const bcAtomic<T>& pAtomic) throw()
			{
				return std::atomic_load(&pAtomic.mAtomic);
			}
			template<typename T>
			static bcInline T bcAtomicLoad(const bcAtomic<T>& pAtomic) throw()
			{
				return std::atomic_load(&pAtomic.mAtomic);
			}
			template<typename T>
			static bcInline T bcAtomicLoad(volatile const bcAtomic<T>& pAtomic, bcMemoryOrder pOrder) throw()
			{
				return std::atomic_load_explicit(&pAtomic.mAtomic, static_cast<std::memory_order>(pOrder));
			}
			template<typename T>
			static bcInline T bcAtomicLoad(const bcAtomic<T>& pAtomic, bcMemoryOrder pOrder) throw()
			{
				return std::atomic_load_explicit(&pAtomic.mAtomic, static_cast<std::memory_order>(pOrder));
			}

			template<typename T>
			static bcInline T bcAtomicFetchAdd(volatile bcAtomic<T>& pAtomic, T pToAdd) throw()
			{
				static_assert(!std::is_same< T, bcBOOL >::value, "bcAtomicFetchAdd is not supported with bool");
				return std::atomic_fetch_add(&pAtomic.mAtomic, pToAdd);
			}
			template<typename T>
			static bcInline T bcAtomicFetchAdd(bcAtomic<T>& pAtomic, T pToAdd) throw()
			{
				static_assert(!std::is_same< T, bcBOOL >::value, "bcAtomicFetchAdd is not supported with bool");
				return std::atomic_fetch_add(&pAtomic.mAtomic, pToAdd);
			}
			template<typename T>
			static bcInline T bcAtomicFetchAdd(volatile bcAtomic<T>& pAtomic, T pToAdd, bcMemoryOrder pOrder) throw()
			{
				static_assert(!std::is_same< T, bcBOOL >::value, "bcAtomicFetchAdd is not supported with bool");
				return std::atomic_fetch_add_explicit(&pAtomic.mAtomic, pToAdd, static_cast<std::memory_order>(pOrder));
			}
			template<typename T>
			static bcInline T bcAtomicFetchAdd(bcAtomic<T>& pAtomic, T pToAdd, bcMemoryOrder pOrder) throw()
			{
				static_assert(!std::is_same< T, bcBOOL >::value, "bcAtomicFetchAdd is not supported with bool");
				return std::atomic_fetch_add_explicit(&pAtomic.mAtomic, pToAdd, static_cast<std::memory_order>(pOrder));
			}

			template<typename T>
			static bcInline T bcAtomicFetchSub(volatile bcAtomic<T>& pAtomic, T pToSub) throw()
			{
				static_assert(!std::is_same< T, bcBOOL >::value, "bcAtomicFetchAdd is not supported with bool");
				return std::atomic_fetch_sub(&pAtomic.mAtomic, pToSub);
			}
			template<typename T>
			static bcInline T bcAtomicFetchSub(bcAtomic<T>& pAtomic, T pToSub) throw()
			{
				static_assert(!std::is_same< T, bcBOOL >::value, "bcAtomicFetchAdd is not supported with bool");
				return std::atomic_fetch_sub(&pAtomic.mAtomic, pToSub);
			}
			template<typename T>
			static bcInline T bcAtomicFetchSub(volatile bcAtomic<T>& pAtomic, T pToSub, bcMemoryOrder pOrder) throw()
			{
				static_assert(!std::is_same< T, bcBOOL >::value, "bcAtomicFetchAdd is not supported with bool");
				return std::atomic_fetch_sub_explicit(&pAtomic.mAtomic, pToSub, static_cast<std::memory_order>(pOrder));
			}
			template<typename T>
			static bcInline T bcAtomicFetchSub(bcAtomic<T>& pAtomic, T pToSub, bcMemoryOrder pOrder) throw()
			{
				static_assert(!std::is_same< T, bcBOOL >::value, "bcAtomicFetchAdd is not supported with bool");
				return std::atomic_fetch_sub_explicit(&pAtomic.mAtomic, pToSub, static_cast<std::memory_order>(pOrder));
			}

			template<typename T>
			static bcInline T bcAtomicFetchAnd(volatile bcAtomic<T>& pAtomic, T pToAnd) throw()
			{
				static_assert(!std::is_same< T, bcBOOL >::value, "bcAtomicFetchAnd is not supported with bool");
				return std::atomic_fetch_and(&pAtomic.mAtomic, pToAnd);
			}
			template<typename T>
			static bcInline T bcAtomicFetchAnd(bcAtomic<T>& pAtomic, T pToAnd) throw()
			{
				static_assert(!std::is_same< T, bcBOOL >::value, "bcAtomicFetchAnd is not supported with bool");
				return std::atomic_fetch_and(&pAtomic.mAtomic, pToAnd);
			}
			template<typename T>
			static bcInline T bcAtomicFetchAnd(volatile bcAtomic<T>& pAtomic, T pToAnd, bcMemoryOrder pOrder) throw()
			{
				static_assert(!std::is_same< T, bcBOOL >::value, "bcAtomicFetchAnd is not supported with bool");
				return std::atomic_fetch_and_explicit(&pAtomic.mAtomic, pToAnd, static_cast<std::memory_order>(pOrder));
			}
			template<typename T>
			static bcInline T bcAtomicFetchAnd(bcAtomic<T>& pAtomic, T pToAnd, bcMemoryOrder pOrder) throw()
			{
				static_assert(!std::is_same< T, bcBOOL >::value, "bcAtomicFetchAnd is not supported with bool");
				return std::atomic_fetch_and_explicit(&pAtomic.mAtomic, pToAnd, static_cast<std::memory_order>(pOrder));
			}

			template<typename T>
			static bcInline T bcAtomicFetchOr(volatile bcAtomic<T>& pAtomic, T pToOr) throw()
			{
				static_assert(!std::is_same< T, bcBOOL >::value, "bcAtomicFetchOr is not supported with bool");
				return std::atomic_fetch_or(&pAtomic.mAtomic, pToOr);
			}
			template<typename T>
			static bcInline T bcAtomicFetchOr(bcAtomic<T>& pAtomic, T pToOr) throw()
			{
				static_assert(!std::is_same< T, bcBOOL >::value, "bcAtomicFetchOr is not supported with bool");
				return std::atomic_fetch_or(&pAtomic.mAtomic, pToOr);
			}
			template<typename T>
			static bcInline T bcAtomicFetchOr(volatile bcAtomic<T>& pAtomic, T pToOr, bcMemoryOrder pOrder) throw()
			{
				static_assert(!std::is_same< T, bcBOOL >::value, "bcAtomicFetchOr is not supported with bool");
				return std::atomic_fetch_or_explicit(&pAtomic.mAtomic, pToOr, static_cast<std::memory_order>(pOrder));
			}
			template<typename T>
			static bcInline T bcAtomicFetchOr(bcAtomic<T>& pAtomic, T pToOr, bcMemoryOrder pOrder) throw()
			{
				static_assert(!std::is_same< T, bcBOOL >::value, "bcAtomicFetchOr is not supported with bool");
				return std::atomic_fetch_or_explicit(&pAtomic.mAtomic, pToOr, static_cast<std::memory_order>(pOrder));
			}

			template<typename T>
			static bcInline bcBOOL bcAtomicCompareExchangeStrong(
				volatile bcAtomic<T>& pAtomic,
				T* pExpectedValue,
				T pNewValue) throw()
			{
				return std::atomic_compare_exchange_strong(&pAtomic.mAtomic, pExpectedValue, pNewValue);
			}
			template<typename T>
			static bcInline bcBOOL bcAtomicCompareExchangeStrong(
				bcAtomic<T>& pAtomic,
				T* pExpectedValue,
				T pNewValue) throw()
			{
				return std::atomic_compare_exchange_strong(&pAtomic.mAtomic, pExpectedValue, pNewValue);
			}
			template<typename T>
			static bcInline bcBOOL bcAtomicCompareExchangeStrong(
				volatile bcAtomic<T>& pAtomic,
				T* pExpectedValue,
				T pNewValue,
				bcMemoryOrder pOrder) throw()
			{
				return pAtomic.mAtomic.compare_exchange_strong(
					*pExpectedValue,
					pNewValue,
					static_cast<std::memory_order>(pOrder));
				/*return std::atomic_compare_exchange_strong_explicit(
				&pAtomic.mAtomic,
				pExpectedValue,
				pNewValue,
				static_cast<std::memory_order>(pOrder));*/
			}
			template<typename T>
			static bcInline bcBOOL bcAtomicCompareExchangeStrong(
				bcAtomic<T>& pAtomic,
				T* pExpectedValue,
				T pNewValue,
				bcMemoryOrder pOrder) throw()
			{
				return pAtomic.mAtomic.compare_exchange_strong(
					*pExpectedValue,
					pNewValue,
					static_cast<std::memory_order>(pOrder));
				/*return std::atomic_compare_exchange_strong_explicit(
				&pAtomic.mAtomic,
				pExpectedValue,
				pNewValue,
				static_cast<std::memory_order>(pOrder));*/
			}
			template<typename T>
			static bcInline bcBOOL bcAtomicCompareExchangeStrong(
				volatile bcAtomic<T>& pAtomic,
				T* pExpectedValue,
				T pNewValue,
				bcMemoryOrder pSuccessOrder,
				bcMemoryOrder pFailureOrder) throw()
			{
				return std::atomic_compare_exchange_strong_explicit(
					&pAtomic.mAtomic,
					pExpectedValue,
					pNewValue,
					static_cast<std::memory_order>(pSuccessOrder),
					static_cast<std::memory_order>(pFailureOrder));
			}
			template<typename T>
			static bcInline bcBOOL bcAtomicCompareExchangeStrong(
				bcAtomic<T>& pAtomic,
				T* pExpectedValue,
				T pNewValue,
				bcMemoryOrder pSuccessOrder,
				bcMemoryOrder pFailureOrder) throw()
			{
				return std::atomic_compare_exchange_strong_explicit(
					&pAtomic.mAtomic,
					pExpectedValue,
					pNewValue,
					static_cast<std::memory_order>(pSuccessOrder),
					static_cast<std::memory_order>(pFailureOrder));
			}

			template<typename T>
			static bcInline bcBOOL bcAtomicCompareExchangeWeak(
				volatile bcAtomic<T>& pAtomic,
				T* pExpectedValue,
				T pNewValue) throw()
			{
				return std::atomic_compare_exchange_weak(&pAtomic.mAtomic, pExpectedValue, pNewValue);
			}
			template<typename T>
			static bcInline bcBOOL bcAtomicCompareExchangeWeak(
				bcAtomic<T>& pAtomic,
				T* pExpectedValue,
				T pNewValue) throw()
			{
				return std::atomic_compare_exchange_weak(&pAtomic.mAtomic, pExpectedValue, pNewValue);
			}
			template<typename T>
			static bcInline bcBOOL bcAtomicCompareExchangeWeak(
				volatile bcAtomic<T>& pAtomic,
				T* pExpectedValue,
				T pNewValue,
				bcMemoryOrder pOrder) throw()
			{
				return pAtomic.mAtomic.compare_exchange_weak(
					*pExpectedValue,
					pNewValue,
					static_cast<std::memory_order>(pOrder));
				/*return std::atomic_compare_exchange_weak_explicit(
				&pAtomic.mAtomic,
				pExpectedValue,
				pNewValue,
				static_cast<std::memory_order>(pOrder));*/
			}
			template<typename T>
			static bcInline bcBOOL bcAtomicCompareExchangeWeak(
				bcAtomic<T>& pAtomic,
				T* pExpectedValue,
				T pNewValue,
				bcMemoryOrder pOrder) throw()
			{
				return pAtomic.mAtomic.compare_exchange_weak(
					*pExpectedValue,
					pNewValue,
					static_cast<std::memory_order>(pOrder));
				/*return std::atomic_compare_exchange_weak_explicit(
				&pAtomic.mAtomic,
				pExpectedValue,
				pNewValue,
				static_cast<std::memory_order>(pOrder));*/
			}
			template<typename T>
			static bcInline bcBOOL bcAtomicCompareExchangeWeak(
				volatile bcAtomic<T>& pAtomic,
				T* pExpectedValue,
				T pNewValue,
				bcMemoryOrder pSuccessOrder,
				bcMemoryOrder pFailureOrder) throw()
			{
				return std::atomic_compare_exchange_weak_explicit(
					&pAtomic.mAtomic,
					pExpectedValue,
					pNewValue,
					static_cast<std::memory_order>(pSuccessOrder),
					static_cast<std::memory_order>(pFailureOrder));
			}
			template<typename T>
			static bcInline bcBOOL bcAtomicCompareExchangeWeak(
				bcAtomic<T>& pAtomic,
				T* pExpectedValue,
				T pNewValue,
				bcMemoryOrder pSuccessOrder,
				bcMemoryOrder pFailureOrder) throw()
			{
				return std::atomic_compare_exchange_weak_explicit(
					&pAtomic.mAtomic,
					pExpectedValue,
					pNewValue,
					static_cast<std::memory_order>(pSuccessOrder),
					static_cast<std::memory_order>(pFailureOrder));
			}
		};

		/*template<typename T>
		bcInline bool atomic_is_lock_free(volatile const atomic<T>*) throw();
		template<typename T>
		bcInline bool atomic_is_lock_free(const atomic<T>*) throw();
		template<typename T>
		void atomic_init(volatile atomic<T>*, void*) throw();
		template<typename T>
		void atomic_init(atomic<T>*, void*) throw();
		template<typename T>
		T atomic_exchange(volatile atomic<T>*, std::memory_order) throw();
		template<typename T>
		T atomic_exchange(atomic<T>*, std::memory_order) throw();
		template<typename T>
		T atomic_exchange_explicit(volatile atomic<T>*, std::memory_order) throw();
		template<typename T>
		T atomic_exchange_explicit(atomic<T>*, std::memory_order) throw();
		template<typename T>
		void atomic_store(volatile atomic<T>*, T) throw();
		template<typename T>
		void atomic_store(atomic<T>*, T) throw();
		template<typename T>
		void atomic_store_explicit(volatile atomic<T>*, T, std::memory_order) throw();
		template<typename T>
		void atomic_store_explicit(atomic<T>*, T, std::memory_order) throw();
		template<typename T>
		T atomic_load(volatile const atomic<T>*) throw();
		template<typename T>
		T atomic_load(const atomic<T>*) throw();
		template<typename T>
		T atomic_load_explicit(volatile const atomic<T>*, std::memory_order) throw();
		template<typename T>
		T atomic_load_explicit(const atomic<T>*, std::memory_order) throw();
		template<typename T>
		bool atomic_compare_exchange_strong(volatile atomic<T>*,T * old_value, T new_value) throw();
		template<typename T>
		bool atomic_compare_exchange_strong(atomic<T>*,T * old_value, T new_value) throw();
		template<typename T>
		bool atomic_compare_exchange_strong_explicit(volatile atomic<T>*,T * old_value, T new_value, std::memory_order success_order, std::memory_order failure_order) throw();
		template<typename T>
		bool atomic_compare_exchange_strong_explicit(atomic<T>*,T * old_value, T new_value, std::memory_order success_order, std::memory_order failure_order) throw();
		template<typename T>
		bool atomic_compare_exchange_weak(volatile atomic<T>*,T * old_value,T new_value) throw();
		template<typename T>
		bool atomic_compare_exchange_weak(atomic<T>*,T * old_value,T new_value) throw();
		template<typename T>
		bool atomic_compare_exchange_weak_explicit(volatile atomic<T>*,T * old_value, T new_value, std::memory_order success_order std::memory_order failure_order) throw();
		template<typename T>
		bool atomic_compare_exchange_weak_explicit( atomic<T>*,T * old_value, T new_value, std::memory_order success_order, std::memory_order failure_order) throw();*/
	}
}