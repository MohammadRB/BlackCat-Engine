// [08/02/2022 MRB]

#pragma once

#include "Core/Math/bcVector2f.h"
#include "Graphic/Resource/State/bcBlendState.h"
#include "Graphic/bcRenderApiInfo.h"

namespace black_cat::graphic
{
	template<bc_render_api TRenderApi>
	class bc_platform_device;
	using bc_device = bc_platform_device<g_current_render_api>;

	template<bc_render_api TRenderApi>
	class bc_platform_device_pipeline;
	using bc_device_pipeline = bc_platform_device_pipeline<g_current_render_api>;

	template<bc_render_api TRenderApi>
	class bc_platform_texture2d;
	using bc_texture2d = bc_platform_texture2d<g_current_render_api>;

	template<bc_render_api TRenderApi>
	class bc_platform_resource_view;
	using bc_resource_view = bc_platform_resource_view<g_current_render_api>;

	template<bc_render_api TRenderApi>
	class bc_platform_render_target_view;
	using bc_render_target_view = bc_platform_render_target_view<g_current_render_api>;

	struct bc_sprite_rect
	{
		bcINT16 m_left;
		bcINT16 m_top;
		bcINT16 m_right;
		bcINT16 m_bottom;
	};

	template<bc_render_api TRenderApi>
	struct bc_platform_sprite_batch_pack
	{
	};

	template<bc_render_api TRenderApi>
	class bc_platform_sprite_batch
	{
	public:
		using platform_pack = bc_platform_sprite_batch_pack<TRenderApi>;

	public:
		explicit bc_platform_sprite_batch(platform_pack p_parameter) noexcept;

		bc_platform_sprite_batch(bc_platform_sprite_batch&&) noexcept;

		~bc_platform_sprite_batch();

		bc_platform_sprite_batch& operator=(bc_platform_sprite_batch&&) noexcept;

		bcSIZE create_blend_state(bc_device& p_device, const bc_blend_state_config& p_blend_state);

		void begin(bc_device_pipeline& p_device_pipeline, const bc_texture2d& p_render_texture, bc_render_target_view& p_render_target, bcSIZE p_blend_state = -1) noexcept;

		void end(bc_device_pipeline& p_device_pipeline) noexcept;
			
		void draw(bc_resource_view& p_texture, const core::bc_vector2i& p_position, const core::bc_vector2i& p_origin = { 0, 0 }, bcFLOAT p_rotation = 0);

		void draw(bc_resource_view& p_texture, const bc_sprite_rect& p_destination_rect, bcFLOAT p_rotation = 0);

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

	using bc_sprite_batch = bc_platform_sprite_batch<g_current_render_api>;
}
