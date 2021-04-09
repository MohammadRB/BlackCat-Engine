// [12/20/2016 MRB]

#pragma once

#include "PhysicsImp/Shape/bcShapeGeometry.h"

#include "Physics/Shape/bcShapeSphere.h"
#include "PhysicsImp/PhysicsImpPCH.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		struct bc_platform_shape_sphere_pack<g_api_physx> : bc_platform_shape_geometry_pack<g_api_physx>
		{
			explicit bc_platform_shape_sphere_pack(const physx::PxSphereGeometry& p_px_sphere)
				: bc_platform_shape_geometry_pack(m_px_sphere),
				m_px_sphere(p_px_sphere)
			{
			}

			bc_platform_shape_sphere_pack(const bc_platform_shape_sphere_pack& p_other)
				: bc_platform_shape_geometry_pack(m_px_sphere),
				m_px_sphere(p_other.m_px_sphere)
			{
			}

			~bc_platform_shape_sphere_pack() = default;

			bc_platform_shape_sphere_pack& operator=(const bc_platform_shape_sphere_pack& p_other)
			{
				bc_platform_shape_geometry_pack::operator=(m_px_sphere);
				m_px_sphere = p_other.m_px_sphere;

				return *this;
			}
			
			physx::PxSphereGeometry m_px_sphere;
		};
	}
}