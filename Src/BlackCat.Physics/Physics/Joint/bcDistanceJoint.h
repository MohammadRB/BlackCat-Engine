// [04/12/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Physics/bcPhysicsApi.h"
#include "Physics/Joint/bcJointLimit.h"
#include "Physics/Joint/bcJoint.h"

namespace black_cat
{
	namespace physics
	{
		template<bc_physics_api TApi>
		struct bc_platform_distance_joint_pack
		{
		};

		template<bc_physics_api TApi>
		class bc_platform_distance_joint : public bc_platform_joint<TApi>
		{
		public:
			using platform_pack = bc_platform_distance_joint_pack<TApi>;

		public:
			bc_platform_distance_joint() noexcept;

			explicit bc_platform_distance_joint(platform_pack& p_pack) noexcept;

			bc_platform_distance_joint(const bc_platform_distance_joint&) noexcept;

			~bc_platform_distance_joint() override;

			bc_platform_distance_joint& operator=(const bc_platform_distance_joint&) noexcept;

			/**
			 * \brief Return the current distance of the joint
			 */
			bcFLOAT get_distance() const noexcept;

			/**
			* \brief Enable the allowed minimum/maximum distance for the joint along with strength and damping of the joint spring.
			* Default 0.0f
			* Range [0, MAX_F32)
			* \param p_limit
			*/
			void enable_limit(const bc_joint_linear_limit_pair& p_limit) noexcept;

			void disable_limit() noexcept;

			bc_joint_linear_limit_pair get_limit() const noexcept;
		};

		using bc_distance_joint = bc_platform_distance_joint<g_current_physics_api>;
		using bc_distance_joint_ref = bc_physics_ref<bc_distance_joint>;
	}
}