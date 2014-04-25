//  [9/18/2013 MRB]

#pragma once

#include "CorePCH.h"
#include "bcConcurrencyDefinition.h"
#include <future>

namespace BlackCat
{
	namespace Core
	{
		template< bcConcurrencyLibrary CL, typename T>
		class bcFutureBase;

		template< typename T >
		class bcFuture;

		template< bcConcurrencyLibrary CL, typename T>
		class bcSharedFutureBase;

		template< typename T >
		class bcSharedFuture;

		template< bcConcurrencyLibrary CL, typename T >
		class bcPromiseBase;

		template< typename T >
		class bcPromise;

		// == bcFuture ================================================================================ MRB ==
		template< typename Concrete, typename T>
		class bcIFutureBase
		{
			//friend class bcPromiseBase< CL, T >;
			//friend class bcPromise< T >;
		private:
			typedef Concrete ConcreteType;
			typedef bcIFutureBase< ConcreteType, T > ThisType;

		protected:

		public:
			bcIFutureBase() {};
			~bcIFutureBase() {};

			bcIFutureBase(ThisType&&) {};
			bcIFutureBase& operator=(ThisType&&) {};

			bcIFutureBase(const ThisType&) = delete;
			bcIFutureBase& operator=(const ThisType&) = delete;

			bcInline virtual bcSharedFuture<T> share() abstract;
			bcInline virtual bcBOOL valid() const abstract;
			bcInline virtual T get() abstract;
			bcInline virtual void wait() abstract;
			bcInline virtual bcFutureStatus waitFor(const bcUINT64 pNano) abstract;
		};

		template< typename T>
		class bcFutureBase< bcConcurrencyLibrary_STD, T > : bcIFutureBase<bcFutureBase< bcConcurrencyLibrary_STD, T >, T>
		{
			friend class bcPromiseBase< bcConcurrencyLibrary_STD, T >;
			friend class bcPromise< T >;
		private:
			typedef bcFutureBase< bcConcurrencyLibrary_STD, T > ThisType;

			std::future< T > mFuture;

		protected:

		public:
			bcFutureBase() {}
			~bcFutureBase() {}

			bcFutureBase(const ThisType&) = delete;
			bcFutureBase& operator=(const ThisType&) =delete;

			bcFutureBase(ThisType&& pOther)
			{
				mFuture = std::move(pOther.mFuture);
			}
			bcFutureBase& operator=(ThisType&& pOther)
			{
				mFuture = std::move(pOther.mFuture);

				return *this;
			}

			bcInline bcSharedFuture<T> share() override
			{
				bcSharedFuture<T> lTemp;
				lTemp.mFuture = mFuture.share();

				return lTemp;
			}
			bcInline bcBOOL valid() const override
			{
				return mFuture.valid();
			}
			bcInline virtual T get() override
			{
				return mFuture.get();
			}
			bcInline void wait() override
			{
				mFuture.wait();
			}
			bcInline bcFutureStatus waitFor(const bcUINT64 pNano) override
			{
				return static_cast<bcFutureStatus>(mFuture.wait_for(std::chrono::nanoseconds(pNano)));
			}
		};

		template< typename T >
		class bcFuture : public bcFutureBase< bcConcurrencyLibrary_STD, T >
		{
			friend class bcPromiseBase< bcConcurrencyLibrary_STD, T >;
			friend class bcPromise< T >;
		private:
			typedef bcFuture< T > ThisType;

		protected:

		public:
			bcFuture() {}
			bcFuture(ThisType&& pOther) : bcFutureBase(std::forward<ThisType>(pOther))
			{
			}
			~bcFuture() {}

			bcFuture& operator=(ThisType&& pOther)
			{
				return static_cast<ThisType&>(static_cast<bcFutureBase< bcConcurrencyLibrary_STD, T >&>(*this) = std::move(pOther));
			}
		};

		template< typename Concrete, typename T>
		class bcISharedFutureBase
		{
			//friend class bcFutureBase< CL, T >;
		private:
			typedef Concrete ConcreteType;
			typedef bcISharedFutureBase< ConcreteType, T > ThisType;

		protected:

