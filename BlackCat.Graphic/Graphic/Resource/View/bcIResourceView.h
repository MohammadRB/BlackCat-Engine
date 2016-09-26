// [02/11/2016 MRB]

#pragma once

#include "Graphic/bcPlatformRenderApi.h"
#include "Graphic/bcGraphicDefinition.h"
#include "Graphic/bcDeviceObject.h"
#include "Graphic/bcResourcePtr.h"

namespace black_cat
{
	namespace graphic
	{
		template< bc_platform_render_api TRenderApi >
		struct bc_platform_iresource_view_pack
		{

		};

		template< bc_platform_render_api TRenderApi >
		class bc_platform_iresource_view : public bc_device_object
		{
		public:
			using platform_pack = bc_platform_iresource_view_pack<TRenderApi>;

		public:
			virtual ~bc_platform_iresource_view();

			virtual bc_resource_view_type get_view_type() const = 0;

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}
			
		protected:
			bc_platform_iresource_view();

			bc_platform_iresource_view(bc_platform_iresource_view&&);

			bc_platform_iresource_view& operator=(bc_platform_iresource_view&&);

		private:
			platform_pack m_pack;
		};

		using bc_iresource_view = bc_platform_iresource_view< g_current_platform_render_api >;

		using bc_iresource_view_ptr = bc_resource_ptr< bc_iresource_view >;
	}
}
