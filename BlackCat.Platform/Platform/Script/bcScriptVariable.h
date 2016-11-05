// [10/07/2016 MRB]

#pragma once

#include "CorePlatform/bcPlatform.h"
#include "Platform/Script/bcScriptReference.h"
#include "Platform/Script/bcScriptContext.h"

namespace black_cat
{
	namespace platform
	{
		enum class bc_script_value_type
		{
			boolean,
			number,
			string,
			object,
			function,
			array,
			null,
			undefined
		};

		template< core_platform::bc_platform TPlatform >
		class bc_platform_script_variable;
		using bc_script_variable = bc_platform_script_variable< core_platform::g_current_platform >;

		template< core_platform::bc_platform TPlatform >
		class bc_platform_script_string;
		using bc_script_string = bc_platform_script_string< core_platform::g_current_platform >;

		template< core_platform::bc_platform TPlatform >
		class bc_platform_script_object;
		using bc_script_object = bc_platform_script_object< core_platform::g_current_platform >;

		template< core_platform::bc_platform TPlatform, typename T >
		class bc_platform_script_array;
		template< typename T >
		using bc_script_array = bc_platform_script_array< core_platform::g_current_platform, T >;

		template< core_platform::bc_platform TPlatform, typename TR, typename ...TA >
		class bc_platform_script_function;
		template< typename TR, typename ...TA >
		using bc_script_function = bc_platform_script_function< core_platform::g_current_platform, TR, TA... >;

		template< core_platform::bc_platform TPlatform >
		class bc_platform_script_error;
		using bc_script_error = bc_platform_script_error< core_platform::g_current_platform >;

		template< core_platform::bc_platform TPlatform >
		struct bc_platform_script_variable_pack
		{
		};

		template< core_platform::bc_platform TPlatform >
		class bc_platform_script_variable : public bc_platform_script_reference< TPlatform >
		{
		public:
			using platform_pack = bc_platform_script_variable_pack< TPlatform >;

		public:
			bc_platform_script_variable() noexcept;

			bc_platform_script_variable(bc_script_context& p_context, bool p_bool);

			bc_platform_script_variable(bc_script_context& p_context, bcINT p_int);

			bc_platform_script_variable(bc_script_context& p_context, bcDOUBLE p_double);

			bc_platform_script_variable(bc_script_context& p_context, bc_script_string& p_string);

			bc_platform_script_variable(bc_script_context& p_context, bc_script_object& p_object);

			template< typename TR, typename ...TA >
			bc_platform_script_variable(bc_script_context& p_context, bc_script_function< TR(TA...) >& p_function);

			bc_platform_script_variable(bc_script_context& p_context, bc_script_error& p_error);

			~bc_platform_script_variable();

			bc_platform_script_variable(const bc_platform_script_variable&) noexcept;

			bc_platform_script_variable& operator=(const bc_platform_script_variable&) noexcept;
			
			bc_script_value_type get_type() const noexcept;

			bool is_boolean() const noexcept;

			bool is_number() const noexcept;

			bool is_string() const noexcept;

			bool is_object() const noexcept;

			bool is_function() const noexcept;

			bool is_array() const noexcept;

			bool is_error() const noexcept;

			bool is_null() const noexcept;

			bool is_undefined() const noexcept;

			void as_boolean(bool p_bool);

			bool as_boolean() const;

			void as_integer(bcINT p_int);

			bcINT as_integer() const;

			void as_double(bcDOUBLE p_double);

			bcDOUBLE as_double() const;

			void as_string(bc_script_string& p_string);

			bc_script_string as_string() const;

			void as_object(bc_script_object& p_object);

			bc_script_object as_object() const;

			template< typename TR, typename ...TA >
			void as_function(bc_script_function< TR(TA...) >& p_function);

			template< typename TR, typename ...TA >
			bc_script_function< TR(TA...) > as_function() const;

			template< typename T >
			void as_array(bc_script_array< T >& p_array);

			template< typename T >
			bc_script_array< T > as_array() const;

			void as_error(bc_script_error& p_error);

			bc_script_error as_error() const;

			/**
			 * \brief Compare two script variable using == operator
			 * \param p_other 
			 * \return true if two variable compare equal
			 */
			bool equals(const bc_script_variable& p_other) const noexcept;

			/**
			 * \brief Compare two script variable using === operator
			 * \param p_other 
			 * \return true if two variable compare strictly equal
			 */
			bool strict_equals(const bc_script_variable& p_other) const noexcept;

			bool is_valid() const noexcept override;

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

			static bc_script_variable _pack_arg(void);

			static bc_script_variable _pack_arg(bool p_value);

			static bc_script_variable _pack_arg(bcINT p_value);

			static bc_script_variable _pack_arg(bcDOUBLE p_value);

			static bc_script_variable _pack_arg(const bc_script_variable& p_value);

			static bc_script_variable _pack_arg(const bc_script_string& p_value);

			static bc_script_variable _pack_arg(const bc_script_object& p_value);

