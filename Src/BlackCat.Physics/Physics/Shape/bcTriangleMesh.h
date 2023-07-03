// [30/11/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Core/Math/bcVector3f.h"
#include "Core/Utility/bcEnumOperand.h"
#include "Physics/bcPhysicsApi.h"
#include "Physics/bcUtility.h"
#include "Physics/bcPhysicsReference.h"
#include "Physics/bcPhysicsRef.h"
#include "Physics/Fundation/bcMaterial.h"
#include "Physics/Shape/bcBoundBox.h"

namespace black_cat
{
	namespace physics
	{
		enum class bc_triangle_mesh_flag
		{
			use_16bit_index = core::bc_enum::value(1), // Use 16 bit indices in vertices of triangle mesh
		};

		struct bc_triangle_mesh_desc
		{
			bc_triangle_mesh_desc(const bc_bounded_strided_typed_data<core::bc_vector3f>& p_points,
				const bc_bounded_strided_data& p_indices,
				bc_triangle_mesh_flag p_flags = core::bc_enum::none< bc_triangle_mesh_flag >())
				: bc_triangle_mesh_desc(p_points, p_indices, bc_bounded_strided_typed_data<bc_material_index>(), p_flags)
			{
			}

			bc_triangle_mesh_desc(const bc_bounded_strided_typed_data<core::bc_vector3f>& p_points,
				const bc_bounded_strided_data& p_indices,
				const bc_bounded_strided_typed_data<bc_material_index>& p_material_indices,
				bc_triangle_mesh_flag p_flags = core::bc_enum::none< bc_triangle_mesh_flag >())
				: m_points(p_points),
				m_indices(p_indices),
				m_material_indices(p_material_indices),
				m_flag(p_flags)
			{
			}

			const bc_bounded_strided_typed_data<core::bc_vector3f> m_points;
			const bc_bounded_strided_data m_indices;
			const bc_bounded_strided_typed_data<bc_material_index> m_material_indices;
			const bc_triangle_mesh_flag m_flag;
		};

		template< bc_physics_api TApi >
		struct bc_platform_triangle_mesh_pack
		{
		};

		template< bc_physics_api TApi >
		class bc_platform_triangle_mesh : public bc_platform_physics_reference< TApi >
		{
		public:
			using platform_pack = bc_platform_triangle_mesh_pack< TApi >;

		public:
			bc_platform_triangle_mesh() noexcept;

			explicit bc_platform_triangle_mesh(platform_pack& p_pack) noexcept;

			bc_platform_triangle_mesh(const bc_platform_triangle_mesh&) noexcept;

			~bc_platform_triangle_mesh() override;

			bc_platform_triangle_mesh& operator=(const bc_platform_triangle_mesh&) noexcept;

			bcUINT32 get_num_vertices() const noexcept;

			bcUINT32 get_num_triangles() const noexcept;

			bc_bounded_strided_data get_vertices() const;

			bc_bounded_strided_data get_indices() const;

			/**
			 * \brief Returns the local-space (vertex space) AABB from the triangle mesh.
			 * \return
			 */
			bc_bound_box get_local_bound() const;

			bool is_valid() const noexcept override;

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

		using bc_triangle_mesh = bc_platform_triangle_mesh< g_current_physics_api >;
		using bc_triangle_mesh_ref = bc_physics_ref< bc_triangle_mesh >;
	}
}