// [12/15/2016 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/bcExport.h"
#include "PhysicsImp/Body/bcRigidBody.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_rigid_body< g_api_physx >::bc_platform_rigid_body() noexcept
			: bc_platform_rigid_actor()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_rigid_body< g_api_physx >::bc_platform_rigid_body(const bc_platform_rigid_body& p_other) noexcept
			: bc_platform_rigid_actor(p_other)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_rigid_body< g_api_physx >& bc_platform_rigid_body< g_api_physx >::operator=(const bc_platform_rigid_body& p_other) noexcept
		{
			bc_platform_rigid_actor::operator=(p_other);

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_rigid_body< g_api_physx >::~bc_platform_rigid_body()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_rigid_body< g_api_physx >::set_cmass_local_pose(const bc_transform& p_pose) noexcept
		{
			auto* l_px_actor = static_cast< physx::PxRigidBody* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_rigid_body& >(*this)).get_platform_pack().m_px_object
			);

			l_px_actor->setCMassLocalPose(const_cast< bc_transform& >(p_pose).get_platform_pack().m_px_transform);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_transform bc_platform_rigid_body< g_api_physx >::get_cmass_local_pose() const noexcept
		{
			auto* l_px_actor = static_cast< physx::PxRigidBody* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_rigid_body& >(*this)).get_platform_pack().m_px_object
			);
			bc_transform l_transform;

			l_transform.get_platform_pack().m_px_transform = l_px_actor->getCMassLocalPose();

			return l_transform;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcFLOAT bc_platform_rigid_body< g_api_physx >::get_mass() const noexcept
		{
			auto* l_px_actor = static_cast< physx::PxRigidBody* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_rigid_body& >(*this)).get_platform_pack().m_px_object
			);

			return l_px_actor->getMass();
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_rigid_body< g_api_physx >::set_mass(bcFLOAT p_mass) noexcept
		{
			auto* l_px_actor = static_cast< physx::PxRigidBody* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_rigid_body& >(*this)).get_platform_pack().m_px_object
			);

			l_px_actor->setMass(p_mass);
		}

		template<>
		BC_PHYSICSIMP_DLL
		core::bc_vector3f bc_platform_rigid_body< g_api_physx >::get_inertia_tensor() const noexcept
		{
			auto* l_px_actor = static_cast< physx::PxRigidBody* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_rigid_body& >(*this)).get_platform_pack().m_px_object
			);
			physx::PxVec3 l_px_vec3;
			l_px_actor->setMassSpaceInertiaTensor(l_px_vec3);

			return core::bc_vector3f(l_px_vec3.x, l_px_vec3.y, l_px_vec3.z);
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_rigid_body< g_api_physx >::set_inertia_tensor(const core::bc_vector3f& p_mass) noexcept
		{
			auto* l_px_actor = static_cast< physx::PxRigidBody* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_rigid_body& >(*this)).get_platform_pack().m_px_object
			);
			l_px_actor->setMassSpaceInertiaTensor(physx::PxVec3(p_mass.x, p_mass.y, p_mass.z));
		}

		template<>
		BC_PHYSICSIMP_DLL
		bool bc_platform_rigid_body< g_api_physx >::update_mass_inertia(const bcFLOAT* p_shape_densities, bcUINT32 p_shape_density_count, const core::bc_vector3f* p_mass_local_pose) noexcept
		{
			auto* l_px_actor = static_cast< physx::PxRigidBody* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_rigid_body& >(*this)).get_platform_pack().m_px_object
			);

			if (p_mass_local_pose)
			{
				auto l_mas_pose = physx::PxVec3(p_mass_local_pose->x, p_mass_local_pose->y, p_mass_local_pose->z);
				return physx::PxRigidBodyExt::updateMassAndInertia
				(
					*l_px_actor,
					p_shape_densities,
					p_shape_density_count,
					&l_mas_pose
				);
			}

			return physx::PxRigidBodyExt::updateMassAndInertia
			(
				*l_px_actor,
				p_shape_densities,
				p_shape_density_count
			);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bool bc_platform_rigid_body< g_api_physx >::update_mass_inertia(bcFLOAT p_density, const core::bc_vector3f* p_mass_local_pose) noexcept
		{
			auto* l_px_actor = static_cast< physx::PxRigidBody* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_rigid_body& >(*this)).get_platform_pack().m_px_object
			);

			if (p_mass_local_pose)
			{
				auto l_mas_pose = physx::PxVec3(p_mass_local_pose->x, p_mass_local_pose->y, p_mass_local_pose->z);
				return physx::PxRigidBodyExt::updateMassAndInertia
				(
					*l_px_actor,
					p_density,
					&l_mas_pose
				);
			}

			return physx::PxRigidBodyExt::updateMassAndInertia
			(
				*l_px_actor,
				p_density
			);
		}

		template<>
		BC_PHYSICSIMP_DLL
		core::bc_vector3f bc_platform_rigid_body< g_api_physx >::get_linear_velocity() const noexcept
		{
			auto* l_px_actor = static_cast< physx::PxRigidBody* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_rigid_body& >(*this)).get_platform_pack().m_px_object
			);
			physx::PxVec3 l_px_vec3 = l_px_actor->getLinearVelocity();

			return core::bc_vector3f(l_px_vec3.x, l_px_vec3.y, l_px_vec3.z);
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_rigid_body< g_api_physx >::set_linear_velocity(const core::bc_vector3f& p_velociy) noexcept
		{
			auto* l_px_actor = static_cast< physx::PxRigidBody* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_rigid_body& >(*this)).get_platform_pack().m_px_object
			);

			l_px_actor->setLinearVelocity(physx::PxVec3(p_velociy.x, p_velociy.y, p_velociy.z));
		}

		template<>
		BC_PHYSICSIMP_DLL
		core::bc_vector3f bc_platform_rigid_body< g_api_physx >::get_angular_velocity() const noexcept
		{
			auto* l_px_actor = static_cast< physx::PxRigidBody* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_rigid_body& >(*this)).get_platform_pack().m_px_object
			);
			physx::PxVec3 l_px_vec3 = l_px_actor->getAngularVelocity();

			return core::bc_vector3f(l_px_vec3.x, l_px_vec3.y, l_px_vec3.z);
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_rigid_body< g_api_physx >::set_angular_velocity(const core::bc_vector3f& p_velociy)
		{
			auto* l_px_actor = static_cast< physx::PxRigidBody* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_rigid_body& >(*this)).get_platform_pack().m_px_object
			);

			l_px_actor->setAngularVelocity(physx::PxVec3(p_velociy.x, p_velociy.y, p_velociy.z));
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_rigid_body< g_api_physx >::add_force(const core::bc_vector3f& p_force, bc_force_mode p_mode) noexcept
		{
			auto* l_px_actor = static_cast< physx::PxRigidBody* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_rigid_body& >(*this)).get_platform_pack().m_px_object
			);

			l_px_actor->addForce(physx::PxVec3(p_force.x, p_force.y, p_force.z), static_cast< physx::PxForceMode::Enum >(p_mode));
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_rigid_body< g_api_physx >::add_force_at_pose(const core::bc_vector3f& p_force, const core::bc_vector3f& p_pose, bc_force_mode p_mode) noexcept
		{
			auto* l_px_actor = static_cast< physx::PxRigidBody* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_rigid_body& >(*this)).get_platform_pack().m_px_object
			);

			physx::PxRigidBodyExt::addForceAtPos
			(
				*l_px_actor,
				physx::PxVec3(p_force.x, p_force.y, p_force.z),
				physx::PxVec3(p_pose.x, p_pose.y, p_pose.z),
				static_cast< physx::PxForceMode::Enum >(p_mode)
			);
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_rigid_body< g_api_physx >::add_force_at_local_pose(const core::bc_vector3f& p_force, const core::bc_vector3f& p_pose, bc_force_mode p_mode) noexcept
		{
			auto* l_px_actor = static_cast< physx::PxRigidBody* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_rigid_body& >(*this)).get_platform_pack().m_px_object
			);

			physx::PxRigidBodyExt::addForceAtLocalPos
			(
				*l_px_actor,
				physx::PxVec3(p_force.x, p_force.y, p_force.z),
				physx::PxVec3(p_pose.x, p_pose.y, p_pose.z),
				static_cast< physx::PxForceMode::Enum >(p_mode)
			);
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_rigid_body< g_api_physx >::add_local_force_at_pose(const core::bc_vector3f& p_force, const core::bc_vector3f& p_pose, bc_force_mode p_mode) noexcept
		{
			auto* l_px_actor = static_cast< physx::PxRigidBody* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_rigid_body& >(*this)).get_platform_pack().m_px_object
			);

			physx::PxRigidBodyExt::addLocalForceAtPos
			(
				*l_px_actor,
				physx::PxVec3(p_force.x, p_force.y, p_force.z),
				physx::PxVec3(p_pose.x, p_pose.y, p_pose.z),
				static_cast< physx::PxForceMode::Enum >(p_mode)
			);
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_rigid_body< g_api_physx >::add_local_force_at_local_pose(const core::bc_vector3f& p_force, const core::bc_vector3f& p_pose, bc_force_mode p_mode) noexcept
		{
			auto* l_px_actor = static_cast< physx::PxRigidBody* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_rigid_body& >(*this)).get_platform_pack().m_px_object
			);

			physx::PxRigidBodyExt::addLocalForceAtLocalPos
			(
				*l_px_actor,
				physx::PxVec3(p_force.x, p_force.y, p_force.z),
				physx::PxVec3(p_pose.x, p_pose.y, p_pose.z),
				static_cast< physx::PxForceMode::Enum >(p_mode)
			);
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_rigid_body< g_api_physx >::add_torque(const core::bc_vector3f& p_torque, bc_force_mode p_mode) noexcept
		{
			auto* l_px_actor = static_cast< physx::PxRigidBody* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_rigid_body& >(*this)).get_platform_pack().m_px_object
			);

			l_px_actor->addTorque
			(
				physx::PxVec3(p_torque.x, p_torque.y, p_torque.z),
				static_cast< physx::PxForceMode::Enum >(p_mode)
			);
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_rigid_body< g_api_physx >::clear_force(bc_force_mode p_mode) noexcept
		{
			auto* l_px_actor = static_cast< physx::PxRigidBody* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_rigid_body& >(*this)).get_platform_pack().m_px_object
			);

			l_px_actor->clearForce(static_cast< physx::PxForceMode::Enum >(p_mode));
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_rigid_body< g_api_physx >::clear_torque(bc_force_mode p_mode) noexcept
		{
			auto* l_px_actor = static_cast< physx::PxRigidBody* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_rigid_body& >(*this)).get_platform_pack().m_px_object
			);

			l_px_actor->clearTorque(static_cast< physx::PxForceMode::Enum >(p_mode));
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_rigid_body_flag bc_platform_rigid_body< g_api_physx >::get_rigid_body_flags() const noexcept
		{
			auto* l_px_actor = static_cast< physx::PxRigidBody* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_rigid_body& >(*this)).get_platform_pack().m_px_object
			);

			return static_cast< bc_rigid_body_flag >(static_cast< physx::PxRigidBodyFlags::InternalType >(l_px_actor->getRigidBodyFlags()));
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_rigid_body< g_api_physx >::set_rigid_body_flags(bc_rigid_body_flag p_flag, bool p_value) noexcept
		{
			auto* l_px_actor = static_cast< physx::PxRigidBody* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_rigid_body& >(*this)).get_platform_pack().m_px_object
			);

			l_px_actor->setRigidBodyFlag(static_cast< physx::PxRigidBodyFlag::Enum >(p_flag), p_value);
		}
	}
}