		public:
			bcISharedFutureBase() {};
			bcISharedFutureBase(const ThisType&) {};
			bcISharedFutureBase(ThisType&&) {};
			bcISharedFutureBase& operator=(const ThisType&) {};
			bcISharedFutureBase& operator=(ThisType&&) {};
			~bcISharedFutureBase() {};

			bcInline virtual bcBOOL valid() const abstract;
			bcInline virtual T get() abstract;
			bcInline virtual void wait() abstract;
			bcInline virtual bcFutureStatus waitFor(const bcUINT64 pNano) abstract;
		};

		template< typename T>
		class bcSharedFutureBase< bcConcurrencyLibrary_STD, T > : bcISharedFutureBase<bcSharedFutureBase< bcConcurrencyLibrary_STD, T >, T>
		{
			friend class bcFutureBase< bcConcurrencyLibrary_STD, T >;
		private:
			typedef bcSharedFutureBase< bcConcurrencyLibrary_STD, T > ThisType;

			std::shared_future< T > mFuture;

		protected:

		public:
			bcSharedFutureBase() {}
			bcSharedFutureBase(const ThisType& pOther)
			{
				mFuture = pOther.mFuture;
			}
			bcSharedFutureBase(ThisType&& pOther)
			{
				mFuture = std::move(pOther.mFuture);
			}
			~bcSharedFutureBase() {}

			bcSharedFutureBase& operator=(const ThisType& pOther)
			{
				mFuture = pOther.mFuture;

				return *this;
			}
			bcSharedFutureBase& operator=(ThisType&& pOther)
			{
				mFuture = std::move(pOther.mFuture);

				return *this;
			}

			bcInline bcBOOL valid() const override
			{
				return mFuture.valid();
			}
			bcInline virtual T get() override
			{
				return mFuture.get();
			}
			bcInline void wait() override
			{
				mFuture.wait();
			}
			bcInline bcFutureStatus waitFor(const bcUINT64 pNano) override
			{
				return static_cast<bcFutureStatus>(mFuture.wait_for(std::chrono::nanoseconds(pNano)));
			}
		};

		template< typename T >
		class bcSharedFuture : public bcSharedFutureBase< bcConcurrencyLibrary_STD, T >
		{
		private:
			typedef bcSharedFuture< T > ThisType;

		protected:

		public:
			bcSharedFuture() {}
			bcSharedFuture(const ThisType& pOther) : bcSharedFutureBase(pOther)
			{
			}
			bcSharedFuture(ThisType&& pOther) : bcSharedFutureBase(std::forward<ThisType>(pOther))
			{
			}
			~bcSharedFuture() {}

			bcSharedFuture& operator=(const ThisType& pOther)
			{
				return static_cast<bcSharedFuture&>(static_cast<bcSharedFutureBase< bcConcurrencyLibrary_STD, T >&>(*this) = pOther);
			}
			bcSharedFuture& operator=(ThisType&& pOther)
			{
				return static_cast<bcSharedFuture&>(static_cast<bcSharedFutureBase< bcConcurrencyLibrary_STD, T >&>(*this) = std::move(pOther));
			}
		};

		// == bcTask ================================================================================== MRB ==

		// == bcPromise =============================================================================== MRB ==
		template< typename Concrete, typename T >
		class bcIPromiseBase
		{
		private:
			typedef Concrete ConcreteType;
			typedef bcIPromiseBase< ConcreteType, T > ThisType;

		protected:

		public:
			bcIPromiseBase() {};
			~bcIPromiseBase() {};

			bcIPromiseBase(const ThisType&) = delete;
			bcIPromiseBase(ThisType&& pOther) {};

			ThisType& operator=(const ThisType&) = delete;
			ThisType& operator=(ThisType&& pOther) {};

			// TODO: Check here
			/*template<typename Allocator>
			bcPromiseBase(std::allocator_arg_t, Allocator const&);*/

			bcInline virtual void swap(ConcreteType&) abstract;
			bcInline virtual bcFuture< T > getFuture() abstract;
			bcInline virtual void setValue(T) abstract;
			bcInline virtual void setException(std::exception_ptr) abstract;
		};

