// [11/30/2016 MRB]

#pragma once

#include "Physics/bcPhysicsApi.h"
#include "Physics/bcUtility.h"
#include "Physics/Shape/bcConvexMesh.h"
#include "Physics/Shape/bcShapeGeometry.h"

namespace black_cat
{
	namespace physics
	{
		template<bc_physics_api TApi>
		struct bc_platform_shape_convex_mesh_pack
		{
		};

		template<bc_physics_api TApi>
		class bc_platform_shape_convex_mesh : public bc_platform_shape_geometry<TApi>
		{
		public:
			using platform_pack = bc_platform_shape_convex_mesh_pack<TApi>;

		public:
			bc_platform_shape_convex_mesh() noexcept;

			explicit bc_platform_shape_convex_mesh(platform_pack& p_pack) noexcept;

			explicit bc_platform_shape_convex_mesh(const bc_convex_mesh& p_convex) noexcept;

			bc_platform_shape_convex_mesh(const bc_geometry_scale& p_scale, const bc_convex_mesh& p_convex) noexcept;

			bc_platform_shape_convex_mesh(const bc_platform_shape_convex_mesh&) noexcept;

			~bc_platform_shape_convex_mesh() override;

			bc_platform_shape_convex_mesh& operator=(const bc_platform_shape_convex_mesh&) noexcept;

			bc_geometry_scale get_scale() const noexcept;

			bc_convex_mesh get_convex() const noexcept;

			bc_shape_type get_type() const noexcept override
			{
				return bc_shape_type::convex_mesh;
			}

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

		using bc_shape_convex_mesh = bc_platform_shape_convex_mesh<g_current_physics_api>;
	}
}