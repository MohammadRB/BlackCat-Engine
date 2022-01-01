// [12/09/2016 MRB]

#pragma once

#include "Physics/bcPhysicsReference.h"
#include "PhysicsImp/PhysicsImpPCH.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		struct bc_platform_physics_reference_pack<g_api_physx>
		{
			bc_platform_physics_reference_pack()
				: m_px_object(nullptr)
			{
			}

			physx::PxBase* m_px_object;
		};

		template<>
		inline bc_platform_physics_reference<g_api_physx>::bc_platform_physics_reference() noexcept = default;

		template<>
		inline bc_platform_physics_reference<g_api_physx>::bc_platform_physics_reference(const bc_platform_physics_reference& p_other) noexcept = default;

		template<>
		inline bc_platform_physics_reference<g_api_physx>::~bc_platform_physics_reference() = default;

		template<>
		inline bc_platform_physics_reference<g_api_physx>& bc_platform_physics_reference<g_api_physx>::operator=(const bc_platform_physics_reference& p_other) noexcept = default;

		template<>
		inline bool bc_platform_physics_reference<g_api_physx>::operator==(const bc_platform_physics_reference& p_other) const noexcept
		{
			return get_platform_pack().m_px_object == p_other.get_platform_pack().m_px_object;
		}

		template<>
		inline bool bc_platform_physics_reference<g_api_physx>::operator!=(const bc_platform_physics_reference& p_other) const noexcept
		{
			return get_platform_pack().m_px_object != p_other.get_platform_pack().m_px_object;
		}

		template<>
		inline bool bc_platform_physics_reference<g_api_physx>::operator==(std::nullptr_t) const noexcept
		{
			return get_platform_pack().m_px_object == nullptr;
		}

		template<>
		inline bool bc_platform_physics_reference<g_api_physx>::operator!=(std::nullptr_t) const noexcept
		{
			return get_platform_pack().m_px_object != nullptr;
		}
	}
}