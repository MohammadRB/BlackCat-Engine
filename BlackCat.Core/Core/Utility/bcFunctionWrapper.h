// [1/30/2015 MRB]

#pragma once

#include "Core/CorePCH.h"
#include "Core/Memory/bcAlloc.h"
#include "Core/Memory/bcPtr.h"

namespace black_cat
{
	namespace core
	{
		template< typename T >
		class bc_function_wrapper;

		// TODO complete as bc_delegate
		template< typename TR, typename ...TArgs >
		class bc_function_wrapper< TR(TArgs...) >
		{
		public:
			using return_type = TR;

		public:
			bc_function_wrapper() = default;

			bc_function_wrapper(const bc_function_wrapper&) = delete;

			bc_function_wrapper(bc_function_wrapper&& pOther) noexcept(true)
				: m_ptr(std::move(pOther.m_ptr))
			{
			}

			template<typename TF>
			bc_function_wrapper(TF&& p_func, bc_alloc_type p_alloc_type = bc_alloc_type::unknown)
				: m_ptr(bcNew(imp<TF>(std::move(p_func)), p_alloc_type))
			{
				static_assert(std::is_convertible<typename std::result_of<TF>::type, return_type>::value,
					"functor object must return appropriate type");
			}

			bc_function_wrapper& operator =(const bc_function_wrapper&) = delete;

			bc_function_wrapper& operator =(bc_function_wrapper&& pOther) noexcept(true)
			{
				m_ptr = std::move(pOther.m_ptr);
				return *this;
			}

			return_type operator ()(TArgs&&... p_args)
			{
				return m_ptr->call(std::forward<TArgs>(p_args)...);
			}

		protected:

		private:
			class imp_base
			{
			public:
				virtual ~imp_base() = default;

				virtual return_type call(TArgs&&... p_args) = 0;
			};

			template< typename TF >
			class imp : public imp_base
			{
			public:
				using function_type = TF;

				imp(function_type&& pFunc) 
					: m_func(std::move(pFunc))
				{
				}

				~imp() = default;

				return_type call(TArgs&&... p_args) override
				{
					return m_func(std::forward<TArgs>(p_args)...);
				}

			protected:

			private:
				function_type m_func;
			};

			bc_unique_ptr<imp_base> m_ptr;
		};
	}
}