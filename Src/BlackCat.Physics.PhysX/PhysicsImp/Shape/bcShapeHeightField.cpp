// [20/12/2016 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/bcExport.h"
#include "PhysicsImp/Shape/bcShapeHeightField.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_height_field<g_api_physx>::bc_platform_shape_height_field() noexcept
			: bc_platform_shape_geometry()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_height_field<g_api_physx>::bc_platform_shape_height_field(platform_pack& p_pack) noexcept
			: bc_platform_shape_geometry(),
			m_pack(p_pack)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_height_field<g_api_physx>::bc_platform_shape_height_field(const bc_height_field& p_height_field, bcUINT16 p_xz_scale, bcFLOAT p_y_scale) noexcept
			: bc_platform_shape_geometry(),
			m_pack(physx::PxHeightFieldGeometry
			(
				static_cast<physx::PxHeightField*>(p_height_field.get_platform_pack().m_px_object),
				static_cast<physx::PxMeshGeometryFlag::Enum>(0),
				p_y_scale,
				p_xz_scale,
				p_xz_scale
			))
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_height_field<g_api_physx>::bc_platform_shape_height_field(const bc_platform_shape_height_field& p_other) noexcept
			: bc_platform_shape_geometry(p_other),
			m_pack(p_other.m_pack)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_height_field<g_api_physx>::~bc_platform_shape_height_field()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_height_field<g_api_physx>& bc_platform_shape_height_field<g_api_physx>::operator=(const bc_platform_shape_height_field& p_other) noexcept
		{
			bc_platform_shape_geometry::operator=(p_other);
			m_pack = p_other.m_pack;

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_height_field bc_platform_shape_height_field<g_api_physx>::get_height_field() const noexcept
		{
			bc_height_field::platform_pack l_pack;
			l_pack.m_px_object = m_pack.m_px_height_field.heightField;
			
			return bc_height_field(l_pack);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT16 bc_platform_shape_height_field<g_api_physx>::get_xz_scale() const noexcept
		{
			return m_pack.m_px_height_field.rowScale;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT16 bc_platform_shape_height_field<g_api_physx>::get_y_scale() const noexcept
		{
			return m_pack.m_px_height_field.heightScale;
		}
	}
}