		template< typename T >
		class bcPromiseBase< bcConcurrencyLibrary_STD, T > : bcIPromiseBase<bcPromiseBase< bcConcurrencyLibrary_STD, T >, T>
		{
		private:
			typedef bcPromiseBase< bcConcurrencyLibrary_STD, T > ThisType;

			std::promise< T > mPromise;

		protected:

		public:
			bcPromiseBase() {}
			~bcPromiseBase() {}

			bcPromiseBase(const ThisType&) = delete;
			bcPromiseBase(ThisType&& pOther)
			{
				mPromise = std::move(pOther.mPromise);
			}

			bcPromiseBase& operator=(const ThisType&) = delete;
			bcPromiseBase& operator=(ThisType&& pOther)
			{
				mPromise = std::move(pOther.mPromise);

				return *this;
			}

			// TODO: Check here
			/*template<typename Allocator>
			bcPromiseBase(std::allocator_arg_t, Allocator const&);*/

			bcInline void swap(ThisType& pOther) override
			{
				mPromise.swap(pOther.mPromise);
			}
			bcInline bcFuture< T > getFuture() override
			{
				bcFuture< T > lTemp;
				lTemp.mFuture = mPromise.get_future();

				return lTemp;
			}
			bcInline void setValue(T pValue) override
			{
				mPromise.set_value(pValue);
			}
			bcInline void setException(std::exception_ptr pException) override
			{
				mPromise.set_exception(pException);
			}
		};

		template< typename T >
		class bcPromise : public bcPromiseBase< bcConcurrencyLibrary_STD, T >
		{
		private:
			typedef bcPromise< T > ThisType;

		protected:

		public:
			bcPromise() {}
			~bcPromise() {}

			bcPromise(const ThisType&) = delete;
			bcPromise(ThisType&& pOther) : bcPromiseBase(std::forward<ThisType>(pOther))
			{
				return *this;
			}

			bcPromiseBase& operator=(const ThisType&) = delete;
			bcPromise& operator=(ThisType&& pOther)
			{
				return static_cast<ThisType&>(static_cast<bcPromiseBase< bcConcurrencyLibrary_STD, T >&>(*this) = std::move(pOther));
			}
		};

		//template< bcConcurrencyLibrary CL, typename T>
		//class bcFutureBase;

		//template< typename T >
		//class bcFuture;

		//template< bcConcurrencyLibrary CL, typename T>
		//class bcSharedFutureBase;

		//template< typename T >
		//class bcSharedFuture;

		//template< bcConcurrencyLibrary CL, typename T >
		//class bcPromiseBase;

		//template< typename T >
		//class bcPromise;

		//// == bcFuture ================================================================================ MRB ==
		//template< bcConcurrencyLibrary CL, typename T>
		//class bcFutureBase
		//{
		//	friend class bcPromiseBase< CL, T >;
		//	friend class bcPromise< T >;
		//private:
		//	typedef bcFutureBase< CL, T > ThisType;

		//	bcFutureBase(const ThisType&);
		//	bcFutureBase& operator=(const ThisType&);

		//protected:

		//public:
		//	bcFutureBase() {};
		//	bcFutureBase(ThisType&&) {};
		//	bcFutureBase& operator=(ThisType&&) {};
		//	~bcFutureBase() {};

		//	bcInline virtual bcSharedFuture<T> share() abstract;
		//	bcInline virtual bcBOOL valid() const abstract;
		//	bcInline virtual T get() abstract;
		//	bcInline virtual void wait() abstract;
		//	bcInline virtual bcFutureStatus waitFor(const bcUINT64 pNano) abstract;
		//};

		//template< typename T>
		//class bcFutureBase< bcConcurrencyLibrary_STD, T > : public bcFutureBase< bcConcurrencyLibrary_None, T >
		//{
		//	friend class bcPromiseBase< bcConcurrencyLibrary_STD, T >;
		//	friend class bcPromise< T >;
		//private:
		//	typedef bcFutureBase< bcConcurrencyLibrary_STD, T > ThisType;

		//	std::future< T > mFuture;

		//	bcFutureBase(const ThisType&);
		//	bcFutureBase& operator=(const ThisType&);

		//protected:

