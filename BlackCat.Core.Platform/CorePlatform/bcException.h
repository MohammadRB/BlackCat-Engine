// [11/11/2014 MRB]

#pragma once

#include "CorePlatform/CorePlatformPCH.h"
#include "CorePlatform/bcType.h"
#include <system_error>
#include <string>

namespace black_cat
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

		explicit bc_platform_exception(bcINT p_code, std::string p_message);

		bc_platform_exception(const bc_platform_exception&) = default;

		bc_platform_exception(bc_platform_exception&&) = default;

		~bc_platform_exception() = default;

		bc_platform_exception& operator=(const bc_platform_exception&) = default;

		bc_platform_exception& operator=(bc_platform_exception&&) = default;

		const bcCHAR* what() const override;

	protected:

	private:
	};

	inline bc_platform_exception::bc_platform_exception(bcINT p_code, const bcCHAR* p_message)
		: system_error(p_code, bc_platform_category(), p_message)
	{
	}

	inline bc_platform_exception::bc_platform_exception(bcINT p_code, std::string p_message)
		: system_error(p_code, bc_platform_category(), p_message)
	{
	}

	inline const bcCHAR* bc_platform_exception::what() const
	{
		return std::system_error::what();
	}

	class bc_thread_resource_exception : public std::runtime_error
	{
	public:
		explicit bc_thread_resource_exception(const bcCHAR* p_message) : std::runtime_error(p_message)
		{
		}

		bc_thread_resource_exception(const bc_thread_resource_exception& p_other) : std::runtime_error(p_other)
		{
		}

		~bc_thread_resource_exception() = default;

	protected:

	private:
	};

	class bc_invalid_operation_exception : public std::runtime_error
	{
	public:
		explicit bc_invalid_operation_exception(const bcCHAR* p_message) : std::runtime_error(p_message)
		{
		}

		explicit bc_invalid_operation_exception(const std::string& p_message) : std::runtime_error(p_message)
		{
		}

		bc_invalid_operation_exception(const bc_invalid_operation_exception& p_other) : std::runtime_error(p_other)
		{
		}

		~bc_invalid_operation_exception() = default;

	protected:

	private:
	};

	using bc_logic_exception = std::logic_error;
	using bc_runtime_exception = std::runtime_error;
	using bc_invalid_argument_exception = std::invalid_argument;
	using bc_out_of_range_exception = std::out_of_range;
	using bc_key_not_found_exception = std::runtime_error;
}
