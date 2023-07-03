// [19/10/2018 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/bcPhysicsRef.h"
#include "PhysicsImp/Fundation/bcScene.h"
#include "PhysicsImp/Fundation/bcCController.h"
#include "PhysicsImp/bcPhysicsReference.h"

namespace black_cat
{
	namespace physics
	{
		void _release_px_reference(bc_platform_scene<g_api_physx>& p_reference)
		{
			p_reference.get_platform_pack().m_data->m_controller_manager->release();
			p_reference.get_platform_pack().m_data->m_px_scene->release();
			p_reference.get_platform_pack().m_data.reset();
		}

		void _release_px_reference(bc_platform_ccontroller<g_api_physx>& p_reference)
		{
			p_reference.get_platform_pack().m_controller->release();
		}

		void _release_px_reference(bc_platform_physics_reference<g_api_physx>& p_reference)
		{
			p_reference.get_platform_pack().m_px_object->release();
			p_reference.get_platform_pack().m_px_object = nullptr;
		}
	}
}