// [02/06/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Input/bcGlobalConfig.h"
#include "Game/Object/Mesh/bcMeshLevelOfDetail.h"

namespace black_cat
{
	namespace game
	{
		std::pair<bcUINT32, const bc_mesh*> bc_mesh_level_of_detail::get_lod_culling(const core::bc_vector3f& p_camera_position,
			const core::bc_vector3f& p_position,
			bcFLOAT p_max_view_distance) const noexcept
		{
			const auto l_lod_distance = p_max_view_distance / 3;
			const auto l_camera_distance = (p_camera_position - p_position).magnitude();
			const auto l_mesh_index = static_cast<bcUINT32>(l_camera_distance / l_lod_distance);

			if (l_mesh_index >= 3)
			{
				return std::make_pair(-1, nullptr);
			}
			
			return std::make_pair(l_mesh_index, m_meshes[l_mesh_index]);
		}

		std::pair<bcUINT32, const bc_mesh*> bc_mesh_level_of_detail::get_lod_culling(const core::bc_vector3f& p_main_camera_position,
			const core::bc_vector3f& p_render_camera_position,
			const core::bc_vector3f& p_position,
			bcFLOAT p_max_view_distance) const noexcept
		{
			const auto l_lod_distance = p_max_view_distance / 3;
			const auto l_main_camera_distance = (p_main_camera_position - p_position).magnitude();
			const auto l_main_camera_mesh_index = static_cast<bcUINT32>(l_main_camera_distance / l_lod_distance);

			if (l_main_camera_mesh_index >= 3)
			{
				return std::make_pair(-1, nullptr);
			}

			const auto l_render_camera_distance = (p_render_camera_position - p_position).magnitude();
			const auto l_render_camera_mesh_index = std::min(static_cast<bcUINT32>(l_render_camera_distance / l_lod_distance), m_count - 1);

			return std::make_pair(l_render_camera_mesh_index, m_meshes[l_render_camera_mesh_index]);
		}
	}	
}