// [13/03/2022 MRB]

#pragma once

#include "Core/Utility/bcEnumOperand.h"

namespace black_cat::sound
{
	enum class bc_sound_mode
	{
		none = 0,
		loop_off = core::bc_enum::value(0),
		loop = core::bc_enum::value(1),
		d3 = core::bc_enum::value(2)
	};
}
