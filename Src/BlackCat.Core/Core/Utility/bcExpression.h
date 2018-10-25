// [05/25/2016 MRB]

#pragma once

#include "CorePlatform/Utility/bcNoCopy.h"
#include "Core/bcExport.h"
#include "Core/Container/bcString.h"
#include "Core/Memory/bcPtr.h"
#include "Core/Utility/bcDelegate.hpp"

namespace black_cat
{
	namespace core
	{
		using bc_expr_numeric_type = bcDOUBLE;
		using bc_expr_func_number_void = bc_delegate< bc_expr_numeric_type() >;
		using bc_expr_func_number_number = bc_delegate< bc_expr_numeric_type(bc_expr_numeric_type) >;
		using bc_expr_func_number_string = bc_delegate< bc_expr_numeric_type(bc_string&) >;

		class _bc_expression_pack;
		class bc_expression;

		class BC_CORE_DLL _bc_expression_setter
		{
		public:
			explicit _bc_expression_setter(bc_expression& p_expression);

			_bc_expression_setter add_constant(const bcCHAR* p_name, bc_expr_numeric_type p_value);

			_bc_expression_setter add_variable(const bcCHAR* p_name, bc_expr_numeric_type p_value);

			_bc_expression_setter add_function(const bcCHAR* p_name, bc_expr_func_number_void&& p_func);

			_bc_expression_setter add_function(const bcCHAR* p_name, bc_expr_func_number_number&& p_func);

			_bc_expression_setter add_function(const bcCHAR* p_name, bc_expr_func_number_string&& p_func);

		protected:

		private:
			bc_expression& m_expression;
		};

		class BC_CORE_DLL bc_expression : public core_platform::bc_no_copy
		{
		public:
			bc_expression();

			bc_expression(bc_expression&&);

			~bc_expression();

			bc_expression& operator=(bc_expression&&);

			_bc_expression_setter add_constant(const bcCHAR* p_name, bc_expr_numeric_type p_value);

			_bc_expression_setter add_variable(const bcCHAR* p_name, bc_expr_numeric_type p_value);

			_bc_expression_setter add_function(const bcCHAR* p_name, bc_expr_func_number_void&& p_func);

			_bc_expression_setter add_function(const bcCHAR* p_name, bc_expr_func_number_number&& p_func);

			_bc_expression_setter add_function(const bcCHAR* p_name, bc_expr_func_number_string&& p_func);

			// Clear all associated parameters
			void clear_parameters();

			// Compile new expression
			// Before calling this function you must clear old associated parameters and create new parameters
			void compile(const bc_string_frame& p_expression) noexcept(false);

			// Compile new expression using another expression context, meaning that compilation and evaluation 
			// will be done with variables and functions that is registered with context expression. While this
			// expression exist, context expression must not destroyed
			void compile(const bc_string_frame& p_expression, const bc_expression& p_context_expression);

			bc_expr_numeric_type evaluate() const;

		protected:

		private:
			bc_unique_ptr< _bc_expression_pack > m_pack;
		};
	}
}
