 // [10/15/2016 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Platform/Script/bcScriptPrototype.h"
#include "Platform/Script/bcScriptPrototypeBuilder.h"

namespace black_cat
{
	namespace platform
	{
		template< typename T >
		class _bc_script_external_object_meta_data
		{
		public:
			template< typename TM >
			using member_variable_ptr_t = TM T::*;
			template< typename TR, typename ...TA >
			using member_function_ptr_t = TR(T::*)(const TA&...);
			using _member_variable_ptr_t = member_variable_ptr_t<bcSIZE>;
			using _member_function_ptr_t = member_function_ptr_t<bcSIZE, bcSIZE>;

		public:
			_bc_script_external_object_meta_data()
				: m_member_variables(),
				m_member_functions(),
				m_script_prototype()
			{
			}

			core::bc_vector_program< _member_variable_ptr_t > m_member_variables;
			core::bc_vector_program< _member_function_ptr_t > m_member_functions;
			bc_script_prototype<T> m_script_prototype;
		};

		/**
		 * \brief Hold an external native object that can be referenced by script objects
		 * \tparam T Type of external native object
		 */
		template< typename T >
		class bc_script_external_object
		{
		public:
			using meta_data = _bc_script_external_object_meta_data<T>;

		private:
			template< typename TM >
			using member_variable_ptr_t = typename meta_data::template member_variable_ptr_t<TM>;
			template< typename TR, typename ...TA >
			using member_function_ptr_t = typename meta_data::template member_function_ptr_t<TR, TA...>;
			using _member_variable_ptr_t = member_variable_ptr_t<bcSIZE>;
			using _member_function_ptr_t = member_function_ptr_t<bcSIZE, bcSIZE>;

		public:
			explicit bc_script_external_object(T&& p_native_object)
				: m_object(std::move(p_native_object))
			{
			}

			bc_script_external_object(bc_script_external_object&& p_other) = default;

			~bc_script_external_object() = default;

			bc_script_external_object& operator=(bc_script_external_object&& p_other) = default;

			T* get() const noexcept
			{
				return &m_object;
			}

			T* operator->() const noexcept
			{
				return &m_object;
			}

			T& operator*() const noexcept
			{
				return m_object;
			}

			static meta_data* get_meta_data()
			{
				return s_object_meta_data;
			}

			static void set_meta_data_ptr(meta_data* p_meta_data)
			{
				s_object_meta_data = p_meta_data;
			}

			static bc_script_prototype<T> get_prototype()
			{
				return s_object_meta_data->m_script_prototype;
			}

			static void set_prototype(bc_script_prototype<T> p_prototype)
			{
				s_object_meta_data->m_script_prototype = p_prototype;
			}

			template< typename TM >
			TM get_member_variable(bcUINT32 p_variable_index)
			{
				static_assert(sizeof(member_variable_ptr_t< TM >) == sizeof(_member_variable_ptr_t), "Ops");

				auto l_member_variable_pointer = reinterpret_cast<member_variable_ptr_t< TM >>
				(
					s_object_meta_data->m_member_variables.at(p_variable_index)
				);
				return m_object.*l_member_variable_pointer;
			}

			template< typename TM >
			void set_member_variable(bcUINT32 p_variable_index, const TM& p_variable)
			{
				static_assert(sizeof(member_variable_ptr_t< TM >) == sizeof(_member_variable_ptr_t), "Ops");

				auto l_member_variable_pointer = reinterpret_cast<member_variable_ptr_t< TM >>
				(
					s_object_meta_data->m_member_variables.at(p_variable_index)
				);
				m_object.*l_member_variable_pointer = p_variable;
			}

			template< typename TM >
			static bcUINT32 set_member_variable_ptr(member_variable_ptr_t<TM> p_member_variable)
			{
				s_object_meta_data->m_member_variables.push_back(reinterpret_cast< _member_variable_ptr_t >(p_member_variable));
				return s_object_meta_data->m_member_variables.size() - 1;
			}

			template< typename TR, typename ...TA >
			TR call_member_function(bcUINT32 p_function_index, const TA&... p_args)
			{
				static_assert(sizeof(member_function_ptr_t< TR, TA... >) == sizeof(_member_function_ptr_t), "Ops");

				auto l_member_function_pointer = reinterpret_cast<member_function_ptr_t< TR, TA... >>
				(
					s_object_meta_data->m_member_functions.at(p_function_index)
				);
				return (m_object.*l_member_function_pointer)(p_args...);
			}

			template< typename TR, typename ...TA >
			static bcUINT32 set_member_function_ptr(member_function_ptr_t<TR, TA...> p_member_function)
			{
				s_object_meta_data->m_member_functions.push_back(reinterpret_cast< _member_function_ptr_t >(p_member_function));
				return s_object_meta_data->m_member_functions.size() - 1;
			}

		private:
			static meta_data* s_object_meta_data;

			T m_object;
		};

		template< typename T >
		typename bc_script_external_object< T >::meta_data* bc_script_external_object< T >::s_object_meta_data = nullptr;
	}
}