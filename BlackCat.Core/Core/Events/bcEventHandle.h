// We use an approche like Quynh Nguyen's delegates to implement our eventObject
// because we use delegates in our eventObject we must pass argumant to delegate
// via our eventObject template argumant

#ifndef bcEventHandle_H
#define bcEventHandle_H

#include "CorePCH.h"
#include "bcVector.h"
#include "bcDelegate.h"
// In Quynh Nguyen's delegates
//#include "Delegate.h"
//#include "compile_time_util.h"

//using namespace BlackCat::Core::Sophia;

namespace BlackCat
{
	namespace Core
	{
//#define BCEVENT_CLASSNAME			COMPILE_TIME_JOIN(bcEventHandle, BCEVENT_PARAM_COUNT)
//#define BC_DELEGATE_CLASSNAME		COMPILE_TIME_JOIN(Delegate, BCEVENT_PARAM_COUNT)
////#define SOPHIA_DETAIL_NAMESPACE	COMPILE_TIME_JOIN(detail,SOPHIA_PARAM_COUNT)
//
//#define BCEVENT_MAKE_PARAMS1_0(t)	
//#define BCEVENT_MAKE_PARAMS1_1(t)    t##1
//#define BCEVENT_MAKE_PARAMS1_2(t)    t##1, ##t##2
//#define BCEVENT_MAKE_PARAMS1_3(t)    t##1, ##t##2, ##t##3
//#define BCEVENT_MAKE_PARAMS1_4(t)    t##1, ##t##2, ##t##3, ##t##4
//#define BCEVENT_MAKE_PARAMS1_5(t)    t##1, ##t##2, ##t##3, ##t##4, ##t##5
//#define BCEVENT_MAKE_PARAMS1_6(t)    t##1, ##t##2, ##t##3, ##t##4, ##t##5, ##t##6
//#define BCEVENT_MAKE_PARAMS1_7(t)    t##1, ##t##2, ##t##3, ##t##4, ##t##5, ##t##6, ##t##7
//#define BCEVENT_MAKE_PARAMS1_8(t)    t##1, ##t##2, ##t##3, ##t##4, ##t##5, ##t##6, ##t##7, ##t##8
//#define BCEVENT_MAKE_PARAMS1_9(t)    t##1, ##t##2, ##t##3, ##t##4, ##t##5, ##t##6, ##t##7, ##t##8, ##t##9
//#define BCEVENT_MAKE_PARAMS1_10(t)   t##1, ##t##2, ##t##3, ##t##4, ##t##5, ##t##6, ##t##7, ##t##8, ##t##9, ##t##10
//
//#define BCEVENT_MAKE_PARAMS2_0(t1, t2)	
//#define BCEVENT_MAKE_PARAMS2_1(t1, t2)   t1##1 t2##1
//#define BCEVENT_MAKE_PARAMS2_2(t1, t2)   t1##1 t2##1, t1##2 t2##2
//#define BCEVENT_MAKE_PARAMS2_3(t1, t2)   t1##1 t2##1, t1##2 t2##2, t1##3 t2##3
//#define BCEVENT_MAKE_PARAMS2_4(t1, t2)   t1##1 t2##1, t1##2 t2##2, t1##3 t2##3, t1##4 t2##4
//#define BCEVENT_MAKE_PARAMS2_5(t1, t2)   t1##1 t2##1, t1##2 t2##2, t1##3 t2##3, t1##4 t2##4, t1##5 t2##5
//#define BCEVENT_MAKE_PARAMS2_6(t1, t2)   t1##1 t2##1, t1##2 t2##2, t1##3 t2##3, t1##4 t2##4, t1##5 t2##5, t1##6 t2##6
//#define BCEVENT_MAKE_PARAMS2_7(t1, t2)   t1##1 t2##1, t1##2 t2##2, t1##3 t2##3, t1##4 t2##4, t1##5 t2##5, t1##6 t2##6, t1##7 t2##7
//#define BCEVENT_MAKE_PARAMS2_8(t1, t2)   t1##1 t2##1, t1##2 t2##2, t1##3 t2##3, t1##4 t2##4, t1##5 t2##5, t1##6 t2##6, t1##7 t2##7, t1##8 t2##8
//#define BCEVENT_MAKE_PARAMS2_9(t1, t2)   t1##1 t2##1, t1##2 t2##2, t1##3 t2##3, t1##4 t2##4, t1##5 t2##5, t1##6 t2##6, t1##7 t2##7, t1##8 t2##8, t1##9 t2##9
//#define BCEVENT_MAKE_PARAMS2_10(t1, t2)  t1##1 t2##1, t1##2 t2##2, t1##3 t2##3, t1##4 t2##4, t1##5 t2##5, t1##6 t2##6, t1##7 t2##7, t1##8 t2##8, t1##9 t2##9, t1##10 t2##10
//
//// typename A0, class A1, class A2, ...
//#define BCEVENT_TEMPLATE_PARAMS    BCEVENT_MAKE_PARAMS1(BCEVENT_PARAM_COUNT,typename A)
//// A0, A1, A2, ...
//#define BCEVENT_TEMPLATE_ARGS      BCEVENT_MAKE_PARAMS1(BCEVENT_PARAM_COUNT,A)
//// A0 a0, A1 a1, A2 a2, ...
//#define BCEVENT_PARAMS             BCEVENT_MAKE_PARAMS2(BCEVENT_PARAM_COUNT,A,a)
//// a0, a1, a2, ...
//#define BCEVENT_ARGS               BCEVENT_MAKE_PARAMS1(BCEVENT_PARAM_COUNT,a)
//
////#define BCEVENT_TEMPLATE_PARAMS_RELAX    BCEVENT_MAKE_PARAMS1(BCEVENT_PARAM_COUNT,typename R_A)
////#define BCEVENT_TEMPLATE_ARGS_RELAX      BCEVENT_MAKE_PARAMS1(BCEVENT_PARAM_COUNT,R_A)
//
//#define BCEVENT_MAKE_PARAMS1(n, t)         COMPILE_TIME_JOIN(BCEVENT_MAKE_PARAMS1_, n) (t)
//#define BCEVENT_MAKE_PARAMS2(n, t1, t2)    COMPILE_TIME_JOIN(BCEVENT_MAKE_PARAMS2_, n) (t1, t2)
//
//// 0 params
//#define BCEVENT_PARAM_COUNT 0
//#include "bcEventHandleTemplate.h"
//#undef BCEVENT_PARAM_COUNT
//
//// 1 params
//#define BCEVENT_PARAM_COUNT 1
//#include "bcEventHandleTemplate.h"
//#undef BCEVENT_PARAM_COUNT
//
//// 2 params
//#define BCEVENT_PARAM_COUNT 2
//#include "bcEventHandleTemplate.h"
//#undef BCEVENT_PARAM_COUNT
//
//// 3 params
//#define BCEVENT_PARAM_COUNT 3
//#include "bcEventHandleTemplate.h"
//#undef BCEVENT_PARAM_COUNT
//
//// 4 params
//#define BCEVENT_PARAM_COUNT 4
//#include "bcEventHandleTemplate.h"
//#undef BCEVENT_PARAM_COUNT
//
//// 5 params
//#define BCEVENT_PARAM_COUNT 5
//#include "bcEventHandleTemplate.h"
//#undef BCEVENT_PARAM_COUNT
//
//// 6 params
//#define BCEVENT_PARAM_COUNT 6
//#include "bcEventHandleTemplate.h"
//#undef BCEVENT_PARAM_COUNT
//
//// 7 params
//#define BCEVENT_PARAM_COUNT 7
//#include "bcEventHandleTemplate.h"
//#undef BCEVENT_PARAM_COUNT
//
//// 8 params
//#define BCEVENT_PARAM_COUNT 8
//#include "bcEventHandleTemplate.h"
//#undef BCEVENT_PARAM_COUNT
//
//// 9 params
//#define BCEVENT_PARAM_COUNT 9
//#include "bcEventHandleTemplate.h"
//#undef BCEVENT_PARAM_COUNT
//
//// 10 params
//#define BCEVENT_PARAM_COUNT 10
//#include "bcEventHandleTemplate.h"
//#undef BCEVENT_PARAM_COUNT
		
