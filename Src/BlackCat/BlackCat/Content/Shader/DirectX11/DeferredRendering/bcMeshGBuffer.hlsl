// [03/10/2019 MRB]

#include "..\bcRegister.hlsli"

SamplerState g_sam_sampler      : register(BC_RENDER_PASS_STATE_S0);

Texture2D g_tex2d_diffuse		: register(BC_RENDER_STATE_T0);
Texture2D g_tex2d_normal		: register(BC_RENDER_STATE_T1);
Texture2D g_tex2d_specular		: register(BC_RENDER_STATE_T2);

cbuffer g_cb_material			: register(BC_RENDER_STATE_CB1)
{
	float4 g_diffuse			: packoffset(c0);
	float g_specular_intency	: packoffset(c1.x);
	float g_specular_power		: packoffset(c1.y);
};

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
	float2 m_texcoord	: TEXCOORD0;
	float3 m_normal     : NORMAL0;
	float3 m_tangent    : TANGENT0;
	float3 m_binormal	: BINORMAL0;
};

struct bc_ps_output
{
	float4 m_diffuse	: SV_Target0;
	float4 m_normal		: SV_Target1;
};

bc_vs_output gbuffer_vs(bc_vs_input p_input)
{
	bc_vs_output l_output;

	l_output.m_position = mul(float4(p_input.m_position, 1), g_world_view_projection);
	l_output.m_texcoord = p_input.m_texcoord;
	l_output.m_normal = mul(p_input.m_normal, (float3x3)g_world);
	l_output.m_tangent = mul(p_input.m_tangent, (float3x3) g_world);
	l_output.m_binormal = mul(l_output.m_normal, l_output.m_tangent);

	return l_output;
}

bc_ps_output gbuffer_ps(bc_vs_output p_input)
{
	bc_ps_output l_output;

	float4 l_diffuse = g_tex2d_diffuse.Sample(g_sam_sampler, p_input.m_texcoord);
	float4 l_normal = g_tex2d_normal.Sample(g_sam_sampler, p_input.m_texcoord);
	float4 l_specular = g_tex2d_specular.Sample(g_sam_sampler, p_input.m_texcoord);

	float3x3 l_tbn;
	l_tbn[0] = p_input.m_tangent;
	l_tbn[1] = p_input.m_binormal;
	l_tbn[2] = p_input.m_normal;

	float3 l_final_normal = (mul(l_normal.xyz, l_tbn) + 1) / 2.0f;

	l_output.m_diffuse = float4(l_diffuse.xyz, l_specular.x);
	l_output.m_normal = float4(l_final_normal, g_specular_power);

	return l_output;
}