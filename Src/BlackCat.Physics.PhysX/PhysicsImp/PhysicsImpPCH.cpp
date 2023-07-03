// [08/12/2016 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/Fundation/bcCController.h"
#include "PhysicsImp/Collision/bcSceneQuery.h"
#include "PhysicsImp/Body/bcActor.h"
#include "PhysicsImp/Body/bcRigidBody.h"

namespace black_cat
{
	namespace physics
	{
		static_assert
		(
			static_cast<bcUINT32>(bc_query_hit_type::none) == physx::PxQueryHitType::eNONE &&
			static_cast<bcUINT32>(bc_query_hit_type::touch) == physx::PxQueryHitType::eTOUCH &&
			static_cast<bcUINT32>(bc_query_hit_type::block) == physx::PxQueryHitType::eBLOCK,
			"bc_query_hit_type value is incompatible with physX values"
		);

		static_assert
		(
			static_cast<bcUINT32>(bc_hit_flag::position) == physx::PxHitFlag::ePOSITION &&
			static_cast<bcUINT32>(bc_hit_flag::normal) == physx::PxHitFlag::eNORMAL &&
			static_cast<bcUINT32>(bc_hit_flag::distance) == physx::PxHitFlag::eDISTANCE &&
			static_cast<bcUINT32>(bc_hit_flag::mesh_multiple) == physx::PxHitFlag::eMESH_MULTIPLE &&
			static_cast<bcUINT32>(bc_hit_flag::mesh_any) == physx::PxHitFlag::eMESH_ANY &&
			static_cast<bcUINT32>(bc_hit_flag::mesh_both_side) == physx::PxHitFlag::eMESH_BOTH_SIDES &&
			static_cast<bcUINT32>(bc_hit_flag::precise_sweep) == physx::PxHitFlag::ePRECISE_SWEEP &&
			static_cast<bcUINT32>(bc_hit_flag::mtd) == physx::PxHitFlag::eMTD &&
			static_cast<bcUINT32>(bc_hit_flag::face_index) == physx::PxHitFlag::eFACE_INDEX,
			"bc_hit_flag value is incompatible with physX values"
		);

		static_assert
		(
			static_cast<bcUINT32>(bc_actor_type::rigid_static) == physx::PxActorType::eRIGID_STATIC &&
			static_cast<bcUINT32>(bc_actor_type::rigid_dynamic) == physx::PxActorType::eRIGID_DYNAMIC,
			"bc_actor_type value is incompatible with physX values"
		);

		static_assert
		(
			static_cast<bcUINT32>(bc_actor_flag::visualization) == physx::PxActorFlag::eVISUALIZATION &&
			static_cast<bcUINT32>(bc_actor_flag::disable_gravity) == physx::PxActorFlag::eDISABLE_GRAVITY &&
			static_cast<bcUINT32>(bc_actor_flag::send_sleep_wake) == physx::PxActorFlag::eSEND_SLEEP_NOTIFIES &&
			static_cast<bcUINT32>(bc_actor_flag::visualization) == physx::PxActorFlag::eVISUALIZATION,
			"bc_actor_flag value is incompatible with physX values"
		);

		static_assert
		(
			static_cast<bcUINT32>(bc_ccontroller_collision_flag::sides) == physx::PxControllerCollisionFlag::eCOLLISION_SIDES &&
			static_cast<bcUINT32>(bc_ccontroller_collision_flag::up) == physx::PxControllerCollisionFlag::eCOLLISION_UP &&
			static_cast<bcUINT32>(bc_ccontroller_collision_flag::down) == physx::PxControllerCollisionFlag::eCOLLISION_DOWN,
			"bc_ccontroller_collision_flag value is incompatible with physX values"
		);

		static_assert
		(
			static_cast<bcUINT32>(bc_force_mode::force) == physx::PxForceMode::eFORCE &&
			static_cast<bcUINT32>(bc_force_mode::impulse) == physx::PxForceMode::eIMPULSE,
			"bc_force_mode value is incompatible with physX values"
		);

		static_assert
		(
			static_cast<bcUINT32>(bc_rigid_body_flag::ccd) == physx::PxRigidBodyFlag::eENABLE_CCD &&
			static_cast<bcUINT32>(bc_rigid_body_flag::kinematic) == physx::PxRigidBodyFlag::eKINEMATIC,
			"bc_rigid_body_flag value is incompatible with physX values"
		);

		static_assert
		(
			static_cast<bcUINT32>(bc_shape_query_flag::touching) == physx::PxQueryHitType::eTOUCH &&
			static_cast<bcUINT32>(bc_shape_query_flag::blocking) == physx::PxQueryHitType::eBLOCK,
			"bc_shape_query_flag value is incompatible with physX values"
		);
	}
}