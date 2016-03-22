// [01/03/2016 MRB]

#pragma once

#include "Graphic/GraphicPCH.h"
#include "Graphic/bcPlatformRenderApi.h"
#include "Graphic/bcResourcePtr.h"
#include "Graphic/Resource/bcResource.h"
#include "Graphic/Resource/Buffer/bcBufferConfig.h"

namespace black_cat
{
	namespace graphic
	{
		template<bc_platform_render_api TRenderApi>
		struct bc_platform_buffer_pack
		{
			bc_buffer_config m_config;
		};
		
		template<bc_platform_render_api TRenderApi>
		class bc_platform_buffer : public bc_iresource
		{
		public:
			using platform_pack = bc_platform_buffer_pack<TRenderApi>;

		public:
			bc_platform_buffer();

			bc_platform_buffer(bc_platform_buffer&& p_other);

			~bc_platform_buffer();

			bc_platform_buffer& operator=(bc_platform_buffer&& p_other);

			bcUINT get_byte_width() const;

			bc_resource_usage get_usage() const;

			bcUINT get_structure_byte_stride() const;

			bc_resource_type get_type() const override
			{
				return bc_resource_type::buffer;
			}

			bc_buffer_config& get_config()
			{
				return m_pack.m_config;
			}

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}
		protected:

		private:
			platform_pack m_pack;
		};

		using bc_buffer = bc_platform_buffer<g_current_platform_render_api>;

		using bc_buffer_ptr = bc_resource_ptr< bc_buffer >;
	}
}