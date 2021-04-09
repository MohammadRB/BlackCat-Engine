// [12/09/2016 MRB]

#pragma once

#include "Physics/bcPhysicsReference.h"
#include "PhysicsImp/PhysicsImpPCH.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		struct bc_platform_physics_reference_pack< g_api_physx >
		{
			bc_platform_physics_reference_pack() : m_px_object(nullptr)
			{
			}

			physx::PxBase* m_px_object;
		};

		template<>
		inline bc_platform_physics_reference< g_current_physics_api >::bc_platform_physics_reference() noexcept = default;

		template<>
		inline bc_platform_physics_reference< g_current_physics_api >::bc_platform_physics_reference(const bc_platform_physics_reference& p_other) noexcept = default;

		template<>
		inline bc_platform_physics_reference< g_current_physics_api >::~bc_platform_physics_reference() = default;

		template<>
		inline bc_platform_physics_reference<g_current_physics_api>& bc_platform_physics_reference<g_current_physics_api>::operator=(const bc_platform_physics_reference& p_other) noexcept = default;
	}
}