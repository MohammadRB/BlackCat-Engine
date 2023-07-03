// [15/04/2017 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Mesh/bcHeightMap.h"

#include <utility>

namespace black_cat
{
	namespace game
	{
		bc_height_map::bc_height_map(bcUINT16 p_width,
			bcUINT16 p_height,
			bcUINT16 p_xz_multiplier,
			bcFLOAT p_y_multiplier,
			bcFLOAT p_physics_y_scale,
			bc_render_state_ptr p_render_state,
			graphic::bc_buffer_ref p_vertex_buffer,
			graphic::bc_buffer_ref p_index_buffer,
			core::bc_vector<bc_height_map_material> p_materials,
			physics::bc_height_field_ref p_px_height_map,
			void* p_px_height_map_deserialize_buffer)
			: m_width(p_width),
			m_height(p_height),
			m_xz_multiplier(p_xz_multiplier),
			m_y_multiplier(p_y_multiplier),
			m_physics_y_scale(p_physics_y_scale),
			m_render_state(std::move(p_render_state)),
			m_vertex_buffer(std::move(p_vertex_buffer)),
			m_index_buffer(std::move(p_index_buffer)),
			m_px_height_map(std::move(p_px_height_map)),
			m_px_height_map_deserialize_buffer(p_px_height_map_deserialize_buffer),
			m_materials(std::move(p_materials))
		{
		}

		bc_height_map::bc_height_map(bc_height_map&& p_other) noexcept
		{
			operator=(std::move(p_other));
		}

		bc_height_map::~bc_height_map()
		{
			if (m_px_height_map_deserialize_buffer)
			{
				m_px_height_map.reset();
				BC_ALIGNED_FREE(m_px_height_map_deserialize_buffer);
			}
		}

		bc_height_map& bc_height_map::operator=(bc_height_map&& p_other) noexcept
		{
			m_width = p_other.m_width;
			m_height = p_other.m_height;
			m_xz_multiplier = p_other.m_xz_multiplier;
			m_y_multiplier = p_other.m_y_multiplier;
			m_physics_y_scale = p_other.m_physics_y_scale;
			m_render_state = std::move(p_other.m_render_state);
			m_vertex_buffer = std::move(p_other.m_vertex_buffer);
			m_index_buffer = std::move(p_other.m_index_buffer);
			m_px_height_map = std::move(p_other.m_px_height_map);
			m_px_height_map_deserialize_buffer = p_other.m_px_height_map_deserialize_buffer;
			m_materials = std::move(p_other.m_materials);
			p_other.m_px_height_map_deserialize_buffer = nullptr;

			return *this;
		}
	}
}