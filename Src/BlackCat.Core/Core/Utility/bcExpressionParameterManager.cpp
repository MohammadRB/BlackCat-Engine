// [05/25/2016 MRB]

#include "Core/CorePCH.h"
#include "Core/Utility/bcExpressionParameterManager.h"
#include "Core/Utility/bcExpressionParameter.h"

namespace black_cat
{
	namespace core
	{
		bc_expression_parameter_manager::bc_expression_parameter_manager()
		{
		}

		bc_expression_parameter_manager::bc_expression_parameter_manager(bc_expression_parameter_manager&& p_other)
			: m_context_expression(std::move(p_other.m_context_expression)),
			m_local_result(std::move(p_other.m_local_result))
		{
		}

		bc_expression_parameter_manager::~bc_expression_parameter_manager()
		{
		}

		bc_expression_parameter_manager& bc_expression_parameter_manager::operator=(bc_expression_parameter_manager&& p_other)
		{
			m_context_expression = std::move(p_other.m_context_expression);
			m_local_result = std::move(p_other.m_local_result);

			return *this;
		}

		void bc_expression_parameter_manager::register_resolver(const bcCHAR* p_parameter, bc_expr_param_func_pack_void&& p_callback)
		{
			auto l_callback_wrapper = [this, p_callback = std::move(p_callback)]()
				{
					bc_parameter_pack l_result = p_callback();

					*this->m_local_result = std::move(l_result);

					return 0;
				};

			m_context_expression.add_function
				(
					p_parameter,
					bc_expr_func_number_void::make_from_big_object(bc_alloc_type::program, std::move(l_callback_wrapper))
				);
		}

		void bc_expression_parameter_manager::register_resolver(const bcCHAR* p_parameter, bc_expr_param_func_pack_number&& p_callback)
		{
			auto l_callback_wrapper = [this, p_callback = std::move(p_callback)](bc_expr_numeric_type p_param)
				{
					bc_parameter_pack l_result = p_callback(p_param);

					*this->m_local_result = std::move(l_result);

					return 0;
				};

			m_context_expression.add_function
				(
					p_parameter,
					bc_expr_func_number_number::make_from_big_object(bc_alloc_type::program, std::move(l_callback_wrapper))
				);
		}

		void bc_expression_parameter_manager::register_resolver(const bcCHAR* p_parameter, bc_expr_param_func_pack_string&& p_callback)
		{
			auto l_callback_wrapper = [this, p_callback = std::move(p_callback)](const bc_string& p_param)
				{
					bc_parameter_pack l_result = p_callback(p_param);

					*this->m_local_result = std::move(l_result);

					return 0;
				};

			m_context_expression.add_function
				(
					p_parameter,
					bc_expr_func_number_string::make_from_big_object(bc_alloc_type::program, std::move(l_callback_wrapper))
				);
		}

		bc_parameter_pack bc_expression_parameter_manager::resolve(const bc_expression_parameter& p_expression) const
		{
			bc_parameter_pack l_result;
			bc_expr_numeric_type l_evaluation_result = p_expression.m_expression.evaluate();
			bc_parameter_pack* l_local_result = m_local_result.get();

			if (l_local_result->has_value())
			{
				l_result = std::move(*l_local_result);
				l_local_result->reset(); // Reset local result for next evaluation
			}
			else
			{
				l_result.set_value(l_evaluation_result);
			}

			return l_result;
		}
	}
}
