// [12/13/2016 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/bcExport.h"
#include "PhysicsImp/Fundation/bcTransform.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_transform<g_api_physx>::bc_platform_transform() noexcept
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_transform<g_api_physx>::bc_platform_transform(const core::bc_vector3f& p_position) noexcept
		{
			m_pack.m_px_transform = physx::PxTransform(p_position.x, p_position.y, p_position.z);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_transform< g_api_physx >::bc_platform_transform(const core::bc_matrix4f& p_rotation) noexcept
		{
			m_pack.m_px_transform = physx::PxTransform(physx::PxMat44
			(
				physx::PxVec4(p_rotation(0, 0), p_rotation(0, 1), p_rotation(0, 2), p_rotation(0, 3)),
				physx::PxVec4(p_rotation(1, 0), p_rotation(1, 1), p_rotation(1, 2), p_rotation(1, 3)),
				physx::PxVec4(p_rotation(2, 0), p_rotation(2, 1), p_rotation(2, 2), p_rotation(2, 3)),
				physx::PxVec4(p_rotation(3, 0), p_rotation(3, 1), p_rotation(3, 2), p_rotation(3, 3))
			));
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_transform<g_api_physx>::bc_platform_transform(const core::bc_vector3f& p_position, const core::bc_matrix3f& p_rotation) noexcept
		{
			m_pack.m_px_transform = physx::PxTransform
			(
				physx::PxVec3(p_position.x, p_position.y, p_position.z),
				physx::PxQuat(physx::PxMat33
				(
					physx::PxVec3(p_rotation(0, 0), p_rotation(0, 1), p_rotation(0, 2)),
					physx::PxVec3(p_rotation(1, 0), p_rotation(1, 1), p_rotation(1, 2)),
					physx::PxVec3(p_rotation(2, 0), p_rotation(2, 1), p_rotation(2, 2))
				))
			);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_transform<g_api_physx>::bc_platform_transform(const bc_platform_transform& p_other) noexcept
		{
			m_pack.m_px_transform = p_other.m_pack.m_px_transform;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_transform<g_api_physx>::~bc_platform_transform()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_transform<g_api_physx>& bc_platform_transform<g_api_physx>::operator=(const bc_platform_transform& p_other) noexcept
		{
			m_pack.m_px_transform = p_other.m_pack.m_px_transform;

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_transform<g_api_physx> bc_platform_transform<g_api_physx>::operator*(const bc_platform_transform& p_other) noexcept
		{
			bc_platform_transform l_result;

			l_result.m_pack.m_px_transform = m_pack.m_px_transform * p_other.m_pack.m_px_transform;

			return l_result;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_transform<g_api_physx>& bc_platform_transform<g_api_physx>::operator*=(const bc_platform_transform& p_other) noexcept
		{
			m_pack.m_px_transform * p_other.m_pack.m_px_transform;

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_transform<g_api_physx> bc_platform_transform<g_api_physx>::get_inverse() const noexcept
		{
			bc_platform_transform l_result;

			l_result.m_pack.m_px_transform = m_pack.m_px_transform.getInverse();

			return l_result;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_transform<g_api_physx> bc_platform_transform<g_api_physx>::get_normalized() const noexcept
		{
			bc_platform_transform l_result;

			l_result.m_pack.m_px_transform = m_pack.m_px_transform.getNormalized();

			return l_result;
		}

		template<>
		BC_PHYSICSIMP_DLL
		core::bc_vector3f bc_platform_transform<g_api_physx>::transform(const core::bc_vector3f& p_vector) const noexcept
		{
			auto l_px_vec3 = m_pack.m_px_transform.transform(physx::PxVec3(p_vector.x, p_vector.y, p_vector.z));

			return core::bc_vector3f(l_px_vec3.x, l_px_vec3.y, l_px_vec3.z);
		}

		template<>
		BC_PHYSICSIMP_DLL
		core::bc_vector3f bc_platform_transform<g_api_physx>::transform_inverse(const core::bc_vector3f& p_vector) const noexcept
		{
			auto l_px_vec3 = m_pack.m_px_transform.transformInv(physx::PxVec3(p_vector.x, p_vector.y, p_vector.z));

			return core::bc_vector3f(l_px_vec3.x, l_px_vec3.y, l_px_vec3.z);
		}

		template<>
		BC_PHYSICSIMP_DLL
		core::bc_vector3f bc_platform_transform<g_api_physx>::rotate(const core::bc_vector3f& p_vector) const noexcept
		{
			auto l_px_vec3 = m_pack.m_px_transform.rotate(physx::PxVec3(p_vector.x, p_vector.y, p_vector.z));

			return core::bc_vector3f(l_px_vec3.x, l_px_vec3.y, l_px_vec3.z);
		}

		template<>
		BC_PHYSICSIMP_DLL
		core::bc_vector3f bc_platform_transform<g_api_physx>::rotate_inverse(const core::bc_vector3f& p_vector) const noexcept
		{
			auto l_px_vec3 = m_pack.m_px_transform.rotateInv(physx::PxVec3(p_vector.x, p_vector.y, p_vector.z));

			return core::bc_vector3f(l_px_vec3.x, l_px_vec3.y, l_px_vec3.z);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bool bc_platform_transform<g_api_physx>::is_finite() const noexcept
		{
			return m_pack.m_px_transform.isFinite();
		}

		template<>
		BC_PHYSICSIMP_DLL
		bool bc_platform_transform<g_api_physx>::is_valid() const noexcept
		{
			return m_pack.m_px_transform.isValid();
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_transform<g_api_physx> bc_platform_transform<g_api_physx>::identity() noexcept
		{
			bc_platform_transform l_result;

			l_result.m_pack.m_px_transform = physx::PxTransform::createIdentity();

			return l_result;
		}
	}
}