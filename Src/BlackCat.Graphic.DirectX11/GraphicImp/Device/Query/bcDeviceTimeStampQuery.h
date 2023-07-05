// [22/04/2022 MRB]

#pragma once

#include "GraphicImp/bcDeviceRef.h"
#include "GraphicImp/bcDeviceReference.h"

#include "Graphic/Device/Query/bcDeviceTimeStampQuery.h"
#include "GraphicImp/GraphicImpPCH.h"

namespace black_cat::graphic
{
	template<>
	struct bc_platform_device_timestamp_query_pack<g_api_dx11> : public bc_platform_device_reference_pack<g_api_dx11>
	{
		ID3D11Query* m_query;
	};
}
