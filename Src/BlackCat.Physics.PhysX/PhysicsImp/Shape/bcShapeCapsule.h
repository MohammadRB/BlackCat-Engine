// [12/20/2016 MRB]

#pragma once

#include "PhysicsImp/Shape/bcShapeGeometry.h"

#include "Physics/Shape/bcShapeCapsule.h"
#include "PhysicsImp/PhysicsImpPCH.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		struct bc_platform_shape_capsule_pack<g_api_physx> : bc_platform_shape_geometry_pack<g_api_physx>
		{
			bc_platform_shape_capsule_pack()
				: bc_platform_shape_geometry_pack(m_px_capsule)
			{
			}

			explicit bc_platform_shape_capsule_pack(const physx::PxCapsuleGeometry& p_px_capsule)
				: bc_platform_shape_geometry_pack(m_px_capsule),
				m_px_capsule(p_px_capsule)
			{
			}

			bc_platform_shape_capsule_pack(const bc_platform_shape_capsule_pack& p_other)
				: bc_platform_shape_geometry_pack(m_px_capsule),
				m_px_capsule(p_other.m_px_capsule)
			{
			}

			~bc_platform_shape_capsule_pack() = default;

			bc_platform_shape_capsule_pack& operator=(const bc_platform_shape_capsule_pack& p_other)
			{
				bc_platform_shape_geometry_pack::operator=(m_px_capsule);
				m_px_capsule = p_other.m_px_capsule;

				return *this;
			}

			physx::PxCapsuleGeometry m_px_capsule;
		};
	}
}