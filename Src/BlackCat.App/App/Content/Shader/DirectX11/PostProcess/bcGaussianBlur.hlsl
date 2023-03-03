// [12/19/2021 MRB]

#include "../bcRegister.hlsli"

SamplerState g_sam_point					: register(BC_RENDER_PASS_STATE_S0);
SamplerState g_sam_linear					: register(BC_RENDER_PASS_STATE_S1);

Texture2D g_tex2d_input						: register(BC_RENDER_PASS_STATE_T0);

cbuffer g_cb_params							: register(BC_RENDER_PASS_STATE_CB1)
{
	uint g_input_width						: packoffset(c0.x);
	uint g_input_height						: packoffset(c0.y);
}

static const float g_weights_7x7[] = { 0.25085, 0.21675, 0.11128, 0.03664 };
static const float g_steps_7x7[] = { 2, 3 };
static const float g_weights_15x15[] = { 0.1061154, 0.1028506, 0.09364651, 0.0801001, 0.06436224, 0.04858317, 0.03445063, 0.02294906 };
static const float g_steps_15x15[] = { 4, 7 };

float4 gaussian_blur_7(float2 p_texcoord, float2 p_dir)
{
	float4 l_color = g_tex2d_input.Sample(g_sam_point, p_texcoord) * g_weights_7x7[0];
	float4 l_fragment_color = float4(0, 0, 0, 0);
	const int l_step_1_length = g_steps_7x7[0];
	const int l_step_2_length = g_steps_7x7[1];
	int i = 1;

	for (; i <= l_step_1_length; i++)
	{
		const float l_offset = ((float)i / g_input_width);
		const float2 l_dir = p_dir * float2(l_offset, l_offset);
		l_fragment_color += g_tex2d_input.Sample(g_sam_point, p_texcoord + l_dir) * g_weights_7x7[i];
		l_fragment_color += g_tex2d_input.Sample(g_sam_point, p_texcoord - l_dir) * g_weights_7x7[i];
	}

	for (; i <= l_step_2_length; i++)
	{
		const float l_offset = ((float)i / g_input_width) * 2;
		const float2 l_dir = p_dir * float2(l_offset, l_offset);
		l_fragment_color += g_tex2d_input.Sample(g_sam_linear, p_texcoord + l_dir) * g_weights_7x7[i];
		l_fragment_color += g_tex2d_input.Sample(g_sam_linear, p_texcoord - l_dir) * g_weights_7x7[i];
	}

	l_color += l_fragment_color;
	return l_color;
}

float4 gaussian_blur_15(float2 p_texcoord, float2 p_dir)
{
	float4 l_color = g_tex2d_input.Sample(g_sam_point, p_texcoord) * g_weights_15x15[0];
	float4 l_fragment_color = float4(0, 0, 0, 0);
	const int l_step_1_length = g_steps_15x15[0];
	const int l_step_2_length = g_steps_15x15[1];
	int i = 1;

	for (; i <= l_step_1_length; i++)
	{
		const float l_offset = ((float)i / g_input_width);
		const float2 l_dir = p_dir * float2(l_offset, l_offset);
		l_fragment_color += g_tex2d_input.Sample(g_sam_point, p_texcoord + l_dir) * g_weights_15x15[i];
		l_fragment_color += g_tex2d_input.Sample(g_sam_point, p_texcoord - l_dir) * g_weights_15x15[i];
	}

	for (; i <= l_step_2_length; i++)
	{
		const float l_offset = ((float)i / g_input_width) * 2;
		const float2 l_dir = p_dir * float2(l_offset, l_offset);
		l_fragment_color += g_tex2d_input.Sample(g_sam_linear, p_texcoord + l_dir) * g_weights_15x15[i];
		l_fragment_color += g_tex2d_input.Sample(g_sam_linear, p_texcoord - l_dir) * g_weights_15x15[i];
	}

	l_color += l_fragment_color;
	return l_color;
}

float4 gaussian_blur_7_hor_ps(float4 p_position : SV_POSITION, float2 p_texcoord : TEXCOORD0) : SV_Target0
{
	return gaussian_blur_7(p_texcoord, float2(1, 0));
}

float4 gaussian_blur_7_ver_ps(float4 p_position : SV_POSITION, float2 p_texcoord : TEXCOORD0) : SV_Target0
{
	return gaussian_blur_7(p_texcoord, float2(0, 1));
}

float4 gaussian_blur_15_hor_ps(float4 p_position : SV_POSITION, float2 p_texcoord : TEXCOORD0) : SV_Target0
{
	return gaussian_blur_15(p_texcoord, float2(1, 0));
}

float4 gaussian_blur_15_ver_ps(float4 p_position : SV_POSITION, float2 p_texcoord : TEXCOORD0) : SV_Target0
{
	return gaussian_blur_15(p_texcoord, float2(0, 1));
}

float4 gaussian_blur_upscale_ps(float4 p_position : SV_POSITION, float2 p_texcoord : TEXCOORD0) : SV_Target0
{
	float4 l_color = g_tex2d_input.Sample(g_sam_linear, p_texcoord);
	return l_color;
}