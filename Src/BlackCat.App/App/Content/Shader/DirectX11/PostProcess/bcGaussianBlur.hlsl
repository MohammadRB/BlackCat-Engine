// [19/12/2021 MRB]

#include "../bcRegister.hlsli"

SamplerState g_sam_point					: register(BC_RENDER_PASS_STATE_S0);
SamplerState g_sam_linear					: register(BC_RENDER_PASS_STATE_S1);

Texture2D g_tex2d_input						: register(BC_RENDER_PASS_STATE_T0);

cbuffer g_cb_params							: register(BC_RENDER_PASS_STATE_CB1)
{
	float g_texel_width						: packoffset(c0.x);
	float g_texel_height : packoffset(c0.y);
}

static const float g_weights_7x7[] = { 0.25085, 0.21675, 0.11128, 0.03664 };
static const float g_steps_7x7[] = { 1, 1, 1, 2 };
static const float g_weights_15x15[] = { 0.1061154, 0.1028506, 0.09364651, 0.0801001, 0.06436224, 0.04858317, 0.03445063, 0.02294906 };
static const float g_steps_15x15[] = { 1, 1, 1, 1, 1, 2, 2, 2 };

float4 gaussian_blur_7(float2 p_texcoord, float2 p_dir_texel_size)
{
	float4 l_color = g_tex2d_input.Sample(g_sam_point, p_texcoord) * g_weights_7x7[0];

	for (int i = 1; i < 4; i++)
	{
		const float2 l_offset = i * p_dir_texel_size * g_steps_7x7[i];
		l_color += g_tex2d_input.Sample(g_sam_point, p_texcoord + l_offset) * g_weights_7x7[i];
		l_color += g_tex2d_input.Sample(g_sam_point, p_texcoord - l_offset) * g_weights_7x7[i];
	}
	
	return l_color;
}

float4 gaussian_blur_15(float2 p_texcoord, float2 p_dir_texel_size)
{
	float4 l_color = g_tex2d_input.Sample(g_sam_point, p_texcoord) * g_weights_15x15[0];

	for (int i = 1; i < 8; i++)
	{
		const float2 l_offset = i * p_dir_texel_size * g_steps_15x15[i];
		l_color += g_tex2d_input.Sample(g_sam_point, p_texcoord + l_offset) * g_weights_15x15[i];
		l_color += g_tex2d_input.Sample(g_sam_point, p_texcoord - l_offset) * g_weights_15x15[i];
	}
	
	return l_color;
}

float4 gaussian_blur_7_hor_ps(float4 p_position : SV_POSITION, float2 p_texcoord : TEXCOORD0) : SV_Target0
{
	return gaussian_blur_7(p_texcoord, float2(g_texel_width, 0));
}

float4 gaussian_blur_7_ver_ps(float4 p_position : SV_POSITION, float2 p_texcoord : TEXCOORD0) : SV_Target0
{
	return gaussian_blur_7(p_texcoord, float2(0, g_texel_height));
}

float4 gaussian_blur_15_hor_ps(float4 p_position : SV_POSITION, float2 p_texcoord : TEXCOORD0) : SV_Target0
{
	return gaussian_blur_15(p_texcoord, float2(g_texel_width, 0));
}

float4 gaussian_blur_15_ver_ps(float4 p_position : SV_POSITION, float2 p_texcoord : TEXCOORD0) : SV_Target0
{
	return gaussian_blur_15(p_texcoord, float2(0, g_texel_height));
}

float4 gaussian_blur_upscale_ps(float4 p_position : SV_POSITION, float2 p_texcoord : TEXCOORD0) : SV_Target0
{
	float4 l_color = g_tex2d_input.Sample(g_sam_linear, p_texcoord);
	return l_color;
}