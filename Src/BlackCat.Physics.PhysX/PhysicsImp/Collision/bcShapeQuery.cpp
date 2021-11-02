// [12/16/2016 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/bcExport.h"
#include "PhysicsImp/bcUtility.h"
#include "PhysicsImp/Collision/bcShapeQuery.h"
#include "PhysicsImp/Body/bcRigidActor.h"
#include "PhysicsImp/Shape/bcShape.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_ray_hit<g_api_physx>::bc_platform_ray_hit() noexcept
			: m_pack()
		{
		}

		template<bc_physics_api TApi>
		bc_platform_ray_hit<TApi>::bc_platform_ray_hit(platform_pack& p_pack) noexcept
			: m_pack(p_pack)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_ray_hit<g_api_physx>::bc_platform_ray_hit(const bc_platform_ray_hit& p_other) noexcept
		{
			m_pack.m_px_hit = p_other.m_pack.m_px_hit;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_ray_hit<g_api_physx>::~bc_platform_ray_hit()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_ray_hit<g_api_physx>& bc_platform_ray_hit<g_api_physx>::operator=(const bc_platform_ray_hit& p_other) noexcept
		{
			m_pack.m_px_hit = p_other.m_pack.m_px_hit;

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_rigid_actor bc_platform_ray_hit<g_api_physx>::get_actor() const noexcept
		{
			bc_rigid_actor::platform_pack l_pack;
			l_pack.m_px_object = m_pack.m_px_hit.actor;
			
			return bc_rigid_actor(l_pack);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_shape bc_platform_ray_hit<g_api_physx>::get_shape() const noexcept
		{
			bc_shape::platform_pack l_pack;
			l_pack.m_px_object = m_pack.m_px_hit.shape;
			
			return bc_shape(l_pack);
		}

		template<>
		BC_PHYSICSIMP_DLL
		core::bc_vector3f bc_platform_ray_hit<g_api_physx>::get_position() const noexcept
		{
			return bc_to_game_hand(m_pack.m_px_hit.position);
		}

		template<>
		BC_PHYSICSIMP_DLL
		core::bc_vector3f bc_platform_ray_hit<g_api_physx>::get_normal() const noexcept
		{
			return bc_to_game_hand(m_pack.m_px_hit.normal);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcFLOAT bc_platform_ray_hit<g_api_physx>::get_distance() const noexcept
		{
			return m_pack.m_px_hit.distance;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT32 bc_platform_ray_hit<g_api_physx>::get_face_index() const noexcept
		{
			return m_pack.m_px_hit.faceIndex;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_overlap_hit<g_api_physx>::bc_platform_overlap_hit() noexcept
			: m_pack()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_overlap_hit<g_api_physx>::bc_platform_overlap_hit(platform_pack& p_pack) noexcept
			: m_pack(p_pack)
		{	
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_overlap_hit<g_api_physx>::bc_platform_overlap_hit(const bc_platform_overlap_hit& p_other) noexcept
		{
			m_pack.m_px_hit = p_other.m_pack.m_px_hit;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_overlap_hit<g_api_physx>::~bc_platform_overlap_hit()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_overlap_hit<g_api_physx>& bc_platform_overlap_hit<g_api_physx>::operator=(const bc_platform_overlap_hit& p_other) noexcept
		{
			m_pack.m_px_hit = p_other.m_pack.m_px_hit;

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_rigid_actor bc_platform_overlap_hit<g_api_physx>::get_actor() const noexcept
		{
			bc_rigid_actor::platform_pack l_pack;
			l_pack.m_px_object = m_pack.m_px_hit.actor;

			return bc_rigid_actor(l_pack);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_shape bc_platform_overlap_hit<g_api_physx>::get_shape() const noexcept
		{
			bc_shape::platform_pack l_pack;
			l_pack.m_px_object = m_pack.m_px_hit.actor;

			return bc_shape(l_pack);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT32 bc_platform_overlap_hit<g_api_physx>::get_face_index() const noexcept
		{
			return m_pack.m_px_hit.faceIndex;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_sweep_hit<g_api_physx>::bc_platform_sweep_hit() noexcept
			: m_pack()
		{
		}

		template<bc_physics_api TApi>
		bc_platform_sweep_hit<TApi>::bc_platform_sweep_hit(platform_pack& p_pack) noexcept
			: m_pack(p_pack)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_sweep_hit<g_api_physx>::bc_platform_sweep_hit(const bc_platform_sweep_hit& p_other) noexcept
		{
			m_pack.m_px_hit = p_other.m_pack.m_px_hit;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_sweep_hit<g_api_physx>::~bc_platform_sweep_hit()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_sweep_hit<g_api_physx>& bc_platform_sweep_hit<g_api_physx>::operator=(const bc_platform_sweep_hit& p_other) noexcept
		{
			m_pack.m_px_hit = p_other.m_pack.m_px_hit;

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_rigid_actor bc_platform_sweep_hit<g_api_physx>::get_actor() const noexcept
		{
			bc_rigid_actor::platform_pack l_pack;
			l_pack.m_px_object = m_pack.m_px_hit.actor;
			
			return bc_rigid_actor(l_pack);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_shape bc_platform_sweep_hit<g_api_physx>::get_shape() const noexcept
		{
			bc_shape::platform_pack  l_pack;
			l_pack.m_px_object = m_pack.m_px_hit.actor;

			return bc_shape(l_pack);
		}

		template<>
		BC_PHYSICSIMP_DLL
		core::bc_vector3f bc_platform_sweep_hit<g_api_physx>::get_position() const noexcept
		{
			return bc_to_game_hand(m_pack.m_px_hit.position);
		}

		template<>
		BC_PHYSICSIMP_DLL
		core::bc_vector3f bc_platform_sweep_hit<g_api_physx>::get_normal() const noexcept
		{
			return bc_to_game_hand(m_pack.m_px_hit.normal);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcFLOAT bc_platform_sweep_hit<g_api_physx>::get_distance() const noexcept
		{
			return m_pack.m_px_hit.distance;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT32 bc_platform_sweep_hit<g_api_physx>::get_face_index() const noexcept
		{
			return m_pack.m_px_hit.faceIndex;
		}

		bcUINT32 BC_PHYSICSIMP_DLL bc_shape_query::ray_cast(const bc_ray& p_ray,
			const bc_shape_geometry& p_shape,
			const bc_transform& p_shape_pose,
			bc_hit_flag p_flags,
			bc_ray_hit* p_hits,
			bcUINT32 p_hits_count)
		{
			const bcUINT32 l_written_count = physx::PxGeometryQuery::raycast
			(
				bc_to_right_hand(p_ray.m_origin),
				bc_to_right_hand(p_ray.m_dir),
				*p_shape.get_platform_pack().m_px_geometry,
				p_shape_pose.get_platform_pack().m_px_transform,
				p_ray.m_length,
				static_cast< physx::PxHitFlag::Enum >(p_flags),
				p_hits_count,
				&p_hits->get_platform_pack().m_px_hit
			);

			bc_overwrite_output_array<bc_ray_hit, physx::PxRaycastHit>(p_hits, l_written_count, [](physx::PxRaycastHit& p_px_hit)
			{
				bc_ray_hit::platform_pack l_pack;
				l_pack.m_px_hit = p_px_hit;
				
				return bc_ray_hit(l_pack);
			});

			return l_written_count;
		}

		bool BC_PHYSICSIMP_DLL bc_shape_query::sweep(const bc_shape_geometry& p_geom0,
			const bc_transform& p_pose0,
			const core::bc_vector3f& p_unit_dir,
			const bcFLOAT p_max_dist,
			const bc_shape_geometry& p_geom1,
			const bc_transform& p_pose1,
			bc_sweep_hit& p_hit,
			bc_hit_flag p_flags,
			const bcFLOAT p_inflation)
		{
			const bool l_result = physx::PxGeometryQuery::sweep
			(
				bc_to_right_hand(p_unit_dir),
				p_max_dist,
				*p_geom0.get_platform_pack().m_px_geometry,
				p_pose0.get_platform_pack().m_px_transform,
				*p_geom1.get_platform_pack().m_px_geometry,
				p_pose1.get_platform_pack().m_px_transform,
				p_hit.get_platform_pack().m_px_hit,
				static_cast< physx::PxHitFlag::Enum >(p_flags),
				p_inflation
			);

			return l_result;
		}

		bool BC_PHYSICSIMP_DLL bc_shape_query::overlap(const bc_shape_geometry& p_geom0,
			const bc_transform& p_pose0,
			const bc_shape_geometry& p_geom1,
			const bc_transform& p_pose1)
		{
			const bool l_result = physx::PxGeometryQuery::overlap
			(
				*p_geom0.get_platform_pack().m_px_geometry,
				p_pose0.get_platform_pack().m_px_transform,
				*p_geom1.get_platform_pack().m_px_geometry,
				p_pose1.get_platform_pack().m_px_transform
			);

			return l_result;
		}

		bool BC_PHYSICSIMP_DLL bc_shape_query::compute_penetration(const bc_shape_geometry& p_geom0,
			const bc_transform& p_pose0,
			const bc_shape_geometry& p_geom1,
			const bc_transform& p_pose1,
			core::bc_vector3f& p_direction,
			bcFLOAT& p_depth)
		{
			auto l_direction = bc_to_right_hand(static_cast< const core::bc_vector3f& >(p_direction));
			const bool l_result = physx::PxGeometryQuery::computePenetration
			(
				l_direction,
				p_depth,
				*p_geom0.get_platform_pack().m_px_geometry,
				p_pose0.get_platform_pack().m_px_transform,
				*p_geom1.get_platform_pack().m_px_geometry,
				p_pose1.get_platform_pack().m_px_transform
			);

			p_direction = bc_to_game_hand(l_direction);

			return l_result;
		}

		bcFLOAT BC_PHYSICSIMP_DLL bc_shape_query::point_distance(const core::bc_vector3f& p_point,
			const bc_shape_geometry& p_geom,
			const bc_transform& p_pose,
			core::bc_vector3f* p_closest_point)
		{
			physx::PxVec3 l_closest_point;

			const bcFLOAT l_result = physx::PxGeometryQuery::pointDistance
			(
				bc_to_right_hand(p_point),
				*p_geom.get_platform_pack().m_px_geometry,
				p_pose.get_platform_pack().m_px_transform,
				&l_closest_point
			);

			*p_closest_point = bc_to_game_hand(l_closest_point);

			return l_result;
		}

		bc_bound_box BC_PHYSICSIMP_DLL bc_shape_query::get_world_bounds(const bc_shape_geometry& p_geom, 
			const bc_transform& p_pose, 
			bcFLOAT p_inflation)
		{
			bc_bound_box::platform_pack l_pack;
			l_pack.m_bound = physx::PxGeometryQuery::getWorldBounds
			(
				*p_geom.get_platform_pack().m_px_geometry,
				p_pose.get_platform_pack().m_px_transform,
				p_inflation
			);
			
			return bc_bound_box(l_pack);
		}
	}
}