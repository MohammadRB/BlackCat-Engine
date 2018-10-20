// [10/19/2018 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/bcPhysicsRef.h"
#include "PhysicsImp/Fundation/bcScene.h"
#include "PhysicsImp/bcPhysicsReference.h"

namespace black_cat
{
	namespace physics
	{
		void _release_px_reference(bc_platform_physics_reference<g_api_physx>& p_reference, std::true_type p_is_scene)
		{
			auto& l_scene = static_cast<bc_platform_scene<g_api_physx>&>(p_reference);

			l_scene.get_platform_pack().m_data->m_px_scene->release();
			l_scene.get_platform_pack().m_data.reset();
		}

		void _release_px_reference(bc_platform_physics_reference<g_api_physx>& p_reference, std::false_type p_is_scene)
		{
			p_reference.get_platform_pack().m_px_object->release();
			p_reference.get_platform_pack().m_px_object = nullptr;
		}
	}
}