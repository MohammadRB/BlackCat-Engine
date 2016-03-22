// [02/12/2016 MRB]

#pragma once

#include "Graphic/bcPlatformRenderApi.h"

namespace black_cat
{
	namespace graphic
	{
		template< bc_platform_render_api TRenderApi >
		struct bc_platform_texture_config_pack
		{
			
		};

		template< bc_platform_render_api TRenderApi >
		class bc_platform_texture_config
		{
		public:
			using platform_pack = bc_platform_texture_config_pack< TRenderApi >;

		public:
			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		protected:

		private:
			platform_pack m_pack;
		};

		using bc_texture_config = bc_platform_texture_config<g_current_platform_render_api>;
	}
}
