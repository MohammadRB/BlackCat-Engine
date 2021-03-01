// [03/10/2019 MRB]

#include "../bcRegister.hlsli"

SamplerState g_sam_sampler		: register(BC_RENDER_PASS_STATE_S0);

Texture2D g_tex2d_diffuse		: register(BC_RENDER_STATE_T0);
Texture2D g_tex2d_normal		: register(BC_RENDER_STATE_T1);
Texture2D g_tex2d_specular		: register(BC_RENDER_STATE_T2);

cbuffer g_decal_cbuffer			: register(BC_RENDER_STATE_CB1)
{
	float g_u0					: packoffset(c0.x);
	float g_v0					: packoffset(c0.y);
	float g_u1					: packoffset(c0.z);
	float g_v1					: packoffset(c0.w);
	float g_width				: packoffset(c1.x);
	float g_height				: packoffset(c1.y);
	uint g_group				: packoffset(c1.z);
};

struct bc_vs_input
{
	float3 m_position : POSITION0;
};

struct bc_vs_output
{
	float4 m_position : SV_POSITION;
};

struct bc_ps_output
{
	float4 m_diffuse : SV_Target0;
	float4 m_normal : SV_Target1;
};

bc_vs_output vs(bc_vs_input p_input)
{
	bc_vs_output l_output;

	l_output.m_position = mul(float4(p_input.m_position, 1), g_world_view_projection);

	return l_output;
}

bc_ps_output ps(bc_vs_output p_input)
{
	bc_ps_output l_output;

	l_output.m_diffuse = float4(1, 1, 1, 1);
	l_output.m_normal = float4(0, 0, 0, 0);
	
	return l_output;
}