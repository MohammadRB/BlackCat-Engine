// [09/02/2016 MRB]

#pragma once

#include "GraphicImp/Resource/Buffer/bcBuffer.h"

#include "Graphic/PipelineStage/bcInputAssemblerStage.h"
#include "GraphicImp/GraphicImpPCH.h"

namespace black_cat::graphic
{
	template< >
	struct bc_platform_input_assembler_stage_pack<g_api_dx11>
	{
	};
}
