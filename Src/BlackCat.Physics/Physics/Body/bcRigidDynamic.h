// [12/01/2016 MRB]

#pragma once

#include "Physics/bcPhysicsApi.h"
#include "Physics/Fundation/bcTransform.h"
#include "Physics/Body/bcRigidBody.h"

namespace black_cat
{
	namespace physics
	{
		template< bc_physics_api TApi >
		struct bc_platform_rigid_dynamic_pack
		{
		};

		template< bc_physics_api TApi >
		class bc_platform_rigid_dynamic : public bc_platform_rigid_body< TApi >
		{
		public:
			using platform_pack = bc_platform_rigid_dynamic_pack< TApi >;

		public:
			bc_platform_rigid_dynamic();

			bc_platform_rigid_dynamic(const bc_platform_rigid_dynamic&) noexcept;

			~bc_platform_rigid_dynamic();

			bc_platform_rigid_dynamic& operator=(const bc_platform_rigid_dynamic&) noexcept;

			/**
			 * \brief Moves kinematically controlled dynamic actors through the game world.
			 * The move command will result in a velocity that will move the body into
			 * the desired pose. After the move is carried out during a single time step,
			 * the velocity is returned to zero. Thus, you must continuously call
			 * this in every time step for kinematic actors so that they move along a path.
			 * This function simply stores the move destination until the next simulation
			 * step is processed, so consecutive calls will simply overwrite the stored target variable.
			 * It is invalid to use this method if simulation is disabled for this actor.
			 * Waking: Yes.
			 */
			void set_kinematic_target(const bc_transform& p_destination) noexcept;

			/**
			 * \brief Get target pose of a kinematically controlled dynamic actor.
			 * \param[out] p_target Transform to write the target pose to. Only valid if the method returns true.
			 * \return True if the actor is a kinematically controlled dynamic and the target has been set, else False.
			*/
			bool get_kinematic_target(bc_transform& p_target) noexcept;

			/**
			 * \brief Sets the linear damping coefficient.
			 * Default: 0.0
			 */
			void set_linear_damping(bcFLOAT p_damp) noexcept;

			/**
			 * \brief Retrieves the linear damping coefficient.
			 */
			bcFLOAT get_linear_damping() const noexcept;

			/**
			 * \brief Sets the angular damping coefficient.
			 * Default: 0.05
			 */
			void set_angular_damping(bcFLOAT p_damp) noexcept;

			/**
			 * \brief Retrieves the angular damping coefficient.
			 */
			bcFLOAT	get_angular_damping() const noexcept;

			/**
			 * \brief Returns true if this body is sleeping.
			 * In general, a dynamic rigid actor is guaranteed to be awake if at least one of the following holds:
			 * \li The wake counter is positive (see #setWakeCounter()).
			 * \li The linear or angular velocity is non-zero.
			 * \li A non-zero force or torque has been applied.
			 * If a dynamic rigid actor is sleeping, the following state is guaranteed:
			 * \li The wake counter is zero.
			 * \li The linear and angular velocity is zero.
			 * \li There is no force update pending.
			 * A kinematic actor is asleep unless a target pose has been set (in which case it will stay awake 
			 * until the end of the next simulation step where no target pose has been set anymore).
			 * \return True if the actor is sleeping.
			 */
			bool is_sleeping() const noexcept;

			/**
			 * \brief Wakes up the actor if it is sleeping.
			 * The actor will get woken up and might cause other touching actors to wake up as well during the next simulation step.
			 * It is invalid to use this method if simulation is disabled for this actor.
			 * It is invalid to use this method for kinematic actors since the sleep state for kinematics is defined 
			 * based on whether a target pose has been set (see the comment in #isSleeping()).
			 */
			void wake_up() noexcept;
			
			/**
			 * \brief Forces the actor to sleep.
			 * The actor will stay asleep during the next simulation step if not touched by another non-sleeping actor.
			 * Any applied force will be cleared and the velocity and the wake counter of the actor will be set to 0.
			 * It is invalid to use this method if simulation is disabled for this actor.
			 * It is invalid to use this method for kinematic actors since the sleep state for kinematics is defined
			 * based on whether a target pose has been set (see the comment in #isSleeping()).
			 */
			void put_to_sleep() noexcept;

			/**
			 * \brief Retrieves the force threshold for contact reports.
			 * Default: MAX_F32
			 * \return Force threshold for contact reports.
			 */
			bcFLOAT get_contact_report_threshold() const noexcept;

			/**
			 * \brief Sets the force threshold for contact reports.
			 * \param[in] p_threshold Force threshold for contact reports.
			 */
			void set_contact_report_threshold(bcFLOAT p_threshold) noexcept;

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		protected:

		private:
			platform_pack m_pack;
		};

		using bc_rigid_dynamic = bc_platform_rigid_dynamic< g_current_physics_api >;
		using bc_rigid_dynamic_ref = bc_physics_ref<bc_rigid_dynamic>;
	}
}