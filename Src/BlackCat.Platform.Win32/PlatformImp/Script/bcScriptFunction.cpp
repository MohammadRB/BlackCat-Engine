// [11/09/2016 MRB]

#include "PlatformImp/PlatformImpPCH.h"
#include "PlatformImp/bcExport.h"
#include "PlatformImp/Script/bcScriptFunction.h"

namespace black_cat
{
	namespace platform
	{
		/*inline JsValueRef CALLBACK _js_call(JsValueRef p_callee, bool p_is_construct_call, JsValueRef* p_arguments, bcUINT16 p_argument_count, void* p_callback_state)
		{
			BC_ASSERT(p_argument_count <= 10);

			const auto l_function = reinterpret_cast<bc_script_function::callback_t>(p_callback_state);
			bc_script_arg_pack<10> l_pack;

			std::transform
			(
				std::next(p_arguments), // First argument is 'this' object
				std::next(p_arguments, p_argument_count),
				std::begin(l_pack),
				[](JsValueRef p_js_value)
				{
					bc_script_variable l_variable;
					l_variable.get_platform_pack().m_js_value = p_js_value;

					return l_variable;
				}
			);

			bc_script_variable l_callback_result = l_function(l_pack.data(), p_argument_count);

			return l_callback_result.get_platform_pack().m_js_value;
		}

		template<bcSIZE TArgCount>
		JsValueRef CALLBACK _js_call(JsValueRef p_callee, bool p_is_construct_call, JsValueRef* p_arguments, bcUINT16 p_argument_count, void* p_callback_state)
		{
			if (p_argument_count != TArgCount + 1) // first arg is this
			{
				_bc_log_wrong_number_of_arguments();
			}

			auto l_function = reinterpret_cast<typename bc_script_function_wrapper< TR, TA... >::callback_t >(p_callback_state);
			bc_script_arg_pack< TA... > l_pack;

			std::transform
			(
				std::next(p_arguments), // First argument is 'this' object
				std::next(p_arguments, p_argument_count),
				std::begin(l_pack),
				[](JsValueRef p_js_value)
				{
					bc_script_variable l_variable;
					l_variable.get_platform_pack().m_js_value = p_js_value;

					return l_variable;
				}
			);

			bc_script_variable l_callback_result = bc_script_function_wrapper< TR, TA... >::_call_callback
			(
				std::ptr_fun(l_function),
				l_pack.data(),
				l_pack.size()
			);

			return l_callback_result.get_platform_pack().m_js_value;
		}*/

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_script_function<core_platform::g_api_win32>::bc_platform_script_function()
			: bc_platform_script_reference()
		{
			m_pack.m_js_function = JS_INVALID_REFERENCE;
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_script_function<core_platform::g_api_win32>::bc_platform_script_function(bc_script_context& p_context/*, callback_t p_callback*/)
			: bc_platform_script_reference(),
			m_pack()
		{
			/*static_assert(sizeof(callback_t) == sizeof(void*), "Function pointer size isn't equal to size of void*");

			bc_chakra_call l_call(p_context);

			l_call = JsCreateFunction(&_js_call, p_callback, &m_pack.m_js_function);*/
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_script_function<core_platform::g_api_win32>::bc_platform_script_function(const bc_platform_script_function& p_other) noexcept
			: bc_platform_script_reference(p_other),
			m_pack(p_other.m_pack)
		{
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_script_function<core_platform::g_api_win32>::~bc_platform_script_function() = default;

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_script_function<core_platform::g_api_win32>& bc_platform_script_function<core_platform::g_api_win32>::operator=(const bc_platform_script_function& p_other) noexcept
		{
			bc_platform_script_reference::operator=(p_other);
			m_pack.m_js_function = p_other.m_pack.m_js_function;

			return *this;
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_script_variable bc_platform_script_function<core_platform::g_api_win32>::operator()(bc_script_variable& p_this, bc_script_variable* p_args, bcSIZE p_arg_count) const
		{
			BC_ASSERT(p_arg_count <= 10);

			std::array<JsValueRef, 10> l_pack{};

			l_pack[0] = p_this.get_platform_pack().m_js_value;
			std::transform
			(
				p_args,
				std::next(p_args, p_arg_count),
				std::next(std::begin(l_pack)),
				[](bc_script_variable& p_js_value)
				{
					return p_js_value.get_platform_pack().m_js_value;
				}
			);

			bc_script_variable l_call_result;
			bc_chakra_call l_call = JsCallFunction
			(
				m_pack.m_js_function,
				l_pack.data(),
				p_arg_count + 1,
				&l_call_result.get_platform_pack().m_js_value
			);

			return l_call_result;
		}

		template<>
		BC_PLATFORMIMP_DLL
		bool bc_platform_script_function<core_platform::g_api_win32>::is_valid() const noexcept
		{
			return m_pack.m_js_function != JS_INVALID_REFERENCE;
		}
	}
}