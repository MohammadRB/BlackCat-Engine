// [12/20/2016 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/bcExport.h"
#include "PhysicsImp/Shape/bcShapeHeightField.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_height_field< g_api_physx >::bc_platform_shape_height_field(platform_pack& p_pack)
			: bc_platform_shape_geometry(bc_platform_shape_geometry::platform_pack(m_pack.m_px_geometry)),
			m_pack(p_pack)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_height_field< g_api_physx >::bc_platform_shape_height_field(const bc_height_field& p_height_field, bcUINT16 p_xz_scale, bcUINT16 p_y_scale)
			: bc_platform_shape_geometry(bc_platform_shape_geometry::platform_pack(m_pack.m_px_geometry)),
			m_pack(physx::PxHeightFieldGeometry())
		{
			m_pack.m_px_geometry.heightField = static_cast< physx::PxHeightField* >
			(
				static_cast< bc_physics_reference& >(const_cast< bc_height_field& >(p_height_field)).get_platform_pack().m_px_object
			);
			m_pack.m_px_geometry.rowScale = p_xz_scale;
			m_pack.m_px_geometry.columnScale = p_xz_scale;
			m_pack.m_px_geometry.heightScale = p_y_scale;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_height_field< g_api_physx >::bc_platform_shape_height_field(const bc_platform_shape_height_field& p_other)
			: bc_platform_shape_geometry(p_other),
			m_pack(p_other.m_pack)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_height_field< g_api_physx >::~bc_platform_shape_height_field()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_height_field< g_api_physx >& bc_platform_shape_height_field< g_api_physx >::operator=(const bc_platform_shape_height_field& p_other)
		{
			bc_platform_shape_geometry::operator=(p_other);
			m_pack.m_px_geometry = p_other.m_pack.m_px_geometry;

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_height_field bc_platform_shape_height_field< g_api_physx >::get_height_field() const noexcept
		{
			bc_height_field l_result;
			static_cast< bc_physics_reference& >(l_result).get_platform_pack().m_px_object = m_pack.m_px_geometry.heightField;

			return l_result;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT16 bc_platform_shape_height_field< g_api_physx >::get_xz_scale() const noexcept
		{
			return m_pack.m_px_geometry.rowScale;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT16 bc_platform_shape_height_field< g_api_physx >::get_y_scale() const noexcept
		{
			return m_pack.m_px_geometry.heightScale;
		}
	}
}