		//public:
		//	bcFutureBase() {}
		//	bcFutureBase(ThisType&& pOther)
		//	{
		//		mFuture = std::move(pOther.mFuture);
		//	}
		//	~bcFutureBase() {}

		//	bcFutureBase& operator=(ThisType&& pOther)
		//	{
		//		mFuture = std::move(pOther.mFuture);

		//		return *this;
		//	}

		//	bcInline bcSharedFuture<T> share() override
		//	{
		//		bcSharedFuture<T> lTemp;
		//		lTemp.mFuture = mFuture.share();

		//		return lTemp;
		//	}
		//	bcInline bcBOOL valid() const override
		//	{
		//		return mFuture.valid();
		//	}
		//	bcInline virtual T get() override
		//	{
		//		return mFuture.get();
		//	}
		//	bcInline void wait() override
		//	{
		//		mFuture.wait();
		//	}
		//	bcInline bcFutureStatus waitFor(const bcUINT64 pNano) override
		//	{
		//		return static_cast<bcFutureStatus>(mFuture.wait_for(std::chrono::nanoseconds(pNano)));
		//	}
		//};

		//template< typename T >
		//class BC_COREDLL_EXP bcFuture : public bcFutureBase< bcConcurrencyLibrary_STD, T >
		//{
		//	friend class bcPromiseBase< bcConcurrencyLibrary_STD, T >;
		//	friend class bcPromise< T >;
		//private:
		//	typedef bcFuture< T > ThisType;

		//protected:

		//public:
		//	bcFuture() {}
		//	bcFuture(ThisType&& pOther) : bcFutureBase(std::forward<ThisType>(pOther))
		//	{
		//	}
		//	~bcFuture() {}

		//	bcFuture& operator=(ThisType&& pOther)
		//	{
		//		return *this;
		//	}
		//};

		//template< bcConcurrencyLibrary CL, typename T>
		//class bcSharedFutureBase
		//{
		//	friend class bcFutureBase< CL, T >;
		//private:
		//	typedef bcSharedFutureBase< CL, T > ThisType;

		//protected:

		//public:
		//	bcSharedFutureBase() {};
		//	bcSharedFutureBase(const ThisType&) {};
		//	bcSharedFutureBase(ThisType&&) {};
		//	bcSharedFutureBase& operator=(const ThisType&) {};
		//	bcSharedFutureBase& operator=(ThisType&&) {};
		//	~bcSharedFutureBase() {};

		//	bcInline virtual bcBOOL valid() const abstract;
		//	bcInline virtual T get() abstract;
		//	bcInline virtual void wait() abstract;
		//	bcInline virtual bcFutureStatus waitFor(const bcUINT64 pNano) abstract;
		//};

		//template< typename T>
		//class bcSharedFutureBase< bcConcurrencyLibrary_STD, T > : public bcSharedFutureBase< bcConcurrencyLibrary_None, T >
		//{
		//	friend class bcFutureBase< bcConcurrencyLibrary_STD, T >;
		//private:
		//	typedef bcSharedFutureBase< bcConcurrencyLibrary_STD, T > ThisType;

		//	std::shared_future< T > mFuture;

		//protected:

		//public:
		//	bcSharedFutureBase() {}
		//	bcSharedFutureBase(const ThisType& pOther)
		//	{
		//		mFuture = pOther.mFuture;
		//	}
		//	bcSharedFutureBase(ThisType&& pOther)
		//	{
		//		mFuture = std::move(pOther.mFuture);
		//	}
		//	~bcSharedFutureBase() {}

		//	bcSharedFutureBase& operator=(const ThisType& pOther)
		//	{
		//		mFuture = pOther.mFuture;

		//		return *this;
		//	}
		//	bcSharedFutureBase& operator=(ThisType&& pOther)
		//	{
		//		mFuture = std::move(pOther.mFuture);

		//		return *this;
		//	}

		//	bcInline bcBOOL valid() const override
		//	{
		//		return mFuture.valid();
		//	}
		//	bcInline virtual T get() override
		//	{
		//		return mFuture.get();
		//	}
		//	bcInline void wait() override
		//	{
		//		mFuture.wait();
		//	}
		//	bcInline bcFutureStatus waitFor(const bcUINT64 pNano) override
		//	{
		//		return static_cast<bcFutureStatus>(mFuture.wait_for(std::chrono::nanoseconds(pNano)));
		//	}
		//};

