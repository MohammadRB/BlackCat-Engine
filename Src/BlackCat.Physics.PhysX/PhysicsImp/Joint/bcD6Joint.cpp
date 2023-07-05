// [15/12/2016 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/bcExport.h"
#include "PhysicsImp/Joint/bcD6Joint.h"

namespace black_cat::physics
{
	template<>
	BC_PHYSICSIMP_DLL
	bc_platform_d6_joint<g_api_physx>::bc_platform_d6_joint() noexcept
		: bc_platform_joint()
	{
	}

	template<>
	BC_PHYSICSIMP_DLL
	bc_platform_d6_joint<g_api_physx>::bc_platform_d6_joint(platform_pack& p_pack) noexcept
		: bc_platform_joint(p_pack)
	{
	}

	template<>
	BC_PHYSICSIMP_DLL
	bc_platform_d6_joint<g_api_physx>::bc_platform_d6_joint(const bc_platform_d6_joint& p_other) noexcept
		: bc_platform_joint(p_other)
	{
	}

	template<>
	BC_PHYSICSIMP_DLL
	bc_platform_d6_joint<g_api_physx>::~bc_platform_d6_joint()
	{
	}

	template<>
	BC_PHYSICSIMP_DLL
	bc_platform_d6_joint<g_api_physx>& bc_platform_d6_joint<g_api_physx>::operator=(const bc_platform_d6_joint& p_other) noexcept
	{
		bc_platform_joint::operator=(p_other);

		return *this;
	}

	template<>
	BC_PHYSICSIMP_DLL
	void bc_platform_d6_joint<g_api_physx>::set_motion(bc_d6_axis p_axis, bc_d6_motion p_type) noexcept
	{
		auto* l_px_joint = static_cast<physx::PxD6Joint*>(get_platform_pack().m_px_object);

		l_px_joint->setMotion(static_cast<physx::PxD6Axis::Enum>(p_axis), static_cast<physx::PxD6Motion::Enum>(p_type));
	}

	template<>
	BC_PHYSICSIMP_DLL
	bc_d6_axis bc_platform_d6_joint<g_api_physx>::get_motion(bc_d6_axis p_axis) const noexcept
	{
		auto* l_px_joint = static_cast<physx::PxD6Joint*>(get_platform_pack().m_px_object);

		return static_cast<bc_d6_axis>(l_px_joint->getMotion(static_cast<physx::PxD6Axis::Enum>(p_axis)));
	}

	template<>
	BC_PHYSICSIMP_DLL
	bcFLOAT bc_platform_d6_joint<g_api_physx>::get_x_angle() const noexcept
	{
		auto* l_px_joint = static_cast<physx::PxD6Joint*>(get_platform_pack().m_px_object);

		return l_px_joint->getTwist();
	}

	template<>
	BC_PHYSICSIMP_DLL
	bcFLOAT bc_platform_d6_joint<g_api_physx>::get_y_angle() const noexcept
	{
		auto* l_px_joint = static_cast<physx::PxD6Joint*>(get_platform_pack().m_px_object);

		return l_px_joint->getSwingYAngle();
	}

	template<>
	BC_PHYSICSIMP_DLL
	bcFLOAT bc_platform_d6_joint<g_api_physx>::get_z_angle() const noexcept
	{
		auto* l_px_joint = static_cast<physx::PxD6Joint*>(get_platform_pack().m_px_object);

		return l_px_joint->getSwingZAngle();
	}

	template<>
	BC_PHYSICSIMP_DLL
	void bc_platform_d6_joint<g_api_physx>::set_linear_limit(const bc_joint_linear_limit& p_limit)
	{
		auto* l_px_joint = static_cast<physx::PxD6Joint*>(get_platform_pack().m_px_object);

		const physx::PxJointLinearLimit l_px_limit(p_limit.m_limit, physx::PxSpring(p_limit.m_stiffness, p_limit.m_damping));

		l_px_joint->setLinearLimit(l_px_limit);
	}

	template<>
	BC_PHYSICSIMP_DLL
	bc_joint_linear_limit bc_platform_d6_joint<g_api_physx>::get_linear_limit() const noexcept
	{
		auto* l_px_joint = static_cast<physx::PxD6Joint*>(get_platform_pack().m_px_object);

		const auto l_px_limit = l_px_joint->getLinearLimit();

		return bc_joint_linear_limit(l_px_limit.value, l_px_limit.stiffness, l_px_limit.damping);
	}

	template<>
	BC_PHYSICSIMP_DLL
	void bc_platform_d6_joint<g_api_physx>::set_twist_limit(const bc_joint_angular_limit& p_limit) noexcept
	{
		auto* l_px_joint = static_cast<physx::PxD6Joint*>(get_platform_pack().m_px_object);

		const physx::PxJointAngularLimitPair l_px_limit
		(
			p_limit.m_lower,
			p_limit.m_upper,
			physx::PxSpring(p_limit.m_stiffness, p_limit.m_damping)
		);

		l_px_joint->setTwistLimit(l_px_limit);
	}

	template<>
	BC_PHYSICSIMP_DLL
	bc_joint_angular_limit bc_platform_d6_joint<g_api_physx>::get_twist_limit() const noexcept
	{
		auto* l_px_joint = static_cast<physx::PxD6Joint*>(get_platform_pack().m_px_object);

		const auto l_px_limit = l_px_joint->getTwistLimit();

		return bc_joint_angular_limit(l_px_limit.upper, l_px_limit.lower, l_px_limit.stiffness, l_px_limit.damping);
	}

	template<>
	BC_PHYSICSIMP_DLL
	void bc_platform_d6_joint<g_api_physx>::set_swing_limit(const bc_joint_cone_limit& p_limit) noexcept
	{
		auto* l_px_joint = static_cast<physx::PxD6Joint*>(get_platform_pack().m_px_object);

		const physx::PxJointLimitCone l_px_limit
		(
			p_limit.m_y_angle,
			p_limit.m_z_angle,
			physx::PxSpring(p_limit.m_stiffness, p_limit.m_damping)
		);

		l_px_joint->setSwingLimit(l_px_limit);
	}

	template<>
	BC_PHYSICSIMP_DLL
	bc_joint_cone_limit bc_platform_d6_joint<g_api_physx>::get_swing_limit() const noexcept
	{
		auto* l_px_joint = static_cast<physx::PxD6Joint*>(get_platform_pack().m_px_object);

		const auto l_px_limit = l_px_joint->getSwingLimit();

		return bc_joint_cone_limit(l_px_limit.yAngle, l_px_limit.zAngle, l_px_limit.stiffness, l_px_limit.damping);
	}
}
