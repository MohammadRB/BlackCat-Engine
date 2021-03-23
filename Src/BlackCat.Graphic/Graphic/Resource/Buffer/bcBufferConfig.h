// [02/12/2016 MRB]

#pragma once

#include "Graphic/bcRenderApi.h"
#include "Graphic/bcGraphicDefinition.h"

namespace black_cat
{
	namespace graphic
	{
		template< bc_render_api TRenderApi >
		struct bc_platform_buffer_config_pack
		{
			bc_resource_type m_type;
		};

		template< bc_render_api TRenderApi >
		class bc_platform_buffer_config
		{
		public:
			using platform_pack = bc_platform_buffer_config_pack< TRenderApi >;

			bcUINT get_byte_width() const;

			bc_resource_usage get_usage() const;

			bcUINT get_structure_byte_stride() const;

		public:
			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		private:
			platform_pack m_pack;
		};

		using bc_buffer_config = bc_platform_buffer_config< g_current_render_api >;
	}
}