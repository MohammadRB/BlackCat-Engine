// [10/18/2016 MRB]

#pragma once

#include "PlatformImp/Script/bcScriptContext.h"
#include "PlatformImp/Script/bcScriptPrototypeBuilder.h"

#include "Platform/Script/bcScriptGlobalPrototypeBuilder.h"

namespace black_cat
{
	namespace platform
	{
		template<>
		struct bc_platform_script_global_prototype_builder_pack<core_platform::g_api_win32>
		{
			JsValueRef m_js_global;
		};

		template<core_platform::bc_platform TPlatform>
		bc_platform_script_global_prototype_builder<TPlatform>::bc_platform_script_global_prototype_builder(bc_script_context& p_context)
			: m_context(p_context)
		{
			m_pack.m_js_global = p_context.get_global().get_platform_pack().m_js_object;
		}

		template<core_platform::bc_platform TPlatform>
		bc_platform_script_global_prototype_builder<TPlatform>::bc_platform_script_global_prototype_builder(bc_platform_script_global_prototype_builder&& p_other) noexcept
			: m_context(p_other.m_context)
		{
			operator=(std::move(p_other));
		}

		template<core_platform::bc_platform TPlatform>
		bc_platform_script_global_prototype_builder<TPlatform>::~bc_platform_script_global_prototype_builder() = default;

		template<core_platform::bc_platform TPlatform>
		bc_platform_script_global_prototype_builder<TPlatform>& bc_platform_script_global_prototype_builder<TPlatform>::operator=(bc_platform_script_global_prototype_builder&& p_other) noexcept
		{
			m_pack.m_js_global = p_other.m_pack.m_js_global;

			p_other.m_pack.m_js_global = JS_INVALID_REFERENCE;

			return *this;
		}

		template<core_platform::bc_platform TPlatform>
		bc_platform_script_global_prototype_builder<TPlatform>& bc_platform_script_global_prototype_builder<TPlatform>::constant(const bcWCHAR* p_name, bc_script_bool p_bool)
		{
			const bool l_succeeded = _create_js_property_constant(m_context, m_pack.m_js_global, p_name, p_bool);

			BC_ASSERT(l_succeeded);

			return *this;
		}

		template<core_platform::bc_platform TPlatform>
		bc_platform_script_global_prototype_builder<TPlatform>& bc_platform_script_global_prototype_builder<TPlatform>::constant(const bcWCHAR* p_name, bc_script_int p_integer)
		{
			const bool l_succeeded = _create_js_property_constant(m_context, m_pack.m_js_global, p_name, p_integer);

			BC_ASSERT(l_succeeded);

			return *this;
		}

		template<core_platform::bc_platform TPlatform>
		bc_platform_script_global_prototype_builder<TPlatform>& bc_platform_script_global_prototype_builder<TPlatform>::constant(const bcWCHAR* p_name, bc_script_double p_double)
		{
			const bool l_succeeded = _create_js_property_constant(m_context, m_pack.m_js_global, p_name, p_double);

			BC_ASSERT(l_succeeded);

			return *this;
		}

		template<core_platform::bc_platform TPlatform>
		bc_platform_script_global_prototype_builder<TPlatform>& bc_platform_script_global_prototype_builder<TPlatform>::constant(const bcWCHAR* p_name, bc_script_string& p_string)
		{
			const bool l_succeeded = _create_js_property_constant(m_context, m_pack.m_js_global, p_name, p_string);

			BC_ASSERT(l_succeeded);

			return *this;
		}

		template<core_platform::bc_platform TPlatform>
		bc_platform_script_global_prototype_builder<TPlatform>& bc_platform_script_global_prototype_builder<TPlatform>::property(const bcWCHAR* p_name, bc_script_bool* p_bool)
		{
			const bool l_succeeded = _create_js_property_default_get_set(m_context, m_pack.m_js_global, p_name, p_bool);

			BC_ASSERT(l_succeeded);

			return *this;
		}

		template<core_platform::bc_platform TPlatform>
		bc_platform_script_global_prototype_builder<TPlatform>& bc_platform_script_global_prototype_builder<TPlatform>::property(const bcWCHAR* p_name, bc_script_int* p_int)
		{
			const bool l_succeeded = _create_js_property_default_get_set(m_context, m_pack.m_js_global, p_name, p_int);

			BC_ASSERT(l_succeeded);

			return *this;
		}

