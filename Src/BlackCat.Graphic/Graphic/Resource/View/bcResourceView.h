// [02/11/2016 MRB]

#pragma once

#include "Graphic/bcRenderApi.h"
#include "Graphic/bcGraphicDefinition.h"
#include "Graphic/bcDeviceReference.h"
#include "Graphic/bcDeviceRef.h"
#include "Graphic/Resource/View/bcIResourceView.h"
#include "Graphic/Resource/View/bcResourceViewConfig.h"

namespace black_cat
{
	namespace graphic
	{
		template< bc_render_api TRenderApi >
		struct bc_platform_shader_view_pack
		{
		};

		template< bc_render_api TRenderApi >
		class bc_platform_resource_view : public bc_platform_iresource_view<TRenderApi>
		{
		public:
			using platform_pack = bc_platform_shader_view_pack<TRenderApi>;

		public:
			bc_platform_resource_view();

			bc_platform_resource_view(platform_pack& p_pack, bc_resource_view_type p_type);

			bc_platform_resource_view(const bc_platform_resource_view&);

			~bc_platform_resource_view();

			bc_platform_resource_view& operator=(const bc_platform_resource_view&);

			bc_resource_view_type get_view_type() const override;

			bool is_valid() const noexcept override;

			bool operator==(const bc_platform_resource_view& p_other) const noexcept;

			bool operator!=(const bc_platform_resource_view& p_other) const noexcept;

			bool operator==(std::nullptr_t) const noexcept;

			bool operator!=(std::nullptr_t) const noexcept;

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		private:
			platform_pack m_pack;
			bc_resource_view_type m_type;
		};

		using bc_resource_view = bc_platform_resource_view< g_current_render_api >;
		using bc_resource_view_ptr = bc_device_ref< bc_resource_view >;
	}
}