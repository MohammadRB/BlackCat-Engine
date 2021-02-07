// [04/15/2017 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Mesh/bcHeightMap.h"

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
			graphic::bc_buffer_ptr p_vertex_buffer,
			graphic::bc_buffer_ptr p_index_buffer,
			physics::bc_height_field_ref p_px_height_map,
			void* p_px_height_map_deserialize_buffer)
			: m_width(p_width),
			m_height(p_height),
			m_xz_multiplier(p_xz_multiplier),
			m_y_multiplier(p_y_multiplier),
			m_physics_y_scale(p_physics_y_scale),
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
			p_other.m_px_height_map_deserialize_buffer = nullptr;

			return *this;
		}

		bcUINT16 bc_height_map::get_width() const
		{
			return m_width;
		}

		bcUINT16 bc_height_map::get_height() const
		{
			return m_height;
		}

		bcUINT16 bc_height_map::get_xz_multiplier() const
		{
			return m_xz_multiplier;
		}

		bcFLOAT bc_height_map::get_y_multiplier() const
		{
			return m_y_multiplier;
		}

		bcFLOAT bc_height_map::get_physics_y_scale() const
		{
			return m_physics_y_scale;
		}

		const bc_render_state* bc_height_map::get_render_state() const
		{
			return m_render_state.get();
		}

		bc_render_state_ptr bc_height_map::get_render_state_ptr() const
		{
			return m_render_state;
		}

		graphic::bc_buffer bc_height_map::get_vertex_buffer() const
		{
			return m_vertex_buffer.get();
		}

		graphic::bc_buffer bc_height_map::get_index_buffer() const
		{
			return m_index_buffer.get();
		}

		physics::bc_height_field bc_height_map::get_px_height_field() const
		{
			return m_px_height_map.get();
		}
	}
}