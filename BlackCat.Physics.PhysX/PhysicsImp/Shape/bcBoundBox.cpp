// [12/08/2016 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/bcExport.h"
#include "PhysicsImp/Shape/bcBoundBox.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_bound_box< g_api_physx >::bc_platform_bound_box() noexcept
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_bound_box< g_api_physx >::bc_platform_bound_box(const core::bc_vector3f& p_center, const core::bc_vector3f& p_half_extend) noexcept
		{
			m_pack.m_bound = physx::PxBounds3::centerExtents
			(
				physx::PxVec3(p_center.x, p_center.y, p_center.z),
				physx::PxVec3(p_half_extend.x, p_half_extend.y, p_half_extend.z)
			);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_bound_box< g_api_physx >::bc_platform_bound_box(const bc_platform_bound_box& p_other) noexcept
			: m_pack(p_other.m_pack)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_bound_box< g_api_physx >::~bc_platform_bound_box()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_bound_box< g_api_physx >& bc_platform_bound_box< g_api_physx >::operator=(const bc_platform_bound_box& p_other) noexcept
		{
			m_pack = p_other.m_pack;

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		core::bc_vector3f bc_platform_bound_box< g_api_physx >::get_center() const noexcept
		{
			auto l_vec3 = m_pack.m_bound.getCenter();
			return core::bc_vector3f(l_vec3.x, l_vec3.y, l_vec3.z);
		}

		template<>
		BC_PHYSICSIMP_DLL
		core::bc_vector3f bc_platform_bound_box< g_api_physx >::get_extend() const noexcept
		{
			auto l_vec3 = m_pack.m_bound.getExtents();
			return core::bc_vector3f(l_vec3.x, l_vec3.y, l_vec3.z);
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_bound_box< g_api_physx >::expand(const core::bc_vector3f& p_point) noexcept
		{
			m_pack.m_bound.include(physx::PxVec3(p_point.x, p_point.y, p_point.z));
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_bound_box< g_api_physx >::expand(const bc_platform_bound_box& p_box) noexcept
		{
			m_pack.m_bound.include(const_cast< bc_platform_bound_box& >(p_box).get_platform_pack().m_bound);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bool bc_platform_bound_box< g_api_physx >::intersect(const bc_platform_bound_box& p_box) noexcept
		{
			return m_pack.m_bound.intersects(const_cast<bc_platform_bound_box&>(p_box).get_platform_pack().m_bound);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bool bc_platform_bound_box< g_api_physx >::contains(const core::bc_vector3f& p_point) const noexcept
		{
			return m_pack.m_bound.contains(physx::PxVec3(p_point.x, p_point.y, p_point.z));
		}

		template<>
		BC_PHYSICSIMP_DLL
		bool bc_platform_bound_box< g_api_physx >::contain(const bc_platform_bound_box& p_box) noexcept
		{
			return m_pack.m_bound.isInside(const_cast<bc_platform_bound_box&>(p_box).get_platform_pack().m_bound);
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_bound_box< g_api_physx >::scale(bcFLOAT p_scale) noexcept
		{
			m_pack.m_bound.scaleFast(p_scale);
		}
	}
}