// [20/12/2016 MRB]

#pragma once

#include "PhysicsImp/Shape/bcTriangleMesh.h"
#include "PhysicsImp/Shape/bcShapeGeometry.h"

#include "Physics/Shape/bcShapeTriangleMesh.h"
#include "PhysicsImp/PhysicsImpPCH.h"

namespace black_cat::physics
{
	template<>
	struct bc_platform_shape_triangle_mesh_pack<g_api_physx> : bc_platform_shape_geometry_pack<g_api_physx>
	{
		bc_platform_shape_triangle_mesh_pack()
			: bc_platform_shape_geometry_pack(m_px_mesh_geometry)
		{
		}

		explicit bc_platform_shape_triangle_mesh_pack(const physx::PxTriangleMeshGeometry& p_px_mesh_geometry)
			: bc_platform_shape_geometry_pack(m_px_mesh_geometry),
			  m_px_mesh_geometry(p_px_mesh_geometry)
		{
		}

		bc_platform_shape_triangle_mesh_pack(const bc_platform_shape_triangle_mesh_pack& p_other)
			: bc_platform_shape_geometry_pack(m_px_mesh_geometry),
			  m_px_mesh_geometry(p_other.m_px_mesh_geometry)
		{
		}

		~bc_platform_shape_triangle_mesh_pack() = default;

		bc_platform_shape_triangle_mesh_pack& operator=(const bc_platform_shape_triangle_mesh_pack& p_other)
		{
			bc_platform_shape_geometry_pack::operator=(m_px_mesh_geometry);
			m_px_mesh_geometry = p_other.m_px_mesh_geometry;

			return *this;
		}

		physx::PxTriangleMeshGeometry m_px_mesh_geometry;
	};
}
