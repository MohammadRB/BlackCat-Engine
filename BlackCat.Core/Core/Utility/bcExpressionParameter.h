// [05/26/2016 MRB]

#pragma once

#include "CorePlatform/Utility/bcNoCopy.h"
#include "CorePlatform/bcException.h"
#include "Core/bcExport.h"
#include "Core/Container/bcString.h"
#include "Core/Utility/bcExpressionParameterManager.h"
#include "Core/Utility/bcExpression.h"
#include "Core/Utility/bcParameterPack.h"

namespace black_cat
{
	namespace core
	{
		// Represent expressions that being defined in data driven parameters and this type of expressions
		// begin with 'expr:'
		class BC_CORE_DLL bc_expression_parameter : public core_platform::bc_no_copy
		{
		public:
			friend class bc_expression_parameter_manager;

		public:
			bc_expression_parameter();

			bc_expression_parameter(bc_expression_parameter&&);

			~bc_expression_parameter();

			bc_expression_parameter& operator=(bc_expression_parameter&&);

			void compile(const bc_string_frame& p_expression) noexcept(false);

			bc_parameter_pack evaluate() const noexcept(false);

			// Expression parameters start with 'expr:'
			static bool is_expression_parameter(const bc_string_frame& p_expression);

		protected:
			
		private:
			static bcSIZE _found_expression_start_token(const bc_string_frame& p_expression);

			bc_expression_parameter_manager* m_manager;
			bc_expression m_expression;
		};

		inline bc_expression_parameter::bc_expression_parameter()
			: m_manager(bc_service_manager::get().get_service<bc_expression_parameter_manager>()),
			m_expression()
		{
		}

		inline bc_expression_parameter::bc_expression_parameter(bc_expression_parameter&&) = default;

		inline bc_expression_parameter::~bc_expression_parameter() = default;

		inline bc_expression_parameter& bc_expression_parameter::operator=(bc_expression_parameter&&) = default;

		inline void bc_expression_parameter::compile(const bc_string_frame& p_expression) noexcept(false)
		{
			auto l_token_length = _found_expression_start_token(p_expression);

			if(l_token_length == -1)
			{
				throw bc_logic_exception("Not a valid expression parameter");
			}

			bc_string_frame l_expression = p_expression.substr(l_token_length);

			m_expression.compile(l_expression, m_manager->m_context_expression);
		}

		inline bc_parameter_pack bc_expression_parameter::evaluate() const noexcept(false)
		{
			return m_manager->resolve(*this);
		}

		inline bool bc_expression_parameter::is_expression_parameter(const bc_string_frame& p_expression)
		{
			return _found_expression_start_token(p_expression) != -1;
		}

		inline bcSIZE bc_expression_parameter::_found_expression_start_token(const bc_string_frame& p_expression)
		{
			bool l_token_found = false;
			auto l_token_pos = p_expression.find("expr");

			if (!(l_token_found = l_token_pos != bc_string_frame::npos))
			{
				return -1;
			}

			l_token_pos += (sizeof("expr") - 1);
			
			auto l_begin = std::begin(p_expression) + l_token_pos;
			auto l_end = std::end(p_expression);
			for (; l_begin != l_end; ++l_begin)
			{
				if (*l_begin == ' ')
				{
					++l_token_pos;
					continue;
				}
				else if (*l_begin == ':')
				{
					++l_token_pos;
					l_token_found = true;
					break;
				}
				else
				{
					l_token_found = false;
					break;
				}
			}

			return l_token_found ? l_token_pos : -1;
		}
	}
}