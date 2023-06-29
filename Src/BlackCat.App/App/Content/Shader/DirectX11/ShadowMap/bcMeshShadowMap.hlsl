// [06/16/2019 MRB]

#include "../bcRegister.hlsli"

struct bc_vs_input
{
	float3 m_position	: POSITION0;
	float2 m_texcoord	: TEXCOORD0;
	float3 m_normal		: NORMAL0;
	float3 m_tangent	: TANGENT0;
};

struct bc_vs_output
{
	float4 m_position	: SV_POSITION;
};

bc_vs_output vs(bc_vs_input p_input)
{
	bc_vs_output l_output;

	l_output.m_position = mul(float4(p_input.m_position, 1), g_world_view_projection);

	return l_output;
}