// [12/16/2016 MRB]

#pragma once

#include "PhysicsImp/Body/bcRigidActor.h"
#include "PhysicsImp/Shape/bcShape.h"

#include "Physics/Fundation/bcPhysicsSimulationCallback.h"
#include "PhysicsImp/PhysicsImpPCH.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		struct bc_platform_trigger_pair_pack<g_api_physx>
		{
			physx::PxTriggerPair* m_px_pair;
			mutable bc_rigid_actor m_trigger_actor;
			mutable bc_rigid_actor m_other_actor;
			mutable bc_shape m_trigger_shape;
			mutable bc_shape m_other_shape;
		};

		template<>
		struct bc_platform_contact_pair_pack<g_api_physx>
		{
			physx::PxContactPairHeader* m_px_pair;
			mutable bc_rigid_actor m_actor0;
			mutable bc_rigid_actor m_actor1;
		};

		template<>
		struct bc_platform_contact_point_pack<g_api_physx>
		{
			physx::PxContactPairPoint m_px_point;
		};

		template<>
		struct bc_platform_contact_shape_pair_pack<g_api_physx>
		{
			physx::PxContactPair* m_px_pair;
			mutable bc_shape m_shape0;
			mutable bc_shape m_shape1;
		};
	}
}