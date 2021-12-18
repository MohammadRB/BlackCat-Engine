// [02/12/2016 MRB]

#pragma once

#include "Graphic/bcRenderApi.h"

namespace black_cat
{
	namespace graphic
	{
		template<bc_render_api TRenderApi>
		struct bc_platform_texture_config_pack
		{
		};

		template<bc_render_api TRenderApi>
		class bc_platform_texture_config
		{
		public:
			using platform_pack = bc_platform_texture_config_pack<TRenderApi>;

			bcUINT32 get_width() const;

			bcUINT32 get_height() const;

			bc_resource_usage get_usage() const;

			bc_texture_ms_config get_sample_count() const;

			bc_format get_format() const;

			platform_pack& get_platform_pack() noexcept
			{
				return m_pack;
			}

			const platform_pack& get_platform_pack() const  noexcept
			{
				return m_pack;
			}

		private:
			platform_pack m_pack;
		};

		using bc_texture_config = bc_platform_texture_config<g_current_render_api>;
	}
}