// [29/01/2021 MRB]

#pragma once

#include <algorithm>
#include <utility>
#include "Core/Math/bcVector3f.h"

namespace black_cat
{
	namespace game
	{
		class bc_mesh;
		
		class bc_mesh_level_of_detail
		{
		public:
			bc_mesh_level_of_detail(const bc_mesh* const* p_meshes, bcUINT32 p_count);

			bc_mesh_level_of_detail(bc_mesh_level_of_detail&&) noexcept = default;

			~bc_mesh_level_of_detail() = default;

			bc_mesh_level_of_detail& operator=(bc_mesh_level_of_detail&&) noexcept = default;

			bcSIZE get_mesh_count() const noexcept;

			const bc_mesh& get_mesh(bcUINT32 p_lod) const noexcept;

			std::pair<bcUINT32, const bc_mesh*> get_lod(const core::bc_vector3f& p_camera_position, 
				const core::bc_vector3f& p_position, 
				bcFLOAT p_max_view_distance) const noexcept;
			
			std::pair<bcUINT32, const bc_mesh*> get_lod_culling(const core::bc_vector3f& p_camera_position,
				const core::bc_vector3f& p_position, 
				bcFLOAT p_max_view_distance) const noexcept;

			std::pair<bcUINT32, const bc_mesh*> get_lod_culling(const core::bc_vector3f& p_main_camera_position,
				const core::bc_vector3f& p_render_camera_position,
				const core::bc_vector3f& p_position,
				bcFLOAT p_max_view_distance) const noexcept;
		
		private:
			const bc_mesh* const* m_meshes;
			bcUINT32 m_count;
		};

		inline bc_mesh_level_of_detail::bc_mesh_level_of_detail(const bc_mesh* const* p_meshes, bcUINT32 p_count)
			: m_meshes(p_meshes),
			m_count(p_count)
		{
			BC_ASSERT(p_count <= 3);
		}

		inline bcSIZE bc_mesh_level_of_detail::get_mesh_count() const noexcept
		{
			return m_count;
		}

		inline const bc_mesh& bc_mesh_level_of_detail::get_mesh(bcUINT32 p_lod) const noexcept
		{
			return *m_meshes[p_lod];
		}

		inline std::pair<bcUINT32, const bc_mesh*> bc_mesh_level_of_detail::get_lod(const core::bc_vector3f& p_camera_position,
			const core::bc_vector3f& p_position, 
			bcFLOAT p_max_view_distance) const noexcept
		{
			const auto l_lod_distance = p_max_view_distance / 3;
			const auto l_camera_distance = (p_camera_position - p_position).magnitude();
			const auto l_mesh_index = std::min(static_cast<bcUINT32>(l_camera_distance / l_lod_distance), m_count - 1);

			return std::make_pair(l_mesh_index, m_meshes[l_mesh_index]);
		}
	}
}