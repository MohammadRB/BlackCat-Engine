// [12/20/2016 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/Fundation/bcCController.h"
#include "PhysicsImp/Body/bcActor.h"
#include "PhysicsImp/Shape/bcShape.h"
#include "PhysicsImp/Joint/bcJoint.h"
#include "PhysicsImp/bcImplementation.h"

namespace black_cat
{
	namespace physics
	{
		bc_px_contact_filter_callback::bc_px_contact_filter_callback(core::bc_unique_ptr<bci_contact_filter_callback> p_imp) noexcept
			: m_imp(std::move(p_imp))
		{
		}

		physx::PxFilterFlags bc_px_contact_filter_callback::pairFound(physx::PxU32 p_pair_id,
			physx::PxFilterObjectAttributes p_attributes0, 
			physx::PxFilterData p_filter_data0, 
			const physx::PxActor* p_a0,
			const physx::PxShape* p_s0,
			physx::PxFilterObjectAttributes p_attributes1, 
			physx::PxFilterData p_filter_data1,
			const physx::PxActor* p_a1, 
			const physx::PxShape* p_s1, 
			physx::PxPairFlags& p_pair_flags)
		{
			const auto l_px_actor0_type = physx::PxGetFilterObjectType(p_attributes0);
			const bool l_px_actor0_is_kinematic = physx::PxFilterObjectIsKinematic(p_attributes0);
			const bool l_px_actor0_is_trigger = physx::PxFilterObjectIsTrigger(p_attributes0);
			const bc_collision_filter l_actor0_filter_data
			(
				static_cast<bc_collision_group>(p_filter_data0.word0),
				static_cast<bc_collision_group>(p_filter_data0.word1)
			);
			const auto l_px_actor1_type = physx::PxGetFilterObjectType(p_attributes1);
			const bool l_px_actor1_is_kinematic = physx::PxFilterObjectIsKinematic(p_attributes1);
			const bool l_px_actor1_is_trigger = physx::PxFilterObjectIsTrigger(p_attributes1);
			const bc_collision_filter l_actor1_filter_data
			(
				static_cast<bc_collision_group>(p_filter_data1.word0),
				static_cast<bc_collision_group>(p_filter_data1.word1)
			);

			bc_actor::platform_pack l_actor_pack0;
			l_actor_pack0.m_px_object = const_cast<physx::PxActor*>(p_a0);
			bc_actor::platform_pack l_actor_pack1;
			l_actor_pack1.m_px_object = const_cast<physx::PxActor*>(p_a1);
			bc_shape::platform_pack l_shape_pack0;
			l_shape_pack0.m_px_object = const_cast<physx::PxShape*>(p_s0);
			bc_shape::platform_pack l_shape_pack1;
			l_shape_pack1.m_px_object = const_cast<physx::PxShape*>(p_s1);
			
			bc_actor l_actor0(l_actor_pack0);
			bc_actor l_actor1(l_actor_pack1);
			bc_shape l_shape0(l_shape_pack0);
			bc_shape l_shape1(l_shape_pack1);

			const auto l_actor0_flags = core::bc_enum::none<bc_filter_object_flag>();
			const auto l_actor1_flags = core::bc_enum::none<bc_filter_object_flag>();

			core::bc_enum::set
			(
				l_actor0_flags,
				bc_filter_object_flag::rigid_static,
				core::bc_enum::has(l_px_actor0_type, physx::PxFilterObjectType::eRIGID_STATIC)
			);
			core::bc_enum::set
			(
				l_actor0_flags,
				bc_filter_object_flag::rigid_dynamic,
				core::bc_enum::has(l_px_actor0_type, physx::PxFilterObjectType::eRIGID_DYNAMIC)
			);
			core::bc_enum::set
			(
				l_actor0_flags,
				bc_filter_object_flag::kinematic,
				l_px_actor0_is_kinematic
			);
			core::bc_enum::set
			(
				l_actor0_flags,
				bc_filter_object_flag::trigger,
				l_px_actor0_is_trigger
			);
			core::bc_enum::set
			(
				l_actor1_flags,
				bc_filter_object_flag::rigid_static,
				core::bc_enum::has(l_px_actor1_type, physx::PxFilterObjectType::eRIGID_STATIC)
			);
			core::bc_enum::set
			(
				l_actor1_flags,
				bc_filter_object_flag::rigid_dynamic,
				core::bc_enum::has(l_px_actor1_type, physx::PxFilterObjectType::eRIGID_DYNAMIC)
			);
			core::bc_enum::set
			(
				l_actor1_flags,
				bc_filter_object_flag::kinematic,
				l_px_actor1_is_kinematic
			);
			core::bc_enum::set
			(
				l_actor1_flags,
				bc_filter_object_flag::trigger,
				l_px_actor1_is_trigger
			);

			const bool l_result = m_imp->pair_found
			(
				l_actor0_flags,
				l_actor0_filter_data,
				&l_actor0,
				&l_shape0,
				l_actor1_flags,
				l_actor1_filter_data,
				&l_actor1,
				&l_shape1
			);

			return l_result ? physx::PxFilterFlag::eDEFAULT : physx::PxFilterFlag::eKILL;
		}

