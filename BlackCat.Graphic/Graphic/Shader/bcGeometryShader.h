// [01/15/2016 MRB]

#pragma once

#include "Graphic/GraphicPCH.h"
#include "Graphic/bcRenderApi.h"
#include "Graphic/bcResourcePtr.h"
#include "Graphic/Shader/bcShader.h"
#include "Graphic/bcDeviceResourceContent.h"

namespace black_cat
{
	namespace graphic
	{
		template< bc_render_api TRenderApi >
		struct bc_platform_geometry_shader_pack
		{

		};

		template< bc_render_api TRenderApi >
		class bc_platform_geometry_shader : public bc_ishader
		{
		public:
			using platform_pack = bc_platform_geometry_shader_pack<TRenderApi>;

		public:
			bc_platform_geometry_shader();

			bc_platform_geometry_shader(bc_platform_geometry_shader&& p_other);

			~bc_platform_geometry_shader();

			bc_platform_geometry_shader& operator=(bc_platform_geometry_shader&& p_other);

			bc_shader_type get_type() const override;

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		protected:

		private:
			platform_pack m_pack;
		};

		using bc_geometry_shader = bc_platform_geometry_shader< g_current_render_api >;

		using bc_geometry_shader_ptr = bc_resource_ptr< bc_geometry_shader >;
		using bc_geometry_shader_content = bc_device_resource_content< bc_geometry_shader >;
		using bc_geometry_shader_content_ptr = core::bc_content_ptr<bc_geometry_shader_content>;
	}
}