// [18/12/2021 MRB]

#include "../bcRegister.hlsli"

SamplerState g_sam_sampler				: register(BC_RENDER_PASS_STATE_S0);

Texture2D g_tex2d_input					: register(BC_RENDER_PASS_STATE_T0);

cbuffer g_cb_params						: register(BC_RENDER_PASS_STATE_CB1)
{
	float g_threshold					: packoffset(c0.x);
	float g_intensity					: packoffset(c0.y);
	float g_apply_multiplier			: packoffset(c0.z);
}

static const float2 g_quad[] =
{
	float2(-1, 1), float2(1, 1), float2(-1, -1), float2(1, 1), float2(1, -1), float2(-1, -1)
};
static const float2 g_quad_tex[] =
{
	float2(0, 0), float2(1, 0), float2(0, 1), float2(1, 0), float2(1, 1), float2(0, 1)
};

struct bc_vs_output
{
	float4 m_position		: SV_POSITION;
	float2 m_texcoord		: TEXCOORD0;
};

struct bc_ps_output
{
	float4 m_color			: SV_Target0;
};

bc_vs_output glow_extract_vs(uint p_vertex_id : SV_VertexID)
{
	bc_vs_output l_output;
	l_output.m_position = float4(g_quad[p_vertex_id], 0, 1);
	l_output.m_texcoord = g_quad_tex[p_vertex_id];

	return l_output;
}

bc_ps_output glow_extract_ps(bc_vs_output p_input)
{
	bc_ps_output l_output;
	
	l_output.m_color = saturate(g_tex2d_input.Sample(g_sam_sampler, p_input.m_texcoord) - g_threshold);
	l_output.m_color *= g_intensity;

	return l_output;
}

bc_ps_output glow_apply_ps(bc_vs_output p_input)
{
	bc_ps_output l_output;

	const float3 l_glow = g_tex2d_input.Sample(g_sam_sampler, p_input.m_texcoord).rgb;
	l_output.m_color = float4(l_glow * g_apply_multiplier, 1);

	return l_output;
}