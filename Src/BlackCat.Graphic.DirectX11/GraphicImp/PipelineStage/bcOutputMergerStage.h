// [09/02/2016 MRB]

#pragma once

#include "GraphicImp/PipelineStage/bcPixelStage.h"
#include "GraphicImp/Resource/View/bcResourceView.h"
#include "GraphicImp/Resource/View/bcRenderTargetView.h"
#include "GraphicImp/Resource/View/bcDepthStencilView.h"

#include "Graphic/PipelineStage/bcOutputMergerStage.h"
#include "GraphicImp/GraphicImpPCH.h"

namespace black_cat
{
	namespace graphic
	{
		template< >
		struct bc_platform_output_merger_stage_pack<g_api_dx11>
		{
		};
	}
}