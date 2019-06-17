// [06/16/2019 MRB]

#include "..\bcRegister.hlsli"



struct bc_vs_input
{
	float3 m_position	: POSITION0;
	float2 m_texcoord	: TEXCOORD0;
	float3 m_normal		: NORMAL0;
	float3 m_tangent		: TANGENT0;
};

struct bc_vs_output
{
	float4 m_position	: SV_POSITION;
	float m_depth		: TEXCOORD0;
};

struct bc_ps_output
{
	float m_depth		: SV_TARGET0
};

bc_vs_output csm_vs(bc_vs_input p_input)
{
	bc_vs_output l_output;

	l_output.m_position = mul(float4(p_input.m_position, 1), g_world_view_projection);
	l_output.m_depth = l_output.m_position.w;

	return l_output;
}

bc_ps_output csm_ps(bc_vs_output p_input)
{
	bc_ps_output l_output;
	l_output.m_depth = p_input.m_depth;

	return l_output;
}