// [12/15/2016 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/bcExport.h"
#include "PhysicsImp/Body/bcRigidActor.h"
#include "PhysicsImp/Joint/bcJoint.h"
#include "PhysicsImp/bcUtility.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_joint<g_api_physx>::bc_platform_joint() noexcept
			: bc_platform_physics_reference(),
			m_pack()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_joint<g_api_physx>::bc_platform_joint(platform_pack& p_pack) noexcept
			: bc_platform_physics_reference(),
			m_pack(p_pack)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_joint<g_api_physx>::bc_platform_joint(const bc_platform_joint& p_other) noexcept
			: bc_platform_physics_reference(p_other),
			m_pack(p_other.m_pack)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_joint<g_api_physx>::~bc_platform_joint()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_joint<g_api_physx>& bc_platform_joint<g_api_physx>::operator=(const bc_platform_joint& p_other) noexcept
		{
			bc_platform_physics_reference::operator=(p_other);
			m_pack = p_other.m_pack;

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		std::pair<bc_rigid_actor, bc_rigid_actor> bc_platform_joint<g_api_physx>::get_actors() const noexcept
		{
			auto* l_px_joint = static_cast<physx::PxJoint*>(m_pack.m_px_object);

			physx::PxRigidActor* l_px_rigid_actor0;
			physx::PxRigidActor* l_px_rigid_actor1;

			l_px_joint->getActors(l_px_rigid_actor0, l_px_rigid_actor1);

			bc_rigid_actor::platform_pack l_actor0_pack;
			bc_rigid_actor::platform_pack l_actor1_pack;

			l_actor0_pack.m_px_object = l_px_rigid_actor0;
			l_actor1_pack.m_px_object = l_px_rigid_actor1;
			
			return std::make_pair(bc_rigid_actor(l_actor0_pack), bc_rigid_actor(l_actor1_pack));
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_joint<g_api_physx>::set_local_pose0(const bc_transform& p_local_pose) noexcept
		{
			auto* l_px_joint = static_cast<physx::PxJoint*>(m_pack.m_px_object);

			l_px_joint->setLocalPose
			(
				physx::PxJointActorIndex::eACTOR0,
				p_local_pose.get_platform_pack().m_px_transform
			);
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_joint<g_api_physx>::set_local_pose1(const bc_transform& p_local_pose) noexcept
		{
			auto* l_px_joint = static_cast<physx::PxJoint*>(m_pack.m_px_object);

			l_px_joint->setLocalPose
			(
				physx::PxJointActorIndex::eACTOR1,
				p_local_pose.get_platform_pack().m_px_transform
			);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_transform bc_platform_joint<g_api_physx>::get_local_pose0() const noexcept
		{
			auto* l_px_joint = static_cast<physx::PxJoint*>(m_pack.m_px_object);
			
			bc_transform::platform_pack l_transform_pack;
			l_transform_pack.m_px_transform = l_px_joint->getLocalPose(physx::PxJointActorIndex::eACTOR0);

			return bc_transform(l_transform_pack);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_transform bc_platform_joint<g_api_physx>::get_local_pose1() const noexcept
		{
			auto* l_px_joint = static_cast<physx::PxJoint*>(m_pack.m_px_object);
			
			bc_transform::platform_pack l_transform_pack;
			l_transform_pack.m_px_transform = l_px_joint->getLocalPose(physx::PxJointActorIndex::eACTOR1);

			return bc_transform(l_transform_pack);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_transform bc_platform_joint<g_api_physx>::get_relative_transform() const noexcept
		{
			auto* l_px_joint = static_cast<physx::PxJoint*>(m_pack.m_px_object);

			bc_transform::platform_pack l_transform_pack;
			l_transform_pack.m_px_transform = l_px_joint->getRelativeTransform();
			
			return bc_transform(l_transform_pack);
		}

		template<>
		BC_PHYSICSIMP_DLL
		core::bc_vector3f bc_platform_joint<g_api_physx>::get_relative_linear_velocity() const noexcept
		{
			auto* l_px_joint = static_cast<physx::PxJoint*>(m_pack.m_px_object);
			const auto l_px_vec = l_px_joint->getRelativeLinearVelocity();

			return bc_to_game_hand(l_px_vec);
		}

		template<>
		BC_PHYSICSIMP_DLL
		core::bc_vector3f bc_platform_joint<g_api_physx>::get_relative_angular_velocity() const noexcept
		{
			auto* l_px_joint = static_cast<physx::PxJoint*>(m_pack.m_px_object);
			const auto l_px_vec = l_px_joint->getRelativeAngularVelocity();

			return bc_to_game_hand(l_px_vec);
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_joint<g_api_physx>::set_break_force(bcFLOAT p_force, bcFLOAT p_torque) noexcept
		{
			auto* l_px_joint = static_cast<physx::PxJoint*>(m_pack.m_px_object);
			l_px_joint->setBreakForce(p_force, p_torque);
		}

		template<>
		BC_PHYSICSIMP_DLL
		std::pair<bcFLOAT, bcFLOAT> bc_platform_joint<g_api_physx>::get_break_force() const noexcept
		{
			auto* l_px_joint = static_cast<physx::PxJoint*>(m_pack.m_px_object);

			bcFLOAT l_force;
			bcFLOAT l_torque;

			l_px_joint->getBreakForce(l_force, l_torque);

			return std::make_pair(l_force, l_torque);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bool bc_platform_joint<g_api_physx>::get_visualization() const noexcept
		{
			auto* l_px_joint = static_cast<physx::PxJoint*>(m_pack.m_px_object);

			return l_px_joint->getConstraintFlags().isSet(physx::PxConstraintFlag::eVISUALIZATION);
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_joint<g_api_physx>::set_visualization(bool p_value) noexcept
		{
			auto* l_px_joint = static_cast<physx::PxJoint*>(m_pack.m_px_object);

			l_px_joint->setConstraintFlag(physx::PxConstraintFlag::eVISUALIZATION, p_value);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bool bc_platform_joint<g_api_physx>::is_broken() const noexcept
		{
			auto* l_px_joint = static_cast<physx::PxJoint*>(m_pack.m_px_object);

			return l_px_joint->getConstraintFlags().isSet(physx::PxConstraintFlag::eBROKEN);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bool bc_platform_joint<g_api_physx>::is_valid() const noexcept
		{
			return m_pack.m_px_object;
		}
	}
}