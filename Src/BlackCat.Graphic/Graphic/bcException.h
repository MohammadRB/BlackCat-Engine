// [01/23/2016 MRB]

#pragma once

#include <exception>
#include "CorePlatform/bcType.h"
#include "CorePlatform/bcException.h"
#include "Core/bcException.h"
#include "Core/Container/bcString.h"

namespace black_cat
{
	class bc_graphic_error_category : public std::error_category
	{
	public:
		const bcCHAR* name() const noexcept override
		{
			return "graphic";
		}

		std::string message(int p_errval) const override
		{
			return std::string();
		}
	};

	inline const std::error_category& bc_graphic_category() noexcept
	{
		static bc_graphic_error_category s_graphic_category;

		return s_graphic_category;
	};

	class bc_graphic_exception : public std::system_error
	{
	public:
		explicit bc_graphic_exception(bcINT p_code, const bcCHAR* p_message);

		explicit bc_graphic_exception(bcINT p_code, core::bc_string p_message);

		bc_graphic_exception(const bc_graphic_exception&) = default;

		bc_graphic_exception(bc_graphic_exception&&) = default;

		~bc_graphic_exception() = default;

		bc_graphic_exception& operator=(const bc_graphic_exception&) = default;

		bc_graphic_exception& operator=(bc_graphic_exception&&) = default;

		const bcCHAR* what() const override;

	protected:

	private:
		core::bc_string m_string; // Due to existence of this string, std::system_error string will left uninitialized
	};

	inline bc_graphic_exception::bc_graphic_exception(bcINT p_code, const bcCHAR* p_message)
		: std::system_error(p_code, bc_graphic_category()),
		m_string(p_message)
	{
	};

	inline bc_graphic_exception::bc_graphic_exception(bcINT p_code, core::bc_string p_message)
		: std::system_error(p_code, bc_graphic_category()),
		m_string(std::move(p_message))
	{
	}

	inline const bcCHAR* bc_graphic_exception::what() const
	{
		return m_string.c_str();
	}
}