// [20/03/2016 MRB]

#pragma once

#include "Graphic/bcRenderApi.h"
#include "Graphic/bcGraphicDefinition.h"

namespace black_cat::graphic
{
	template<bc_render_api TRenderApi>
	struct bc_platform_depth_stencil_view_config_pack
	{
	};

	template<bc_render_api TRenderApi>
	class bc_platform_depth_stencil_view_config
	{
	public:
		using platform_pack = bc_platform_depth_stencil_view_config_pack<TRenderApi>;

		platform_pack& get_platform_pack() noexcept
		{
			return m_pack;
		}

		const platform_pack& get_platform_pack() const noexcept
		{
			return m_pack;
		}

	private:
		platform_pack m_pack;
	};

	using bc_depth_stencil_view_config = bc_platform_depth_stencil_view_config<g_current_render_api>;
}
