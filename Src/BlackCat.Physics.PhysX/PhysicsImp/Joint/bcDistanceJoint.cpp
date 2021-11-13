// [12/15/2016 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/bcExport.h"
#include "PhysicsImp/Joint/bcDistanceJoint.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_distance_joint<g_api_physx>::bc_platform_distance_joint() noexcept
			: bc_platform_joint()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_distance_joint<g_api_physx>::bc_platform_distance_joint(platform_pack& p_pack) noexcept
			: bc_platform_joint(p_pack)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_distance_joint<g_api_physx>::bc_platform_distance_joint(const bc_platform_distance_joint& p_other) noexcept
			: bc_platform_joint(p_other)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_distance_joint<g_api_physx>::~bc_platform_distance_joint()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_distance_joint<g_api_physx>& bc_platform_distance_joint<g_api_physx>::operator=(const bc_platform_distance_joint& p_other) noexcept
		{
			bc_platform_joint::operator=(p_other);

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcFLOAT bc_platform_distance_joint<g_api_physx>::get_distance() const noexcept
		{
			auto* l_px_joint = static_cast<physx::PxDistanceJoint*>(get_platform_pack().m_px_object);

			return l_px_joint->getDistance();
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_distance_joint<g_api_physx>::enable_limit(const bc_joint_linear_limit_pair& p_limit) noexcept
		{
			auto* l_px_joint = static_cast<physx::PxDistanceJoint*>(get_platform_pack().m_px_object);

			l_px_joint->setDistanceJointFlag(physx::PxDistanceJointFlag::eMIN_DISTANCE_ENABLED, true);
			l_px_joint->setDistanceJointFlag(physx::PxDistanceJointFlag::eMAX_DISTANCE_ENABLED, true);
			l_px_joint->setDistanceJointFlag(physx::PxDistanceJointFlag::eSPRING_ENABLED, true);

			l_px_joint->setMinDistance(p_limit.m_lower);
			l_px_joint->setMaxDistance(p_limit.m_upper);
			l_px_joint->setStiffness(p_limit.m_stiffness);
			l_px_joint->setDamping(p_limit.m_damping);
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_distance_joint<g_api_physx>::disable_limit() noexcept
		{
			auto* l_px_joint = static_cast<physx::PxDistanceJoint*>(get_platform_pack().m_px_object);

			l_px_joint->setDistanceJointFlag(physx::PxDistanceJointFlag::eMIN_DISTANCE_ENABLED, false);
			l_px_joint->setDistanceJointFlag(physx::PxDistanceJointFlag::eMAX_DISTANCE_ENABLED, false);
			l_px_joint->setDistanceJointFlag(physx::PxDistanceJointFlag::eSPRING_ENABLED, false);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_joint_linear_limit_pair bc_platform_distance_joint<g_api_physx>::get_limit() const noexcept
		{
			auto* l_px_joint = static_cast<physx::PxDistanceJoint*>(get_platform_pack().m_px_object);

			return bc_joint_linear_limit_pair
			(
				l_px_joint->getMaxDistance(),
				l_px_joint->getMinDistance(),
				l_px_joint->getStiffness(),
				l_px_joint->getDamping()
			);
		}
	}
}