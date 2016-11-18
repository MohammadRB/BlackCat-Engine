// [10/11/2016 MRB]

#pragma once

#include "PlatformImp/Script/bcScriptRef.h"
#include "PlatformImp/Script/bcScriptReference.h"
#include "PlatformImp/Script/bcScriptContext.h"

#include "Platform/bcException.h"
#include "Platform/Script/bcScriptError.h"
#include "PlatformImp/PlatformImpPCH.h"
#include "PlatformImp/Script/bcScriptContext.h"

namespace black_cat
{
	namespace platform
	{
		template<>
		struct bc_platform_script_error_pack< core_platform::g_api_win32 >
		{
			JsValueRef m_js_error;
		};

		class bc_chakra_call
		{
		public:
			bc_chakra_call();

			bc_chakra_call(bc_script_context& p_context);

			bc_chakra_call(JsErrorCode p_error_code);

			bc_chakra_call(bc_script_context& p_context, JsErrorCode p_error_code);

			bc_chakra_call(const bc_chakra_call& p_other);

			~bc_chakra_call() noexcept(false);

			bc_chakra_call& operator=(JsErrorCode p_error_code);

			bc_chakra_call& operator=(const bc_chakra_call& p_other);

			bool successed() const noexcept;

			JsErrorCode get_error_code() const noexcept;

			void throw_if_faild();

			void clear();

		protected:

		private:
			bc_script_context* m_context;
			JsErrorCode m_error_code;
		};

		inline bc_chakra_call::bc_chakra_call()
			: m_context(nullptr),
			m_error_code(JsNoError)
		{
		}

		inline bc_chakra_call::bc_chakra_call(bc_script_context& p_context)
			: m_context(&p_context),
			m_error_code(JsNoError)
		{
		}

		inline bc_chakra_call::bc_chakra_call(JsErrorCode p_error_code)
			: m_context(nullptr),
			m_error_code(p_error_code)
		{
		}

		inline bc_chakra_call::bc_chakra_call(bc_script_context& p_context, JsErrorCode p_error_code)
			: m_context(&p_context),
			m_error_code(p_error_code)
		{
		}

		inline bc_chakra_call::bc_chakra_call(const bc_chakra_call& p_other)
		{
			throw_if_faild();

			m_context = p_other.m_context;
			m_error_code = p_other.m_error_code;
		}

		inline bc_chakra_call::~bc_chakra_call() noexcept(false)
		{
			throw_if_faild();
		}

		inline bc_chakra_call& bc_chakra_call::operator=(JsErrorCode p_error_code)
		{
			throw_if_faild();

			m_error_code = p_error_code;

			return *this;
		}

		inline bc_chakra_call& bc_chakra_call::operator=(const bc_chakra_call& p_other)
		{
			throw_if_faild();

			m_context = p_other.m_context;
			m_error_code = p_other.m_error_code;

			return *this;
		}

		inline bool bc_chakra_call::successed() const noexcept
		{
			return m_error_code == JsNoError;
		}

		inline JsErrorCode bc_chakra_call::get_error_code() const noexcept
		{
			return m_error_code;
		}

		inline void bc_chakra_call::throw_if_faild()
		{
			switch (m_error_code)
			{
			case JsNoError:
				{
					return;
				}

			case JsErrorScriptTerminated:
				{
					clear();
					throw bc_script_interrupted_exception();
				}

			/*case JsErrorScriptException:
				{
					if (m_context)
					{
						bc_script_error l_execution_error = m_context->get_clear_exception();
						auto l_error_message = l_execution_error.error_message();

						clear();
						throw bc_script_execute_exception(core::bc_to_string_frame(l_execution_error.error_message()).c_str());
					}
					else
					{
						clear();
						throw bc_script_execute_exception("Chakra script execution error");
					}
				}*/

			/*case JsErrorScriptCompile:
				{
					core::bc_string_frame l_compile_error_msg;

					if (m_context)
					{
						bc_script_error l_compile_error = m_context->get_clear_exception();
						l_compile_error_msg = core::bc_to_string_frame(l_compile_error.error_message());
					}
					else
					{
						l_compile_error_msg = "Chakra script compilation error";
					}

					clear();
					throw bc_script_compile_exception("", 0, 0, l_compile_error_msg.c_str());
				}*/

			case JsErrorNoCurrentContext:
				{
					bcINT l_context_error_code = static_cast< bcINT >(m_error_code);
					core::bc_string_frame l_context_error_msg("Chakra api call without active context");

					clear();
					throw bc_platform_exception(l_context_error_code, l_context_error_msg.c_str());
				}

			default:
				{
					bcINT l_error_code = static_cast< bcINT >(m_error_code);
					core::bc_string_frame l_error_msg("Chakra error code: ");
					l_error_msg += core::bc_to_string_frame(l_error_code);

					clear();
					throw bc_platform_exception(l_error_code, l_error_msg.c_str());
				}
			}
		}

		inline void bc_chakra_call::clear()
		{
			m_error_code = JsNoError;
		}
	}
}
