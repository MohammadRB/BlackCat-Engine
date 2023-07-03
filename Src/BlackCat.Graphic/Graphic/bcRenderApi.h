// [31/12/2015 MRB]

#pragma once

#include "Graphic/GraphicPCH.h"

namespace black_cat
{
	namespace graphic
	{
		enum class bc_render_api
		{
			unknown = 0,
			directx11 = 1
		};

		constexpr bc_render_api g_api_unknown = bc_render_api::unknown;
		constexpr bc_render_api g_api_dx11 = bc_render_api::directx11;
		constexpr bc_render_api g_current_render_api = g_api_dx11;
	}
}
