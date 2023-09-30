// [29/11/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Core/Utility/bcEnumOperand.h"
#include "Core/Math/bcVector3f.h"
#include "Physics/bcPhysicsApi.h"
#include "Physics/bcUtility.h"
#include "Physics/bcPhysicsReference.h"
#include "Physics/bcPhysicsRef.h"
#include "Physics/Shape/bcBoundBox.h"

namespace black_cat::physics
{
	struct bc_convex_mesh_polygon
	{
		bc_convex_mesh_polygon(bcUINT16 p_num_vertices, bcUINT16 p_index_base)
			: m_num_vertices(p_num_vertices),
			  m_index_base(p_index_base)
		{
		}

		const bcUINT16 m_num_vertices;
		const bcUINT16 m_index_base;
	};

	enum class bc_convex_mesh_flag
	{
		//use_16bit_index = core::bc_enum::value(0),	// Use 16 bit indices in vertices of convex mesh
		//compute_convex = core::bc_enum::value(1)	// Flag to indicate convex mesh must be computed
	};

	struct bc_convex_mesh_desc
	{
		bc_convex_mesh_desc(const bc_bounded_strided_typed_data<core::bc_vector3f>& p_points,
		                    bcUINT16 p_vertex_limit = 256,
		                    bc_convex_mesh_flag p_flags = core::bc_enum::none<bc_convex_mesh_flag>()/*core::bc_enum::or({ bc_convex_mesh_flag::use_16bit_index })*/)
			: m_points(p_points),
			  m_vertex_limit(p_vertex_limit),
			  m_flags(p_flags)
		{
		}

		/**
		 * \brief Vertex positions data in bc_vector3f format.
		 */
		const bc_bounded_strided_typed_data<core::bc_vector3f> m_points;
		/**
		 * \brief Limits the number of vertices of the result convex mesh. Hard maximum limit is 256 and minimum limit is 4. 
		 */
		const bcUINT16 m_vertex_limit;
		const bc_convex_mesh_flag m_flags;
	};

	template< bc_physics_api TApi >
	struct bc_platform_convex_mesh_pack
	{
	};

	template< bc_physics_api TApi >
	class bc_platform_convex_mesh : public bc_platform_physics_reference<TApi>
	{
	public:
		using platform_pack = bc_platform_convex_mesh_pack< TApi >;
		constexpr static bcUINT16 s_polygon_limit = platform_pack::s_polygon_limit;

	public:
		bc_platform_convex_mesh() noexcept;

		explicit bc_platform_convex_mesh(platform_pack& p_pack) noexcept;

		bc_platform_convex_mesh(const bc_platform_convex_mesh&) noexcept;

		~bc_platform_convex_mesh() override;

		bc_platform_convex_mesh& operator=(const bc_platform_convex_mesh&) noexcept;

		bcUINT32 get_num_vertices() const noexcept;

		bcUINT32 get_num_polygons() const noexcept;

		bc_bounded_strided_data get_vertices() const;

		bc_strided_data get_indices() const;

		bc_convex_mesh_polygon get_polygon(bcINT32 p_index) const;

		/**
		 * \brief Returns the local-space (vertex space) AABB from the convex mesh.
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

	using bc_convex_mesh = bc_platform_convex_mesh< g_current_physics_api >;
	using bc_convex_mesh_ref = bc_physics_ref< bc_convex_mesh >;
}
