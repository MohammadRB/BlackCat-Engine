// [23/02/2021 MRB]

#pragma once

#include "Core/Utility/bcEnumOperand.h"
#include "Physics/Collision/bcQueryGroup.h"

namespace black_cat
{
	namespace game
	{
		enum class bc_actor_physics_group : core::bc_enum::enum_t<physics::bc_query_group>
		{
			terrain = static_cast<core::bc_enum::enum_t<physics::bc_query_group>>(physics::bc_query_group::group1),
			skinned_mesh = static_cast<core::bc_enum::enum_t<physics::bc_query_group>>(physics::bc_query_group::group2),
			all = static_cast<core::bc_enum::enum_t<physics::bc_query_group>>(physics::bc_query_group::all)
		};
	}	
}