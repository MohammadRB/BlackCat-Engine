// [12/15/2016 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/bcExport.h"
#include "PhysicsImp/Body/bcRigidDynamic.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_rigid_dynamic< g_api_physx >::bc_platform_rigid_dynamic()
			: bc_platform_rigid_body()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_rigid_dynamic< g_api_physx >::bc_platform_rigid_dynamic(const bc_platform_rigid_dynamic& p_other) noexcept
			: bc_platform_rigid_body(p_other)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_rigid_dynamic< g_api_physx >::~bc_platform_rigid_dynamic()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_rigid_dynamic< g_api_physx >& bc_platform_rigid_dynamic< g_api_physx >::operator=(const bc_platform_rigid_dynamic& p_other) noexcept
		{
			bc_platform_rigid_body::operator=(p_other);

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_rigid_dynamic< g_api_physx >::set_kinematic_target(const bc_transform& p_destination) noexcept
		{
			auto* l_px_rigid = static_cast< physx::PxRigidDynamic* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_rigid_dynamic& >(*this)).get_platform_pack().m_px_object
			);

			l_px_rigid->setKinematicTarget(const_cast< bc_transform& >(p_destination).get_platform_pack().m_px_transform);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bool bc_platform_rigid_dynamic< g_api_physx >::get_kinematic_target(bc_transform& p_target) noexcept
		{
			auto* l_px_rigid = static_cast< physx::PxRigidDynamic* >
			(
				static_cast< bc_platform_physics_reference& >(*this).get_platform_pack().m_px_object
			);

			return l_px_rigid->getKinematicTarget(const_cast< bc_transform& >(p_target).get_platform_pack().m_px_transform);
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_rigid_dynamic< g_api_physx >::set_linear_damping(bcFLOAT p_damp) noexcept
		{
			auto* l_px_rigid = static_cast< physx::PxRigidDynamic* >
			(
				static_cast< bc_platform_physics_reference& >(*this).get_platform_pack().m_px_object
			);

			l_px_rigid->setLinearDamping(p_damp);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcFLOAT bc_platform_rigid_dynamic< g_api_physx >::get_linear_damping() const noexcept
		{
			auto* l_px_rigid = static_cast< physx::PxRigidDynamic* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_rigid_dynamic& >(*this)).get_platform_pack().m_px_object
			);

			return l_px_rigid->getLinearDamping();
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_rigid_dynamic< g_api_physx >::set_angular_damping(bcFLOAT p_damp) noexcept
		{
			auto* l_px_rigid = static_cast< physx::PxRigidDynamic* >
			(
				static_cast< bc_platform_physics_reference& >(*this).get_platform_pack().m_px_object
			);

			l_px_rigid->setAngularDamping(p_damp);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcFLOAT bc_platform_rigid_dynamic< g_api_physx >::get_angular_damping() const noexcept
		{
			auto* l_px_rigid = static_cast< physx::PxRigidDynamic* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_rigid_dynamic& >(*this)).get_platform_pack().m_px_object
			);

			return l_px_rigid->getAngularDamping();
		}

		template<>
		BC_PHYSICSIMP_DLL
		bool bc_platform_rigid_dynamic< g_api_physx >::is_sleeping() const noexcept
		{
			auto* l_px_rigid = static_cast< physx::PxRigidDynamic* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_rigid_dynamic& >(*this)).get_platform_pack().m_px_object
			);

			return l_px_rigid->isSleeping();
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_rigid_dynamic< g_api_physx >::wake_up() noexcept
		{
			auto* l_px_rigid = static_cast< physx::PxRigidDynamic* >
			(
				static_cast< bc_platform_physics_reference& >(*this).get_platform_pack().m_px_object
			);

			l_px_rigid->wakeUp();
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_rigid_dynamic< g_api_physx >::put_to_sleep() noexcept
		{
			auto* l_px_rigid = static_cast< physx::PxRigidDynamic* >
			(
				static_cast< bc_platform_physics_reference& >(*this).get_platform_pack().m_px_object
			);

			l_px_rigid->putToSleep();
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcFLOAT bc_platform_rigid_dynamic< g_api_physx >::get_contact_report_threshold() const noexcept
		{
			auto* l_px_rigid = static_cast< physx::PxRigidDynamic* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_rigid_dynamic& >(*this)).get_platform_pack().m_px_object
			);

			return l_px_rigid->getContactReportThreshold();
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_rigid_dynamic< g_api_physx >::set_contact_report_threshold(bcFLOAT p_threshold) noexcept
		{
			auto* l_px_rigid = static_cast< physx::PxRigidDynamic* >
			(
				static_cast< bc_platform_physics_reference& >(*this).get_platform_pack().m_px_object
			);

			l_px_rigid->setContactReportThreshold(p_threshold);
		}
	}
}