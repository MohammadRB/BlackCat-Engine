// [11/28/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Math/bcVector3f.h"
#include "Physics/bcPhysicsApi.h"
#include "Physics/bcPhysicsReference.h"
#include "Physics/Fundation/bcFundation.h"
#include "Physics/Fundation/bcTransform.h"
#include "Physics/Fundation/bcPhysics.h"
#include "Physics/Fundation/bcSceneBuilder.h"
#include "Physics/Fundation/bcSceneDebug.h"
#include "Physics/Collision/bcQueryGroup.h"
#include "Physics/Collision/bcSceneQuery.h"
#include "Physics/Body/bcActor.h"

namespace black_cat
{
	namespace physics
	{
		/*template< bc_physics_api TApi >
		class bc_platform_actor;
		using bc_actor = bc_platform_actor< g_current_physics_api >;*/

		template< bc_physics_api TApi >
		class bc_platform_aggregate;
		using bc_aggregate = bc_platform_aggregate< g_current_physics_api >;

		template< bc_physics_api TApi >
		class bc_platform_shape_geometry;
		using bc_shape_geometry = bc_platform_shape_geometry< g_current_physics_api >;

		template< bc_physics_api TApi >
		class bc_platform_joint;
		using bc_joint = bc_platform_joint< g_current_physics_api >;

		enum class bc_actor_type;

		struct bc_updated_actor
		{
		public:
			bc_updated_actor(const bc_actor& p_actor, const bc_transform& p_transform)
				: m_actor(p_actor),
				m_global_pose(p_transform)
			{
			}

			bc_actor m_actor;
			bc_transform m_global_pose;
		};

		template< bc_physics_api TApi >
		struct bc_platform_scene_pack
		{
		};

		template< bc_physics_api TApi >
		class bc_platform_scene : public bc_platform_physics_reference<TApi>
		{
		public:
			using platform_pack = bc_platform_scene_pack<TApi>;
			friend bc_platform_physics<TApi>;

		public:
			bc_platform_scene();

			bc_platform_scene(const bc_platform_scene& p_other) noexcept;

			~bc_platform_scene();

			bc_platform_scene& operator=(const bc_platform_scene& p_other) noexcept;

			/**
			 * \brief Adds an actor to this scene.
			 * \note If the actor is already assigned to a scene, the call is ignored and an error is issued.
			 * \note If the actor is a PxRigidActor then each assigned Constraint object will get added to the scene automatically if
			 * it connects to another actor that is part of the scene already.
			 * \param[in] p_actor Actor to add to scene.
			 */
			void add_actor(bc_actor& p_actor);

			/**
			 * \brief Removes an actor from this scene.
			 * \note If the actor is not part of this scene (see #PxActor::getScene), the call is ignored and an error is issued.
			 * \note If the actor is a PxRigidActor then all assigned Constraint objects will get removed from the scene automatically.
			 * \note If the actor is in an aggregate it will be removed from the aggregate.
			 * \param[in] p_actor Actor to remove from scene.
			 * \param[in] wake_on_lost_touch Specifies whether touching objects from the previous frame should get woken up in the next frame. Only applies to Articulation and RigidActor types.
			 */
			void remove_actor(bc_actor& p_actor, bool wake_on_lost_touch = true);

			/**
			* \brief Retrieve the number of actors of certain types in the scene.
			* \param[in] p_types Combination of actor types.
			* \return the number of actors.
			*/
			bcUINT32 get_actors_count(bc_actor_type p_types) const noexcept;

			/**
			* \brief Retrieve an array of all the actors of certain types in the scene.
			* \param[in] p_types Combination of actor types to retrieve.
			* \param[out] p_buffer The buffer to receive actor pointers.
			* \param[in] p_buffer_size Size of provided user buffer.
			* \param p_start_index
			* \return Number of actors written to the buffer.
			*/
			bcUINT32 get_actors(bc_actor_type p_types, bc_actor* p_buffer, bcUINT32 p_buffer_size, bcUINT32 p_start_index = 0) const;

			/**
			 * \brief Adds an aggregate to this scene.
			 * \note If the aggregate is already assigned to a scene, the call is ignored and an error is issued.
			 * \note If the aggregate already contains actors, those actors are added to the scene as well.
			 * \param[in] p_aggregate Aggregate to add to scene.
			 */
			void add_aggregate(bc_aggregate& p_aggregate);

			/**
			 * \brief Removes an aggregate from this scene.
			 * \note If the aggregate is not part of this scene, the call is ignored and an error is issued.
			 * \note If the aggregate contains actors, those actors are removed from the scene as well.
			 * \param[in] p_aggregate Aggregate to remove from scene.
			 * \param[in] wake_on_lost_touch Specifies whether touching objects from the previous frame should get woken up in the next frame. Only applies to PxArticulation and PxRigidActor types.
			 */
			void remove_aggregate(bc_aggregate& p_aggregate, bool wake_on_lost_touch = true);

			/**
			* \brief Returns the number of aggregates in the scene.
			* \return the number of aggregates in this scene.
			*/
			bcUINT32 get_aggregates_count()	const noexcept;

			/**
			* \brief Retrieve all the aggregates in the scene.
			* \param[out] p_buffer The buffer to receive aggregates pointers.
			* \param[in] p_buffer_size Size of provided user buffer.
			* \param p_start_index
			* \return Number of aggregates written to the buffer.
			*/
			bcUINT32 get_aggregates(bc_aggregate* p_buffer, bcUINT32 p_buffer_size, bcUINT32 p_start_index = 0) const;

			/**
			\brief Returns the number of constraints in the scene.
			\return the number of constraints in this scene.
			*/
			bcUINT32 get_joints_count() const noexcept;

