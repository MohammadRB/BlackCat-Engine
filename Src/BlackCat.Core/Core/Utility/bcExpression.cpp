// [05/25/2016 MRB]

#define _SCL_SECURE_NO_WARNINGS // For compiling exprtk.hpp. also /bigobj compiler option has been enabled

#include "Core/CorePCH.h"

#include "CorePlatform/bcException.h"
#include "Core/Container/bcList.h"
#include "Core/Utility/bcExpression.h"
#include "Core/Utility/bcParameterPack.h"
#include "3rdParty/ExprTK/exprtk.hpp" 

namespace black_cat
{
	namespace core
	{
		// -- Function wrappers --------------------------------------------------------------------------------

		struct _bc_expr_ifunc0_wrapper : public exprtk::ifunction< bc_expr_numeric_type >
		{
		public:
			using delegate_type = bc_expr_func_number_void;

		public:
			explicit _bc_expr_ifunc0_wrapper(delegate_type&& p_func)
				: ifunction<bc_expr_numeric_type>(0),
				m_func(std::move(p_func))
			{
			}

			bc_expr_numeric_type operator()() override
			{
				return m_func();
			}

		protected:

		private:
			delegate_type m_func;
		};

		struct _bc_expr_ifunc1_wrapper : public exprtk::ifunction< bc_expr_numeric_type >
		{
		public:
			using delegate_type = bc_expr_func_number_number;

		public:
			explicit _bc_expr_ifunc1_wrapper(delegate_type&& p_func)
				: ifunction<bc_expr_numeric_type>(1),
				m_func(std::move(p_func))
			{
			}

			bc_expr_numeric_type operator()(const bc_expr_numeric_type& p_v1) override
			{
				return m_func(p_v1);
			}

		protected:

		private:
			delegate_type m_func;
		};

		struct _bc_expr_igenfunc1_wrapper : public exprtk::igeneric_function< bc_expr_numeric_type >
		{
		public:
			using delegate_type = bc_expr_func_number_string;

		public:
			explicit _bc_expr_igenfunc1_wrapper(delegate_type&& p_func)
				: igeneric_function("S"),
				m_func(std::move(p_func))
			{
			}

			bc_expr_numeric_type operator()(parameter_list_t p_parameters) override
			{
				typedef typename exprtk::igeneric_function<bc_expr_numeric_type>::generic_type generic_type;
				typedef typename generic_type::string_view string_t;

				string_t l_parameter(p_parameters[0]);
				bc_string l_string(std::begin(l_parameter), std::end(l_parameter));

				return m_func(l_string);
			}

		protected:

		private:
			delegate_type m_func;
		};

		// -- Expression pack definition --------------------------------------------------------------------------------

		class _bc_expression_pack
		{
		public:
			exprtk::symbol_table< bc_expr_numeric_type > m_symbol_table;
			exprtk::expression< bc_expr_numeric_type > m_expression;
			bc_list< bc_any > m_arguments;
		};

		// -- _bc_expression_setter --------------------------------------------------------------------------------

		_bc_expression_setter::_bc_expression_setter(bc_expression& p_expression)
			: m_expression(p_expression)
		{
		}

		_bc_expression_setter _bc_expression_setter::add_constant(const bcCHAR* p_name, bc_expr_numeric_type p_value)
		{
			m_expression.add_constant(p_name, p_value);

			return _bc_expression_setter(m_expression);
		}

		_bc_expression_setter _bc_expression_setter::add_variable(const bcCHAR* p_name, bc_expr_numeric_type p_value)
		{
			m_expression.add_variable(p_name, p_value);

			return _bc_expression_setter(m_expression);
		}

		_bc_expression_setter _bc_expression_setter::add_function(const bcCHAR* p_name, bc_expr_func_number_void&& p_func)
		{
			m_expression.add_function(p_name, std::move(p_func));

			return _bc_expression_setter(m_expression);
		}

		_bc_expression_setter _bc_expression_setter::add_function(const bcCHAR* p_name, bc_expr_func_number_number&& p_func)
		{
			m_expression.add_function(p_name, std::move(p_func));

			return _bc_expression_setter(m_expression);
		}

