// [02/09/2016 MRB]

#pragma once

#include "GraphicImp/Resource/Buffer/bcBuffer.h"

#include "Graphic/PipelineStage/bcStreamOutputStage.h"
#include "GraphicImp/GraphicImpPCH.h"

namespace black_cat
{
	namespace graphic
	{
		template< >
		struct bc_platform_stream_output_stage_pack<g_api_dx11>
		{
		};
	}
}