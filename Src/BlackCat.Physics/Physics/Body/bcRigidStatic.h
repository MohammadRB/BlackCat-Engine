// [12/01/2016 MRB]

#pragma once

#include "Physics/bcPhysicsApi.h"
#include "Physics/Body/bcRigidBody.h"

namespace black_cat
{
	namespace physics
	{
		template< bc_physics_api TApi >
		struct bc_platform_rigid_static_pack
		{
		};

		template< bc_physics_api TApi >
		class bc_platform_rigid_static : public bc_platform_rigid_body< TApi >
		{
		public:
			using platform_pack = bc_platform_rigid_static_pack< TApi >;

		public:
			bc_platform_rigid_static();

			bc_platform_rigid_static(const bc_platform_rigid_static&) noexcept;

			~bc_platform_rigid_static();

			bc_platform_rigid_static& operator=(const bc_platform_rigid_static&) noexcept;

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		protected:

		private:
			platform_pack m_pack;
		};

		using bc_rigid_static = bc_platform_rigid_static< g_current_physics_api >;
		using bc_rigid_static_ref = bc_physics_ref<bc_rigid_static>;
	}
}