// [11/30/2016 MRB]

#pragma once

#include "Physics/bcUtility.h"
#include "Physics/Shape/bcTriangleMesh.h"
#include "Physics/Shape/bcShapeGeometry.h"

namespace black_cat
{
	namespace physics
	{
		template<bc_physics_api TApi>
		struct bc_platform_shape_triangle_mesh_pack
		{
		};

		template<bc_physics_api TApi>
		class bc_platform_shape_triangle_mesh : public bc_platform_shape_geometry<TApi>
		{
		public:
			using platform_pack = bc_platform_shape_triangle_mesh_pack<TApi>;

		public:
			explicit bc_platform_shape_triangle_mesh(platform_pack& p_pack);

			explicit bc_platform_shape_triangle_mesh(const bc_triangle_mesh& p_triangle);

			bc_platform_shape_triangle_mesh(const bc_geometry_scale& p_scale, const bc_triangle_mesh& p_triangle);

			bc_platform_shape_triangle_mesh(const bc_platform_shape_triangle_mesh&);

			~bc_platform_shape_triangle_mesh();

			bc_platform_shape_triangle_mesh& operator=(const bc_platform_shape_triangle_mesh&);

			bc_geometry_scale get_scale() const noexcept;

			bc_triangle_mesh get_mesh() const noexcept;

			bc_shape_type get_type() const noexcept override
			{
				return bc_shape_type::triangle_mesh;
			}

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		protected:

		private:
			platform_pack m_pack;
		};

		using bc_shape_triangle_mesh = bc_platform_shape_triangle_mesh<g_current_physics_api>;
	}
}