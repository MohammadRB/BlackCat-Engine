// [09/02/2022 MRB]

#pragma once

#include "App/bcConstant.h"

namespace box
{
	using namespace black_cat;

	BC_PARAMETER(team, "team");

	enum class bx_team : bcUBYTE
	{
		red, blue
	};
}
