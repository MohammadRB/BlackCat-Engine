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
		struct bc_platform_prismatic_joint_pack
		{
		};

		template< bc_physics_api TApi >
		class bc_platform_prismatic_joint : public bc_platform_joint<TApi>
		{
		public:
			using platform_pack = bc_platform_prismatic_joint_pack<TApi>;

		public:
			bc_platform_prismatic_joint() noexcept;

			explicit bc_platform_prismatic_joint(platform_pack& p_pack) noexcept;

			bc_platform_prismatic_joint(const bc_platform_prismatic_joint&) noexcept;

			~bc_platform_prismatic_joint() override;

			bc_platform_prismatic_joint& operator=(const bc_platform_prismatic_joint&) noexcept;

			/**
			 * \brief returns the displacement of the joint along its axis.
			 */
			bcFLOAT get_position() const noexcept;

			/**
			 * \brief returns the velocity of the joint along its axis
			 */
			bcFLOAT get_velocity() const noexcept;

			/**
			 * \brief sets the joint limit parameters.
			 * The limit range is [-MAX_F32, MAX_F32], but note that the width of the limit (upper-lower) must also be
			 * a valid float.
			 */
			void enable_limit(bc_joint_linear_limit_pair& p_limit) noexcept;
			
			void disable_limit() noexcept;

			bc_joint_linear_limit_pair get_limit() const noexcept;
		};

		using bc_prismatic_joint = bc_platform_prismatic_joint< g_current_physics_api >;
		using bc_prismatic_joint_ref = bc_physics_ref<bc_prismatic_joint>;
	}
}