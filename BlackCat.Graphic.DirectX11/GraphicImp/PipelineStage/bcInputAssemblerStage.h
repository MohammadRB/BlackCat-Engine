// [02/09/2016 MRB]

#pragma once

#include "GraphicImp/Resource/Buffer/bcBuffer.h"

#include "Graphic/PipelineStage/bcInputAssemblerStage.h"
#include "GraphicImp/GraphicImpPCH.h"

namespace black_cat
{
	namespace graphic
	{
		template< >
		struct bc_platform_input_assembler_stage_pack<g_api_dx11>
			: public bc_platform_input_assembler_stage_pack<g_api_unknown>
		{
		};
	}
}