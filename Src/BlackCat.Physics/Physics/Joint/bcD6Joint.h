// [12/04/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Physics/bcPhysicsApi.h"
#include "Physics/Joint/bcJoint.h"
#include "Physics/Joint/bcJointLimit.h"

namespace black_cat
{
	namespace physics
	{
		enum class bc_d6_axis
		{
			along_x = 0,		// motion along the X axix
			along_y = 1,		// motion along the Y axis
			along_z = 2,		// motion along the Z axis
			around_x = 3,		// motion around the X axis
			around_y = 4,		// motion around the Y axis
			around_z = 5,		// motion around the Z axis
		};

		enum class bc_d6_motion
		{
			locked,
			limited,
			free
		};

		template< bc_physics_api TApi >
		struct bc_platform_d6_joint_pack
		{
		};

		template< bc_physics_api TApi >
		class bc_platfrom_d6_joint : public bc_platform_joint<TApi>
		{
		public:
			using platform_pack = bc_platform_d6_joint_pack< TApi >;

		public:
			bc_platfrom_d6_joint() noexcept;

			bc_platfrom_d6_joint(const bc_platfrom_d6_joint&) noexcept;

			~bc_platfrom_d6_joint();

			bc_platfrom_d6_joint& operator=(const bc_platfrom_d6_joint&) noexcept;

			/**
			 * \brief Set the motion type around the specified axis.
			 * \param[in] p_axis the axis around which motion is specified
			 * \param[in] p_type the motion type around the specified axis
			 * Default: all degrees of freedom are locked
			 */
			void set_motion(bc_d6_axis p_axis, bc_d6_motion p_type) noexcept;

			/**
			 * \brief Get the motion type around the specified axis.
			 * \param[in] p_axis the degree of freedom around which the motion type is specified
			 * \return the motion type around the specified axis
			 */
			bc_d6_axis get_motion(bc_d6_axis p_axis) const noexcept;

			/**
			 * \brief get the twist angle of the joint
			 */
			bcFLOAT get_x_angle() const noexcept;

			/**
			 * \brief get the swing angle of the joint from the Y axis
			 */
			bcFLOAT get_y_angle() const noexcept;

			/**
			 * \brief get the swing angle of the joint from the Z axis
			 */
			bcFLOAT get_z_angle() const noexcept;

			/**
			 * \brief Set the linear limit for the joint along with strength and damping of the joint spring.
			 * Default MAX_F32
			 * Range (0, MAX_F32)
			 * \param p_limit
			 */
			void set_linear_limit(const bc_joint_linear_limit& p_limit);

			/**
			 * \brief Get the linear limit for the joint.
			 * \return the allowed minimum distance
			 */
			bc_joint_linear_limit get_linear_limit() const noexcept;

			/**
			 * \brief Set the twist limit for the joint.
			 * The twist limit controls the range of motion around the twist axis.
			 * Default PI/2
			 * Range (-2*PI, 2*PI)
			 * \param p_limit
			 */
			void set_twist_limit(bc_joint_angular_limit& p_limit) noexcept;

			/**
			 * \brief
			 * \return
			 */
			bc_joint_angular_limit get_twist_limit() const noexcept;

			/**
			 * \brief Enable the the limit cone for the joint along with strength and damping of the joint spring.
			 * Default PI/2
			 * Range (0,PI)
			 * \param p_limit
			 */
			void set_swing_limit(bc_joint_cone_limit& p_limit) noexcept;

			/**
			 * \brief
			 * \return
			 */
			bc_joint_cone_limit get_swing_limit() const noexcept;

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		protected:

		private:
			platform_pack m_pack;
		};

		using bc_d6_joint = bc_platfrom_d6_joint< g_current_physics_api >;
		using bc_d6_joint_ref = bc_physics_ref<bc_d6_joint>;
	}
}