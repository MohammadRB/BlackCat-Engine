// [12/15/2016 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/bcExport.h"
#include "PhysicsImp/Body/bcRigidActor.h"
#include "PhysicsImp/Joint/bcJoint.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_joint< g_api_physx >::bc_platform_joint() noexcept
			: bc_platform_physics_reference()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_joint< g_api_physx >::bc_platform_joint(const bc_platform_joint& p_other) noexcept
			: bc_platform_physics_reference(p_other)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_joint< g_api_physx >::~bc_platform_joint()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_joint< g_api_physx >& bc_platform_joint< g_api_physx >::operator=(const bc_platform_joint& p_other) noexcept
		{
			bc_platform_physics_reference::operator=(p_other);

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		std::pair< bc_rigid_actor, bc_rigid_actor > bc_platform_joint< g_api_physx >::get_actors() const noexcept
		{
			auto* l_px_joint = static_cast< physx::PxJoint* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_joint& >(*this)).get_platform_pack().m_px_object
			);

			bc_rigid_actor l_actor0;
			bc_rigid_actor l_actor1;

			physx::PxRigidActor* l_px_rigid_actor0 = static_cast< physx::PxRigidActor* >(static_cast< bc_platform_physics_reference& >(l_actor0).get_platform_pack().m_px_object);
			physx::PxRigidActor* l_px_rigid_actor1 = static_cast< physx::PxRigidActor* >(static_cast< bc_platform_physics_reference& >(l_actor1).get_platform_pack().m_px_object);

			l_px_joint->getActors(l_px_rigid_actor0, l_px_rigid_actor1);

			return std::make_pair(l_actor0, l_actor1);
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_joint< g_api_physx >::set_local_pose0(const bc_transform& p_local_pose) noexcept
		{
			auto* l_px_joint = static_cast< physx::PxJoint* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_joint& >(*this)).get_platform_pack().m_px_object
			);

			l_px_joint->setLocalPose
			(
				physx::PxJointActorIndex::eACTOR0,
				const_cast< bc_transform& >(p_local_pose).get_platform_pack().m_px_transform
			);
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_joint< g_api_physx >::set_local_pose1(const bc_transform& p_local_pose) noexcept
		{
			auto* l_px_joint = static_cast< physx::PxJoint* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_joint& >(*this)).get_platform_pack().m_px_object
			);

			l_px_joint->setLocalPose
			(
				physx::PxJointActorIndex::eACTOR1,
				const_cast< bc_transform& >(p_local_pose).get_platform_pack().m_px_transform
			);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_transform bc_platform_joint< g_api_physx >::get_local_pose0() const noexcept
		{
			auto* l_px_joint = static_cast< physx::PxJoint* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_joint& >(*this)).get_platform_pack().m_px_object
			);
			bc_transform l_transform;
			l_transform.get_platform_pack().m_px_transform = l_px_joint->getLocalPose(physx::PxJointActorIndex::eACTOR0);

			return l_transform;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_transform bc_platform_joint< g_api_physx >::get_local_pose1() const noexcept
		{
			auto* l_px_joint = static_cast< physx::PxJoint* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_joint& >(*this)).get_platform_pack().m_px_object
			);
			bc_transform l_transform;
			l_transform.get_platform_pack().m_px_transform = l_px_joint->getLocalPose(physx::PxJointActorIndex::eACTOR1);

			return l_transform;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_transform bc_platform_joint< g_api_physx >::get_relative_transform() const noexcept
		{
			auto* l_px_joint = static_cast< physx::PxJoint* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_joint& >(*this)).get_platform_pack().m_px_object
			);
			bc_transform l_transform;
			l_transform.get_platform_pack().m_px_transform = l_px_joint->getRelativeTransform();

			return l_transform;
		}

		template<>
		BC_PHYSICSIMP_DLL
		core::bc_vector3f bc_platform_joint< g_api_physx >::get_relative_linear_velocity() const noexcept
		{
			auto* l_px_joint = static_cast< physx::PxJoint* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_joint& >(*this)).get_platform_pack().m_px_object
			);
			auto l_px_vec = l_px_joint->getRelativeLinearVelocity();

			return core::bc_vector3f(l_px_vec.x, l_px_vec.y, l_px_vec.z);
		}

		template<>
		BC_PHYSICSIMP_DLL
		core::bc_vector3f bc_platform_joint< g_api_physx >::get_relative_angular_velocity() const noexcept
		{
			auto* l_px_joint = static_cast< physx::PxJoint* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_joint& >(*this)).get_platform_pack().m_px_object
			);
			auto l_px_vec = l_px_joint->getRelativeAngularVelocity();

			return core::bc_vector3f(l_px_vec.x, l_px_vec.y, l_px_vec.z);
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_joint< g_api_physx >::set_break_force(bcFLOAT p_force, bcFLOAT p_torque) noexcept
		{
			auto* l_px_joint = static_cast< physx::PxJoint* >
			(
				static_cast< bc_platform_physics_reference& >(*this).get_platform_pack().m_px_object
			);
			l_px_joint->setBreakForce(p_force, p_torque);
		}

		template<>
		BC_PHYSICSIMP_DLL
		std::pair< bcFLOAT, bcFLOAT > bc_platform_joint< g_api_physx >::get_break_force() const noexcept
		{
			auto* l_px_joint = static_cast< physx::PxJoint* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_joint& >(*this)).get_platform_pack().m_px_object
			);

			bcFLOAT l_force;
			bcFLOAT l_torque;

			l_px_joint->getBreakForce(l_force, l_torque);

			return std::make_pair(l_force, l_torque);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bool bc_platform_joint< g_api_physx >::broken() const noexcept
		{
			auto* l_px_joint = static_cast< physx::PxJoint* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_joint& >(*this)).get_platform_pack().m_px_object
			);

			return l_px_joint->getConstraintFlags().isSet(physx::PxConstraintFlag::eBROKEN);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bool bc_platform_joint< g_api_physx >::is_valid() const noexcept
		{
			return static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_joint& >(*this)).get_platform_pack().m_px_object != nullptr;
		}
	}
}
