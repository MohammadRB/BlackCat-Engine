// [12/19/2014 MRB]

#pragma once

#include "Core/CorePCH.h"

namespace black_cat
{
	namespace core
	{
#define BC_MACRO_COMMA ,

		//http://stackoverflow.com/questions/27566894/how-to-check-if-a-type-is-defined-by-typedef-or-using-in-template-parameters
#define member_type_or_default(p_type, p_member_type, p_default) \
		{ \
		private: \
			template< class ... > struct voider { using type = void; }; \
			template< class ...Args > using void_t = typename voider< Args... >::type; \
			\
			template< class T, class = void > \
			struct _type_or_default \
			{ \
			public: \
				using type = p_default; \
			}; \
			\
			template< class T > \
			struct _type_or_default< T, void_t< typename T::p_member_type > > \
			{ \
			public: \
				using type = typename T::p_member_type; \
			}; \
			\
			\
		public: \
			using type = typename _type_or_default< p_type >::type; \
		}

		template< template< typename > class T, typename ...TInner >
		struct bc_template_template
		{
		public:
			using type = T< TInner... >;
		};

		template< typename T >
		struct bc_get_pointer_type
			member_type_or_default
			(
				T,
				pointer,
				typename T::value_type*
			);

		template< typename T >
		struct bc_get_const_pointer_type
			member_type_or_default
			(
				T,
				const_pointer,
				typename std::pointer_traits< typename bc_get_pointer_type< T >::type >::template rebind< const typename T::value_type >
			);

		template< typename T >
		struct bc_get_void_pointer_type
			member_type_or_default
			(
				T,
				void_pointer,
				typename std::pointer_traits< typename bc_get_pointer_type< T >::type >::template rebind< void >
			);

		template< typename T >
		struct bc_get_const_void_pointer_type
			member_type_or_default
			(
				T,
				const_void_pointer,
				typename std::pointer_traits< typename bc_get_pointer_type< T >::type >::template rebind< const void >
			);

		template< typename T >
		struct bc_get_reference_type
			member_type_or_default
			(
				T,
				reference,
				typename T::value_type&
			);

		template< typename T >
		struct bc_get_const_reference_type
			member_type_or_default
			(
				T,
				const_reference,
				const typename T::value_type&
			);

		template< typename T >
		struct bc_get_difference_type
			member_type_or_default
			(
				T,
				difference_type,
				typename std::pointer_traits< typename bc_get_pointer_type< T >::type >::difference_type
			);

		template< typename T >
		struct bc_get_size_type
			member_type_or_default
			(
				T,
				size_type,
				bcSIZE
			);

		/*template< typename T, typename ...TOther >
		struct bc_get_rebind_type
			member_type_or_default(T,
				template rebind< TOther >::other,
				typename bc_template_template<T BC_MACRO_COMMA TOther...>::type);*/

		// http://stackoverflow.com/questions/15393938/find-out-if-a-c-object-is-callable
		template< typename T >
		struct is_callable_impl
		{
		private:
			typedef char (&yes)[1];
			typedef char (&no)[2];

			struct Fallback
			{
				void operator()();
			};

			struct Derived : T, Fallback
			{
			};

			template< typename U, U >
			struct Check;

			template< typename >
			static yes test(...);

			template< typename C >
			static no test(Check< void (Fallback::*)(), &C::operator() >*);

		public:
			static const bool value = sizeof(test< Derived >(0)) == sizeof(yes);
		};

		template< typename T >
		struct is_callable
			: std::conditional
			<
				std::is_class< T >::value,
				is_callable_impl< T >,
				std::false_type
			>::type
		{
		};
	}
}