		void bc_px_contact_filter_callback::pairLost(physx::PxU32 p_pair_id, 
			physx::PxFilterObjectAttributes p_attributes0, 
			physx::PxFilterData p_filter_data0, 
			physx::PxFilterObjectAttributes p_attributes1, 
			physx::PxFilterData p_filter_data1, 
			bool p_object_removed)
		{
		}

		bool bc_px_contact_filter_callback::statusChange(physx::PxU32& p_pair_id, 
			physx::PxPairFlags& p_pair_flags, 
			physx::PxFilterFlags& p_filter_flags)
		{
			return false;
		}

		bc_px_contact_modify_callback::bc_px_contact_modify_callback(core::bc_unique_ptr<bci_contact_modify_callback> p_imp) noexcept
			: m_imp(std::move(p_imp))
		{
		}

		void bc_px_contact_modify_callback::onContactModify(physx::PxContactModifyPair* const p_pairs, physx::PxU32 p_count)
		{
			core::bc_vector_frame<bc_contact_modify_pair> l_pairs(p_count);

			for (bcUINT32 i = 0; i <p_count; ++i)
			{
				bc_contact_modify_pair::platform_pack l_pack;
				l_pack.m_px_pair = &p_pairs[i];
				l_pack.m_is_ccd = false;

				l_pairs[i] = bc_contact_modify_pair(l_pack);
			}

			m_imp->on_contact_modify(l_pairs.data(), p_count);
		}

		void bc_px_contact_modify_callback::onCCDContactModify(physx::PxContactModifyPair* const p_pairs, physx::PxU32 p_count)
		{
			core::bc_vector_frame<bc_contact_modify_pair> l_pairs(p_count);

			for (bcUINT32 i = 0; i <p_count; ++i)
			{
				bc_contact_modify_pair::platform_pack l_pack;
				l_pack.m_px_pair = &p_pairs[i];
				l_pack.m_is_ccd = true;

				l_pairs[i] = bc_contact_modify_pair(l_pack);
			}

			m_imp->on_contact_modify(l_pairs.data(), p_count);
		}

		bc_px_simulation_callback::bc_px_simulation_callback(core::bc_unique_ptr<bci_physics_simulation_callback> p_imp) noexcept
			: m_imp(std::move(p_imp))
		{
		}

		void bc_px_simulation_callback::onConstraintBreak(physx::PxConstraintInfo* p_constraints, physx::PxU32 p_count)
		{
			core::bc_vector_frame<bc_joint> l_joints(p_count);

			for (bcUINT32 i = 0; i <p_count; ++i)
			{
				bc_joint::platform_pack l_pack;
				l_pack.m_px_object = static_cast<physx::PxJoint*>(p_constraints[i].externalReference);

				l_joints[i] = bc_joint(l_pack);
			}

			m_imp->on_joint_break(l_joints.data(), p_count);
		}

