// [15/07/2016 MRB]

#include "bcRegister.hlsli"

// == Resource ===================================================================================

// == Input / Output ==============================================================================

struct bc_vs_input
{
	float3 m_position	: POSITION0;
};

struct bc_vs_output
{
	float4 m_position	: SV_POSITION;
};

struct bc_ps_output
{
	float4 m_color		: SV_Target;
};

// == Shader =======================================================================================

bc_vs_output vs(bc_vs_input p_input)
{
	bc_vs_output l_output;

	l_output.m_position = mul(float4(p_input.m_position, 1), g_view_projection);

	return l_output;
}

bc_ps_output ps(bc_vs_output p_input)
{
	bc_ps_output l_output;

    l_output.m_color = float4(1, 1, 1, 1);

    return l_output;
}