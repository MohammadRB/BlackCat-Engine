// [03/10/2019 MRB]

#include "bcRegister.hlsli"

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
};

struct bc_ps_output
{
	float4 m_color		: SV_Target;
};

bc_vs_output gbuffer_vs(bc_vs_input p_input)
{
	bc_vs_output l_output;

	float4x4 l_world_view_projection = mul(g_world, g_viewprojection);
	l_output.m_position = mul(float4(p_input.m_position, 1), l_world_view_projection);
	l_output.m_texcoord = p_input.m_texcoord;
	l_output.m_normal = mul(p_input.m_normal, (float3x3)g_world);
	l_output.m_tangent = mul(p_input.m_tangent, (float3x3) g_world);

	return l_output;
}

bc_ps_output gbuffer_ps(bc_vs_output p_input)
{
	bc_ps_output l_output;

	float4 l_diffuse = g_tex2d_diffuse.Sample(g_sam_sampler, p_input.m_texcoord);

	float3 l_light_dir = -g_light_dir;
	float3 l_normal = p_input.m_normal;

	float l_ndl = max(0, dot(l_normal, l_light_dir));
	l_diffuse.xyz = l_ndl * g_light_color;

	l_output.m_color = float4(l_diffuse.xyz, 1);

	return l_output;
}