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
		template< bc_physics_api TApi >
		struct bc_platform_revolute_joint_pack
		{
		};

		template< bc_physics_api TApi >
		class bc_platform_revolute_joint : public bc_platform_joint<TApi>
		{
		public:
			using platform_pack = bc_platform_revolute_joint_pack< TApi >;

		public:
			bc_platform_revolute_joint() noexcept;

			explicit bc_platform_revolute_joint(platform_pack& p_pack) noexcept;

			bc_platform_revolute_joint(const bc_platform_revolute_joint&) noexcept;

			~bc_platform_revolute_joint();

			bc_platform_revolute_joint& operator=(const bc_platform_revolute_joint&) noexcept;

			/**
			 * \brief return the angle of the joint, in the range (-Pi, Pi]
			 */
			bcFLOAT get_angle() const noexcept;

			/**
			 * \brief return the velocity of the joint
			 */
			bcFLOAT get_velocity() const noexcept;

			/**
			 * \brief Enable the allowed minimum/maximum angle for the joint along with strength and damping of the joint spring.
			 * Default 0.0f
			 * Range (-2*PI, 2*PI)
			 * \param p_limit
			 */
			void enable_limit(bc_joint_angular_limit& p_limit) noexcept;

			void disable_limit() noexcept;

			bc_joint_angular_limit get_limit() const noexcept;

			platform_pack& get_platform_pack() noexcept override
			{
				return m_pack;
			}

			const platform_pack& get_platform_pack() const noexcept override
			{
				return m_pack;
			}

		private:
			platform_pack m_pack;
		};

		using bc_revolute_joint = bc_platform_revolute_joint< g_current_physics_api >;
		using bc_revolute_joint_ref = bc_physics_ref< bc_revolute_joint >;
	}
}