			/**
			 * \brief Retrieve all the constraints in the scene.
			 * \param[out] p_buffer The buffer to receive constraint shader pointers.
			 * \param[in] p_buffer_size Size of provided user buffer.
			 * \param p_start_index
			 * \return Number of constraints written to the buffer.
			 */
			bcUINT32 get_joints(bc_joint* p_buffer, bcUINT32 p_buffer_size, bcUINT32 p_start_index = 0) const;

			/**
			 * \brief Advances the simulation by an elapsed time.
			 * This function run simulation async and return immediately.
			 * \param p_elapsed_time Amount of time to advance simulation by.
			 */
			void update(const core_platform::bc_clock::update_param& p_elapsed_time);

			/**
			 * \brief Check if simulation has been done.
			 * \return 
			 */
			bool update_done() const noexcept;

			/**
			 * \brief Wait until simulation job done.
			 */
			void wait() const noexcept;

			/**
			 * \brief Sets a constant gravity for the entire scene.
			 * \param[in] p_gravity A new gravity vector
			 */
			void set_gravity(const core::bc_vector3f& p_gravity) noexcept;

			/**
			 * \brief Retrieves the current gravity setting.
			 * \return The current gravity for the scene.
			 */
			core::bc_vector3f get_gravity() const noexcept;

			void enable_visualization() noexcept;

			void disable_visualization() noexcept;

			/**
			 * \brief Retrieves the debug render buffer.
			 * \note Do not use this method while the simulation is running. Calls to this method while result in undefined behaviour.
			 * \return The render buffer.
			 */
			bc_scene_debug get_debug_buffer() noexcept;

			/**
			 * \brief Get updated actors through last update call
			 * \return 
			 */
			core::bc_vector_frame<bc_updated_actor> get_active_actors() noexcept;

			/**
			 * \brief Performs a ray cast against objects in the scene.
			 * \note Touching hits are not ordered.
			 * \note Shooting a ray from within an object leads to different results depending on the shape type.
			 * \param[in] p_ray	Ray to query
			 * \param[out] p_buffer	Ray cast hit buffer used to report ray cast hits.
			 * \param[in] p_hit_flags Specifies which properties per hit should be computed and returned via the hit callback.
			 * \param[in] p_query_flags	
			 * \param[in] p_query_groups Specify witch groups must be included in query.
			 * \param p_filter_callback Custom filtering logic
			 * \return True if any touching or blocking hits were found.
			 */
			bool raycast(const bc_ray& p_ray, 
				bc_scene_ray_query_buffer& p_buffer, 
				bc_hit_flag p_hit_flags = bc_hit_flag::hit_info,
				bc_query_flags p_query_flags = core::bc_enum::mask_or({ bc_query_flags::statics, bc_query_flags::dynamics}),
				bc_query_group p_query_groups = bc_query_group::all,
				bc_scene_query_post_filter_callback* p_filter_callback = nullptr) const;

			/**
			 * \brief Performs a sweep test against objects in the scene.
			 * \note Touching hits are not ordered.
			 * \note If a shape from the scene is already overlapping with the query shape in its starting position, the hit is returned.
			 * \param[in] p_geometry Geometry of object to sweep (supported types are: box, sphere, capsule, convex).
			 * \param[in] p_pose Pose of the sweep object.
			 * \param[in] p_unit_dir Normalized direction of the sweep.
			 * \param[in] p_max_dist Sweep distance. Needs to be in [0, inf) range and >0 if eASSUME_NO_INITIAL_OVERLAP was specified. Will be clamped to PX_MAX_SWEEP_DISTANCE.
			 * \param[out] p_buffer sweep hit buffer used to report sweep hits.
			 * \param[in] p_hit_flags Specifies which properties per hit should be computed and returned.
			 * \param[in] p_query_groups Specify witch groups must be included in query.
			 * \return True if any touching or blocking hits were found or any hit was found in case PxQueryFlag::eANY_HIT was specified.
			 */
			bool sweep(const bc_shape_geometry& p_geometry,
				const bc_transform& p_pose,
				const core::bc_vector3f& p_unit_dir,
				bcFLOAT p_max_dist,
				bc_scene_sweep_query_buffer& p_buffer,
				bc_hit_flag p_hit_flags = bc_hit_flag::hit_info,
				bc_query_flags p_query_flags = core::bc_enum::mask_or({ bc_query_flags::statics, bc_query_flags::dynamics }),
				bc_query_group p_query_groups = bc_query_group::all,
				bc_scene_query_post_filter_callback* p_filter_callback = nullptr) const;

			/**
			\brief Performs an overlap test of a given geometry against objects in the scene.
			\param[in] p_geometry Geometry of object to check for overlap (supported types are: box, sphere, capsule, convex).
			\param[in] p_pose Pose of the object.
			\param[out] p_buffer overlap hit buffer used to report overlap hits.
			\param[in] p_query_flags
			\param[in] p_query_groups Specify witch groups must be included in query.
			\return True if any touching or blocking hits were found.
			*/
			bool overlap(const bc_shape_geometry& p_geometry,
				const bc_transform& p_pose,
				bc_scene_overlap_query_buffer& p_buffer,
				bc_query_flags p_query_flags = core::bc_enum::mask_or({ bc_query_flags::statics, bc_query_flags::dynamics }),
				bc_query_group p_query_groups = bc_query_group::all,
				bc_scene_query_post_filter_callback* p_filter_callback = nullptr) const;

			void lock();

			void lock_shared();

			void unlock();

			void unlock_shared();

			bool is_valid() const noexcept override;

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		private:
			platform_pack m_pack;
		};

		using bc_scene = bc_platform_scene< g_current_physics_api >;
		using bc_scene_ref = bc_physics_ref<bc_scene>;
	}
}