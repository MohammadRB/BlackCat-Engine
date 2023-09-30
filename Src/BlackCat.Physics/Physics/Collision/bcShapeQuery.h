// [05/12/2016 MRB]

#pragma once

#include "Core/Utility/bcEnumOperand.h"
#include "Core/Math/bcVector3f.h"
#include "Physics/bcPhysicsApi.h"
#include "Physics/bcExport.h"
#include "Physics/Fundation/bcTransform.h"
#include "Physics/Shape/bcBoundBox.h"
#include "Physics/Shape/bcShapeGeometry.h"

namespace black_cat::physics
{
	template<bc_physics_api TApi>
	class bc_platform_rigid_actor;
	using bc_rigid_actor = bc_platform_rigid_actor<g_current_physics_api>;

	template<bc_physics_api TApi>
	class bc_platform_shape;
	using bc_shape = bc_platform_shape<g_current_physics_api>;

	enum class bc_hit_flag
	{
		position = core::bc_enum::value(0),			// "position" member of #PxQueryHit is valid
		normal = core::bc_enum::value(1),			// "normal" member of #PxQueryHit is valid
		distance = core::bc_enum::value(2),			// "distance" member of #PxQueryHit is valid
		//eASSUME_NO_INITIAL_OVERLAP = (1 <<4),		// Performance hint flag for sweeps when it is known upfront there's no initial overlap.
		// NOTE: using this flag may cause undefined results if shapes are initially overlapping.
		mesh_multiple = core::bc_enum::value(5),	// Report all hits for meshes rather than just the first. Not applicable to sweep queries.
		mesh_any = core::bc_enum::value(6),			// Report any first hit for meshes. If neither eMESH_MULTIPLE nor eMESH_ANY is specified, a single closest hit will be reported for meshes.
		mesh_both_side = core::bc_enum::value(7),	// Report hits with back faces of mesh triangles.
		precise_sweep = core::bc_enum::value(8),	// Use more accurate but slower narrow phase sweep tests.
		// May provide better compatibility with PhysX 3.2 sweep behavior. Ignored on SPU.
		mtd = core::bc_enum::value(9),				// Report the minimum translation depth, normal and contact point. Ignored on SPU.
		face_index = core::bc_enum::value(10),		// Member of #PxQueryHit is valid
		hit_info = position | normal | distance
	};

	struct bc_ray
	{
		bc_ray(const core::bc_vector3f& p_origin, const core::bc_vector3f& p_dir, bcFLOAT p_length)
			: m_origin(p_origin),
			  m_dir(p_dir),
			  m_length(p_length)
		{
		}

		const core::bc_vector3f m_origin;
		const core::bc_vector3f m_dir;
		const bcFLOAT m_length;
	};

	template<bc_physics_api TApi>
	struct bc_platform_ray_hit_pack
	{
	};

	template<bc_physics_api TApi>
	class bc_platform_ray_hit
	{
	public:
		using platform_pack = bc_platform_ray_hit_pack<TApi>;

	public:
		bc_platform_ray_hit() noexcept;

		explicit bc_platform_ray_hit(platform_pack& p_pack) noexcept;

		bc_platform_ray_hit(const bc_platform_ray_hit&) noexcept;

		~bc_platform_ray_hit();

		bc_platform_ray_hit& operator=(const bc_platform_ray_hit&) noexcept;

		bc_rigid_actor get_actor() const noexcept;

		bc_shape get_shape() const noexcept;

		core::bc_vector3f get_position() const noexcept;

		core::bc_vector3f get_normal() const noexcept;

		bcFLOAT get_distance() const noexcept;

		bcUINT32 get_face_index() const noexcept;

		platform_pack& get_platform_pack() noexcept
		{
			return m_pack;
		}

		const platform_pack& get_platform_pack() const noexcept
		{
			return m_pack;
		}

	private:
		platform_pack m_pack;
	};

	using bc_ray_hit = bc_platform_ray_hit<g_current_physics_api>;

	template<bc_physics_api TApi>
	struct bc_platform_overlap_hit_pack
	{
	};

	template<bc_physics_api TApi>
	class bc_platform_overlap_hit
	{
	public:
		using platform_pack = bc_platform_overlap_hit_pack<TApi>;

	public:
		bc_platform_overlap_hit() noexcept;

		explicit bc_platform_overlap_hit(platform_pack& p_pack) noexcept;

		bc_platform_overlap_hit(const bc_platform_overlap_hit&) noexcept;

		~bc_platform_overlap_hit();

		bc_platform_overlap_hit& operator=(const bc_platform_overlap_hit&) noexcept;

		bc_rigid_actor get_actor() const noexcept;

		bc_shape get_shape() const noexcept;

		bcUINT32 get_face_index() const noexcept;