		//template< typename T >
		//class BC_COREDLL_EXP bcSharedFuture : public bcSharedFutureBase< bcConcurrencyLibrary_STD, T >
		//{
		//private:
		//	typedef bcSharedFuture< T > ThisType;

		//protected:

		//public:
		//	bcSharedFuture() {}
		//	bcSharedFuture(const ThisType& pOther) : bcSharedFutureBase(pOther)
		//	{
		//	}
		//	bcSharedFuture(ThisType&& pOther) : bcSharedFutureBase(std::forward<ThisType>(pOther))
		//	{
		//	}
		//	~bcSharedFuture() {}

		//	bcSharedFuture& operator=(const ThisType& pOther)
		//	{
		//		return *this;
		//	}
		//	bcSharedFuture& operator=(ThisType&& pOther)
		//	{
		//		return *this;
		//	}
		//};

		//// == bcTask ================================================================================== MRB ==

		//// == bcPromise =============================================================================== MRB ==
		//template< bcConcurrencyLibrary CL, typename T >
		//class bcPromiseBase
		//{
		//private:
		//	typedef bcPromiseBase< CL, T > ThisType;

		//	bcPromiseBase(const ThisType&);
		//	bcPromiseBase& operator=(const ThisType&);

		//protected:

		//public:
		//	bcPromiseBase() {};
		//	bcPromiseBase(ThisType&& pOther) {};
		//	~bcPromiseBase() {};

		//	bcPromiseBase& operator=(ThisType&& pOther) {};

		//	// TODO: Check here
		//	/*template<typename Allocator>
		//	bcPromiseBase(std::allocator_arg_t, Allocator const&);*/

		//	bcInline void swap(ThisType&);
		//	bcInline bcFuture< T >  getFuture();
		//	bcInline void setValue(T);
		//	bcInline void setException(std::exception_ptr);
		//};

		//template< typename T >
		//class bcPromiseBase< bcConcurrencyLibrary_STD, T > : public bcPromiseBase< bcConcurrencyLibrary_None, T >
		//{
		//private:
		//	typedef bcPromiseBase< bcConcurrencyLibrary_STD, T > ThisType;

		//	std::promise< T > mPromise;

		//	bcPromiseBase(const ThisType&);
		//	bcPromiseBase& operator=(const ThisType&);

		//protected:

		//public:
		//	bcPromiseBase() {}
		//	bcPromiseBase(ThisType&& pOther)
		//	{
		//		mPromise = std::move(pOther.mPromise);
		//	}
		//	~bcPromiseBase() {}

		//	bcPromiseBase& operator=(ThisType&& pOther)
		//	{
		//		mPromise = std::move(pOther.mPromise);

		//		return *this;
		//	}

		//	// TODO: Check here
		//	/*template<typename Allocator>
		//	bcPromiseBase(std::allocator_arg_t, Allocator const&);*/

		//	bcInline void swap(ThisType& pOther)
		//	{
		//		mPromise.swap(pOther.mPromise);
		//	}
		//	bcInline bcFuture< T > getFuture()
		//	{
		//		bcFuture< T > lTemp;
		//		lTemp.mFuture = mPromise.get_future();

		//		return lTemp;
		//	}
		//	bcInline void setValue(T pValue)
		//	{
		//		mPromise.set_value(pValue);
		//	}
		//	bcInline void setException(std::exception_ptr pException)
		//	{
		//		mPromise.set_exception(pException);
		//	}
		//};

		//template< typename T >
		//class BC_COREDLL_EXP bcPromise : public bcPromiseBase< bcConcurrencyLibrary_STD, T >
		//{
		//private:
		//	typedef bcPromise< T > ThisType;

		//protected:

		//public:
		//	bcPromise() {}
		//	~bcPromise() {}

		//	bcPromise(ThisType&& pOther) : bcPromiseBase(std::forward<ThisType>(pOther))
		//	{
		//		return *this;
		//	}

		//	bcPromise& operator=(ThisType&& pOther)
		//	{
		//		return *this;
		//	}
		//};
	}
}