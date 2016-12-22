// [01/06/2016 MRB]

#pragma once

#include "Graphic/GraphicPCH.h"
#include "Graphic/bcRenderApi.h"
#include "Graphic/bcResourcePtr.h"
#include "Graphic/Resource/bcResource.h"
#include "Graphic/Resource/Texture/bcTextureConfig.h"
#include "Graphic/bcDeviceResourceContent.h"

namespace black_cat
{
	namespace graphic
	{
		template< bc_render_api TRenderApi >
		struct bc_platform_texture2d_pack
		{
			bc_texture_config m_config;
		};

		template< bc_render_api TRenderApi >
		class bc_platform_texture2d : public bc_iresource
			
		{
		public:
			using platform_pack = bc_platform_texture2d_pack<TRenderApi>;

		public:
			bc_platform_texture2d();

			bc_platform_texture2d(bc_platform_texture2d&& p_other);

			~bc_platform_texture2d();
			
			bc_platform_texture2d& operator=(bc_platform_texture2d&& p_other);

			bcUINT32 get_width() const;

			bcUINT32 get_height() const;

			bc_resource_usage get_usage() const;

			bool is_multisampled() const;

			bc_texture_ms_config get_sample_count() const;

			bc_format get_format() const;

			bc_resource_type get_type() const override
			{
				return bc_resource_type::texture_2d;
			}

			bc_texture_config& get_config()
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

		using bc_texture2d = bc_platform_texture2d< g_current_render_api >;

		using bc_texture2d_ptr = bc_resource_ptr< bc_texture2d >;
		using bc_texture2d_content = bc_device_resource_content< bc_texture2d >;
		using bc_texture2d_content_ptr = core::bc_content_ptr<bc_texture2d_content>;
	}
}
