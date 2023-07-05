// [11/02/2016 MRB]

#pragma once

#include "Graphic/bcRenderApi.h"
#include "Graphic/bcGraphicDefinition.h"
#include "Graphic/bcDeviceReference.h"
#include "Graphic/bcDeviceRef.h"

namespace black_cat::graphic
{
	template<bc_render_api TRenderApi>
	struct bci_platform_resource_view_pack
	{
	};

	template<bc_render_api TRenderApi>
	class bci_platform_resource_view : public bc_platform_device_reference<TRenderApi>
	{
	public:
		using platform_pack = bci_platform_resource_view_pack<TRenderApi>;

	public:
		virtual ~bci_platform_resource_view() override;

		virtual bc_resource_view_type get_view_type() const noexcept = 0;

		virtual platform_pack& get_resource_view_platform_pack() noexcept = 0;

		virtual const platform_pack& get_resource_view_platform_pack() const noexcept = 0;
			
	protected:
		bci_platform_resource_view() noexcept;

		explicit bci_platform_resource_view(platform_pack& p_pack) noexcept;

		bci_platform_resource_view(const bci_platform_resource_view&) noexcept;

		bci_platform_resource_view& operator=(const bci_platform_resource_view&) noexcept;
	};

	using bci_resource_view = bci_platform_resource_view<g_current_render_api>;
}
