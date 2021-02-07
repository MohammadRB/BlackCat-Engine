// [01/29/2021 MRB]

#pragma once

#include <algorithm>
#include "Core/Math/bcVector3f.h"

namespace black_cat
{
	namespace game
	{
		class bc_mesh;
		
		class bc_mesh_level_of_detail
		{
		public:
			static bcFLOAT s_lod_factor_multiplier;
			static bcUINT32 s_lod_culling_index;
			
		public:
			bc_mesh_level_of_detail(const bc_mesh* const* p_meshes, bcUINT32 p_count);

			bc_mesh_level_of_detail(bc_mesh_level_of_detail&&) noexcept = default;

			~bc_mesh_level_of_detail() = default;

			bc_mesh_level_of_detail& operator=(bc_mesh_level_of_detail&&) noexcept = default;

			bcSIZE mesh_count() const noexcept;

			const bc_mesh& get_mesh(bcUINT32 p_lod) const noexcept;

			const bc_mesh& get_mesh(const core::bc_vector3f& p_camera_position, const 
				core::bc_vector3f& p_position, 
				bcFLOAT p_lod_factor) const noexcept;
			
			const bc_mesh* get_mesh_nullable(const core::bc_vector3f& p_camera_position, 
				const core::bc_vector3f& p_position, 
				bcFLOAT p_lod_factor) const noexcept;

			const bc_mesh* get_mesh_nullable(const core::bc_vector3f& p_main_camera_position,
				const core::bc_vector3f& p_render_camera_position,
				const core::bc_vector3f& p_position,
				bcFLOAT p_lod_factor) const noexcept;
		
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

		inline const bc_mesh& bc_mesh_level_of_detail::get_mesh(const core::bc_vector3f& p_camera_position, 
			const core::bc_vector3f& p_position, 
			bcFLOAT p_lod_factor) const noexcept
		{
			const auto l_distance = (p_camera_position - p_position).magnitude();
			const auto l_mesh_index = static_cast<bcUINT32>(l_distance / p_lod_factor);
			return get_mesh(l_mesh_index);
		}

		inline const bc_mesh* bc_mesh_level_of_detail::get_mesh_nullable(const core::bc_vector3f& p_camera_position, 
			const core::bc_vector3f& p_position, 
			bcFLOAT p_lod_factor) const noexcept
		{
			const auto l_distance = (p_camera_position - p_position).magnitude();
			const auto l_mesh_index = static_cast<bcUINT32>(l_distance / p_lod_factor);

			if(l_mesh_index > s_lod_culling_index)
			{
				return nullptr;
			}
			
			return &get_mesh(l_mesh_index);
		}

		inline const bc_mesh* bc_mesh_level_of_detail::get_mesh_nullable(
			const core::bc_vector3f& p_main_camera_position,
			const core::bc_vector3f& p_render_camera_position,
			const core::bc_vector3f& p_position,
			bcFLOAT p_lod_factor) const noexcept
		{
			const auto l_main_camera_distance = (p_main_camera_position - p_position).magnitude();
			const auto l_main_camera_mesh_index = static_cast<bcUINT32>(l_main_camera_distance / p_lod_factor);

			if (l_main_camera_mesh_index > s_lod_culling_index)
			{
				return nullptr;
			}

			const auto l_render_camera_distance = (p_render_camera_position - p_position).magnitude();
			const auto l_render_camera_mesh_index = static_cast<bcUINT32>(l_render_camera_distance / p_lod_factor);

			return &get_mesh(l_render_camera_mesh_index);
		}
	}
}