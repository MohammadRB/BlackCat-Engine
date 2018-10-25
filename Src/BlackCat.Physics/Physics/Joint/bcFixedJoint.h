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
		class bc_platfrom_fixed_joint : public bc_platform_joint<TApi>
		{
		public:
			using platform_pack = bc_platform_fixed_joint_pack< TApi >;

		public:
			bc_platfrom_fixed_joint() noexcept;

			bc_platfrom_fixed_joint(const bc_platfrom_fixed_joint&) noexcept;

			~bc_platfrom_fixed_joint();

			bc_platfrom_fixed_joint& operator=(const bc_platfrom_fixed_joint&) noexcept;

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}
			
		protected:

		private:
			platform_pack m_pack;
		};

		using bc_fixed_joint = bc_platfrom_fixed_joint< g_current_physics_api >;
		using bc_fixed_joint_ref = bc_physics_ref<bc_fixed_joint>;
	}
}