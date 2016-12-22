// [02/12/2016 MRB]

#pragma once

#include "Graphic/bcRenderApi.h"
#include "Graphic/bcGraphicDefinition.h"

namespace black_cat
{
	namespace graphic
	{
		template< bc_render_api TRenderApi >
		struct bc_platform_resource_view_config_pack
		{
			bc_resource_view_type m_type;
		};

		template< bc_render_api TRenderApi >
		class bc_platform_resource_view_config
		{
		public:
			using platform_pack = bc_platform_resource_view_config_pack< TRenderApi >;

		public:
			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		protected:

		private:
			platform_pack m_pack;
		};

		using bc_resource_view_config = bc_platform_resource_view_config< g_current_render_api >;
	}
}
