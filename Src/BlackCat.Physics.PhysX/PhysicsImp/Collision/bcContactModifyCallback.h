// [12/16/2016 MRB]

#pragma once

#include "Physics/Collision/bcContactModifyCallback.h"
#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/Body/bcRigidActor.h"
#include "PhysicsImp/Shape/bcShape.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		struct bc_platform_contact_modify_set_pack<g_api_physx>
		{
			physx::PxContactSet* m_px_set;
		};

		template<>
		struct bc_platform_contact_modify_pair_pack<g_api_physx>
		{
			physx::PxContactModifyPair* m_px_pair;
			mutable bc_platform_rigid_actor<g_api_physx> m_actor0;
			mutable bc_platform_rigid_actor<g_api_physx> m_actor1;
			mutable bc_platform_shape<g_api_physx> m_shape_0;
			mutable bc_platform_shape<g_api_physx> m_shape_1;
			mutable bc_platform_contact_modify_set<g_api_physx> m_contacts;
			bool m_is_ccd;
		};
	}
}