		platform_pack& get_platform_pack() noexcept
		{
			return m_pack;
		}

		const platform_pack& get_platform_pack() const noexcept
		{
			return m_pack;
		}

	private:
		platform_pack m_pack;
	};

	using bc_overlap_hit = bc_platform_overlap_hit<g_current_physics_api>;

	template<bc_physics_api TApi>
	struct bc_platform_sweep_hit_pack
	{
	};

	template<bc_physics_api TApi>
	class bc_platform_sweep_hit
	{
	public:
		using platform_pack = bc_platform_sweep_hit_pack<TApi>;

	public:
		bc_platform_sweep_hit() noexcept;

		explicit bc_platform_sweep_hit(platform_pack& p_pack) noexcept;

		bc_platform_sweep_hit(const bc_platform_sweep_hit&) noexcept;

		~bc_platform_sweep_hit();

		bc_platform_sweep_hit& operator=(const bc_platform_sweep_hit&) noexcept;

		bc_rigid_actor get_actor() const noexcept;

		bc_shape get_shape() const noexcept;

		core::bc_vector3f get_position() const noexcept;

		core::bc_vector3f get_normal() const noexcept;

		bcFLOAT get_distance() const noexcept;

		bcUINT32 get_face_index() const noexcept;

		platform_pack& get_platform_pack() noexcept
		{
			return m_pack;
		}

		const platform_pack& get_platform_pack() const noexcept
		{
			return m_pack;
		}

	private:
		platform_pack m_pack;
	};

	using bc_sweep_hit = bc_platform_sweep_hit<g_current_physics_api>;

	class bc_shape_query
	{
	public:
		/**
		 * \brief Ray-cast test against a geometry object.
		 * \param[in] p_ray The ray to test against shape
		 * \param[in] p_box The box object to test the ray against
		 * \param[in] p_flags Specification of the kind of information to retrieve on hit.
		 * \param[out] p_hits Ray-cast hits information
		 * \param[in] p_hits_count max number of returned hits = size of 'rayHits' buffer
		 * \return Number of hits between the ray and the geometry object
		 */
		static BC_PHYSICS_DLL bcUINT32 ray_cast(const bc_ray& p_ray,
		                                        const bc_bound_box& p_box,
		                                        bc_hit_flag p_flags,
		                                        bc_ray_hit* p_hits,
		                                        bcUINT32 p_hits_count);

		/**
		 * \brief Ray-cast test against a geometry object.
		 * \param[in] p_ray The ray to test against shape
		 * \param[in] p_shape The geometry object to test the ray against
		 * \param[in] p_shape_pose Pose of the geometry object
		 * \param[in] p_flags Specification of the kind of information to retrieve on hit.
		 * \param[out] p_hits Ray-cast hits information
		 * \param[in] p_hits_count max number of returned hits = size of 'rayHits' buffer
		 * \return Number of hits between the ray and the geometry object
		 */
		static BC_PHYSICS_DLL bcUINT32 ray_cast(const bc_ray& p_ray,
		                                        const bc_shape_geometry& p_shape,
		                                        const bc_transform& p_shape_pose,
		                                        bc_hit_flag p_flags,
		                                        bc_ray_hit* p_hits,
		                                        bcUINT32 p_hits_count);

		/**
		 * \brief Sweep a specified geometry object in space and test for collision with a given object.			
		 * \param[in] p_geom0 The geometry object to sweep.
		 * \param[in] p_pose0 Pose of the geometry object to sweep
		 * \param[in] p_unit_dir Normalized direction along which object geom0 should be swept
		 * \param[in] p_max_dist Maximum sweep distance, has to be in the [0, inf) range
		 * \param[in] p_geom1 The geometry object to test the sweep against
		 * \param[in] p_pose1 Pose of the geometry object to sweep against
		 * \param[out] p_hit The sweep hit information. Only valid if this method returns true.
		 * \param[in] p_flags Specify which properties per hit should be computed and written to result hit array. Combination of #PxHitFlag flags
		 * \param[in] p_inflation Surface of the swept shape is additively extruded in the normal direction, rounding corners and edges.
		 * \return True if the swept geometry object geom0 hits the object geom1
		 
		 * The following combinations are supported.
		 * \li PxSphereGeometry vs. {PxSphereGeometry, PxPlaneGeometry, PxCapsuleGeometry, PxBoxGeometry, PxConvexMeshGeometry, PxTriangleMeshGeometry, PxHeightFieldGeometry}
		 * \li PxCapsuleGeometry vs. {PxSphereGeometry, PxPlaneGeometry, PxCapsuleGeometry, PxBoxGeometry, PxConvexMeshGeometry, PxTriangleMeshGeometry, PxHeightFieldGeometry}
		 * \li PxBoxGeometry vs. {PxSphereGeometry, PxPlaneGeometry, PxCapsuleGeometry, PxBoxGeometry, PxConvexMeshGeometry, PxTriangleMeshGeometry, PxHeightFieldGeometry}
		 * \li PxConvexMeshGeometry vs. {PxSphereGeometry, PxPlaneGeometry, PxCapsuleGeometry, PxBoxGeometry, PxConvexMeshGeometry, PxTriangleMeshGeometry, PxHeightFieldGeometry}
		 */
		static BC_PHYSICS_DLL bool sweep(const bc_shape_geometry& p_geom0,
		                                 const bc_transform& p_pose0,
		                                 const bc_shape_geometry& p_geom1,
		                                 const bc_transform& p_pose1,
		                                 const core::bc_vector3f& p_unit_dir,
		                                 const bcFLOAT p_max_dist,
		                                 bc_sweep_hit& p_hit,
		                                 bc_hit_flag p_flags,
		                                 const bcFLOAT p_inflation = 0.f);