		void bc_px_simulation_callback::onWake(physx::PxActor** p_actors, physx::PxU32 p_count)
		{
			core::bc_vector_frame<bc_actor> l_actors(p_count);

			for (bcUINT32 i = 0; i <p_count; ++i)
			{
				bc_actor::platform_pack l_pack;
				l_pack.m_px_object = p_actors[i];

				l_actors[i] = bc_actor(l_pack);
			}

			m_imp->on_wake(l_actors.data(), p_count);
		}

		void bc_px_simulation_callback::onSleep(physx::PxActor** p_actors, physx::PxU32 p_count)
		{
			core::bc_vector_frame<bc_actor> l_actors(p_count);

			for (bcUINT32 i = 0; i <p_count; ++i)
			{
				bc_actor::platform_pack l_pack;
				l_pack.m_px_object = p_actors[i];
				
				l_actors[i] = bc_actor(l_pack);
			}

			m_imp->on_sleep(l_actors.data(), p_count);
		}

		void bc_px_simulation_callback::onTrigger(physx::PxTriggerPair* p_pairs, physx::PxU32 p_count)
		{
			core::bc_vector_frame<bc_trigger_pair> l_pairs(p_count);

			for (bcUINT32 i = 0; i <p_count; ++i)
			{
				bc_trigger_pair::platform_pack l_pack;
				l_pack.m_px_pair = &p_pairs[i];
				
				l_pairs[i] = bc_trigger_pair(l_pack);
			}

			m_imp->on_trigger(l_pairs.data(), p_count);
		}

		void bc_px_simulation_callback::onContact(const physx::PxContactPairHeader& p_pair_header, 
			const physx::PxContactPair* p_pairs, 
			physx::PxU32 p_pairs_count)
		{
			bc_contant_pair::platform_pack l_pack;
			l_pack.m_px_pair = &const_cast<physx::PxContactPairHeader&>(p_pair_header);

			const bc_contant_pair l_pair(l_pack);

			core::bc_vector_frame<bc_contact_shape_pair> l_shape_pairs(p_pairs_count);

			for (bcUINT32 i = 0; i <p_pairs_count; ++i)
			{
				bc_contact_shape_pair::platform_pack l_contact_pack;
				l_contact_pack.m_px_pair = const_cast<physx::PxContactPair*>(&p_pairs[i]);
				
				l_shape_pairs[i] = bc_contact_shape_pair(l_contact_pack);
			}

			m_imp->on_contact(l_pair, l_shape_pairs.data(), p_pairs_count);
		}

		void bc_px_simulation_callback::onAdvance(const physx::PxRigidBody* const* p_body_buffer,
			const physx::PxTransform* p_pose_buffer,
			const physx::PxU32 p_count)
		{
		}

		bc_px_query_filter_callback::bc_px_query_filter_callback(bc_scene_query_post_filter_callback* p_post_filter_callback,
			bool p_high_detail_query_shape,
			bc_shape_query_flag p_default_type) noexcept
			: m_post_filter_callback(p_post_filter_callback),
			m_high_detail_query_shape(p_high_detail_query_shape),
			m_default_type(p_default_type)
		{
		}

		physx::PxQueryHitType::Enum bc_px_query_filter_callback::preFilter(const physx::PxFilterData& p_filter_data, 
			const physx::PxShape* p_shape, 
			const physx::PxRigidActor* p_actor, 
			physx::PxHitFlags& p_query_flags)
		{
			bc_shape::platform_pack l_shape_pack;
			l_shape_pack.m_px_object = const_cast<physx::PxShape*>(p_shape);
			const bc_shape l_shape(l_shape_pack);

			if(!m_high_detail_query_shape && l_shape.get_high_detail_query_shape())
			{
				return physx::PxQueryHitType::eNONE;
			}

			// TODO this check will be done in PhysX fixed pipeline
			const bc_query_group l_query_groups = static_cast<bc_query_group>(p_filter_data.word0);
			const bc_query_group l_shape_group = l_shape.get_query_group();

			// If filter data is zero or query groups does not include shape query group then discard test
			if (l_query_groups != bc_query_group::all && !core::bc_enum::has(l_query_groups, l_shape_group))
			{
				return physx::PxQueryHitType::eNONE;
			}

			const bc_shape_query_flag l_flags = l_shape.get_query_flags();

			if (core::bc_enum::has(l_flags, bc_shape_query_flag::touching))
			{
				return physx::PxQueryHitType::eTOUCH;
			}

			return static_cast<physx::PxQueryHitType::Enum>(m_default_type);
		}

