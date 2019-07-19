// [06/04/2016 MRB]

// This file is used duo to dependencies between bcPtr.h and bcDelegate.h

#pragma once

#include "Core/Memory/bcPtr.h"
#include "Core/Utility/bcDelegate.h"

namespace black_cat
{
	namespace core
	{
		template< typename TR, typename ...TA >
		template< typename TFunctor >
		typename bc_delegate< TR(TA ...) >::this_type bc_delegate< TR(TA ...) >::make_from_big_object(bc_alloc_type p_alloc_type, TFunctor&& p_functor)
		{
			using TFunctor_t = typename std::decay< TFunctor >::type;

			auto l_pointer = bc_make_unique< TFunctor_t >(p_alloc_type, std::forward<TFunctor_t>(p_functor));

			return bc_delegate([l_functor_pointer = std::move(l_pointer)](TA&&... p_args)
			{
				return (*l_functor_pointer.get())(std::forward< TA >(p_args)...);
			});
		}

		template< typename TR, typename ...TA >
		template< typename TFunctor >
		typename bc_delegate< TR(TA ...) >::this_type bc_delegate< TR(TA ...) >::make_from_big_object(TFunctor&& p_functor)
		{
			return make_from_big_object(bc_alloc_type::unknown, std::forward<TFunctor>(p_functor));
		}
	}
}