			template< typename T >
			static bc_script_variable _pack_arg(const bc_script_array< T >& p_value);

			static void _unpack_arg(const bc_script_variable& p_pack, bool* p_value);

			static void _unpack_arg(const bc_script_variable& p_pack, bcINT* p_value);

			static void _unpack_arg(const bc_script_variable& p_pack, bcDOUBLE* p_value);

			static void _unpack_arg(const bc_script_variable& p_pack, bc_script_variable* p_value);

			static void _unpack_arg(const bc_script_variable& p_pack, bc_script_string* p_value);

			static void _unpack_arg(const bc_script_variable& p_pack, bc_script_object* p_value);

			template< typename TR, typename ...TA >
			static void _unpack_arg(const bc_script_variable& p_pack, bc_script_function<TR(TA...)>* p_value);

			template< typename T >
			static void _unpack_arg(const bc_script_variable& p_pack, bc_script_array< T >* p_value);

		protected:

		private:
			platform_pack m_pack;
		};

		template< core_platform::bc_platform TPlatform >
		bc_script_variable bc_platform_script_variable< TPlatform >::_pack_arg(void)
		{
			bc_script_variable l_result;

			return l_result;
		}

		template< core_platform::bc_platform TPlatform >
		bc_script_variable bc_platform_script_variable< TPlatform >::_pack_arg(bool p_value)
		{
			bc_script_variable l_result;

			l_result.as_boolean(p_value);

			return l_result;
		}

		template< core_platform::bc_platform TPlatform >
		bc_script_variable bc_platform_script_variable< TPlatform >::_pack_arg(bcINT p_value)
		{
			bc_script_variable l_result;

			l_result.as_integer(p_value);

			return l_result;
		}

		template< core_platform::bc_platform TPlatform >
		bc_script_variable bc_platform_script_variable< TPlatform >::_pack_arg(bcDOUBLE p_value)
		{
			bc_script_variable l_result;

			l_result.as_double(p_value);

			return l_result;
		}

		template< core_platform::bc_platform TPlatform >
		bc_script_variable bc_platform_script_variable<TPlatform>::_pack_arg(const bc_script_variable& p_value)
		{
			return p_value;
		}

		template< core_platform::bc_platform TPlatform >
		bc_script_variable bc_platform_script_variable< TPlatform >::_pack_arg(const bc_script_string& p_value)
		{
			bc_script_variable l_result;

			l_result.as_string(p_value);

			return l_result;
		}

		template< core_platform::bc_platform TPlatform >
		bc_script_variable bc_platform_script_variable< TPlatform >::_pack_arg(const bc_script_object& p_value)
		{
			bc_script_variable l_result;

			l_result.as_object(p_value);

			return l_result;
		}

		template< core_platform::bc_platform TPlatform >
		template< typename T >
		bc_script_variable bc_platform_script_variable< TPlatform >::_pack_arg(const bc_script_array< T >& p_value)
		{
			bc_script_variable l_result;

			l_result.as_array(p_value);

			return l_result;
		}

		template< core_platform::bc_platform TPlatform >
		void bc_platform_script_variable< TPlatform >::_unpack_arg(const bc_script_variable& p_pack, bool* p_value)
		{
			*p_value = p_pack.as_boolean();
		}

		template< core_platform::bc_platform TPlatform >
		void bc_platform_script_variable< TPlatform >::_unpack_arg(const bc_script_variable& p_pack, bcINT* p_value)
		{
			*p_value = p_pack.as_integer();
		}

		template< core_platform::bc_platform TPlatform >
		void bc_platform_script_variable< TPlatform >::_unpack_arg(const bc_script_variable& p_pack, bcDOUBLE* p_value)
		{
			*p_value = p_pack.as_double();
		}

		template< core_platform::bc_platform TPlatform >
		void bc_platform_script_variable< TPlatform >::_unpack_arg(const bc_script_variable& p_pack, bc_script_variable* p_value)
		{
			*p_value = p_pack;
		}

		template< core_platform::bc_platform TPlatform >
		void bc_platform_script_variable< TPlatform >::_unpack_arg(const bc_script_variable& p_pack, bc_script_string* p_value)
		{
			*p_value = p_pack.as_string();
		}

		template< core_platform::bc_platform TPlatform >
		void bc_platform_script_variable< TPlatform >::_unpack_arg(const bc_script_variable& p_pack, bc_script_object* p_value)
		{
			*p_value = p_pack.as_object();
		}

		template< core_platform::bc_platform TPlatform >
		template< typename TR, typename ... TA >
		void bc_platform_script_variable<TPlatform>::_unpack_arg(const bc_script_variable& p_pack, bc_script_function<TR(TA...)>* p_value)
		{
			*p_value = p_pack.as_function<TR, TA...>();
		}

		template< core_platform::bc_platform TPlatform >
		template< typename T >
		void bc_platform_script_variable< TPlatform >::_unpack_arg(const bc_script_variable& p_pack, bc_script_array<T>* p_value)
		{
			*p_value = p_pack.as_array< T >();
		}
	}
}