		physx::PxQueryHitType::Enum bc_px_query_filter_callback::postFilter(const physx::PxFilterData& p_filter_data, const physx::PxQueryHit& p_hit)
		{
			BC_ASSERT(m_post_filter_callback);

			bc_rigid_actor::platform_pack l_actor_pack;
			l_actor_pack.m_px_object = p_hit.actor;
			bc_shape::platform_pack l_shape_pack;
			l_shape_pack.m_px_object = p_hit.shape;
			
			bc_scene_query_post_filter_data l_data;
			l_data.m_actor = bc_rigid_actor(l_actor_pack);
			l_data.m_shape = bc_shape(l_shape_pack);
			l_data.m_face_index = p_hit.faceIndex;
			
			return static_cast<physx::PxQueryHitType::Enum>((*m_post_filter_callback)(l_data));
		}

		bc_px_controller_hit_callback::bc_px_controller_hit_callback(bci_ccontroller_hit_callback* p_callback) noexcept
			: m_callback(p_callback)
		{
		}

		void bc_px_controller_hit_callback::onShapeHit(const physx::PxControllerShapeHit& p_hit)
		{
			bc_ccontroller_shape_hit::platform_pack l_pack;
			l_pack.m_px_hit = &p_hit;
			
			m_callback->on_shape_hit(bc_ccontroller_shape_hit(l_pack));
		}

		void bc_px_controller_hit_callback::onControllerHit(const physx::PxControllersHit& p_hit)
		{
			bc_ccontroller_controller_hit::platform_pack l_pack;
			l_pack.m_px_hit = &p_hit;
			
			m_callback->on_ccontroller_hit(bc_ccontroller_controller_hit(l_pack));
		}

		void bc_px_controller_hit_callback::onObstacleHit(const physx::PxControllerObstacleHit& p_hit)
		{
		}

		bc_px_ccontroller_query_filter::bc_px_ccontroller_query_filter(bc_scene_query_pre_filter_callback* p_pre_filter,
			bc_scene_query_post_filter_callback* p_post_filter,
			bool p_high_detail_query_shape) noexcept
			: m_pre_filter(p_pre_filter),
			m_post_filter(p_post_filter),
			m_high_detail_query_shape(p_high_detail_query_shape)
		{
		}

		physx::PxQueryHitType::Enum bc_px_ccontroller_query_filter::preFilter(const physx::PxFilterData& p_filter_data,
			const physx::PxShape* p_shape,
			const physx::PxRigidActor* p_actor,
			physx::PxHitFlags& p_query_flags)
		{
			bc_shape::platform_pack l_shape_pack;
			l_shape_pack.m_px_object = const_cast<physx::PxShape*>(p_shape);
			const bc_shape l_shape(l_shape_pack);

			if (!m_high_detail_query_shape && l_shape.get_high_detail_query_shape())
			{
				return physx::PxQueryHitType::eNONE;
			}

			if(!m_pre_filter)
			{
				return static_cast<physx::PxQueryHitType::Enum>(l_shape.get_query_flags());
			}
			
			bc_rigid_actor::platform_pack l_actor_pack;
			l_actor_pack.m_px_object = const_cast<physx::PxRigidActor*>(p_actor);
			
			bc_scene_query_pre_filter_data l_data;
			l_data.m_actor = bc_rigid_actor(l_actor_pack);
			l_data.m_shape = l_shape;
			
			return static_cast<physx::PxQueryHitType::Enum>((*m_pre_filter)(l_data));
		}

