// [02/11/2016 MRB]

#pragma once

#include "Graphic/bcPlatformRenderApi.h"
#include "Graphic/bcGraphicDefinition.h"
#include "Graphic/bcResourcePtr.h"
#include "Graphic/Resource/View/bcResourceView.h"
#include "Graphic/Resource/View/bcRenderTargetViewConfig.h"

namespace black_cat
{
	namespace graphic
	{
		template< bc_platform_render_api TRenderApi >
		struct bc_platform_render_target_view_pack
		{
			bc_render_target_view_config m_config;
		};

		template< bc_platform_render_api TRenderApi >
		class bc_platform_render_target_view : public bc_iresource_view
		{
		public:
			using platform_pack = bc_platform_render_target_view_pack<TRenderApi>;

		public:
			bc_platform_render_target_view();

			bc_platform_render_target_view(bc_platform_render_target_view&&);

			~bc_platform_render_target_view();

			bc_platform_render_target_view& operator=(bc_platform_render_target_view&&);

			bc_resource_view_type get_view_type() const override;

			bc_render_target_view_config& get_config()
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

		using bc_render_target_view = bc_platform_render_target_view< g_current_platform_render_api >;

		using bc_render_target_view_ptr = bc_resource_ptr<bc_render_target_view>;
	}
}