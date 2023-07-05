// [22/01/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Core/Container/bcString.h"

namespace black_cat::graphic
{
	struct bc_device_info
	{
		core::bc_wstring m_name;
	};

	struct bc_device_pipeline_statistic
	{
		bcUINT64 m_ia_vertices;
		bcUINT64 m_ia_primitives;
		bcUINT64 m_vs_invocations;
		bcUINT64 m_hs_invocations;
		bcUINT64 m_ds_invocations;
		bcUINT64 m_gs_invocations;
		bcUINT64 m_gs_primitives;
		bcUINT64 m_ps_invocations;
		bcUINT64 m_cs_invocations;
		bcUINT64 m_r_invocations; // Number of primitives sent to the rasterizer
		bcUINT64 m_r_primitives;
	};
}
