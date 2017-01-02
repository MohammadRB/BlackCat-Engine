// [02/11/2016 MRB]

#pragma once

#include "Graphic/bcRenderApi.h"
#include "Graphic/bcGraphicDefinition.h"
#include "Graphic/bcDeviceReference.h"
#include "Graphic/bcDeviceRef.h"

namespace black_cat
{
	namespace graphic
	{
		template< bc_render_api TRenderApi >
		struct bc_platform_iresource_view_pack
		{
		};

		template< bc_render_api TRenderApi >
		class bc_platform_iresource_view : public bc_platform_device_reference<TRenderApi>
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

			explicit bc_platform_iresource_view(platform_pack& p_pack);

			bc_platform_iresource_view(const bc_platform_iresource_view&);

			bc_platform_iresource_view& operator=(const bc_platform_iresource_view&);

		private:
			platform_pack m_pack;
		};

		using bc_iresource_view = bc_platform_iresource_view< g_current_render_api >;
	}
}