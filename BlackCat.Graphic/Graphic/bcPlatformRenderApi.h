// [12/31/2015 MRB]

#pragma once

#include "Graphic/GraphicPCH.h"

namespace black_cat
{
	namespace graphic
	{
		enum class bc_platform_render_api
		{
			unknown,
			directx11
		};

		constexpr bc_platform_render_api g_api_unknown = bc_platform_render_api::unknown;
		constexpr bc_platform_render_api g_api_dx11 = bc_platform_render_api::directx11;
		constexpr bc_platform_render_api g_current_platform_render_api = g_api_dx11;
	}
}
