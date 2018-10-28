// [10/02/2016 MRB]

#pragma once

#include "CorePlatform/bcPlatform.h"
#include "CorePlatform/Utility/bcNoCopy.h"
#include "Core/Container/bcString.h"
#include "Platform/Script/bcScriptRef.h"
#include "Platform/Script/bcScriptReference.h"
#include "Platform/Script/bcScriptRuntime.h"

namespace black_cat
{
	namespace platform
	{
		using bc_script_bool = bool;
		using bc_script_int = bcINT32;
		using bc_script_double = bcDOUBLE;

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

		template< core_platform::bc_platform TPlatform, typename T >
		class bc_platform_script_prototype_builder;
		template< typename T >
		using bc_script_prototype_builder = bc_platform_script_prototype_builder< core_platform::g_current_platform, T >;

		template< core_platform::bc_platform TPlatform, typename T >
		class bc_platform_script_prototype;
		template< typename T >
		using bc_script_prototype = bc_platform_script_prototype< core_platform::g_current_platform, T >;

		template< core_platform::bc_platform TPlatform >
		class bc_platform_script_global_prototype_builder;
		using bc_script_global_prototype_builder = bc_platform_script_global_prototype_builder< core_platform::g_current_platform >;

		template< core_platform::bc_platform TPlatform >
		class bc_script_context_scope;

		template< core_platform::bc_platform TPlatform >
		struct bc_platform_script_context_pack
		{
		};

		/**
		 * \brief Creating a context is a two pass operation. after creating context itself you must create 
		 * context global object prototype and set it on context.
		 * Incompatible with movable memory
		 * \tparam TPlatform 
		 */
		template< core_platform::bc_platform TPlatform >
		class bc_platform_script_context : public bc_platform_script_reference< TPlatform >
		{
		public:
			using platform_pack = bc_platform_script_context_pack< TPlatform >;
			using scope = bc_script_context_scope< TPlatform >;
			friend bc_script_runtime;
			friend scope;

		public:
			bc_platform_script_context();

			bc_platform_script_context(const bc_platform_script_context&) noexcept;

			~bc_platform_script_context();

			bc_platform_script_context& operator=(const bc_platform_script_context&) noexcept;

			/**
			 * \brief Create global prototype builder to define global property and function definitions.
			 * \return 
			 */
			bc_script_global_prototype_builder create_global_prototype_builder();
			
			/**
			 * \brief Set global object prototype that contain global property and function definitions.
			 * This function must be called only one time to set global object.
			 * \param p_global_prototype Global object prototype
			 */
			void register_global_prototype(bc_script_global_prototype_builder&& p_global_prototype);

			bc_script_variable create_undefined();

			bc_script_variable create_null();

			bc_script_variable create_variable(bc_script_bool p_bool);

			bc_script_variable create_variable(bc_script_int p_integer);

			bc_script_variable create_variable(bc_script_double p_double);

			bc_script_variable create_variable(bc_script_string& p_string);

			bc_script_variable create_variable(bc_script_object& p_object);

			template< typename T >
			bc_script_variable create_variable(bc_script_array< T >& p_array);

			template< typename TR, typename ...TA >
			bc_script_variable create_variable(bc_script_function< TR(TA...) >& p_function);

			bc_script_variable create_variable(bc_script_error& p_error);

			bc_script_string create_string(core::bc_wstring& p_string);

			template< typename T >
			bc_script_object create_object(bc_script_prototype< T >& p_prototype, T&& p_native_object);

			template< typename T >
			bc_script_array< T > create_array(bcSIZE p_length);

			template< typename T >
			bc_script_array< T > create_array(std::initializer_list< T >& p_array);

			/*template< typename TR, typename ...TA >
			bc_script_function<TR(TA...)> create_function(typename bc_script_function<TR, TA...>::callback_t p_delegate);*/

			bc_script_error create_error(const core::bc_wstring& p_message);

			template< typename T >
			bc_script_prototype_builder<T> create_prototype_builder();

			template< typename T >
			bc_script_prototype<T> create_prototype(bc_script_prototype_builder<T>& p_prototype_builder);

			template< typename T >
			void register_prototype(bcWCHAR* p_object_name, bc_script_prototype< T >& p_object_prototype);

			/**
			 * \brief This function check if context is in exception state.
			 * When call to runtime throw exception (either by an exception thrown when executing script 
			 * or by invalid calls to api) the runtime will put in exception state and any call to runtime
			 * will throw exception.
			 * \return boolean that indicate context has any exception
			 */
			bool has_exception() const noexcept;

			/**
			 * \brief If context is in exception state return exception associated with context and clear it. 
			 * Calling this function on a context that is not in exception state will throw exception.
			 * \return Exception object
			 */
			bc_script_error get_clear_exception();

			/**
			 * \brief Set runtime in exception state
			 * \param p_error 
			 */
			void set_exception(bc_script_error& p_error);

			/**
			 * \brief Get context global object
			 * \return 
			 */
			bc_script_object get_global() const;

			bool is_valid() const noexcept override;

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		protected:

		private:
			explicit bc_platform_script_context(bc_script_runtime& p_runtime) noexcept;

			bc_script_runtime* m_runtime;
			platform_pack m_pack;
		};

		using bc_script_context = bc_platform_script_context< core_platform::g_current_platform >;
		using bc_script_context_ref = bc_script_ref< bc_script_context >;

		template< core_platform::bc_platform TPlatform >
		class bc_script_context_scope : public core_platform::bc_no_copy
		{
		public:
			bc_script_context_scope(bc_script_context& p_context) noexcept
				: m_context(p_context),
				m_prev_context(*m_context.m_runtime->get_active_context())
			{
				m_context.m_runtime->set_active_context(&p_context);
			}

			~bc_script_context_scope()
			{
				m_context.m_runtime->set_active_context(&m_prev_context);
			}

			bc_script_context& get_context() const noexcept
			{
				return m_context;
			}

		protected:

		private:
			bc_script_context& m_context;
			bc_script_context& m_prev_context;
		};
	}
}
