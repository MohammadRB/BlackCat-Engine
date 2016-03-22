// [2/27/2015 MRB]

#pragma once

#include "Platform/PlatformPCH.h"
#include "Core/CoreExceptions.h"

namespace black_cat
{
	namespace platform
	{
		class bc_platform_error_category : public std::error_category
		{
		public:
			const bcCHAR* name() const noexcept override
			{
				return "platform";
			}

			std::string message(int p_errval) const override
			{
				return std::string();
			}
		};

		inline const std::error_category& bc_platform_category() noexcept
		{
			static bc_platform_error_category s_graphic_category;

			return s_graphic_category;
		};

		class bc_platform_exception : public std::system_error
		{
		public:
			explicit bc_platform_exception(bcINT p_code, const bcCHAR* p_message);

			explicit bc_platform_exception(bcINT p_code, core::bc_string p_message);

			bc_platform_exception(const bc_platform_exception&) = default;

			bc_platform_exception(bc_platform_exception&&) = default;

			~bc_platform_exception() = default;

			bc_platform_exception& operator=(const bc_platform_exception&) = default;

			bc_platform_exception& operator=(bc_platform_exception&&) = default;

			const bcCHAR* what() const override;

		protected:

		private:
			core::bc_string m_string; // Due to existence of this string, std::system_error string will left uninitialized
		};

		inline bc_platform_exception::bc_platform_exception(bcINT p_code, const bcCHAR* p_message)
			: std::system_error(p_code, bc_platform_category()),
			m_string(p_message)
		{
		};

		inline bc_platform_exception::bc_platform_exception(bcINT p_code, core::bc_string p_message)
			: std::system_error(p_code, bc_platform_category()),
			m_string(std::move(p_message))
		{
		}

		inline const bcCHAR* bc_platform_exception::what() const
		{
			return m_string.c_str();
		};
	}
}