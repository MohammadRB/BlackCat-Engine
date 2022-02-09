// [02/08/2022 MRB]

#pragma once

#include "Graphic/bcRenderApiInfo.h"

namespace black_cat
{
	namespace graphic
	{
		template<bc_render_api TRenderApi>
		class bc_platform_device;
		using bc_device = bc_platform_device<g_current_render_api>;

		template<bc_render_api TRenderApi>
		class bc_platform_device_swap_buffer;
		using bc_device_swap_buffer = bc_platform_device_swap_buffer<g_current_render_api>;

		template<bc_render_api TRenderApi>
		class bc_platform_render_target_view;
		using bc_render_target_view = bc_platform_render_target_view<g_current_render_api>;

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

			void begin(bc_device& p_device, const bc_device_swap_buffer& p_swap_buffer, bc_render_target_view& p_render_target) noexcept;

			void end(bc_device& p_device) noexcept;

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
}