// [09/13/2016 MRB]

#pragma once

#include "Core/bcConstant.h"
#include "Core/Math/bcVector3f.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "Game/System/Render/bcRenderState.h"

namespace black_cat
{
	namespace game
	{
		class bc_height_map : public core::bc_icontent
		{
			BC_CONTENT(height_map)

		public:
			bc_height_map(const core::bc_vector3f& p_position,
				bcUINT16 p_width,
				bcUINT16 p_height,
				bcUINT16 p_xz_multiplier,
				bcUINT16 p_y_multiplier,
				bc_render_state_ptr& p_render_state);

			bc_height_map(bc_height_map&& p_other) = default;

			virtual ~bc_height_map() = default;

			bc_height_map& operator=(bc_height_map&& p_other) = default;

			const core::bc_vector3f& get_position() const
			{
				return m_position;
			}

			bcUINT16 get_width() const
			{
				return m_width;
			}

			bcUINT16 get_height() const
			{
				return m_height;
			}

			bcUINT16 get_xz_multiplier() const
			{
				return m_xz_multiplier;
			}

			bcUINT16 get_y_multiplier() const
			{
				return m_y_multiplier;
			}

			const bc_render_state* get_render_state() const
			{
				return m_render_state.get();
			}

		protected:

		private:
			core::bc_vector3f m_position;
			bcUINT16 m_width;
			bcUINT16 m_height;
			bcUINT16 m_xz_multiplier;
			bcUINT16 m_y_multiplier;

			bc_render_state_ptr m_render_state;
		};

		using bc_height_map_ptr = core::bc_content_ptr< bc_height_map >;

		inline bc_height_map::bc_height_map(const core::bc_vector3f& p_position,
			bcUINT16 p_width,
			bcUINT16 p_height,
			bcUINT16 p_xz_multiplier,
			bcUINT16 p_y_multiplier,
			bc_render_state_ptr& p_render_state)
			: m_position(p_position),
			m_width(p_width),
			m_height(p_height),
			m_xz_multiplier(p_xz_multiplier),
			m_y_multiplier(p_y_multiplier),
			m_render_state(p_render_state)
		{
		}

	}
}
