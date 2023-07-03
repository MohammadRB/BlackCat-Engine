// [03/12/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Core/Utility/bcEnumOperand.h"

namespace black_cat
{
	namespace physics
	{
		enum class bc_collision_group : bcUINT32
		{
			group1 = core::bc_enum::value(0),
			group2 = core::bc_enum::value(1),
			group3 = core::bc_enum::value(2),
			group4 = core::bc_enum::value(3),
			group5 = core::bc_enum::value(4),
			group6 = core::bc_enum::value(5),
			group7 = core::bc_enum::value(6),
			group8 = core::bc_enum::value(7),
			group9 = core::bc_enum::value(8),
			group10 = core::bc_enum::value(9),
			group11 = core::bc_enum::value(10),
			group12 = core::bc_enum::value(11),
			group13 = core::bc_enum::value(12),
			group14 = core::bc_enum::value(13),
			group15 = core::bc_enum::value(14),
			group16 = core::bc_enum::value(15),
			group17 = core::bc_enum::value(16),
			group18 = core::bc_enum::value(17),
			group19 = core::bc_enum::value(18),
			group20 = core::bc_enum::value(19),
			group21 = core::bc_enum::value(20),
			group22 = core::bc_enum::value(21),
			group23 = core::bc_enum::value(22),
			group24 = core::bc_enum::value(23),
			group25 = core::bc_enum::value(24),
			group26 = core::bc_enum::value(25),
			group27 = core::bc_enum::value(26),
			group28 = core::bc_enum::value(27),
			group29 = core::bc_enum::value(28),
			group30 = core::bc_enum::value(29),
			group31 = core::bc_enum::value(30),
			all = group1 | group2 | group3 | group4 | group5 | group6 | group7 | group8 | group9 | group10 |
			group11 | group12 | group13 | group14 | group15 | group16 | group17 | group18 | group19 | group20 |
			group21 | group22 | group23 | group24 | group25 | group26 | group27 | group28 | group29 | group30 |
			group31
		};

		struct bc_collision_filter
		{
			bc_collision_filter(bc_collision_group p_group, bc_collision_group p_mask)
				: m_group(p_group),
				m_mask(p_mask)
			{
			}

			const bc_collision_group m_group;
			const bc_collision_group m_mask;
		};
	}
}