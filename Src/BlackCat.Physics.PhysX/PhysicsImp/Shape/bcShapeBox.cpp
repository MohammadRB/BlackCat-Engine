// [12/20/2016 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"

#include "GraphicImp/bcRenderApiInfo.h"
#include "PhysicsImp/bcExport.h"
#include "PhysicsImp/Shape/bcShapeBox.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_box<g_api_physx>::bc_platform_shape_box(platform_pack& p_pack)
			: bc_platform_shape_geometry(bc_platform_shape_geometry::platform_pack(m_pack.m_px_geometry)),
			m_pack(p_pack)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_box<g_api_physx>::bc_platform_shape_box(bcFLOAT p_x, bcFLOAT p_y, bcFLOAT p_z)
			: bc_platform_shape_geometry(bc_platform_shape_geometry::platform_pack(m_pack.m_px_geometry)),
			m_pack(physx::PxBoxGeometry(p_x, p_y, p_z))
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_box<g_api_physx>::bc_platform_shape_box(const core::bc_vector3f& p_half_extends)
			: bc_platform_shape_geometry(bc_platform_shape_geometry::platform_pack(m_pack.m_px_geometry)),
			m_pack(physx::PxBoxGeometry(p_half_extends.x, p_half_extends.y, p_half_extends.z))
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_box<g_api_physx>::bc_platform_shape_box(const bc_platform_shape_box& p_other)
			: bc_platform_shape_geometry(bc_platform_shape_geometry::platform_pack(m_pack.m_px_geometry)),
			m_pack(p_other.m_pack)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_box<g_api_physx>::~bc_platform_shape_box()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape_box<g_api_physx>& bc_platform_shape_box<g_api_physx>::operator=(const bc_platform_shape_box& p_other)
		{
			bc_platform_shape_geometry::operator=(bc_platform_shape_geometry::platform_pack(m_pack.m_px_geometry));
			m_pack.m_px_geometry = p_other.m_pack.m_px_geometry;

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		core::bc_vector3f bc_platform_shape_box<g_api_physx>::get_half_extends() const noexcept
		{
			return core::bc_vector3f
			(
				m_pack.m_px_geometry.halfExtents.x, 
				m_pack.m_px_geometry.halfExtents.y, 
				m_pack.m_px_geometry.halfExtents.z
			);
		}

		template< >
		BC_PHYSICSIMP_DLL
		void bc_platform_shape_box<g_api_physx>::get_points(core::bc_array<core::bc_vector3f, 8>& p_result) const noexcept
		{
			const auto l_z_sign = graphic::bc_render_api_info::use_left_handed() ? +1 : -1;
			const auto l_half_x = m_pack.m_px_geometry.halfExtents.x;
			const auto l_half_y = m_pack.m_px_geometry.halfExtents.y;
			const auto l_half_z = m_pack.m_px_geometry.halfExtents.z;

			p_result[0] = core::bc_vector3f(-l_half_x, l_half_y, l_half_z * l_z_sign);
			p_result[1] = core::bc_vector3f(-l_half_x, l_half_y, -l_half_z * l_z_sign);
			p_result[2] = core::bc_vector3f(l_half_x, l_half_y, l_half_z * l_z_sign);
			p_result[3] = core::bc_vector3f(l_half_x, l_half_y, -l_half_z * l_z_sign);
			p_result[4] = core::bc_vector3f(-l_half_x, -l_half_y, l_half_z * l_z_sign);
			p_result[5] = core::bc_vector3f(-l_half_x, -l_half_y, -l_half_z * l_z_sign);
			p_result[6] = core::bc_vector3f(l_half_x, -l_half_y, l_half_z * l_z_sign);
			p_result[7] = core::bc_vector3f(l_half_x, -l_half_y, -l_half_z * l_z_sign);
		}
	}
}