		/**
		 * \brief Overlap test for two geometry objects.
		 * \param[in] p_geom0 The first geometry object
		 * \param[in] p_pose0 Pose of the first geometry object
		 * \param[in] p_geom1 The second geometry object
		 * \param[in] p_pose1 Pose of the second geometry object
		 * \return True if the two geometry objects overlap

		 * All combinations are supported except:
		 * \li PxPlaneGeometry vs. {PxPlaneGeometry, PxTriangleMeshGeometry, PxHeightFieldGeometry}
		 * \li PxTriangleMeshGeometry vs. {PxTriangleMeshGeometry, PxHeightFieldGeometry}
		 * \li PxHeightFieldGeometry vs. {PxHeightFieldGeometry}
		 */
		static BC_PHYSICS_DLL bool overlap(const bc_shape_geometry& p_geom0,
		                                   const bc_transform& p_pose0,
		                                   const bc_shape_geometry& p_geom1,
		                                   const bc_transform& p_pose1);

		/**
		 * \brief Compute minimum translational distance (MTD) between two geometry objects.
		 * The function returns a unit vector ('direction') and a penetration depth ('depth').
		 * The penetration vector D = direction * depth should be applied to the first object, to get out of the second object.
		 * Returned depth should always be positive or null.
		 * If objects do not overlap, the function can not compute the MTD and returns false.
		 * \param[in] p_geom0 The first geometry object
		 * \param[in] p_pose0 Pose of the first geometry object
		 * \param[in] p_geom1 The second geometry object
		 * \param[in] p_pose1 Pose of the second geometry object
		 * \param[out] p_direction Computed MTD unit direction
		 * \param[out] p_depth Penetration depth. Always positive or null.
		 * \return True if the MTD has successfully been computed, i.e. if objects do overlap.

		 * All combinations of geom objects are supported except:
		 * - plane/plane
		 * - plane/mesh
		 * - plane/height-field
		 * - mesh/mesh
		 * - mesh/heightfield
		 * - height-field/height-field
		 */
		static BC_PHYSICS_DLL bool compute_penetration(const bc_shape_geometry& p_geom0,
		                                               const bc_transform& p_pose0,
		                                               const bc_shape_geometry& p_geom1,
		                                               const bc_transform& p_pose1,
		                                               core::bc_vector3f& p_direction,
		                                               bcFLOAT& p_depth);

		/**
		 * \brief Computes distance between a point and a geometry object.
		 * Currently supported geometry objects: box, sphere, capsule, convex.
		 * \param[in] p_point The point P
		 * \param[in] p_geom The geometry object
		 * \param[in] p_pose Pose of the geometry object
		 * \param[out] p_closest_point Optionally returned closest point to P on the geom object. Only valid when returned distance is strictly positive.
		 * \return Square distance between the point and the geom object, or 0.0 if the point is inside the object, or -1.0 if the geometry type is not supported.
		 */
		static BC_PHYSICS_DLL bcFLOAT point_distance(const core::bc_vector3f& p_point,
		                                             const bc_shape_geometry& p_geom,
		                                             const bc_transform& p_pose,
		                                             core::bc_vector3f* p_closest_point = nullptr);

		/**
		 * \brief get the bounds for a geometry object
		 * \param[in] p_geom The geometry object
		 * \param[in] p_pose Pose of the geometry object
		 * \param[in] p_inflation  Scale factor for computed world bounds. Box extents are multiplied by this value.
		 * \return The bounds of the object
		 */
		static BC_PHYSICS_DLL bc_bound_box get_world_bounds(const bc_shape_geometry& p_geom,
		                                                    const bc_transform& p_pose,
		                                                    bcFLOAT p_inflation = 1.01f);
	};
}
