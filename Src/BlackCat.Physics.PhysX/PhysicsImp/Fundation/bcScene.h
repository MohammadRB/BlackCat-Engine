// [17/12/2016 MRB]

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
			core::bc_unique_ptr<bc_px_simulation_callback> m_simulation_callback;
			core::bc_unique_ptr<bc_px_contact_filter_callback> m_contact_filter_callback;
			core::bc_unique_ptr<bc_px_contact_modify_callback> m_contact_modify_callback;
			core::bc_unique_ptr<bc_px_filter_shader_data> m_filter_shader_data;
		};

		template<>
		struct bc_platform_scene_pack<g_api_physx> : bc_platform_physics_reference_pack<g_api_physx>
		{
			core::bc_shared_ptr< _bc_px_scene_pack_data > m_data;
		};

		template<>
		inline void bc_platform_scene<g_api_physx>::lock()
		{
			m_pack.m_data->m_px_scene->lockWrite();
		}

		template<>
		inline void bc_platform_scene<g_api_physx>::lock_shared()
		{
			m_pack.m_data->m_px_scene->lockRead();
		}

		template<>
		inline void bc_platform_scene<g_api_physx>::unlock()
		{
			m_pack.m_data->m_px_scene->unlockWrite();
		}

		template<>
		inline void bc_platform_scene<g_api_physx>::unlock_shared()
		{
			m_pack.m_data->m_px_scene->unlockRead();
		}

		inline bc_scene_lock::bc_scene_lock(bc_scene* p_scene)
			: m_scene(p_scene)
		{
			if (m_scene)
			{
				m_scene->lock();
			}
		}

		inline bc_scene_lock::~bc_scene_lock()
		{
			if (m_scene)
			{
				m_scene->unlock();
			}
		}

		inline bc_scene_shared_lock::bc_scene_shared_lock(bc_scene* p_scene)
			: m_scene(p_scene)
		{
			if (m_scene)
			{
				m_scene->lock_shared();
			}
		}

		inline bc_scene_shared_lock::~bc_scene_shared_lock()
		{
			if (m_scene)
			{
				m_scene->unlock_shared();
			}
		}
	}
}