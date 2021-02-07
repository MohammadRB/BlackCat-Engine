// [12/01/2016 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Physics/bcPhysicsApi.h"
#include "Physics/Body/bcRigidActor.h"

namespace black_cat
{
	namespace physics
	{
		enum class bc_rigid_body_flag
		{
			kinematic = core::bc_enum::value(0),
			ccd = core::bc_enum::value(2)
		};

		enum class bc_force_mode
		{
			force,
			impulse
		};

		template< bc_physics_api TApi >
		struct bc_platform_rigid_body_pack
		{
		};

		template< bc_physics_api TApi >
		class bc_platform_rigid_body : public bc_platform_rigid_actor< TApi >
		{
		public:
			using platform_pack = bc_platform_rigid_body_pack< TApi >;

		public:
			bc_platform_rigid_body() noexcept;

			bc_platform_rigid_body(const bc_platform_rigid_body&) noexcept;

			virtual ~bc_platform_rigid_body();

			bc_platform_rigid_body& operator=(const bc_platform_rigid_body&) noexcept;

			virtual	void set_cmass_local_pose(const bc_transform& p_pose) noexcept;

			virtual bc_transform get_cmass_local_pose() const noexcept;

			virtual	bcFLOAT get_mass() const noexcept;

			/**
			 * \brief Sets the mass of a dynamic actor. The mass must be non-negative.
			 * setMass() does not update the inertial properties of the body. to change the inertia tensor use update_mass_inertia().
			 * Default: 1.0
			 * Waking: No
			 * \param p_mass 
			 */
			virtual	void set_mass(bcFLOAT p_mass) noexcept;

			virtual core::bc_vector3f get_inertia_tensor() const noexcept;

			/**
			 * \brief Sets the inertia tensor, using a parameter specified in mass space coordinates.
			 * Default: (1.0, 1.0, 1.0)
			 * Waking: No
			 * \param p_mass 
			 */
			virtual void set_inertia_tensor(const core::bc_vector3f& p_mass) noexcept;
			
			/**
			 * \brief Computation of mass properties for a rigid body actor
			 * To simulate a dynamic rigid actor, actor needs a mass and an inertia tensor.
			 * This method will set the mass, center of mass, and inertia tensor.
			 * if no collision shapes are found, the inertia tensor is set to (1,1,1) and the mass to 1.
			 * if massLocalPose is non-NULL, the rigid body's center of mass parameter  will be set
			 * to the user provided value (massLocalPose) and the inertia tensor will be resolved at that point.
			 * \param[in] p_shape_densities The per shape densities. There must be one entry for each shape which has the PxShapeFlag::eSIMULATION_SHAPE set (or for all shapes if includeNonSimShapes is set to true). Other shapes are ignored. The density values must be greater than 0.
			 * \param[in] p_shape_density_count The number of provided density values.
			 * \param[in] p_mass_local_pose The center of mass relative to the actor frame.  If set to null then (0,0,0) is assumed.
			 * \return Boolean. True on success else false.
			 */
			virtual bool update_mass_inertia(const bcFLOAT* p_shape_densities, bcUINT32 p_shape_density_count, const core::bc_vector3f* p_mass_local_pose = nullptr) noexcept;
			
			/**
			 * \brief Computation of mass properties for a rigid body actor
			 * \param[in] p_density The density of the body. Used to compute the mass of the body. The density must be greater than 0.
			 * \param[in] p_mass_local_pose The center of mass relative to the actor frame.  If set to null then (0,0,0) is assumed.
			 * \return Boolean. True on success else false.
			 */
			virtual bool update_mass_inertia(bcFLOAT p_density, const core::bc_vector3f* p_mass_local_pose = nullptr) noexcept;

			virtual	core::bc_vector3f get_linear_velocity() const noexcept;

			/**
			 * \brief Sets the linear velocity of the actor.
			 * It is invalid to use this method if simulation is disabled for this actor.
			 * Default: (0.0, 0.0, 0.0)
			 * Waking: Yes
			 * \param p_velocity 
			 */
			virtual	void set_linear_velocity(const core::bc_vector3f& p_velocity) noexcept;

			virtual	core::bc_vector3f get_angular_velocity() const noexcept;

			/**
			 * \brief Sets the angular velocity of the actor.
			 * It is invalid to use this method if simulation is disabled for this actor.
			 * Default: (0.0, 0.0, 0.0)
			 * Waking: Yes
			 * \param p_velocity 
			 */
			virtual void set_angular_velocity(const core::bc_vector3f& p_velocity);

			/**
			 * \brief Applies a force (or impulse) defined in the global coordinate frame to the actor at its center of mass.
			 * This will not induce a torque.
			 * It is invalid to use this method if simulation is disabled for this actor.
			 * Waking: Yes.
			 * \param p_force 
			 * \param p_mode 
			 */
			virtual	void add_force(const core::bc_vector3f& p_force, bc_force_mode p_mode = bc_force_mode::force) noexcept;

			virtual	void add_force_at_pose(const core::bc_vector3f& p_force, const core::bc_vector3f& p_pose, bc_force_mode p_mode = bc_force_mode::force) noexcept;

			virtual	void add_force_at_local_pose(const core::bc_vector3f& p_force, const core::bc_vector3f& p_pose, bc_force_mode p_mode = bc_force_mode::force) noexcept;

			virtual	void add_local_force_at_pose(const core::bc_vector3f& p_force, const core::bc_vector3f& p_pose, bc_force_mode p_mode = bc_force_mode::force) noexcept;

			virtual	void add_local_force_at_local_pose(const core::bc_vector3f& p_force, const core::bc_vector3f& p_pose, bc_force_mode p_mode = bc_force_mode::force) noexcept;

			/**
			 * \brief Applies an impulsive torque defined in the global coordinate frame to the actor.
			 * It is invalid to use this method if simulation is disabled for this actor.
			 * Waking: Yes.
			 * \param p_torque 
			 * \param p_mode 
			 */
			virtual void add_torque(const core::bc_vector3f& p_torque, bc_force_mode p_mode = bc_force_mode::force) noexcept;

			/**
			 * \brief Clears the accumulated forces (sets the accumulated force back to zero).
			 * It is invalid to use this method if simulation is disabled for this actor.
			 */
			virtual void clear_force(bc_force_mode p_mode = bc_force_mode::force) noexcept;

			/**
			 * \brief Clears the impulsive torque defined in the global coordinate frame to the actor.
			 * It is invalid to use this method if simulation is disabled for this actor.
			 */
			virtual	void clear_torque(bc_force_mode p_mode = bc_force_mode::force) noexcept;

			virtual bc_rigid_body_flag get_rigid_body_flags() const noexcept;

			virtual void set_rigid_body_flags(bc_rigid_body_flag p_flag, bool p_value) noexcept;

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		private:
			platform_pack m_pack;
		};

		using bc_rigid_body = bc_platform_rigid_body< g_current_physics_api >;
		using bc_rigid_body_ref = bc_physics_ref<bc_rigid_body>;
	}
}