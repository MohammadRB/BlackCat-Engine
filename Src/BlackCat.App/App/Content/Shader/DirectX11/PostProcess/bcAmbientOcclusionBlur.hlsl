// [15/07/2023 MRB]

#include "../bcRegister.hlsli"
#include "../bcHelper.hlsli"

SamplerState g_sam_point					: register(BC_RENDER_PASS_STATE_S0);
SamplerState g_sam_linear					: register(BC_RENDER_PASS_STATE_S1);

Texture2D g_tex2d_depth						: register(BC_RENDER_PASS_STATE_T0);
Texture2D g_tex2d_normal					: register(BC_RENDER_PASS_STATE_T1);
Texture2D g_tex2d_ao_buffer					: register(BC_RENDER_PASS_STATE_T2);
Texture2D g_tex2d_input						: register(BC_RENDER_PASS_STATE_T3);

cbuffer g_cb_params							: register(BC_RENDER_PASS_STATE_CB1)
{
	float g_texel_width						: packoffset(c0.x);
	float g_texel_height					: packoffset(c0.y);
	float g_blur_distance					: packoffset(c0.z);
	float g_blur_angle						: packoffset(c0.w);
}

static const float g_weights_7x7[] = { 0.25085, 0.21675, 0.11128, 0.03664 };
static const float g_steps_7x7[] = { 1, 1, 2, 2 };

struct bc_vs_output
{
	float4 m_position						: SV_POSITION;
	float2 m_texcoord						: TEXCOORD0;
};

bool should_blur(float p_center_depth, float3 p_center_normal, float2 p_texcoord)
{
	const float l_depth = g_tex2d_depth.Sample(g_sam_point, p_texcoord).x;
	const float l_linear_depth = bc_convert_to_scaled_linear_depth(l_depth, g_near_plane, g_far_plane);
	const float l_depth_diff = abs(p_center_depth - l_linear_depth);

	const bool l_depth_is_close = l_depth_diff <= g_blur_distance;
	if (!l_depth_is_close)
	{
		return false;
	}

	return true;
	//const float3 l_normal = bc_to_decoded_normal(g_tex2d_normal.Sample(g_sam_point, p_texcoord).xyz);
	//const float l_normal_diff = dot(p_center_normal, l_normal);
	//return l_normal_diff >= g_blur_angle;
}

bc_vs_output ao_blur_vs(uint p_vertex_id : SV_VertexID)
{
	bc_vs_output l_output;
	l_output.m_position = float4(g_ss_quad[p_vertex_id], 0, 1);
	l_output.m_texcoord = g_ss_quad_tex[p_vertex_id];

	return l_output;
}

float4 edge_aware_blur(float2 p_texcoord, float2 p_texel_size)
{
	float4 l_color = g_tex2d_ao_buffer.Sample(g_sam_linear, p_texcoord) * g_weights_7x7[0];
	float l_total_weight = g_weights_7x7[0];

	const float l_center_depth = g_tex2d_depth.Sample(g_sam_point, p_texcoord).x;
	const float l_center_linear_depth = bc_convert_to_scaled_linear_depth(l_center_depth, g_near_plane, g_far_plane);
	//const float3 l_center_normal = bc_to_decoded_normal(g_tex2d_normal.Sample(g_sam_point, p_texcoord).xyz);
	
	for (int i = 1; i < 4; i++)
	{
		const float2 l_offset = i * p_texel_size * g_steps_7x7[i];

		float2 l_texcoord = p_texcoord + l_offset;
		if (should_blur(l_center_linear_depth, 0, l_texcoord))
		{
			l_color += g_tex2d_ao_buffer.Sample(g_sam_linear, l_texcoord) * g_weights_7x7[i];
			l_total_weight += g_weights_7x7[i];
		}

		l_texcoord = p_texcoord - l_offset;
		if (should_blur(l_center_linear_depth, 0, l_texcoord))
		{
			l_color += g_tex2d_ao_buffer.Sample(g_sam_linear, l_texcoord) * g_weights_7x7[i];
			l_total_weight += g_weights_7x7[i];
		}
	}
	
	return l_color / l_total_weight;
}

float4 ao_hor_blur_ps(bc_vs_output p_input) : SV_Target0
{
	return edge_aware_blur(p_input.m_texcoord, float2(g_texel_width, 0));
}

float4 ao_ver_blur_ps(bc_vs_output p_input) : SV_Target0
{
	return edge_aware_blur(p_input.m_texcoord, float2(0, g_texel_height));
}