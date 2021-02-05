// [12/17/2016 MRB]

#pragma once

#include "Core/Memory/bcPtr.h"
#include "Core/Container/bcVector.h"
#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/Fundation/bcFundation.h"
#include "PhysicsImp/Body/bcActor.h"
#include "PhysicsImp/Shape/bcShape.h"
#include "PhysicsImp/Joint/bcJoint.h"
#include "PhysicsImp/Fundation/bcSimulationEventCallback.h"
#include "PhysicsImp/Collision/bcContactFilterCallback.h"
#include "PhysicsImp/Collision/bcContactModifyCallback.h"
#include "PhysicsImp/Collision/bcSceneQuery.h"

namespace black_cat
{
	namespace physics
	{
		class bc_px_filter_shader_data
		{
		public:
			bc_px_filter_shader_data()
				: m_use_ccd(false),
				m_use_filter_callback(false)
			{
			}

			bool m_use_ccd;
			bool m_use_filter_callback;
		};

		class bc_px_allocator : public physx::PxAllocatorCallback
		{
		public:
			explicit bc_px_allocator(core::bc_unique_ptr< bci_allocator > p_imp)
				: m_imp(std::move(p_imp))
			{
			}

			void* allocate(size_t p_size, const bcCHAR* p_type_name, const bcCHAR* p_file_name, bcINT p_line) override
			{
				return m_imp->allocate(p_size, p_file_name, p_line);
			}

			void deallocate(void* p_ptr) override
			{
				m_imp->free(p_ptr);
			}

			core::bc_unique_ptr< bci_allocator > m_imp;
		};

		class bc_px_task_dispatcher : public physx::PxCpuDispatcher
		{
		public:
			explicit bc_px_task_dispatcher(core::bc_unique_ptr< bci_task_dispatcher > p_imp)
				: m_imp(std::move(p_imp))
			{
			}

			void submitTask(physx::PxBaseTask& task) override
			{
				m_imp->add_task(bc_task(bc_task::platform_pack(&task)));
			}

			physx::PxU32 getWorkerCount() const override
			{
				return m_imp->worker_count();
			}

			core::bc_unique_ptr< bci_task_dispatcher > m_imp;
		};

		class bc_px_logger : public physx::PxErrorCallback
		{
		public:
			explicit bc_px_logger(core::bc_unique_ptr<bci_logger> p_imp)
				: m_imp(std::move(p_imp))
			{
			}

			void reportError(physx::PxErrorCode::Enum p_code, const bcCHAR* p_message, const bcCHAR* p_file, bcINT p_line) override
			{
				switch (p_code)
				{
				case physx::PxErrorCode::ePERF_WARNING:
					m_imp->info(p_code, p_message, p_file, p_line);
					break;
				case physx::PxErrorCode::eDEBUG_INFO:
				case physx::PxErrorCode::eDEBUG_WARNING:
					m_imp->debug(p_code, p_message, p_file, p_line);
					break;
				case physx::PxErrorCode::eINTERNAL_ERROR:
				case physx::PxErrorCode::eINVALID_OPERATION:
				case physx::PxErrorCode::eINVALID_PARAMETER:
				case physx::PxErrorCode::eOUT_OF_MEMORY:
					m_imp->error(p_code, p_message, p_file, p_line);
					break;
				default:;
				}
			}

			core::bc_unique_ptr<bci_logger> m_imp;
		};

		class bc_px_contact_filter_callback : public physx::PxSimulationFilterCallback
		{
		public:
			explicit bc_px_contact_filter_callback(core::bc_unique_ptr< bci_contact_filter_callback > p_imp);

			physx::PxFilterFlags pairFound(physx::PxU32 p_pair_id,
				physx::PxFilterObjectAttributes p_attributes0,
				physx::PxFilterData p_filter_data0,
				const physx::PxActor* p_a0,
				const physx::PxShape* p_s0,
				physx::PxFilterObjectAttributes p_attributes1,
				physx::PxFilterData p_filter_data1,
				const physx::PxActor* p_a1,
				const physx::PxShape* p_s1,
				physx::PxPairFlags& p_pair_flags) override;

			void pairLost(physx::PxU32 p_pair_id,
				physx::PxFilterObjectAttributes p_attributes0,
				physx::PxFilterData p_filter_data0,
				physx::PxFilterObjectAttributes p_attributes1,
				physx::PxFilterData p_filter_data1,
				bool p_object_removed) override;

			bool statusChange(physx::PxU32& p_pair_id,
				physx::PxPairFlags& p_pair_flags,
				physx::PxFilterFlags& p_filter_flags) override;

			core::bc_unique_ptr< bci_contact_filter_callback > m_imp;
		};

		class bc_px_contact_modify_callback : public physx::PxContactModifyCallback, public physx::PxCCDContactModifyCallback
		{
		public:
			explicit bc_px_contact_modify_callback(core::bc_unique_ptr< bci_contact_modify_callback > p_imp);

			void onContactModify(physx::PxContactModifyPair* const p_pairs, physx::PxU32 p_count) override;

			void onCCDContactModify(physx::PxContactModifyPair* const p_pairs, physx::PxU32 p_count) override;

			core::bc_unique_ptr< bci_contact_modify_callback > m_imp;
		};

		class bc_px_simulation_callback : public physx::PxSimulationEventCallback
		{
		public:
			explicit bc_px_simulation_callback(core::bc_unique_ptr< bci_simulation_event_callback > p_imp);

			void onConstraintBreak(physx::PxConstraintInfo* p_constraints, physx::PxU32 p_count) override;

			void onWake(physx::PxActor** p_actors, physx::PxU32 p_count) override;

			void onSleep(physx::PxActor** p_actors, physx::PxU32 p_count) override;

			void onTrigger(physx::PxTriggerPair* p_pairs, physx::PxU32 p_count) override;

			void onContact(const physx::PxContactPairHeader& p_pair_header,
				const physx::PxContactPair* p_pairs,
				physx::PxU32 p_pairs_count) override;

			core::bc_unique_ptr< bci_simulation_event_callback > m_imp;
		};

		class bc_px_query_filter_callback : public physx::PxQueryFilterCallback
		{
		public:
			explicit bc_px_query_filter_callback(bc_scene_query_post_filter_callback* p_post_filter_callback)
				: m_post_filter_callback(p_post_filter_callback)
			{
			}
			
			physx::PxQueryHitType::Enum preFilter(const physx::PxFilterData& p_filter_data,
				const physx::PxShape* p_shape,
				const physx::PxRigidActor* p_actor,
				physx::PxHitFlags& p_query_flags) override;

			physx::PxQueryHitType::Enum postFilter(const physx::PxFilterData& p_filter_data,
				const physx::PxQueryHit& p_hit) override;

		private:
			bc_scene_query_post_filter_callback* m_post_filter_callback;
		};

		physx::PxFilterFlags bc_px_filter_shader(physx::PxFilterObjectAttributes p_attributes0,
			physx::PxFilterData p_filter_data0,
			physx::PxFilterObjectAttributes p_attributes1,
			physx::PxFilterData p_filter_data1,
			physx::PxPairFlags& p_pair_flags,
			const void* p_constant_block,
			physx::PxU32 p_constant_block_size);
	}
}