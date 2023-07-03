// [28/11/2016 MRB]

#pragma once

namespace black_cat
{
	namespace physics
	{
		enum class bc_physics_api
		{
			unknown = 0,
			physx= 1
		};

		constexpr bc_physics_api g_api_unknown = bc_physics_api::unknown;
		constexpr bc_physics_api g_api_physx = bc_physics_api::physx;
		constexpr bc_physics_api g_current_physics_api = g_api_physx;
	}
}