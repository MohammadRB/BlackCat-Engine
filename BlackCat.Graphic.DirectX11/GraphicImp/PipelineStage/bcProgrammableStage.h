// [02/10/2016 MRB]

#pragma once

#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "GraphicImp/Resource/State/bcSamplerState.h"
#include "GraphicImp/Resource/View/bcShaderView.h"

#include "Graphic/PipelineStage/bcProgrammableStage.h"
#include "GraphicImp/GraphicImpPCH.h"

namespace black_cat
{
	namespace graphic
	{
		template< >
		struct bc_platform_programmable_stage_pack<g_api_dx11>
			: public bc_platform_programmable_stage_pack<g_api_unknown>
		{
		};
	}
}