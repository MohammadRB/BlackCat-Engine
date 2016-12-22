// [12/12/2016 MRB]

#pragma once

#include "PhysicsImp/bcPhysicsReference.h"
#include "PhysicsImp/bcPhysicsRef.h"
#include "PhysicsImp/Fundation/bcTransform.h"
#include "PhysicsImp/Fundation/bcMaterial.h"
#include "PhysicsImp/Shape/bcShapeGeometry.h"
#include "PhysicsImp/Shape/bcShapeSphere.h"
#include "PhysicsImp/Shape/bcShapeBox.h"
#include "PhysicsImp/Shape/bcShapeCapsule.h"
#include "PhysicsImp/Shape/bcShapePlane.h"
#include "PhysicsImp/Shape/bcShapeConvexMesh.h"
#include "PhysicsImp/Shape/bcShapeTriangleMesh.h"
#include "PhysicsImp/Shape/bcShapeHeightField.h"

#include "Physics/Shape/bcShape.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		struct bc_platform_shape_pack<g_api_physx>
		{
		};
	}
}