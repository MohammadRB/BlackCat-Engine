// [02/11/2016 MRB]

#pragma once

#include "Graphic/bcRenderApi.h"
#include "Graphic/bcGraphicDefinition.h"
#include "Graphic/bcResourcePtr.h"
#include "Graphic/Resource/View/bcIResourceView.h"
#include "Graphic/Resource/View/bcResourceViewConfig.h"

namespace black_cat
{
	namespace graphic
	{
		template< bc_render_api TRenderApi >
		struct bc_platform_shader_view_pack
		{
			bc_resource_view_type m_type;
			bc_resource_view_config m_config;
		};

		template< bc_render_api TRenderApi >
		class bc_platform_resource_view : public bc_iresource_view
		{
		public:
			using platform_pack = bc_platform_shader_view_pack<TRenderApi>;

		public:
			bc_platform_resource_view();

			bc_platform_resource_view(bc_platform_resource_view&&);

			~bc_platform_resource_view();

			bc_platform_resource_view& operator=(bc_platform_resource_view&&);

			bc_resource_view_type get_view_type() const override;

			bc_resource_view_config& get_config()
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

		using bc_resource_view = bc_platform_resource_view< g_current_render_api >;

		using bc_resource_view_ptr = bc_resource_ptr< bc_resource_view >;
	}
}
