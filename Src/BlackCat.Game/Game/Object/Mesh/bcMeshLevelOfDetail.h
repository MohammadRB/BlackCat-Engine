// [01/29/2021 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"

namespace black_cat
{
	namespace game
	{
		class bc_mesh_level_of_detail
		{
		public:
			bc_mesh_level_of_detail(const bc_mesh* const* p_meshes, bcUINT32 p_count);

			bc_mesh_level_of_detail(bc_mesh_level_of_detail&&) noexcept = default;

			~bc_mesh_level_of_detail() = default;

			bc_mesh_level_of_detail& operator=(bc_mesh_level_of_detail&&) noexcept = default;

			bcSIZE mesh_count() const noexcept;

			const bc_mesh& get_mesh(bcUINT32 p_lod) const noexcept;

			const bc_mesh& get_mesh(const core::bc_vector3f& p_camera_position, const core::bc_vector3f& p_position, bcFLOAT p_lod_factor) const noexcept;
		
		private:
			const bc_mesh* const* m_meshes;
			bcUINT32 m_count;
		};

		inline bc_mesh_level_of_detail::bc_mesh_level_of_detail(const bc_mesh* const* p_meshes, bcUINT32 p_count)
			: m_meshes(p_meshes),
			m_count(p_count)
		{
		}

		inline bcSIZE bc_mesh_level_of_detail::mesh_count() const noexcept
		{
			return m_count;
		}

		inline const bc_mesh& bc_mesh_level_of_detail::get_mesh(bcUINT32 p_lod) const noexcept
		{
			return *m_meshes[std::min(p_lod, m_count - 1)];
		}

		inline const bc_mesh& bc_mesh_level_of_detail::get_mesh(const core::bc_vector3f& p_camera_position, const core::bc_vector3f& p_position, bcFLOAT p_lod_factor) const noexcept
		{
			const auto l_distance = (p_camera_position - p_position).magnitude();
			return get_mesh(static_cast< bcUINT32 >(l_distance / p_lod_factor));
		}
	}
}