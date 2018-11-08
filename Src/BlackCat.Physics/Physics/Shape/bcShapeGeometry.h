// [12/05/2016 MRB]

#pragma once

#include "Physics/bcPhysicsApi.h"

namespace black_cat
{
	namespace physics
	{
		enum class bc_shape_type
		{
			sphere,
			plane,
			capsule,
			box,
			convex_mesh,
			triangle_mesh,
			height_field
		};

		template<bc_physics_api TApi>
		struct bc_platform_shape_geometry_pack
		{
		};

		template<bc_physics_api TApi>
		class bc_platform_shape_geometry
		{
		public:
			using platform_pack = bc_platform_shape_geometry_pack< TApi >;

		public:
			virtual ~bc_platform_shape_geometry();

			virtual bc_shape_type get_type() const noexcept = 0;

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		protected:
			bc_platform_shape_geometry(platform_pack p_pack);

			bc_platform_shape_geometry(const bc_platform_shape_geometry&);

			bc_platform_shape_geometry& operator=(const bc_platform_shape_geometry&);

		private:
			platform_pack m_pack;
		};

		using bc_shape_geometry = bc_platform_shape_geometry<g_current_physics_api>;
	}
}