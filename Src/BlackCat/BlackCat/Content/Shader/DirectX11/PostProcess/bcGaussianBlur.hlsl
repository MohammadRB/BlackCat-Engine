// [12/19/2021 MRB]

#include "../bcRegister.hlsli"

SamplerState g_sampler						: register(BC_RENDER_PASS_STATE_S0);

Texture2D g_tex2d_input						: register(BC_RENDER_PASS_STATE_T0);

// https://www.shadertoy.com/view/XdfGDH
float kernel(in int p_i, in float p_sigma)
{
	return 0.39894 * exp(-0.5 * p_i * p_i / (p_sigma * p_sigma)) / p_sigma;
}

float4 gaussian_blur_ps(float4 p_position : SV_POSITION, float2 p_texcoord : TEXCOORD0) : SV_Target0
{
	const int l_size = 2;
	const int l_radius = 11;
	const int l_kernel_size = (l_radius - 1) / 2;
	float l_kernel[l_radius];
	float4 l_final_color = 0.0;

	// Create the 1-D kernel
	float l_sigma = 11.0;
	float l_normalization_factor = 0.0;
	for (int l_k_ite = 0; l_k_ite <= l_kernel_size; ++l_k_ite)
	{
		l_kernel[l_kernel_size + l_k_ite] = l_kernel[l_kernel_size - l_k_ite] = kernel(l_k_ite, l_sigma);
	}

	// Get the normalization factor (as the gaussian has been clamped)
	for (int l_ite = 0; l_ite < l_radius; ++l_ite)
	{
		l_normalization_factor += l_kernel[l_ite];
	}

	float2 l_input_texture_size;
	float l_input_Levels;
	g_tex2d_input.GetDimensions(0, l_input_texture_size.x, l_input_texture_size.y, l_input_Levels);

	[unroll]
	for (int l_i = -l_kernel_size; l_i <= l_kernel_size; ++l_i)
	{
		[unroll]
		for (int l_j = -l_kernel_size; l_j <= l_kernel_size; ++l_j)
		{
			l_final_color += 
				l_kernel[l_kernel_size + l_j] *
				l_kernel[l_kernel_size + l_i] *
				g_tex2d_input.Sample(g_sampler, p_texcoord + (float2(float(l_i * l_size), float(l_j * l_size)) / l_input_texture_size));

		}
	}

	return float4((l_final_color / (l_normalization_factor * l_normalization_factor)).rgb, 1.0);
}