// [28/11/2016 MRB]

#pragma once

#include "Core/bcException.h"

namespace black_cat
{
	namespace physics
	{
		class bc_physics_error_category : public std::error_category
		{
		public:
			const bcCHAR* name() const noexcept override
			{
				return "physics";
			}

			std::string message(int p_errval) const override
			{
				return std::string();
			}
		};

		inline const std::error_category& bc_physics_category() noexcept
		{
			static bc_physics_error_category s_physics_category;

			return s_physics_category;
		}

		class bc_physics_exception : public std::system_error
		{
		public:
			bc_physics_exception(bcINT p_code, const bcCHAR* p_message);

			bc_physics_exception(const bc_physics_exception&) = default;

			bc_physics_exception(bc_physics_exception&&) = default;

			~bc_physics_exception() override = default;

			bc_physics_exception& operator=(const bc_physics_exception&) = default;

			bc_physics_exception& operator=(bc_physics_exception&&) = default;
		};

		inline bc_physics_exception::bc_physics_exception(bcINT p_code, const bcCHAR* p_message)
			: std::system_error(p_code, bc_physics_category(), p_message)
		{
		}
	}
}