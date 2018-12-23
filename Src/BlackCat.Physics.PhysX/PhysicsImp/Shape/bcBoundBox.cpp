// [12/08/2016 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/bcExport.h"
#include "PhysicsImp/Fundation/bcTransform.h"
#include "PhysicsImp/Shape/bcBoundBox.h"
#include "PhysicsImp/bcUtility.h"

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
				bc_to_right_hand(p_center),
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
			return bc_to_game_hand(l_vec3);
		}

		template<>
		BC_PHYSICSIMP_DLL
		core::bc_vector3f bc_platform_bound_box< g_api_physx >::get_half_extends() const noexcept
		{
			auto l_vec3 = m_pack.m_bound.getExtents();
			return core::bc_vector3f(l_vec3.x, l_vec3.y, l_vec3.z);
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_bound_box< g_api_physx >::expand(const core::bc_vector3f& p_point) noexcept
		{
			m_pack.m_bound.include(bc_to_right_hand(p_point));
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_bound_box< g_api_physx >::expand(const bc_platform_bound_box& p_box) noexcept
		{
			m_pack.m_bound.include(const_cast< bc_platform_bound_box& >(p_box).get_platform_pack().m_bound);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bool bc_platform_bound_box< g_api_physx >::intersect(const bc_platform_bound_box& p_box) const noexcept
		{
			return m_pack.m_bound.intersects(const_cast<bc_platform_bound_box&>(p_box).get_platform_pack().m_bound);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bool bc_platform_bound_box< g_api_physx >::contains(const core::bc_vector3f& p_point) const noexcept
		{
			return m_pack.m_bound.contains(bc_to_right_hand(p_point));
		}

		template<>
		BC_PHYSICSIMP_DLL
		bool bc_platform_bound_box< g_api_physx >::contains(const bc_platform_bound_box& p_box) const noexcept
		{
			return const_cast<bc_platform_bound_box&>(p_box).get_platform_pack().m_bound.isInside(m_pack.m_bound);
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_bound_box< g_api_physx >::scale(bcFLOAT p_scale) noexcept
		{
			m_pack.m_bound.scaleFast(p_scale);
		}

		template< >
		BC_PHYSICSIMP_DLL
		bool bc_platform_bound_box< g_api_physx >::is_empty() const noexcept
		{
			return m_pack.m_bound.isEmpty();
		}

		template< >
		BC_PHYSICSIMP_DLL
		void bc_platform_bound_box< g_api_physx >::set_empty() noexcept
		{
			m_pack.m_bound = m_pack.m_bound.empty();
		}

		template< >
		BC_PHYSICSIMP_DLL
		void bc_platform_bound_box< g_api_physx >::transform(const bc_transform& p_transform) noexcept
		{
			m_pack.m_bound = physx::PxBounds3::transformSafe(const_cast<bc_transform&>(p_transform).get_platform_pack().m_px_transform, m_pack.m_bound);
		}

		template< >
		BC_PHYSICSIMP_DLL
		void bc_platform_bound_box< g_api_physx >::transform(const core::bc_matrix4f& p_transform) noexcept
		{
			transform(bc_transform(p_transform));
		}

		template< >
		BC_PHYSICSIMP_DLL
		void bc_platform_bound_box< g_api_physx >::transform(const core::bc_matrix3f& p_transform) noexcept
		{
			m_pack.m_bound = physx::PxBounds3::transformSafe(bc_to_right_hand(p_transform), m_pack.m_bound);
		}

		template< >
		BC_PHYSICSIMP_DLL
		void bc_platform_bound_box< g_api_physx >::get_points(core::bc_array<core::bc_vector3f, 8>& p_result) const noexcept
		{
			const auto l_z_sign = graphic::bc_render_api_info::is_left_handed() ? +1 : -1;
			const auto l_position = get_center();
			const auto l_half_extend = get_half_extends();

			p_result[0] = l_position + core::bc_vector3f(-l_half_extend.x, l_half_extend.y, l_half_extend.z * l_z_sign);
			p_result[1] = l_position + core::bc_vector3f(-l_half_extend.x, l_half_extend.y, -l_half_extend.z * l_z_sign);
			p_result[2] = l_position + core::bc_vector3f(l_half_extend.x, l_half_extend.y, -l_half_extend.z * l_z_sign);
			p_result[3] = l_position + core::bc_vector3f(l_half_extend.x, l_half_extend.y, l_half_extend.z * l_z_sign);
			p_result[4] = l_position + core::bc_vector3f(-l_half_extend.x, -l_half_extend.y, l_half_extend.z * l_z_sign);
			p_result[5] = l_position + core::bc_vector3f(-l_half_extend.x, -l_half_extend.y, -l_half_extend.z * l_z_sign);
			p_result[6] = l_position + core::bc_vector3f(l_half_extend.x, -l_half_extend.y, -l_half_extend.z * l_z_sign);
			p_result[7] = l_position + core::bc_vector3f(l_half_extend.x, -l_half_extend.y, l_half_extend.z * l_z_sign);
		}
	}
}