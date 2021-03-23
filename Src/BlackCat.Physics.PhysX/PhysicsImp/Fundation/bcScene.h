// [12/17/2016 MRB]

#pragma once

#include "PhysicsImp/bcPhysicsReference.h"
#include "PhysicsImp/Fundation/bcFundation.h"
#include "PhysicsImp/Fundation/bcTransform.h"
#include "PhysicsImp/Fundation/bcPhysics.h"
#include "PhysicsImp/Fundation/bcSceneBuilder.h"
#include "PhysicsImp/Fundation/bcCController.h"
#include "PhysicsImp/Fundation/bcSceneDebug.h"
#include "PhysicsImp/Collision/bcSceneQuery.h"
#include "PhysicsImp/Body/bcActor.h"

#include "Physics/Fundation/bcScene.h"
#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/Fundation/bcFundation.h"

namespace black_cat
{
	namespace physics
	{
		struct _bc_px_scene_pack_data
		{
			physx::PxScene* m_px_scene;
			physx::PxControllerManager* m_controller_manager;
			bci_allocator* m_allocator;
			bci_task_dispatcher* m_task_dispatcher;
			core::bc_unique_ptr< bc_px_simulation_callback > m_simulation_callback;
			core::bc_unique_ptr< bc_px_contact_filter_callback > m_contact_filter_callback;
			core::bc_unique_ptr< bc_px_contact_modify_callback > m_contact_modify_callback;
			core::bc_unique_ptr< bc_px_filter_shader_data > m_filter_shader_data;
		};

		template<>
		struct bc_platform_scene_pack< g_api_physx >
		{
			core::bc_shared_ptr< _bc_px_scene_pack_data > m_data;
		};
	}
}