// [12/15/2016 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/bcExport.h"
#include "PhysicsImp/Body/bcRigidStatic.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_rigid_static< g_api_physx >::bc_platform_rigid_static()
			: bc_platform_rigid_body()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_rigid_static< g_api_physx >::bc_platform_rigid_static(const bc_platform_rigid_static& p_other) noexcept
			: bc_platform_rigid_body(p_other)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_rigid_static< g_api_physx >::~bc_platform_rigid_static()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_rigid_static< g_api_physx >& bc_platform_rigid_static< g_api_physx >::operator=(const bc_platform_rigid_static& p_other) noexcept
		{
			bc_platform_rigid_body::operator=(p_other);

			return *this;
		}
	}
}