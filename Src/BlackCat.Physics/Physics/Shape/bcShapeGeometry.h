// [05/12/2016 MRB]

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
			using platform_pack = bc_platform_shape_geometry_pack<TApi>;

		public:
			virtual ~bc_platform_shape_geometry() = default;

			virtual bc_shape_type get_type() const noexcept = 0;

			virtual platform_pack& get_platform_pack() noexcept = 0;
			
			virtual const platform_pack& get_platform_pack() const noexcept = 0;

		protected:
			bc_platform_shape_geometry() = default;

			bc_platform_shape_geometry(const bc_platform_shape_geometry&) = default;

			bc_platform_shape_geometry& operator=(const bc_platform_shape_geometry&) = default;
		};

		using bc_shape_geometry = bc_platform_shape_geometry<g_current_physics_api>;
	}
}