		_bc_expression_setter _bc_expression_setter::add_function(const bcCHAR* p_name, bc_expr_func_number_string&& p_func)
		{
			m_expression.add_function(p_name, std::move(p_func));

			return _bc_expression_setter(m_expression);
		}

		// -- bc_expression_parser --------------------------------------------------------------------------------

		bc_expression::bc_expression()
			: m_pack(bc_make_unique< _bc_expression_pack >())
		{
			m_pack->m_expression.register_symbol_table(m_pack->m_symbol_table);
		}

		bc_expression::bc_expression(bc_expression&& p_other) noexcept
			: m_pack(std::move(p_other.m_pack))
		{	
		}

		bc_expression::~bc_expression()
		{
		}

		bc_expression& bc_expression::operator=(bc_expression&& p_other) noexcept
		{
			m_pack = std::move(p_other.m_pack);

			return *this;
		}

		_bc_expression_setter bc_expression::add_constant(const bcCHAR* p_name, bc_expr_numeric_type p_value)
		{
			m_pack->m_arguments.push_back(bc_any(p_value));
			m_pack->m_symbol_table.add_constant(p_name, *(m_pack->m_arguments.rbegin()->as<bc_expr_numeric_type>()));

			return _bc_expression_setter(*this);
		}

		_bc_expression_setter bc_expression::add_variable(const bcCHAR* p_name, bc_expr_numeric_type p_value)
		{
			m_pack->m_arguments.push_back(bc_any(p_value));
			m_pack->m_symbol_table.add_variable(p_name, *(m_pack->m_arguments.rbegin()->as<bc_expr_numeric_type>()));

			return _bc_expression_setter(*this);
		}

		_bc_expression_setter bc_expression::add_function(const bcCHAR* p_name, bc_expr_func_number_void&& p_func)
		{
			_bc_expr_ifunc0_wrapper l_func(std::move(p_func));
			m_pack->m_arguments.push_back(bc_any(std::move(l_func)));
			m_pack->m_symbol_table.add_function(p_name, *(m_pack->m_arguments.rbegin()->as<_bc_expr_ifunc0_wrapper>()));

			return _bc_expression_setter(*this);
		}

		_bc_expression_setter bc_expression::add_function(const bcCHAR* p_name, bc_expr_func_number_number&& p_func)
		{
			_bc_expr_ifunc1_wrapper l_func(std::move(p_func));
			m_pack->m_arguments.push_back(bc_any(std::move(l_func)));
			m_pack->m_symbol_table.add_function(p_name, *(m_pack->m_arguments.rbegin()->as<_bc_expr_ifunc1_wrapper>()));

			return _bc_expression_setter(*this);
		}

		_bc_expression_setter bc_expression::add_function(const bcCHAR* p_name, bc_expr_func_number_string&& p_func)
		{
			_bc_expr_igenfunc1_wrapper l_func(std::move(p_func));
			m_pack->m_arguments.push_back(bc_any(std::move(l_func)));
			m_pack->m_symbol_table.add_function(p_name, *(m_pack->m_arguments.rbegin()->as<_bc_expr_igenfunc1_wrapper>()));

			return _bc_expression_setter(*this);
		}

		void bc_expression::clear_parameters()
		{
			m_pack->m_symbol_table.clear();
			m_pack->m_arguments.clear();
		}

		void bc_expression::compile(const bc_string_frame& p_expression) noexcept(false)
		{
			exprtk::parser< bc_expr_numeric_type > l_parser;

			if(!l_parser.compile(p_expression.c_str(), m_pack->m_expression))
			{
				throw bc_runtime_exception(l_parser.error());
			}
		}

		void bc_expression::compile(const bc_string_frame& p_expression, const bc_expression& p_context_expression)
		{
			m_pack->m_expression.register_symbol_table(p_context_expression.m_pack->m_symbol_table);
			compile(p_expression);
		}

		bc_expr_numeric_type bc_expression::evaluate() const
		{
			return m_pack->m_expression.value();
		}
	}
}
