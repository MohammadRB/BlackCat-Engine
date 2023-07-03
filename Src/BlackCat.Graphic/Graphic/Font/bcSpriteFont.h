// [08/02/2022 MRB]

#pragma once

#include "Core/Container/bcString.h"
#include "Core/Math/bcVector2f.h"
#include "Core/Math/bcVector3f.h"
#include "Graphic/bcRenderApiInfo.h"
#include "Graphic/Font/bcSpriteBatch.h"

namespace black_cat
{
	namespace graphic
	{
		template<bc_render_api TRenderApi>
		struct bc_platform_sprite_font_pack
		{
		};

		template<bc_render_api TRenderApi>
		class bc_platform_sprite_font
		{
		public:
			using platform_pack = bc_platform_sprite_font_pack<TRenderApi>;

		public:
			explicit bc_platform_sprite_font(platform_pack p_parameter) noexcept;

			bc_platform_sprite_font(bc_platform_sprite_font&&) noexcept;

			~bc_platform_sprite_font();

			bc_platform_sprite_font& operator=(bc_platform_sprite_font&&) noexcept;

			core::bc_vector2f measure_string(const bcWCHAR* p_text);

			void draw_string(bc_sprite_batch& p_batch, 
				const bcWCHAR* p_string,
				const core::bc_vector2i& p_position,
				const core::bc_vector3f& p_color = {1, 1, 1},
				bcFLOAT p_scale = 1.f);

			platform_pack& get_platform_pack() noexcept
			{
				return m_pack;
			}

			const platform_pack& get_platform_pack() const noexcept
			{
				return m_pack;
			}

		private:
			platform_pack m_pack;
		};

		using bc_sprite_font = bc_platform_sprite_font<g_current_render_api>;
	}
}