		template< typename T >
		class bcEventHandle;
			
		template< typename R, typename ...A >
		class BC_COREDLL_EXP bcEventHandle< R(A...) >
		{
		public:
			typedef bcDelegate< R(A...) > DelegateType;
			typedef bcEventHandle< R(A...) > ThisType;

		private:

		protected:
			bcVector< DelegateType, 2> mDelegates;

		public:
			bcEventHandle()
			{
			};
			~bcEventHandle()
			{
			};

			bcUINT32 DelegateCount() const
			{
				return mDelegates.Size();
			};

			template < typename ...A >
			void operator()(A&&... pArgs) 
			{
				for(auto i = mDelegates.Begin(); i != mDelegates.End(); ++i)
				{
					(*i)(std::forward<A>(pArgs)...);
				}
			};

			ThisType& operator+=(DelegateType& pDel)
			{
				mDelegates.PushBack(pDel);

				return *this;
			}
			ThisType& operator-=(const DelegateType& pDel)
			{
				// Because we remove items from vector, using iterators
				// may cause to error
				/*for(auto i = mDelegates.Begin(); i != mDelegates.End(); ++i)
				{
					if(i->compare(pDel) == 0)
						mDelegates.Erase(i);
				}*/
				for(bcUINT32 i = 0; i < mDelegates.Size(); ++i)
				{
					if(*(mDelegates[i]) == pDel)
						mDelegates.Erase(mDelegates.Begin() + i);
				}
				
				return *this;
			}

			void Clear()
			{
				mDelegates.Clear();
			};
		};
	}
}

#endif