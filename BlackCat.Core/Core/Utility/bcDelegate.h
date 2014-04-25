#ifndef bcDelegate_H
#define bcDelegate_H

#include "CorePCH.h"
#include "CoreUtility.h"

namespace BlackCat
{
	namespace Core
	{
		template< typename T >
		class bcDelegate;
			
		template< typename R, typename ...A >
		class bcDelegate< R(A...) >
		{
		public:
			typedef bcDelegate< R(A...) > ThisType;

		private:
			class DelegateTempClass
			{
			public:
				R DelegateTempFunc(A... pArg);
			};

		protected:
			typedef R (DelegateTempClass::*MemFuncType)(A...);
			typedef R (*FuncType)(A...);
			
			DelegateTempClass* mObjPtr;
			union
			{
				MemFuncType mMemFuncPtr;
				FuncType mFuncPtr;
			};

			// Bind to member function
			template< typename Obj >
			bcInline void _Bind(Obj* pObjPtr, R (Obj::*pFunc)(A...))
			{
				mObjPtr = (DelegateTempClass*)pObjPtr;
				mMemFuncPtr = reinterpret_cast<MemFuncType>(pFunc);
			};
			// Bind to const member function
			template< typename Obj >
			bcInline void _Bind(const Obj* pObjPtr, R (Obj::*pFunc)(A...) const)
			{
				mObjPtr = (DelegateTempClass*)const_cast<Obj*>(pObjPtr);
				mMemFuncPtr = reinterpret_cast<MemFuncType>(pFunc);
			};
			// Bind to static member function & normal functions
			bcInline void _Bind(FuncType pFuncPtr)
			{
				mObjPtr = nullptr;
				mFuncPtr = pFuncPtr;
			};
			bcInline void _Assign(const ThisType& pOther){
				mObjPtr = pOther.mObjPtr;
				if(mObjPtr)
					mMemFuncPtr = pOther.mMemFuncPtr;
				else
					mFuncPtr = pOther.mFuncPtr;
			}
			

		public:
			bcDelegate() : mObjPtr(nullptr), mFuncPtr(nullptr)
			{
			};
			template< typename Obj >
			bcDelegate(Obj* pObjPtr, R (Obj::*pFunc)(A...))
			{
				_Bind(pObjPtr, pFunc);
			};
			template< typename Obj >
			bcDelegate(const Obj* pObjPtr, R (Obj::*pFunc)(A...) const)
			{
				_Bind(pObjPtr, pFunc);
			};
			bcDelegate(FuncType pFuncPtr)
			{
				_Bind(pFuncPtr);
			};
			bcDelegate(const ThisType& pOther)
			{
				_Assign(pOther);
			};
			~bcDelegate()
			{
			};

			template< typename Obj >
			void Bind(Obj* pObjPtr, R (Obj::*pFunc)(A...))
			{
				_Bind(pObjPtr, pFunc);
			};
			template< typename Obj >
			void Bind(const Obj* pObjPtr, R (Obj::*pFunc)(A...) const)
			{
				_Bind(pObjPtr, pFunc);
			};
			void Bind(FuncType pFuncPtr)
			{
				_Bind(pFuncPtr);
			};

			void Swap(const ThisType& pOther)
			{
				Swap<DelegateTempClass*>(mObjPtr, pOther.mObjPtr);
				if(mObjPtr)
					Swap<MemFuncType>(mMemFuncPtr, pOther.mMemFuncPtr);
				else
					Swap<FuncType>(mMemFuncPtr, pOther.mMemFuncPtr);
			};

			template < typename ...A >
			R operator()(A&&... pArgs) const 
			{
				if(mObjPtr)
					return (mObjPtr->*mMemFuncPtr)(std::forward<A>(pArgs)...);
				else
					return mFuncPtr(std::forward<A>(pArgs)...);
			};

			ThisType& operator=(const ThisType& pOther)
			{
				_Assign(pOther);

				return *this;
			};

			bcBOOL operator==(const ThisType& pOther) const
			{
				if(mObjPtr == pOther.mObjPtr)
				{
					if(mObjPtr)
					{
						if(mMemFuncPtr == pOther.mMemFuncPtr)
							return bcTRUE;
						else
							return bcFALSE;
					}
					else
					{
						if(mFuncPtr == pOther.mFuncPtr)
							return bcTRUE;
						else
							return bcFALSE;
					}

				}
				else
					return bcFALSE;
			};
			bcBOOL operator!=(const ThisType& pOther) const
			{
				if(mObjPtr == pOther.mObjPtr)
				{
					if(mObjPtr)
					{
						if(mMemFuncPtr == pOther.mMemFuncPtr)
							return !bcTRUE;
						else
							return !bcFALSE;
					}
					else
					{
						if(mFuncPtr == pOther.mFuncPtr)
							return !bcTRUE;
						else
							return !bcFALSE;
					}

				}
				else
					return !bcFALSE;
			};

			void Clear()
			{
				mObjPtr = nullptr;
				mMemFuncPtr = nullptr;
				mFuncPtr = nullptr;
			};
		};
	}
}

#endif