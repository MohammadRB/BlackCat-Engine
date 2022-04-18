// [12/19/2021 MRB]

#include "../bcRegister.hlsli"

SamplerState g_sampler						: register(BC_RENDER_PASS_STATE_S0);

Texture2D g_tex2d_input						: register(BC_RENDER_PASS_STATE_T0);

cbuffer g_cb_params							: register(BC_RENDER_PASS_STATE_CB1)
{
	uint g_input_width						: packoffset(c0.x);
	uint g_input_height						: packoffset(c0.y);
}

static const float g_offset[] = { 0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0 };
static const float g_weight[] = { 0.1061154, 0.1028506, 0.09364651, 0.0801001, 0.06436224, 0.04858317, 0.03445063, 0.02294906 };
static const float g_size = 2.5;

float4 gaussian_blur_hor_ps(float4 p_position : SV_POSITION, float2 p_texcoord : TEXCOORD0) : SV_Target0
{
	float4 l_color = g_tex2d_input.Sample(g_sampler, p_texcoord) * g_weight[0];
	float4 l_fragment_color = float4(0, 0, 0, 0);
	
	for (int i = 1; i < 8; i++)
	{
		const float l_offset = (g_offset[i] / g_input_width) * g_size;
		l_fragment_color += g_tex2d_input.Sample(g_sampler, p_texcoord + float2(l_offset, 0)) * g_weight[i];
		l_fragment_color += g_tex2d_input.Sample(g_sampler, p_texcoord - float2(l_offset, 0)) * g_weight[i];
	}

	l_color += l_fragment_color;
	return l_color;
}

float4 gaussian_blur_ver_ps(float4 p_position : SV_POSITION, float2 p_texcoord : TEXCOORD0) : SV_Target0
{
	float4 l_color = g_tex2d_input.Sample(g_sampler, p_texcoord) * g_weight[0];
	float4 l_fragment_color = float4(0, 0, 0, 0);

	for (int i = 1; i < 8; i++)
	{
		const float l_offset = (g_offset[i] / g_input_height) * g_size;
		l_fragment_color += g_tex2d_input.Sample(g_sampler, p_texcoord + float2(0, l_offset)) * g_weight[i];
		l_fragment_color += g_tex2d_input.Sample(g_sampler, p_texcoord - float2(0, l_offset)) * g_weight[i];
	}

	l_color += l_fragment_color;
	return l_color;
}