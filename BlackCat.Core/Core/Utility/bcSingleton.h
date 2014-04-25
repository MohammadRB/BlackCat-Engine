#pragma once

#include "CorePCH.h"
#include "CoreUtility.h"

namespace BlackCat
{
	namespace Core
	{
		template< typename T >
		class bcSingleton;

		template < typename C, typename ...A >
		class bcSingleton< C(A...) > : public bcInitializable< A... >
		{
		private:
			typedef bcSingleton< C(A...) > ThisType;

			// MVC++11 doesn't support deleted function 
			bcSingleton(const ThisType& pOther);
			ThisType& operator=(const ThisType& pOther);

		protected:
			static C* mInstance;

		public:
			bcSingleton() {};
			~bcSingleton() {};

			//bcSingleton(const ThisType& pOther) = delete;
			//ThisType& operator=(const ThisType& pOther) = delete;

			static void StartUp(A... pArgs)
			{
				bcAssert(mInstance == nullptr);
				mInstance = new C();
				mInstance->Initialize(pArgs...);
			};
			static void Close()
			{
				bcAssert(mInstance != nullptr);
				mInstance->Destroy();
				delete mInstance;
				mInstance = nullptr;
			};

			static C* Get()
			{
				bcAssert(mInstance != nullptr);
				return mInstance;
			};
		};

		template < typename C, typename ...A >
		C* bcSingleton< C(A...) >::mInstance = nullptr;
	}
}