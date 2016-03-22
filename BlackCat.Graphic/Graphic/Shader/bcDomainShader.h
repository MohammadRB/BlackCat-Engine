// [01/11/2016 MRB]

#pragma once

#include "Graphic/GraphicPCH.h"
#include "Graphic/bcPlatformRenderApi.h"
#include "Graphic/bcResourcePtr.h"
#include "Graphic/Shader/bcShader.h"

namespace black_cat
{
	namespace graphic
	{
		template< bc_platform_render_api TRenderApi >
		struct bc_platform_domain_shader_pack
		{

		};

		template< bc_platform_render_api TRenderApi >
		class bc_platform_domain_shader : public bc_ishader
		{
		public:
			using platform_pack = bc_platform_domain_shader_pack<TRenderApi>;

		public:
			bc_platform_domain_shader();

			bc_platform_domain_shader(bc_platform_domain_shader&& p_other);

			~bc_platform_domain_shader();

			bc_platform_domain_shader& operator=(bc_platform_domain_shader&& p_other);

			bc_shader_type get_type() const override;

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		protected:

		private:
			platform_pack m_pack;

		};

		using bc_domain_shader = bc_platform_domain_shader< g_current_platform_render_api >;

		using bc_domain_shader_ptr = bc_resource_ptr< bc_domain_shader >;
	}
}