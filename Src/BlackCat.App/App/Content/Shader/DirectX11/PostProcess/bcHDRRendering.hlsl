// [02/10/2023 MRB]

#include "../bcRegister.hlsli"
#include "../bcHelper.hlsli"
#include "bcToneMap.hlsli"

SamplerState g_sam_point			: register(BC_RENDER_PASS_STATE_S0);
Texture2D g_tex2d_hdr_input			: register(BC_RENDER_PASS_STATE_T0);
Texture2D g_tex2d_last_luminance	: register(BC_RENDER_PASS_STATE_T1);
Texture2D g_tex2d_luminance			: register(BC_RENDER_PASS_STATE_T2);
Texture2D g_tex2d_avg_luminance		: register(BC_RENDER_PASS_STATE_T3);

cbuffer g_cb_params					: register(BC_RENDER_PASS_STATE_CB1)
{
	int g_luminance_mipmap_count	: packoffset(c0.x);
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
	float4 m_position				: SV_POSITION;
	float2 m_texcoord				: TEXCOORD0;
};

struct bc_ps_output
{
	float4 m_color					: SV_Target0;
};

bc_vs_output hdr_quad_vs(uint p_vertex_id : SV_VertexID)
{
	bc_vs_output l_output;
	l_output.m_position = float4(g_quad[p_vertex_id], 0, 1);
	l_output.m_texcoord = g_quad_tex[p_vertex_id];

	return l_output;
}

bc_ps_output hdr_luminance_ps(bc_vs_output p_input)
{
	const float4 l_color = g_tex2d_hdr_input.Sample(g_sam_point, p_input.m_texcoord);
	const float l_luminance = calculate_luminance(l_color);

	bc_ps_output l_output;
	l_output.m_color = l_luminance;

	return l_output;
}

bc_ps_output hdr_adaptive_luminance_ps(bc_vs_output p_input)
{
	const float l_last_lum = exp(g_tex2d_last_luminance.Sample(g_sam_point, float2(.5f, .5f)).r);
	const float l_current_lum = g_tex2d_luminance.SampleLevel(g_sam_point, float2(.5f, .5f), g_luminance_mipmap_count).r;

	// Adapt the luminance using Pattanaik's technique
	const float l_tau = 0.7f;
	const float l_adapted_lum = l_last_lum + (l_current_lum - l_last_lum) * (1 - exp(-g_elapsed_second * l_tau));
	const float l_log_adapted_lum = log(0.00001f + l_adapted_lum);

	bc_ps_output l_output;
	l_output.m_color = l_log_adapted_lum;

	return l_output;
}

bc_ps_output hdr_tone_map_ps(bc_vs_output p_input)
{
	const float3 l_hdr_color = g_tex2d_hdr_input.Sample(g_sam_point, p_input.m_texcoord).rgb;
	const float l_average_luminance = exp(g_tex2d_avg_luminance.Sample(g_sam_point, float2(.5f, .5f)).r);

	float l_exposure;
	const float3 l_color = tone_map(l_hdr_color, l_average_luminance, 0, l_exposure);
	
	bc_ps_output l_output;
	l_output.m_color = float4(l_color, 1.0f);
	
	return l_output;
}