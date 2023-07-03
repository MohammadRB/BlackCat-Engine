// [12/10/2016 MRB]

#pragma once

#include "PlatformImp/Script/bcScriptRef.h"
#include "PlatformImp/Script/bcScriptReference.h"
#include "PlatformImp/Script/bcScriptContext.h"
#include "PlatformImp/Script/bcScriptVariable.h"

#include "Platform/Script/bcScriptFunction.h"
#include "PlatformImp/PlatformImpPCH.h"
#include "PlatformImp/Script/bcScriptError.h"

namespace black_cat
{
	namespace platform
	{
		template<>
		struct bc_platform_script_function_pack<platform::g_api_win32>
		{
			JsValueRef m_js_function;
		};

		template<platform::bc_platform TPlatform>
		template<typename TCallable, bcSIZE TArgCount>
		bc_script_variable bc_platform_script_function<TPlatform>::call_callback(TCallable p_callable, bc_script_arg_pack<TArgCount>& p_args)
		{
			return _call_callback(p_callable, p_args, std::make_index_sequence<TArgCount>{});
		}

		template<platform::bc_platform TPlatform>
		template<typename TCallable, bcSIZE TArgCount, bcSIZE ...TArgCountI>
		bc_script_variable bc_platform_script_function<TPlatform>::_call_callback(TCallable p_callable, bc_script_arg_pack<TArgCount>& p_args, std::index_sequence<TArgCountI...>)
		{
			return p_callable(p_args[TArgCountI]...);
		}

		template<platform::bc_platform TPlatform, typename TR, typename ... TA>
		bc_platform_script_function_wrapper<TPlatform, TR(TA...)>::bc_platform_script_function_wrapper()
			: bc_platform_script_function<TPlatform>()
		{
		}

		template <platform::bc_platform TPlatform, typename TR, typename ... TA>
		bc_platform_script_function_wrapper<TPlatform, TR(TA...)>::bc_platform_script_function_wrapper(const bc_platform_script_function<TPlatform>& p_function)
			: bc_platform_script_function<TPlatform>(p_function)
		{
		}

		template<platform::bc_platform TPlatform, typename TR, typename ... TA>
		bc_platform_script_function_wrapper<TPlatform, TR(TA...)>::bc_platform_script_function_wrapper(const bc_platform_script_function_wrapper& p_other) noexcept
			: bc_platform_script_function<TPlatform>(p_other)
		{
		}

		template<platform::bc_platform TPlatform, typename TR, typename ... TA>
		bc_platform_script_function_wrapper<TPlatform, TR(TA...)>::~bc_platform_script_function_wrapper()
		{
		}

		template<platform::bc_platform TPlatform, typename TR, typename ... TA>
		bc_platform_script_function_wrapper<TPlatform, TR(TA...)>& bc_platform_script_function_wrapper<TPlatform, TR(TA...)>::operator=(const bc_platform_script_function_wrapper& p_other) noexcept
		{
			bc_platform_script_function<TPlatform>::operator=(p_other);

			return *this;
		}

		template<platform::bc_platform TPlatform, typename TR, typename ...TA>
		TR bc_platform_script_function_wrapper<TPlatform, TR(TA...)>::operator()(bc_script_variable& p_this, TA&... p_args) const
		{
			bc_script_variable l_call_result;
			bc_script_arg_pack<sizeof...(TA)> l_packed_args;

			_pack_args(l_packed_args, p_args...);

			l_call_result = bc_platform_script_function<TPlatform>::operator()(p_this, l_packed_args.data(), l_packed_args.size());

			TR l_result;
			bc_script_variable::_unpack_arg(l_call_result, &l_result);

			return l_result;
		}

		template<platform::bc_platform TPlatform, typename TR, typename ...TA>
		void bc_platform_script_function_wrapper<TPlatform, TR(TA...)>::_pack_args(bc_script_arg_pack<sizeof...(TA)>& p_pack, const TA&... p_args)
		{
			bcUINT32 l_counter = 0;

			auto l_expansion_list =
			{
				(
					[&l_counter, &p_pack, &p_args]()
					{
						p_pack[l_counter++] = bc_script_variable::_pack_arg(p_args);
						return true;
					}()
				)...
			};
		}

		template<platform::bc_platform TPlatform, typename TR, typename ...TA>
		void bc_platform_script_function_wrapper<TPlatform, TR(TA...)>::_unpack_args(bc_script_arg_pack<sizeof...(TA)>& p_pack, const TA&... p_args)
		{
			bcUINT32 l_counter = 0;

			auto l_expansion_list =
			{
				(
					[&l_counter, &p_pack, &p_args]()
					{
						bc_script_variable::_unpack_arg(p_pack[l_counter++], &p_args);
						return true;
					}()
				)...
			};
		}

		template<platform::bc_platform TPlatform, typename TR, typename ...TA>
		template<typename TCallable>
		TR bc_platform_script_function_wrapper<TPlatform, TR(TA...)>::_call_callback(TCallable p_callable, bc_script_variable* p_args, bcUINT32 p_arg_count)
		{
			bcUINT32 l_counter = 0;
			return p_callable
			(
				(
					[&l_counter, p_args]()
					{
						TA l_var;

						bc_script_variable::_unpack_arg(p_args[l_counter++], &l_var);

						return l_var;
					}()
				)...
			);
		}
	}
}