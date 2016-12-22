// [12/17/2016 MRB]

#pragma once

#include "PhysicsImp/bcPhysicsRef.h"
#include "PhysicsImp/Fundation/bcFundation.h"

#include "Core/Memory/bcPtr.h"
#include "Physics/Fundation/bcPhysics.h"
#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/bcImplementation.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		struct bc_platform_physics_pack<g_api_physx>
		{
			physx::PxFoundation* m_px_fundation;
#ifdef BC_DEBUG
			physx::PxProfileZoneManager* m_px_profile;
#endif
			physx::PxPhysics* m_px_physics;
			physx::PxCooking* m_px_cooking;

			core::bc_unique_ptr< bc_px_allocator > m_allocator;
			core::bc_unique_ptr< bc_px_logger > m_logger;
		};
	}
}