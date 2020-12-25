// [12/17/2016 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "Physics/Shape/bcShapeGeometry.h"
#include "PhysicsImp/bcExport.h"
#include "PhysicsImp/bcUtility.h"
#include "PhysicsImp/Fundation/bcScene.h"
#include "PhysicsImp/Body/bcActor.h"
#include "PhysicsImp/Body/bcAggregate.h"
#include "PhysicsImp/Shape/bcShapeGeometry.h"
#include "PhysicsImp/Shape/bcShape.h"
#include "PhysicsImp/Joint/bcJoint.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_scene< g_api_physx >::bc_platform_scene()
		{
			m_pack.m_data = nullptr;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_scene< g_api_physx >::bc_platform_scene(const bc_platform_scene& p_other) noexcept
			: bc_platform_physics_reference(p_other)
		{
			operator=(p_other);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_scene< g_api_physx >::~bc_platform_scene()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_scene< g_api_physx >& bc_platform_scene< g_api_physx >::operator=(const bc_platform_scene& p_other) noexcept
		{
			bc_platform_physics_reference::operator=(p_other);
			m_pack.m_data = p_other.m_pack.m_data;

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_scene< g_api_physx >::add_actor(bc_actor& p_actor)
		{
			auto* l_px_actor = static_cast< physx::PxActor* >
			(
				static_cast< bc_physics_reference& >(p_actor).get_platform_pack().m_px_object
			);

			m_pack.m_data->m_px_scene->addActor(*l_px_actor);
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_scene< g_api_physx >::remove_actor(bc_actor& p_actor, bool wake_on_lost_touch)
		{
			auto* l_px_actor = static_cast< physx::PxActor* >
			(
				static_cast< bc_physics_reference& >(p_actor).get_platform_pack().m_px_object
			);

			m_pack.m_data->m_px_scene->removeActor(*l_px_actor, wake_on_lost_touch);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT32 bc_platform_scene< g_api_physx >::get_actors_count(bc_actor_type p_types) const noexcept
		{
			return m_pack.m_data->m_px_scene->getNbActors(static_cast< physx::PxActorTypeFlag::Enum >(p_types));
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT32 bc_platform_scene< g_api_physx >::get_actors(bc_actor_type p_types, bc_actor* p_buffer, bcUINT32 p_buffer_size, bcUINT32 p_start_index) const
		{
			bcUINT32 l_written_count = m_pack.m_data->m_px_scene->getActors
			(
				static_cast< physx::PxActorTypeFlag::Enum >(p_types),
				reinterpret_cast< physx::PxActor** >(p_buffer),
				p_buffer_size,
				p_start_index
			);

			bc_overwrite_output_array<bc_actor, physx::PxActor>(p_buffer, l_written_count, [](bc_actor* p_bc_actor, physx::PxActor* p_px_actor)
			{
				static_cast<bc_physics_reference&>(*p_bc_actor).get_platform_pack().m_px_object = p_px_actor;
			});

			return l_written_count;
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_scene< g_api_physx >::add_aggregate(bc_aggregate& p_aggregate)
		{
			auto* l_px_aggregate = static_cast< physx::PxAggregate* >
			(
				static_cast< bc_physics_reference& >(p_aggregate).get_platform_pack().m_px_object
			);

			m_pack.m_data->m_px_scene->addAggregate(*l_px_aggregate);
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_scene< g_api_physx >::remove_aggregate(bc_aggregate& p_aggregate, bool wake_on_lost_touch)
		{
			auto* l_px_aggregate = static_cast< physx::PxAggregate* >
			(
				static_cast< bc_physics_reference& >(p_aggregate).get_platform_pack().m_px_object
			);

			m_pack.m_data->m_px_scene->removeAggregate(*l_px_aggregate, wake_on_lost_touch);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT32 bc_platform_scene< g_api_physx >::get_aggregates_count() const noexcept
		{
			return m_pack.m_data->m_px_scene->getNbAggregates();
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT32 bc_platform_scene< g_api_physx >::get_aggregates(bc_aggregate* p_buffer, bcUINT32 p_buffer_size, bcUINT32 p_start_index) const
		{
			bcUINT32 l_written_count = m_pack.m_data->m_px_scene->getAggregates
			(
				reinterpret_cast< physx::PxAggregate** >(p_buffer),
				p_buffer_size,
				p_start_index
			);

			bc_overwrite_output_array<bc_aggregate, physx::PxAggregate>(p_buffer, l_written_count, [](bc_aggregate* p_bc_aggregate, physx::PxAggregate* p_px_aggregate)
			{
				static_cast<bc_physics_reference&>(*p_bc_aggregate).get_platform_pack().m_px_object = p_px_aggregate;
			});

			return l_written_count;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT32 bc_platform_scene< g_api_physx >::get_joints_count() const noexcept
		{
			return m_pack.m_data->m_px_scene->getNbConstraints();
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT32 bc_platform_scene< g_api_physx >::get_joints(bc_joint* p_buffer, bcUINT32 p_buffer_size, bcUINT32 p_start_index) const
		{
			bcUINT32 l_written_count = m_pack.m_data->m_px_scene->getConstraints
			(
				reinterpret_cast< physx::PxConstraint** >(p_buffer),
				p_buffer_size,
				p_start_index
			);

			bc_overwrite_output_array<bc_joint, physx::PxConstraint>(p_buffer, l_written_count, [](bc_joint* p_bc_joint, physx::PxConstraint* p_px_constraint)
			{
				static_cast<bc_physics_reference&>(*p_bc_joint).get_platform_pack().m_px_object = p_px_constraint;
			});

			return l_written_count;
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_scene< g_api_physx >::update(const core_platform::bc_clock::update_param& p_time)
		{
			constexpr bcUINT32 l_block_size = 16 * 1024;
			constexpr bcUINT32 l_num_block_to_alloc = 10;

			void* l_temp_buffer = m_pack.m_data->m_allocator->temp_allocate(l_block_size * l_num_block_to_alloc, __FILE__, __LINE__);

			m_pack.m_data->m_px_scene->simulate(p_time.m_fixed_elapsed_second, nullptr, l_temp_buffer, l_block_size * l_num_block_to_alloc);

			m_pack.m_data->m_allocator->temp_free(l_temp_buffer);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bool bc_platform_scene< g_api_physx >::update_done() const noexcept
		{
			return m_pack.m_data->m_px_scene->checkResults();
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_scene< g_api_physx >::wait() const noexcept
		{
			m_pack.m_data->m_px_scene->fetchResults(true);
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_scene< g_api_physx >::set_gravity(const core::bc_vector3f& p_gravity) noexcept
		{
			m_pack.m_data->m_px_scene->setGravity(bc_to_right_hand(p_gravity));
		}

		template<>
		BC_PHYSICSIMP_DLL
		core::bc_vector3f bc_platform_scene< g_api_physx >::get_gravity() const noexcept
		{
			auto l_px_vec = m_pack.m_data->m_px_scene->getGravity();

			return bc_to_game_hand(l_px_vec);
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_scene< g_api_physx >::enable_visualization() noexcept
		{
			m_pack.m_data->m_px_scene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.0f);
			m_pack.m_data->m_px_scene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_AABBS, true);
			m_pack.m_data->m_px_scene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_SHAPES, true);
			m_pack.m_data->m_px_scene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_COMPOUNDS, true);
			m_pack.m_data->m_px_scene->setVisualizationParameter(physx::PxVisualizationParameter::eJOINT_LOCAL_FRAMES, true);
			m_pack.m_data->m_px_scene->setVisualizationParameter(physx::PxVisualizationParameter::eJOINT_LIMITS, true);
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_scene< g_api_physx >::disable_visualization() noexcept
		{
			m_pack.m_data->m_px_scene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 0.0f);
			m_pack.m_data->m_px_scene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_AABBS, false);
			m_pack.m_data->m_px_scene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_SHAPES, false);
			m_pack.m_data->m_px_scene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_COMPOUNDS, false);
			m_pack.m_data->m_px_scene->setVisualizationParameter(physx::PxVisualizationParameter::eJOINT_LOCAL_FRAMES, false);
			m_pack.m_data->m_px_scene->setVisualizationParameter(physx::PxVisualizationParameter::eJOINT_LIMITS, false);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_scene_debug bc_platform_scene< g_api_physx >::get_debug_buffer() noexcept
		{
			bc_scene_debug l_result;
			l_result.get_platform_pack().m_px_debug = &m_pack.m_data->m_px_scene->getRenderBuffer();
			
			return l_result;
		}

		template<>
		BC_PHYSICSIMP_DLL
		core::bc_vector_frame<bc_updated_actor> bc_platform_scene< g_api_physx >::get_active_actors() noexcept
		{
			physx::PxU32 l_transform_count;
			const physx::PxActiveTransform* l_transforms = m_pack.m_data->m_px_scene->getActiveTransforms(l_transform_count);

			core::bc_vector_frame<bc_updated_actor> l_result;
			l_result.reserve(l_transform_count);

			for (physx::PxU32 l_index = 0; l_index < l_transform_count; ++l_index)
			{
				bc_actor l_actor;
				static_cast<bc_physics_reference&>(l_actor).get_platform_pack().m_px_object = l_transforms[l_index].actor;
				bc_transform l_transform;
				l_transform.get_platform_pack().m_px_transform = l_transforms[l_index].actor2World;

				l_result.emplace_back(l_actor, l_transform);
			}

			return l_result;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bool bc_platform_scene<g_api_physx>::raycast(const bc_ray& p_ray, 
			bc_scene_ray_query_buffer& p_buffer, 
			bc_hit_flag p_hit_flags, 
			bc_query_flags p_query_flags, 
			bc_query_group p_query_groups) const
		{
			physx::PxVec3 l_origin = bc_to_right_hand(p_ray.m_origin);
			physx::PxVec3 l_dir = bc_to_right_hand(p_ray.m_dir);
			physx::PxQueryFilterData l_px_filter_data
			(
				physx::PxFilterData(static_cast< physx::PxU32 >(p_query_groups), 0, 0, 0),
				static_cast< physx::PxQueryFlag::Enum >(p_query_flags)
			);
			l_px_filter_data.flags |= physx::PxQueryFlag::ePREFILTER;

			return m_pack.m_data->m_px_scene->raycast
			(
				l_origin,
				l_dir,
				p_ray.m_length,
				p_buffer.get_platform_pack().m_px_query,
				static_cast< physx::PxHitFlag::Enum >(p_hit_flags),
				l_px_filter_data,
				&const_cast<bc_platform_scene&>(*this).m_pack.m_data->m_query_filter_callback
			);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bool bc_platform_scene<g_api_physx>::sweep(const bc_shape_geometry& p_geometry, 
			const bc_transform& p_pose, 
			const core::bc_vector3f& p_unit_dir, 
			const bcFLOAT p_max_dist, 
			bc_scene_sweep_query_buffer& p_buffer, 
			bc_hit_flag p_hit_flags, 
			bc_query_flags p_query_flags, 
			bc_query_group p_query_groups) const
		{
			physx::PxVec3 l_dir = bc_to_right_hand(p_unit_dir);
			physx::PxQueryFilterData l_px_filter_data
			(
				physx::PxFilterData(static_cast< physx::PxU32 >(p_query_groups), 0, 0, 0), 
				static_cast< physx::PxQueryFlag::Enum >(p_query_flags)
			);
			l_px_filter_data.flags |= physx::PxQueryFlag::ePREFILTER;

			return m_pack.m_data->m_px_scene->sweep
			(
				*const_cast<bc_shape_geometry&>(p_geometry).get_platform_pack().m_px_geometry,
				const_cast< bc_transform& >(p_pose).get_platform_pack().m_px_transform,
				l_dir,
				p_max_dist,
				p_buffer.get_platform_pack().m_px_query,
				static_cast< physx::PxHitFlag::Enum >(p_hit_flags),
				l_px_filter_data,
				&const_cast<bc_platform_scene&>(*this).m_pack.m_data->m_query_filter_callback
			);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bool bc_platform_scene<g_api_physx>::overlap(const bc_shape_geometry& p_geometry, 
			const bc_transform& p_pose, 
			bc_scene_overlap_query_buffer& p_buffer, 
			bc_query_flags p_query_flags, 
			bc_query_group p_query_groups) const
		{
			physx::PxQueryFilterData l_px_filter_data
			(
				physx::PxFilterData(static_cast< physx::PxU32 >(p_query_groups), 0, 0, 0),
				static_cast< physx::PxQueryFlag::Enum >(p_query_flags)
			);
			// Because in overlap queries me must not return block flag from filter callback, there is no need
			// to specify ePREFILTER flag and usage of PxQueryFilterCallback

			return m_pack.m_data->m_px_scene->overlap
			(
				*const_cast<bc_shape_geometry&>(p_geometry).get_platform_pack().m_px_geometry,
				const_cast< bc_transform& >(p_pose).get_platform_pack().m_px_transform,
				p_buffer.get_platform_pack().m_px_query,
				l_px_filter_data
			);
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_scene< g_api_physx >::lock()
		{
			m_pack.m_data->m_px_scene->lockWrite();
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_scene< g_api_physx >::lock_shared()
		{
			m_pack.m_data->m_px_scene->lockRead();
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_scene< g_api_physx >::unlock()
		{
			m_pack.m_data->m_px_scene->unlockWrite();
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_scene< g_api_physx >::unlock_shared()
		{
			m_pack.m_data->m_px_scene->unlockRead();
		}

		template<>
		BC_PHYSICSIMP_DLL
		bool bc_platform_scene< g_api_physx >::is_valid() const noexcept
		{
			return m_pack.m_data != nullptr;
		}
	}
}