// [12/20/2016 MRB]

#pragma once

#include "PhysicsImp/Shape/bcShapeGeometry.h"

#include "Physics/Shape/bcShapeBox.h"
#include "PhysicsImp/PhysicsImpPCH.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		struct bc_platform_shape_box_pack<g_api_physx> : bc_platform_shape_geometry_pack<g_api_physx>
		{
			explicit bc_platform_shape_box_pack(const physx::PxBoxGeometry& p_px_box)
				: bc_platform_shape_geometry_pack(m_px_box),
				m_px_box(p_px_box)
			{
			}

			bc_platform_shape_box_pack(const bc_platform_shape_box_pack& p_other)
				: bc_platform_shape_geometry_pack(m_px_box),
				m_px_box(p_other.m_px_box)
			{
			}

			~bc_platform_shape_box_pack() = default;

			bc_platform_shape_box_pack& operator=(const bc_platform_shape_box_pack& p_other)
			{
				bc_platform_shape_geometry_pack::operator=(m_px_box);
				m_px_box = p_other.m_px_box;
				
				return *this;
			}
			
			physx::PxBoxGeometry m_px_box;
		};
	}
}