// [10/07/2016 MRB]

#pragma once

#include "CorePlatform/bcPlatform.h"
#include "Platform/Script/bcScriptRef.h"
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
			error,
			null,
			undefined
		};

		template<platform::bc_platform TPlatform>
		class bc_platform_script_string;
		using bc_script_string = bc_platform_script_string<platform::g_current_platform>;

		template<platform::bc_platform TPlatform>
		class bc_platform_script_object;
		using bc_script_object = bc_platform_script_object<platform::g_current_platform>;

		template<platform::bc_platform TPlatform>
		class bc_platform_script_array;
		using bc_script_array = bc_platform_script_array<platform::g_current_platform>;

		template<platform::bc_platform TPlatform>
		class bc_platform_script_function;
		using bc_script_function = bc_platform_script_function<platform::g_current_platform>;

		template<platform::bc_platform TPlatform>
		class bc_platform_script_error;
		using bc_script_error = bc_platform_script_error<platform::g_current_platform>;

		template<platform::bc_platform TPlatform>
		struct bc_platform_script_variable_pack
		{
		};

		template<platform::bc_platform TPlatform>
		class bc_platform_script_variable : public bc_platform_script_reference<TPlatform>
		{
		public:
			using platform_pack = bc_platform_script_variable_pack<TPlatform>;

		public:
			bc_platform_script_variable() noexcept;

			bc_platform_script_variable(bc_script_context& p_context, bc_script_bool p_bool);

			bc_platform_script_variable(bc_script_context& p_context, bc_script_int p_int);

			bc_platform_script_variable(bc_script_context& p_context, bc_script_double p_double);

			bc_platform_script_variable(bc_script_context& p_context, bc_script_string& p_string);

			bc_platform_script_variable(bc_script_context& p_context, bc_script_array& p_array);

			bc_platform_script_variable(bc_script_context& p_context, bc_script_object& p_object);

			bc_platform_script_variable(bc_script_context& p_context, bc_script_function& p_function);

			bc_platform_script_variable(bc_script_context& p_context, bc_script_error& p_error);

			bc_platform_script_variable(const bc_platform_script_variable&) noexcept;
			
			~bc_platform_script_variable();

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

			void as_boolean(bc_script_bool p_bool);

			bc_script_bool as_boolean() const;

			void as_integer(bc_script_int p_int);

			bc_script_int as_integer() const;

			void as_double(bc_script_double p_double);

			bc_script_double as_double() const;

			void as_string(const bc_script_string& p_string);

			bc_script_string as_string() const;

			void as_object(const bc_script_object& p_object);

			bc_script_object as_object() const;

			void as_function(const bc_script_function& p_function);

			bc_script_function as_function() const;

			void as_array(const bc_script_array& p_array);

			bc_script_array as_array() const;

			void as_error(const bc_script_error& p_error);

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

			static bc_script_variable _pack_arg(bc_script_bool p_value);

			static bc_script_variable _pack_arg(bc_script_int p_value);

			static bc_script_variable _pack_arg(bc_script_double p_value);

			static bc_script_variable _pack_arg(const bc_script_variable& p_value);

			static bc_script_variable _pack_arg(const bc_script_string& p_value);

			static bc_script_variable _pack_arg(const bc_script_object& p_value);

			static bc_script_variable _pack_arg(const bc_script_array& p_value);

			static void _unpack_arg(const bc_script_variable& p_pack, bc_script_bool* p_value);

			static void _unpack_arg(const bc_script_variable& p_pack, bc_script_int* p_value);

			static void _unpack_arg(const bc_script_variable& p_pack, bc_script_double* p_value);

			static void _unpack_arg(const bc_script_variable& p_pack, bc_script_variable* p_value);

			static void _unpack_arg(const bc_script_variable& p_pack, bc_script_string* p_value);

			static void _unpack_arg(const bc_script_variable& p_pack, bc_script_object* p_value);

			static void _unpack_arg(const bc_script_variable& p_pack, bc_script_function* p_value);

			static void _unpack_arg(const bc_script_variable& p_pack, bc_script_array* p_value);

		private:
			platform_pack m_pack;
		};

		using bc_script_variable = bc_platform_script_variable<platform::g_current_platform>;
		using bc_script_variable_ref = bc_script_ref<bc_script_variable>;
	}
}