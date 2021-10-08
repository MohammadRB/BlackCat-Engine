// [02/06/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Input/bcGlobalConfig.h"
#include "Game/Object/Mesh/bcMeshLevelOfDetail.h"

namespace black_cat
{
	namespace game
	{
		std::pair<bcUINT32, const bc_mesh*> bc_mesh_level_of_detail::get_lod_culling(const core::bc_vector3f& p_main_camera_position,
			const core::bc_vector3f& p_render_camera_position,
			const core::bc_vector3f& p_position,
			bcFLOAT p_lod_factor) const noexcept
		{
			const auto l_main_camera_distance = (p_main_camera_position - p_position).magnitude();
			const auto l_main_camera_mesh_index = static_cast<bcUINT32>(l_main_camera_distance / p_lod_factor);

			if (l_main_camera_mesh_index > bc_get_global_config().get_lod_culling_index())
			{
				return std::make_pair(-1, nullptr);
			}

			const auto l_render_camera_distance = (p_render_camera_position - p_position).magnitude();
			const auto l_render_camera_mesh_index = std::min(static_cast<bcUINT32>(l_render_camera_distance / p_lod_factor), m_count - 1);

			return std::make_pair(l_render_camera_mesh_index, m_meshes[l_render_camera_mesh_index]);
		}
	}	
}