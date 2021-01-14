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
		struct bc_platform_script_global_prototype_builder_pack< core_platform::g_api_win32 >
		{
			JsValueRef m_js_global;
		};

		template< core_platform::bc_platform TPlatform >
		bc_platform_script_global_prototype_builder< TPlatform >::bc_platform_script_global_prototype_builder(bc_script_context& p_context)
			: m_context(p_context)
		{
			m_pack.m_js_global = p_context.get_global().get_platform_pack().m_js_object;
		}

		template< core_platform::bc_platform TPlatform >
		bc_platform_script_global_prototype_builder< TPlatform >::bc_platform_script_global_prototype_builder(bc_platform_script_global_prototype_builder&& p_other) noexcept
			: m_context(p_other.m_context)
		{
			operator=(std::move(p_other));
		}

		template< core_platform::bc_platform TPlatform >
		bc_platform_script_global_prototype_builder< TPlatform >::~bc_platform_script_global_prototype_builder() = default;

		template< core_platform::bc_platform TPlatform >
		bc_platform_script_global_prototype_builder< TPlatform >& bc_platform_script_global_prototype_builder< TPlatform >::operator=(bc_platform_script_global_prototype_builder&& p_other) noexcept
		{
			m_pack.m_js_global = p_other.m_pack.m_js_global;

			p_other.m_pack.m_js_global = JS_INVALID_REFERENCE;

			return *this;
		}

		template< core_platform::bc_platform TPlatform >
		bc_platform_script_global_prototype_builder< TPlatform >& bc_platform_script_global_prototype_builder< TPlatform >::constant(const bcWCHAR* p_name, bc_script_bool p_bool)
		{
			bool l_successed = _create_js_property_constant(m_context, m_pack.m_js_global, p_name, p_bool);

			BC_ASSERT(l_successed);

			return *this;
		}

		template< core_platform::bc_platform TPlatform >
		bc_platform_script_global_prototype_builder< TPlatform >& bc_platform_script_global_prototype_builder< TPlatform >::constant(const bcWCHAR* p_name, bc_script_int p_integer)
		{
			bool l_successed = _create_js_property_constant(m_context, m_pack.m_js_global, p_name, p_integer);

			BC_ASSERT(l_successed);

			return *this;
		}

		template< core_platform::bc_platform TPlatform >
		bc_platform_script_global_prototype_builder< TPlatform >& bc_platform_script_global_prototype_builder< TPlatform >::constant(const bcWCHAR* p_name, bc_script_double p_double)
		{
			bool l_successed = _create_js_property_constant(m_context, m_pack.m_js_global, p_name, p_double);

			BC_ASSERT(l_successed);

			return *this;
		}

		template< core_platform::bc_platform TPlatform >
		bc_platform_script_global_prototype_builder< TPlatform >& bc_platform_script_global_prototype_builder< TPlatform >::constant(const bcWCHAR* p_name, bc_script_string& p_string)
		{
			bool l_successed = _create_js_property_constant(m_context, m_pack.m_js_global, p_name, p_string);

			BC_ASSERT(l_successed);

			return *this;
		}

		template< core_platform::bc_platform TPlatform >
		bc_platform_script_global_prototype_builder< TPlatform >& bc_platform_script_global_prototype_builder< TPlatform >::property(const bcWCHAR* p_name, bc_script_bool* p_bool)
		{
			bool l_successed = _create_js_property_default_get_set(m_context, m_pack.m_js_global, p_name, p_bool);

			BC_ASSERT(l_successed);

			return *this;
		}

		template< core_platform::bc_platform TPlatform >
		bc_platform_script_global_prototype_builder< TPlatform >& bc_platform_script_global_prototype_builder< TPlatform >::property(const bcWCHAR* p_name, bc_script_int* p_int)
		{
			bool l_successed = _create_js_property_default_get_set(m_context, m_pack.m_js_global, p_name, p_int);

			BC_ASSERT(l_successed);

			return *this;
		}

		template< core_platform::bc_platform TPlatform >
		bc_platform_script_global_prototype_builder< TPlatform >& bc_platform_script_global_prototype_builder< TPlatform >::property(const bcWCHAR* p_name, bc_script_double* p_double)
		{
			bool l_successed = _create_js_property_default_get_set(m_context, m_pack.m_js_global, p_name, p_double);

			BC_ASSERT(l_successed);

			return *this;
		}

		template< core_platform::bc_platform TPlatform >
		bc_platform_script_global_prototype_builder< TPlatform >& bc_platform_script_global_prototype_builder< TPlatform >::property(const bcWCHAR* p_name, bc_script_string* p_string)
		{
			bool l_successed = _create_js_property_default_get_set(m_context, m_pack.m_js_global, p_name, p_string);

			BC_ASSERT(l_successed);

			return *this;
		}

		template< core_platform::bc_platform TPlatform >
		bc_platform_script_global_prototype_builder< TPlatform >& bc_platform_script_global_prototype_builder< TPlatform >::property(const bcWCHAR* p_name, bc_script_object* p_object)
		{
			bool l_successed = _create_js_property_default_get_set(m_context, m_pack.m_js_global, p_name, p_object);

			BC_ASSERT(l_successed);

			return *this;
		}

		template< core_platform::bc_platform TPlatform >
		bc_platform_script_global_prototype_builder< TPlatform >& bc_platform_script_global_prototype_builder< TPlatform >::property(const bcWCHAR* p_name, bc_script_property_descriptor< bc_script_bool >& p_descriptor)
		{
			bool l_successed = _create_js_property_get_set(m_context, m_pack.m_js_global, p_name, p_descriptor);

			BC_ASSERT(l_successed);

			return *this;
		}

		template< core_platform::bc_platform TPlatform >
		bc_platform_script_global_prototype_builder< TPlatform >& bc_platform_script_global_prototype_builder< TPlatform >::property(const bcWCHAR* p_name, bc_script_property_descriptor< bc_script_int >& p_descriptor)
		{
			bool l_successed = _create_js_property_get_set(m_context, m_pack.m_js_global, p_name, p_descriptor);

			BC_ASSERT(l_successed);

			return *this;
		}

		template< core_platform::bc_platform TPlatform >
		bc_platform_script_global_prototype_builder< TPlatform >& bc_platform_script_global_prototype_builder< TPlatform >::property(const bcWCHAR* p_name, bc_script_property_descriptor< bc_script_double >& p_descriptor)
		{
			bool l_successed = _create_js_property_get_set(m_context, m_pack.m_js_global, p_name, p_descriptor);

			BC_ASSERT(l_successed);

			return *this;
		}

		template< core_platform::bc_platform TPlatform >
		bc_platform_script_global_prototype_builder< TPlatform >& bc_platform_script_global_prototype_builder< TPlatform >::property(const bcWCHAR* p_name, bc_script_property_descriptor< bc_script_string >& p_descriptor)
		{
			bool l_successed = _create_js_property_get_set(m_context, m_pack.m_js_global, p_name, p_descriptor);

			BC_ASSERT(l_successed);

			return *this;
		}

		template< core_platform::bc_platform TPlatform >
		bc_platform_script_global_prototype_builder< TPlatform >& bc_platform_script_global_prototype_builder< TPlatform >::property(const bcWCHAR* p_name, bc_script_property_descriptor< bc_script_object >& p_descriptor)
		{
			bool l_successed = _create_js_property_get_set(m_context, m_pack.m_js_global, p_name, p_descriptor);

			BC_ASSERT(l_successed);

			return *this;
		}

		template< core_platform::bc_platform TPlatform >
		template< typename TR, typename ...TA >
		bc_platform_script_global_prototype_builder< TPlatform >& bc_platform_script_global_prototype_builder< TPlatform >::function(const bcWCHAR* p_name, bc_script_free_function< TR, TA... > p_func)
		{
			bool l_successed = _create_js_function(m_context, m_pack.m_js_global, p_name, p_func);

			BC_ASSERT(l_successed);

			return *this;
		}
	}
}
