// [10/10/2016 MRB]

#include "PlatformImp/PlatformImpPCH.h"
#include "PlatformImp/bcExport.h"
#include "PlatformImp/Script/bcScriptContext.hpp"
#include "PlatformImp/Script/bcScriptRuntime.h"
#include "PlatformImp/Script/bcScriptError.h"
#include "PlatformImp/Script/bcScriptVariable.h"
#include "PlatformImp/Script/bcScriptByteCode.h"
#include "PlatformImp/Script/bcScriptObject.h"

namespace black_cat
{
	namespace platform
	{
		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_script_runtime< core_platform::g_api_win32 >::bc_platform_script_runtime() noexcept
		{
			m_pack.m_active_context = nullptr;
			auto l_code = JsCreateRuntime(JsRuntimeAttributeDispatchSetExceptionsToDebugger, nullptr, &m_pack.m_js_runtime);
			if (l_code != JsNoError)
			{
				throw bc_platform_exception(static_cast<bcINT>(l_code), "Failed to create Chakra runtime");
			}
		}

		/*template<>
		BC_PLATFORMIMP_DLL
		bc_platform_script_runtime< core_platform::g_api_win32 >::bc_platform_script_runtime(bc_platform_script_runtime&& p_other) noexcept
		{
			operator=(std::move(p_other));
		}*/

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_script_runtime< core_platform::g_api_win32 >::~bc_platform_script_runtime()
		{
			m_external_object_meta_data.clear();

			const auto l_code = JsDisposeRuntime(m_pack.m_js_runtime);
			if (l_code != JsNoError)
			{
				throw bc_platform_exception(static_cast<bcINT>(l_code), "Failed to dispose Chakra runtime");
			}
		}

		/*template<>
		BC_PLATFORMIMP_DLL
		bc_platform_script_runtime< core_platform::g_api_win32 >& bc_platform_script_runtime< core_platform::g_api_win32 >::operator=(bc_platform_script_runtime&& p_other) noexcept
		{
			m_external_object_meta_data = std::move(p_other.m_external_object_meta_data);
			m_pack.m_js_runtime = p_other.m_pack.m_js_runtime;
			m_pack.m_active_context = p_other.m_pack.m_active_context;
			p_other.m_pack.m_js_runtime = JS_INVALID_RUNTIME_HANDLE;
			p_other.m_pack.m_active_context = nullptr;

			return *this;
		}*/

		template<>
		BC_PLATFORMIMP_DLL
		bc_script_context bc_platform_script_runtime< core_platform::g_api_win32 >::create_context()
		{
			bc_script_context l_context(*this);

			auto l_code = JsCreateContext(m_pack.m_js_runtime, &l_context.get_platform_pack().m_js_context);
			if (l_code != JsNoError)
			{
				throw bc_platform_exception(static_cast<bcINT>(l_code), "Failed to create Chakra context");
			}

			return l_context;
		}

		template<>
		BC_PLATFORMIMP_DLL
		void bc_platform_script_runtime< core_platform::g_api_win32 >::set_active_context(bc_script_context* p_context)
		{
			JsErrorCode l_code;
			
			if(p_context)
			{
				l_code = JsSetCurrentContext(p_context->get_platform_pack().m_js_context);
			}
			else
			{
				l_code = JsSetCurrentContext(JS_INVALID_REFERENCE);
			}

			if (l_code != JsNoError)
			{
				throw bc_platform_exception(static_cast<bcINT>(l_code), "Failed to set Chakra active context");
			}

			m_pack.m_active_context = p_context;
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_script_context* bc_platform_script_runtime< core_platform::g_api_win32 >::get_active_context() const
		{
			return m_pack.m_active_context;
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_script_bytecode bc_platform_script_runtime< core_platform::g_api_win32 >::compile_script(const bcWCHAR* p_script)
		{
			bc_chakra_call l_call(*m_pack.m_active_context);
			bc_script_bytecode l_bytecode;

			l_call = JsParseScript
			(
				p_script,
				JS_SOURCE_CONTEXT_NONE,
				L"",
				&l_bytecode.get_platform_pack().m_js_parsed_script
			);

			// For script compilation error let user handle exception with checking context exception state
			if (l_call.get_error_code() == JsErrorScriptCompile)
			{
				l_call.clear();
			}

			return l_bytecode;
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_script_variable bc_platform_script_runtime< core_platform::g_api_win32 >::run_script(bc_script_bytecode& p_script)
		{
			bc_chakra_call l_call(*m_pack.m_active_context);
			bc_script_variable l_result;

			l_call = JsCallFunction
			(
				p_script.get_platform_pack().m_js_parsed_script,
				&m_pack.m_active_context->get_global().get_platform_pack().m_js_object,
				1,
				&l_result.get_platform_pack().m_js_value
			);

			// For script execution error let user handle exception with checking context exception state
			if(l_call.get_error_code() == JsErrorScriptException)
			{
				l_call.clear();
			}

			return l_result;
		}

		template<>
		BC_PLATFORMIMP_DLL
		void bc_platform_script_runtime< core_platform::g_api_win32 >::interrupt_script_execution()
		{
			bc_chakra_call l_call(*m_pack.m_active_context);

			l_call = JsDisableRuntimeExecution(m_pack.m_js_runtime);
			l_call = JsEnableRuntimeExecution(m_pack.m_js_runtime);
		}

		template<>
		BC_PLATFORMIMP_DLL
		void bc_platform_script_runtime< core_platform::g_api_win32 >::collect_garbage()
		{
			bc_chakra_call l_call(*m_pack.m_active_context);
			
			l_call = JsCollectGarbage(m_pack.m_js_runtime);
		}

		template<>
		BC_PLATFORMIMP_DLL
		bcSIZE bc_platform_script_runtime< core_platform::g_api_win32 >::memory_usage() const
		{
			bc_chakra_call l_call(*m_pack.m_active_context);
			bcSIZE l_usage;

			l_call = JsGetRuntimeMemoryUsage(m_pack.m_js_runtime, &l_usage);

			return l_usage;
		}
	}
}