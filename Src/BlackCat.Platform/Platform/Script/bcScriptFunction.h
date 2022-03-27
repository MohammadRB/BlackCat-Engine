// [10/10/2016 MRB]

#pragma once

#include "CorePlatform/bcPlatform.h"
#include "Core/Container/bcArray.h"
#include "Platform/Script/bcScriptRef.h"
#include "Platform/Script/bcScriptReference.h"
#include "Platform/Script/bcScriptContext.h"
#include "Platform/Script/bcScriptVariable.h"

namespace black_cat
{
	namespace platform
	{
		template<bcSIZE TArgCount>
		using bc_script_arg_pack = core::bc_array<bc_script_variable, TArgCount>;

		template<bc_platform TPlatform>
		struct bc_platform_script_function_pack
		{
		};

		template<bc_platform TPlatform>
		class bc_platform_script_function : public bc_platform_script_reference<TPlatform>
		{
		public:
			using platform_pack = bc_platform_script_function_pack<TPlatform>;
			using callback_t = bc_script_variable(*)(bc_script_variable*, bcSIZE);
			friend bc_script_context;

		public:
			bc_platform_script_function();

			bc_platform_script_function(const bc_platform_script_function&) noexcept;

			~bc_platform_script_function();

			bc_platform_script_function& operator=(const bc_platform_script_function&) noexcept;

			bc_script_variable operator()(bc_script_variable& p_this, bc_script_variable* p_args, bcSIZE p_arg_count) const;

			bool is_valid() const noexcept override;

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

			template<typename TCallable, bcSIZE TArgCount>
			static bc_script_variable call_callback(TCallable p_callable, bc_script_arg_pack<TArgCount>& p_args);
		
		protected:
			bc_platform_script_function(bc_script_context& p_context/*, callback_t p_callback*/);

		private:
			template<typename TCallable, bcSIZE TArgCount, bcSIZE ...TArgCountI>
			static bc_script_variable _call_callback(TCallable p_callable, bc_script_arg_pack<TArgCount>& p_args, std::index_sequence<TArgCountI...>);
			
			platform_pack m_pack;
		};

		template<bc_platform TPlatform, typename TR, typename ...TA>
		class bc_platform_script_function_wrapper;
		
		/**
		 * \brief Wrap a script function.
		 * \tparam TR Function return type.
		 * \tparam TA Function argument types.
		 */
		template<bc_platform TPlatform, typename TR, typename ...TA>
		class bc_platform_script_function_wrapper<TPlatform, TR(TA...)> : public bc_platform_script_function<TPlatform>
		{
		public:
			using callback_t = TR(*)(TA...);
			friend bc_script_context;
			template<bc_platform TPlatform, typename TR, typename ...TA>
			friend class bc_platform_script_function_wrapper;

		public:
			bc_platform_script_function_wrapper();
			
			bc_platform_script_function_wrapper(const bc_platform_script_function<TPlatform>& p_function);

			bc_platform_script_function_wrapper(const bc_platform_script_function_wrapper&) noexcept;

			~bc_platform_script_function_wrapper();

			bc_platform_script_function_wrapper& operator=(const bc_platform_script_function_wrapper&) noexcept;

			TR operator()(bc_script_variable& p_this, TA&... p_args) const;

			template<typename TCallable>
			static TR _call_callback(TCallable p_callable, bc_script_variable* p_args, bcUINT32 p_arg_count);

		private:
			static void _pack_args(bc_script_arg_pack<sizeof...(TA)>& p_pack, const TA&... p_args);

			static void _unpack_args(bc_script_arg_pack<sizeof...(TA)>& p_pack, const TA&... p_args);
		};

		using bc_script_function = bc_platform_script_function<g_current_platform>;
		using bc_script_function_ref = bc_script_ref<bc_script_function>;

		template<typename TR, typename ...TA>
		using bc_script_function_wrapper = bc_platform_script_function_wrapper<g_current_platform, TR, TA...>;
		template<typename TR, typename ...TA>
		using bc_script_function_wrapper_ref = bc_script_ref<bc_script_function_wrapper<TR, TA...>>;
	}
}