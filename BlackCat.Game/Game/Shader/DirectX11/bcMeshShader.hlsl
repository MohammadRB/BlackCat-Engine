// [07/15/2016 MRB]

#include "bcRegister.hlsli"

// == Resource ===================================================================================

SamplerState g_sam_sampler      : register(BC_RENDER_PASS_STATE_S0);

Texture2D g_text2d_diffuse		: register(BC_RENDER_STATE_T0);
Texture2D g_text2d_normal		: register(BC_RENDER_STATE_T1);
Texture2D g_text2d_specular		: register(BC_RENDER_STATE_T2);

cbuffer g_cb_object_state       : register(BC_RENDER_STATE_CB0)
{
    float4x4 g_world            : packoffset(c0);
}

cbuffer g_cb_material			: register(BC_RENDER_STATE_CB1)
{
	float4 g_diffuse			: packoffset(c0);
	float g_specular_intency	: packoffset(c1.x);
	float g_specular_power		: packoffset(c1.y);
};

// == Input / Output ==============================================================================

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
};

struct bc_ps_output
{
	float4 m_color		: SV_Target;
};

// == Shader =======================================================================================

bc_vs_output vs(bc_vs_input p_input)
{
	bc_vs_output l_output;

	float4x4 l_world_view_projection = mul(g_world, g_viewprojection);
	l_output.m_position = mul(float4(p_input.m_position, 1), l_world_view_projection);
	l_output.m_texcoord = p_input.m_texcoord;

	return l_output;
}

bc_ps_output ps(bc_vs_output p_input)
{
	bc_ps_output l_putput;

	l_putput.m_color = g_text2d_diffuse.Sample(g_sam_sampler, p_input.m_texcoord);

	return l_putput;
}