		template<core_platform::bc_platform TPlatform>
		bc_platform_script_global_prototype_builder<TPlatform>& bc_platform_script_global_prototype_builder<TPlatform>::property(const bcWCHAR* p_name, bc_script_double* p_double)
		{
			const bool l_succeeded = _create_js_property_default_get_set(m_context, m_pack.m_js_global, p_name, p_double);

			BC_ASSERT(l_succeeded);

			return *this;
		}

		template<core_platform::bc_platform TPlatform>
		bc_platform_script_global_prototype_builder<TPlatform>& bc_platform_script_global_prototype_builder<TPlatform>::property(const bcWCHAR* p_name, bc_script_string* p_string)
		{
			const bool l_succeeded = _create_js_property_default_get_set(m_context, m_pack.m_js_global, p_name, p_string);

			BC_ASSERT(l_succeeded);

			return *this;
		}

		template<core_platform::bc_platform TPlatform>
		bc_platform_script_global_prototype_builder<TPlatform>& bc_platform_script_global_prototype_builder<TPlatform>::property(const bcWCHAR* p_name, bc_script_object* p_object)
		{
			const bool l_succeeded = _create_js_property_default_get_set(m_context, m_pack.m_js_global, p_name, p_object);

			BC_ASSERT(l_succeeded);

			return *this;
		}

		template<core_platform::bc_platform TPlatform>
		bc_platform_script_global_prototype_builder<TPlatform>& bc_platform_script_global_prototype_builder<TPlatform>::property(const bcWCHAR* p_name, bc_script_property_descriptor<bc_script_bool>& p_descriptor)
		{
			const bool l_succeeded = _create_js_property_get_set(m_context, m_pack.m_js_global, p_name, p_descriptor);

			BC_ASSERT(l_succeeded);

			return *this;
		}

		template<core_platform::bc_platform TPlatform>
		bc_platform_script_global_prototype_builder<TPlatform>& bc_platform_script_global_prototype_builder<TPlatform>::property(const bcWCHAR* p_name, bc_script_property_descriptor<bc_script_int>& p_descriptor)
		{
			const bool l_succeeded = _create_js_property_get_set(m_context, m_pack.m_js_global, p_name, p_descriptor);

			BC_ASSERT(l_succeeded);

			return *this;
		}

		template<core_platform::bc_platform TPlatform>
		bc_platform_script_global_prototype_builder<TPlatform>& bc_platform_script_global_prototype_builder<TPlatform>::property(const bcWCHAR* p_name, bc_script_property_descriptor<bc_script_double>& p_descriptor)
		{
			const bool l_succeeded = _create_js_property_get_set(m_context, m_pack.m_js_global, p_name, p_descriptor);

			BC_ASSERT(l_succeeded);

			return *this;
		}

		template<core_platform::bc_platform TPlatform>
		bc_platform_script_global_prototype_builder<TPlatform>& bc_platform_script_global_prototype_builder<TPlatform>::property(const bcWCHAR* p_name, bc_script_property_descriptor<bc_script_string>& p_descriptor)
		{
			const bool l_succeeded = _create_js_property_get_set(m_context, m_pack.m_js_global, p_name, p_descriptor);

			BC_ASSERT(l_succeeded);

			return *this;
		}

		template<core_platform::bc_platform TPlatform>
		bc_platform_script_global_prototype_builder<TPlatform>& bc_platform_script_global_prototype_builder<TPlatform>::property(const bcWCHAR* p_name, bc_script_property_descriptor<bc_script_object>& p_descriptor)
		{
			const bool l_succeeded = _create_js_property_get_set(m_context, m_pack.m_js_global, p_name, p_descriptor);

			BC_ASSERT(l_succeeded);

			return *this;
		}

		template<core_platform::bc_platform TPlatform>
		template<typename TR, typename ...TA>
		bc_platform_script_global_prototype_builder<TPlatform>& bc_platform_script_global_prototype_builder<TPlatform>::function(const bcWCHAR* p_name, TR(*p_func)(const TA&...))
		{
			static_assert(std::is_same_v<bc_script_variable, std::decay_t<TR>>, "function return type must be bc_script_variable");
			static_assert(std::conjunction_v<std::is_same<bc_script_variable, std::decay_t<TA>>...>, "function argument types must be bc_script_variable");
			
			const bool l_succeeded = _create_js_function<sizeof...(p_func)>(m_context, m_pack.m_js_global, p_name, p_func);

			BC_ASSERT(l_succeeded);

			return *this;
		}
	}
}