// [12/09/2016 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/bcExport.h"
#include "PhysicsImp/bcPhysicsReference.h"

namespace black_cat
{
	namespace physics
	{
		//_bc_physics_reference_counter bc_platform_physics_reference_pack< g_api_physx >::s_reference_counters;

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_physics_reference< g_current_physics_api >::bc_platform_physics_reference() noexcept
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_physics_reference< g_current_physics_api >::bc_platform_physics_reference(const bc_platform_physics_reference& p_other) noexcept
		{
			operator=(p_other);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_physics_reference< g_current_physics_api >::~bc_platform_physics_reference()
		{
			//// There isn't any strong reference to this object
			//if (m_pack.m_ref_counter_index == _bc_physics_reference_counter::invalid_index)
			//{
			//	m_pack.m_px_object->release();
			//}
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_physics_reference< g_current_physics_api >& bc_platform_physics_reference< g_current_physics_api >::operator=(const bc_platform_physics_reference& p_other) noexcept
		{
			m_pack = p_other.m_pack;

			return *this;
		}
	}
}