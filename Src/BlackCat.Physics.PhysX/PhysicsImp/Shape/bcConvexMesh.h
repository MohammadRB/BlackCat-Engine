// [11/12/2016 MRB]

#pragma once

#include "PhysicsImp/bcPhysicsReference.h"
#include "PhysicsImp/bcPhysicsRef.h"
#include "PhysicsImp/Shape/bcBoundBox.h"

#include "Physics/Shape/bcConvexMesh.h"
#include "PlatformImp/PlatformImpPCH.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		struct bc_platform_convex_mesh_pack<g_api_physx> : bc_platform_physics_reference_pack<g_api_physx>
		{
			constexpr static bcUINT16 s_polygon_limit = 256;
		};
	}
}