// [12/12/2016 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/bcExport.h"
#include "PhysicsImp/Fundation/bcMaterial.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_material<g_api_physx>::bc_platform_material() noexcept
			: bc_platform_physics_reference(),
			m_pack()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_material<g_api_physx>::bc_platform_material(platform_pack& p_pack)
			: bc_platform_physics_reference(),
			m_pack(p_pack)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_material< g_api_physx >::bc_platform_material(const bc_platform_material& p_other) noexcept
			: bc_platform_physics_reference(p_other),
			m_pack(p_other.m_pack)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_material< g_api_physx >::~bc_platform_material()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_material< g_api_physx >& bc_platform_material< g_api_physx >::operator=(const bc_platform_material& p_other) noexcept
		{
			bc_platform_physics_reference::operator=(p_other);
			m_pack = p_other.m_pack;

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcFLOAT bc_platform_material< g_api_physx >::get_static_friction() const noexcept
		{
			physx::PxMaterial* l_px_material = static_cast<physx::PxMaterial*>(m_pack.m_px_object);

			return l_px_material->getStaticFriction();
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_material< g_api_physx >::set_static_friction(bcFLOAT p_friction) noexcept
		{
			physx::PxMaterial* l_px_material = static_cast<physx::PxMaterial*>(m_pack.m_px_object);

			l_px_material->setStaticFriction(p_friction);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcFLOAT bc_platform_material< g_api_physx >::get_dynamic_friction() const noexcept
		{
			physx::PxMaterial* l_px_material = static_cast<physx::PxMaterial*>(m_pack.m_px_object);

			return l_px_material->getDynamicFriction();
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_material< g_api_physx >::set_dynamic_friction(bcFLOAT p_friction) noexcept
		{
			physx::PxMaterial* l_px_material = static_cast<physx::PxMaterial*>(m_pack.m_px_object);

			l_px_material->setDynamicFriction(p_friction);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcFLOAT bc_platform_material< g_api_physx >::get_restitution() const noexcept
		{
			physx::PxMaterial* l_px_material = static_cast<physx::PxMaterial*>(m_pack.m_px_object);

			return l_px_material->getRestitution();
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_material< g_api_physx >::set_restitution(bcFLOAT p_restitution) noexcept
		{
			physx::PxMaterial* l_px_material = static_cast<physx::PxMaterial*>(m_pack.m_px_object);

			l_px_material->setRestitution(p_restitution);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bool bc_platform_material< g_api_physx >::is_valid() const noexcept
		{
			return m_pack.m_px_object != nullptr;
		}
	}
}