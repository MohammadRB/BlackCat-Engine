// [20/12/2016 MRB]

#pragma once

#include "PhysicsImp/Shape/bcShapeGeometry.h"
#include "Physicsimp/Fundation/bcTransform.h"

#include "Physics/Shape/bcShapePlane.h"
#include "PhysicsImp/PhysicsImpPCH.h"

namespace black_cat::physics
{
	template<>
	struct bc_platform_shape_plane_pack<g_api_physx> : bc_platform_shape_geometry_pack<g_api_physx>
	{
		bc_platform_shape_plane_pack()
			: bc_platform_shape_geometry_pack(m_px_plane_geometry)
		{
		}

		explicit bc_platform_shape_plane_pack(const physx::PxPlaneGeometry& p_px_plane_geometry, const physx::PxPlane& p_px_plane)
			: bc_platform_shape_geometry_pack(m_px_plane_geometry),
			  m_px_plane_geometry(p_px_plane_geometry),
			  m_px_plane(p_px_plane)
		{
		}

		bc_platform_shape_plane_pack(const bc_platform_shape_plane_pack& p_other)
			: bc_platform_shape_geometry_pack(m_px_plane_geometry),
			  m_px_plane_geometry(p_other.m_px_plane_geometry),
			  m_px_plane(p_other.m_px_plane)
		{
		}

		~bc_platform_shape_plane_pack() = default;

		bc_platform_shape_plane_pack& operator=(const bc_platform_shape_plane_pack& p_other)
		{
			bc_platform_shape_geometry_pack::operator=(m_px_plane_geometry);
			m_px_plane_geometry = p_other.m_px_plane_geometry;
			m_px_plane = p_other.m_px_plane;

			return *this;
		}

		physx::PxPlaneGeometry m_px_plane_geometry;
		physx::PxPlane m_px_plane;
	};
}
