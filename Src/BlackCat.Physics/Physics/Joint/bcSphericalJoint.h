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
		template<bc_physics_api TApi>
		struct bc_platform_spherical_joint_pack
		{
		};

		template<bc_physics_api TApi>
		class bc_platform_spherical_joint : public bc_platform_joint<TApi>
		{
		public:
			using platform_pack = bc_platform_spherical_joint_pack<TApi>;

		public:
			bc_platform_spherical_joint() noexcept;

			explicit bc_platform_spherical_joint(platform_pack& p_pack) noexcept;

			bc_platform_spherical_joint(const bc_platform_spherical_joint&) noexcept;

			~bc_platform_spherical_joint() override;

			bc_platform_spherical_joint& operator=(const bc_platform_spherical_joint&) noexcept;

			/**
			* \brief Enable the the limit cone for the joint along with strength and damping of the joint spring.
			* Default PI/2
			* Range (0,PI)
			* \param p_limit
			*/
			void enable_limit(const bc_joint_cone_limit& p_limit) noexcept;

			void disable_limit() noexcept;

			bc_joint_cone_limit get_limit() const noexcept;
		};

		using bc_spherical_joint = bc_platform_spherical_joint<g_current_physics_api>;
		using bc_spherical_joint_ref = bc_physics_ref<bc_spherical_joint>;
	}
}