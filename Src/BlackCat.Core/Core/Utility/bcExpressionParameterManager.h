// [05/23/2016 MRB]

#pragma once

#include "Core/bcExport.h"
#include "Core/bcConstant.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcString.h"
#include "Core/Utility/bcServiceManager.h"
#include "Core/Utility/bcDelegate.hpp"
#include "Core/Utility/bcParameterPack.h"
#include "Core/Utility/bcExpression.h"
#include "Core/Concurrency/bcThreadLocal.h"

namespace black_cat
{
	namespace core
	{
		class bc_expression_parameter;

		using bc_expr_param_func_pack_void = bc_delegate< bc_parameter_pack(void) >;
		using bc_expr_param_func_pack_number = bc_delegate< bc_parameter_pack(bc_expr_numeric_type) >;
		using bc_expr_param_func_pack_string = bc_delegate< bc_parameter_pack(const bc_string&) >;

		class BC_CORE_DLL bc_expression_parameter_manager : public bc_iservice
		{
			BC_SERVICE(expression_parameter_manager)

		public:
			friend class bc_expression_parameter;

		public:
			bc_expression_parameter_manager();

			bc_expression_parameter_manager(bc_expression_parameter_manager&&) noexcept = delete;

			~bc_expression_parameter_manager();

			bc_expression_parameter_manager& operator=(bc_expression_parameter_manager&&) noexcept = delete;

			void register_resolver(const bcCHAR* p_parameter, bc_expr_param_func_pack_void&& p_callback);

			void register_resolver(const bcCHAR* p_parameter, bc_expr_param_func_pack_number&& p_callback);

			void register_resolver(const bcCHAR* p_parameter, bc_expr_param_func_pack_string&& p_callback);

			// This function can be called with multiple threads to resolve expressions
			bc_parameter_pack resolve(const bc_expression_parameter& p_expression) const;

		protected:

		private:
			bc_expression m_context_expression;
			bc_thread_local< bc_parameter_pack > m_local_result;	// Every resolver will set it's value inside this parameter pack
																	// so if this parameter pack has value, expression parameter is 
																	// a system variable accessing expression otherwise we use it's
																	// result directly
		};
	}
}