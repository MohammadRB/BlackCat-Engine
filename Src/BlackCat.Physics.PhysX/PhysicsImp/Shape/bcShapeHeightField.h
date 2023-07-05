// [20/12/2016 MRB]

#pragma once

#include "PhysicsImp/Shape/bcHeightField.h"
#include "PhysicsImp/Shape/bcShapeGeometry.h"

#include "Physics/Shape/bcShapeHeightField.h"
#include "PhysicsImp/PhysicsImpPCH.h"

namespace black_cat::physics
{
	template<>
	struct bc_platform_shape_height_field_pack<g_api_physx> : bc_platform_shape_geometry_pack<g_api_physx>
	{
		bc_platform_shape_height_field_pack()
			: bc_platform_shape_geometry_pack(m_px_height_field)
		{
		}

		explicit bc_platform_shape_height_field_pack(const physx::PxHeightFieldGeometry& p_px_height_field)
			: bc_platform_shape_geometry_pack(m_px_height_field),
			  m_px_height_field(p_px_height_field)
		{
		}

		bc_platform_shape_height_field_pack(const bc_platform_shape_height_field_pack& p_other)
			: bc_platform_shape_geometry_pack(m_px_height_field),
			  m_px_height_field(p_other.m_px_height_field)
		{
		}

		~bc_platform_shape_height_field_pack() = default;

		bc_platform_shape_height_field_pack& operator=(const bc_platform_shape_height_field_pack& p_other)
		{
			bc_platform_shape_geometry_pack::operator=(m_px_height_field);
			m_px_height_field = p_other.m_px_height_field;

			return *this;
		}

		physx::PxHeightFieldGeometry m_px_height_field;
	};
}
