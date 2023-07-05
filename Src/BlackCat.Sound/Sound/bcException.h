// [10/03/2022 MRB]

#pragma once

#include "Core/bcException.h"

namespace black_cat::sound
{
	class bc_sound_error_category : public std::error_category
	{
	public:
		const bcCHAR* name() const noexcept override
		{
			return "physics";
		}

		std::string message(int p_errval) const override
		{
			return {};
		}
	};

	inline const std::error_category& bc_physics_category() noexcept
	{
		static bc_sound_error_category s_sound_category;
		return s_sound_category;
	}

	class bc_sound_exception : public std::system_error
	{
	public:
		bc_sound_exception(bcINT32 p_code, const bcCHAR* p_message);

		bc_sound_exception(const bc_sound_exception&) = default;

		bc_sound_exception(bc_sound_exception&&) = default;

		~bc_sound_exception() override = default;

		bc_sound_exception& operator=(const bc_sound_exception&) = default;

		bc_sound_exception& operator=(bc_sound_exception&&) = default;
	};

	inline bc_sound_exception::bc_sound_exception(bcINT32 p_code, const bcCHAR* p_message)
		: std::system_error(p_code, bc_physics_category(), p_message)
	{
	}
}
