// [12/08/2016 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/Collision/bcSceneQuery.h"
#include "PhysicsImp/Collision/bcShapeQuery.h"

namespace black_cat
{
	namespace physics
	{
		static_assert
		(
			static_cast< bcUINT32 >(bc_query_hit_type::none) == physx::PxQueryHitType::eNONE &&
			static_cast< bcUINT32 >(bc_query_hit_type::touch) == physx::PxQueryHitType::eTOUCH &&
			static_cast< bcUINT32 >(bc_query_hit_type::block) == physx::PxQueryHitType::eBLOCK,
			"bc_query_hit_type value is incompatible with physX values"
		);

		static_assert
		(
			static_cast< bcUINT32 >(bc_hit_flag::position) == physx::PxHitFlag::ePOSITION &&
			static_cast< bcUINT32 >(bc_hit_flag::normal) == physx::PxHitFlag::eNORMAL &&
			static_cast< bcUINT32 >(bc_hit_flag::distance) == physx::PxHitFlag::eDISTANCE &&
			static_cast< bcUINT32 >(bc_hit_flag::mesh_multiple) == physx::PxHitFlag::eMESH_MULTIPLE &&
			static_cast< bcUINT32 >(bc_hit_flag::mesh_any) == physx::PxHitFlag::eMESH_ANY &&
			static_cast< bcUINT32 >(bc_hit_flag::mesh_both_side) == physx::PxHitFlag::eMESH_BOTH_SIDES &&
			static_cast< bcUINT32 >(bc_hit_flag::hit_info) == physx::PxHitFlag::eDEFAULT,
			"bc_hit_flag value is incompatible with physX values"
		);
	}
}