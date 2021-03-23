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
		public:
			bc_platform_physics_reference_pack()
				: m_px_object(nullptr)
			{
			}

			physx::PxBase* m_px_object;
		};
	}
}