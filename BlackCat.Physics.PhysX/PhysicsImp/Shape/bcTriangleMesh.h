// [12/12/2016 MRB]

#pragma once

#include "PhysicsImp/bcPhysicsReference.h"
#include "PhysicsImp/bcPhysicsRef.h"
#include "PhysicsImp/Fundation/bcMaterial.h"
#include "PhysicsImp/Shape/bcBoundBox.h"

#include "Physics/Shape/bcTriangleMesh.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		struct bc_platform_triangle_mesh_pack< g_api_physx >
		{
		};
	}
}