		physx::PxQueryHitType::Enum bc_px_ccontroller_query_filter::postFilter(const physx::PxFilterData& p_filter_data, const physx::PxQueryHit& p_hit)
		{
			BC_ASSERT(m_post_filter);

			bc_rigid_actor::platform_pack l_actor_pack;
			l_actor_pack.m_px_object = p_hit.actor;
			bc_shape::platform_pack l_shape_pack;
			l_shape_pack.m_px_object = p_hit.shape;
			
			bc_scene_query_post_filter_data l_data;
			l_data.m_actor = bc_rigid_actor(l_actor_pack);
			l_data.m_shape = bc_shape(l_shape_pack);
			l_data.m_face_index = p_hit.faceIndex;

			return static_cast<physx::PxQueryHitType::Enum>((*m_post_filter)(l_data));
		}

		bc_px_ccontroller_collision_filter::bc_px_ccontroller_collision_filter(bc_ccontroller_collision_filter_callback* p_callback) noexcept
			: m_callback(p_callback)
		{
		}

		bool bc_px_ccontroller_collision_filter::filter(const physx::PxController& p_controller1, const physx::PxController& p_controller2)
		{
			bc_ccontroller::platform_pack l_controller1_pack;
			bc_ccontroller::platform_pack l_controller2_pack;

			l_controller1_pack.m_controller = const_cast<physx::PxController*>(&p_controller1);
			l_controller2_pack.m_controller = const_cast<physx::PxController*>(&p_controller2);

			return (*m_callback)(bc_ccontroller(l_controller1_pack), bc_ccontroller(l_controller2_pack));
		}

		physx::PxFilterFlags bc_px_filter_shader(physx::PxFilterObjectAttributes p_attributes0, 
			physx::PxFilterData p_filter_data0, 
			physx::PxFilterObjectAttributes p_attributes1, 
			physx::PxFilterData p_filter_data1, 
			physx::PxPairFlags& p_pair_flags, 
			const void* p_constant_block, 
			physx::PxU32 p_constant_block_size)
		{
			p_pair_flags |= physx::PxPairFlag::eNOTIFY_CONTACT_POINTS;

			const auto* l_constant_block = static_cast<const bc_px_filter_shader_data*>(p_constant_block);

			// If filter callback is required so let callback be invoked
			if (l_constant_block->m_use_filter_callback)
			{
				return physx::PxFilterFlag::eCALLBACK;
			}

			// for triggers let act by default trigger behaviour
			if (physx::PxFilterObjectIsTrigger(p_attributes0) || physx::PxFilterObjectIsTrigger(p_attributes1))
			{
				p_pair_flags |= physx::PxPairFlag::eTRIGGER_DEFAULT;
				return physx::PxFilterFlag::eDEFAULT;
			}

			p_pair_flags |= physx::PxPairFlag::eCONTACT_DEFAULT;

			// Check collision group and masks
			if ((p_filter_data0.word0 & p_filter_data1.word1) && (p_filter_data1.word0 & p_filter_data0.word1))
			{
				p_pair_flags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;
				p_pair_flags |= physx::PxPairFlag::eNOTIFY_TOUCH_LOST;
			}

			const auto l_collision_notify0 = static_cast<bc_shape_notify_flag>(p_filter_data0.word2);
			const auto l_collision_notify1 = static_cast<bc_shape_notify_flag>(p_filter_data1.word2);

			// If object is tagged to report contact notifications, so notify
			if (core::bc_enum::has(l_collision_notify0, bc_shape_notify_flag::notify_touch) ||
				core::bc_enum::has(l_collision_notify1, bc_shape_notify_flag::notify_touch))
			{
				p_pair_flags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;
				p_pair_flags |= physx::PxPairFlag::eNOTIFY_TOUCH_LOST;
			}

			// If object is tagged to modify contacts, so modify
			if (core::bc_enum::has(l_collision_notify0, bc_shape_notify_flag::notify_modify) ||
				core::bc_enum::has(l_collision_notify1, bc_shape_notify_flag::notify_modify))
			{
				p_pair_flags |= physx::PxPairFlag::eMODIFY_CONTACTS;
			}

			// Enable CCD contact detection if required			
			if (l_constant_block->m_use_ccd)
			{
				p_pair_flags |= physx::PxPairFlag::eDETECT_CCD_CONTACT;
				p_pair_flags |= physx::PxPairFlag::eNOTIFY_TOUCH_CCD;
			}

			return physx::PxFilterFlag::eDEFAULT;
		}
	}
}