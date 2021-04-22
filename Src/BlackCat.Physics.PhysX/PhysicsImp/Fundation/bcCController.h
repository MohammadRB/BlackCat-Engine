// [03/22/2021 MRB]

#pragma once

#include "PhysicsImp/bcPhysicsReference.h"
#include "PhysicsImp/bcPhysicsRef.h"
#include "PhysicsImp/Body/bcRigidDynamic.h"
#include "PhysicsImp/Fundation/bcCControllerSimulationCallback.h"
#include "PhysicsImp/Fundation/bcMaterial.h"

#include "Physics/Fundation/bcCController.h"
#include "PhysicsImp/bcImplementation.h"
#include "PhysicsImp/PhysicsImpPCH.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		struct bc_platform_ccontroller_pack<g_api_physx> : bc_platform_physics_reference_pack<g_api_physx>
		{
			bc_platform_ccontroller_pack()
				: m_controller(nullptr)
			{
			}
			
			physx::PxController* m_controller;
			core::bc_shared_ptr<bc_px_controller_hit_callback> m_hit_callback;
		};
	}	
}