// [04/15/2017 MRB]

#include "Game/GamePCH.h"
#include "Game/bcExport.h"
#include "Game/Object/Mesh/bcHeightMap.h"

namespace black_cat
{
	namespace game
	{
		bc_height_map::bc_height_map(const core::bc_vector3f& p_position,
			bcUINT16 p_width,
			bcUINT16 p_height,
			bcUINT16 p_xz_multiplier,
			bcFLOAT p_y_multiplier,
			bc_render_state_ptr p_render_state,
			graphic::bc_buffer_ptr p_vertex_buffer,
			graphic::bc_buffer_ptr p_index_buffer,
			physics::bc_height_field_ref p_px_height_map,
			void* p_px_height_map_deserialize_buffer)
			: m_position(p_position),
			m_width(p_width),
			m_height(p_height),
			m_xz_multiplier(p_xz_multiplier),
			m_y_multiplier(p_y_multiplier),
			m_render_state(p_render_state),
			m_vertex_buffer(std::move(p_vertex_buffer)),
			m_index_buffer(std::move(p_index_buffer)),
			m_px_height_map(std::move(p_px_height_map)),
			m_px_height_map_deserialize_buffer(p_px_height_map_deserialize_buffer)
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
				bcAlignedFree(m_px_height_map_deserialize_buffer);
			}
		}

		bc_height_map& bc_height_map::operator=(bc_height_map&& p_other) noexcept
		{
			m_position = p_other.m_position;
			m_width = p_other.m_width;
			m_height = p_other.m_height;
			m_xz_multiplier = p_other.m_xz_multiplier;
			m_y_multiplier = p_other.m_y_multiplier;
			m_render_state = std::move(p_other.m_render_state);
			m_vertex_buffer = std::move(p_other.m_vertex_buffer);
			m_index_buffer = std::move(p_other.m_index_buffer);
			m_px_height_map = std::move(p_other.m_px_height_map);
			m_px_height_map_deserialize_buffer = p_other.m_px_height_map_deserialize_buffer;
			p_other.m_px_height_map_deserialize_buffer = nullptr;

			return *this;
		}
	}
}