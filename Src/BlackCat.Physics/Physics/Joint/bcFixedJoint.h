// [12/04/2016 MRB]

#pragma once

#include "Physics/bcPhysicsApi.h"
#include "Physics/Joint/bcJoint.h"

namespace black_cat
{
	namespace physics
	{
		template< bc_physics_api TApi >
		struct bc_platform_fixed_joint_pack
		{
		};

		template< bc_physics_api TApi >
		class bc_platform_fixed_joint : public bc_platform_joint<TApi>
		{
		public:
			using platform_pack = bc_platform_fixed_joint_pack< TApi >;

		public:
			bc_platform_fixed_joint() noexcept;

			explicit bc_platform_fixed_joint(platform_pack& p_pack) noexcept;

			bc_platform_fixed_joint(const bc_platform_fixed_joint&) noexcept;

			~bc_platform_fixed_joint() override;

			bc_platform_fixed_joint& operator=(const bc_platform_fixed_joint&) noexcept;
		};

		using bc_fixed_joint = bc_platform_fixed_joint< g_current_physics_api >;
		using bc_fixed_joint_ref = bc_physics_ref<bc_fixed_joint>;
	}
}