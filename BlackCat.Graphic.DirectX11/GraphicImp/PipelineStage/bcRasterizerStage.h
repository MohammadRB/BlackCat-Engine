// [02/09/2016 MRB]

#pragma once

#include "Graphic/PipelineStage/bcRasterizerStage.h"
#include "GraphicImp/GraphicImpPCH.h"

namespace black_cat
{
	namespace graphic
	{
		template< >
		struct bc_platform_rasterizer_stage_pack<g_api_dx11>
			: public bc_platform_rasterizer_stage_pack<g_api_unknown>
		{
		};
	}
}