// [03/04/2023 MRB]

#include "../bcRegister.hlsli"
#include "../bcHelper.hlsli"

SamplerState g_sam_point				: register(BC_RENDER_PASS_STATE_S0);

Texture2D g_tex2d_depth					: register(BC_RENDER_PASS_STATE_T0);
Texture2D g_tex2d_normal				: register(BC_RENDER_PASS_STATE_T1);

cbuffer g_cb_params						: register(BC_RENDER_PASS_STATE_CB1)
{
	uint g_num_samples					: packoffset(c0.x);
	float g_radius						: packoffset(c0.y);
	float g_intensity					: packoffset(c0.z);
	float g_bias						: packoffset(c0.w);
	float g_cot_fov						: packoffset(c1.x);
	float4 g_sin_cos[32]				: packoffset(c2);
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

float depth_to_view_z(float p_depth_value)
{
	return g_projection[3][2] / (p_depth_value - g_projection[2][2]);
}

float calculate_screen_space_distance(float p_distance, float p_view_z)
{
	return (p_distance * g_cot_fov) / p_view_z;
}

float2 rotate_vector(float2 p_vector, uint p_rotation_index)
{
	const float l_sin = g_sin_cos[p_rotation_index].x;
	const float l_cos = g_sin_cos[p_rotation_index].y;

	return float2(p_vector.x * l_cos - p_vector.y * l_sin, p_vector.x * l_sin + p_vector.y * l_cos);
}

void calculate_occlusion(float3 p_vector, float3 p_normal, inout float p_occlusion, inout float p_weight)
{
	float d = dot(p_normal, p_vector);
	float w = saturate(1.0 - d / g_radius);
	float c = saturate(d * rsqrt(dot(p_vector, p_vector)) - g_bias);

	p_occlusion += w - w * sqrt(1.0 - c * c);
	p_weight += w;
}

bc_vs_output ao_vs(uint p_vertex_id : SV_VertexID)
{
	bc_vs_output l_output;
	l_output.m_position = float4(g_quad[p_vertex_id], 0, 1);
	l_output.m_texcoord = g_quad_tex[p_vertex_id];

	return l_output;
}

float4 ao_ps(bc_vs_output p_input) : SV_Target0
{
	const float3 l_normal = bc_to_decoded_normal(g_tex2d_normal.Sample(g_sam_point, p_input.m_texcoord).rgb);
	const float3 l_view_normal = normalize(mul(float4(l_normal, 0), g_view).xyz);
	const float l_depth = g_tex2d_depth.Sample(g_sam_point, p_input.m_texcoord).r;
	const float l_linear_depth = bc_convert_to_linear_depth(l_depth, g_near_plane, g_far_plane);
	const float l_view_z = depth_to_view_z(l_depth);
	const float l_ss_radius = calculate_screen_space_distance(g_radius, l_view_z);

	if (l_ss_radius * g_screen_width < 1.f)
	{
		return 1;
	}

	float l_occlusion = 0;
	float l_weight = 0;

	for (uint i = 0; i < g_num_samples; i += 2)
	{
		const float l_sample1_radius = (i + 1) / (float)g_num_samples;
		const float l_sample2_radius = (g_num_samples - i) / (float)g_num_samples;

		const float2 l_sample1_vector = normalize(rotate_vector(float2(1, 0), i));
		const float2 l_sample2_vector = normalize(rotate_vector(float2(1, 0), i + 1));
		const float2 l_sample1_offset = l_sample1_vector * l_ss_radius * l_sample1_radius * float2(1, g_screen_width / g_screen_height);
		const float2 l_sample2_offset = l_sample2_vector * l_ss_radius * l_sample2_radius * float2(1, g_screen_width / g_screen_height);

		const float l_sample1_depth = g_tex2d_depth.Sample(g_sam_point, p_input.m_texcoord + l_sample1_offset).r;
		const float l_sample2_depth = g_tex2d_depth.Sample(g_sam_point, p_input.m_texcoord + l_sample2_offset).r;
		const float l_sample1_linear_depth = bc_convert_to_linear_depth(l_sample1_depth, g_near_plane, g_far_plane);
		const float l_sample2_linear_depth = bc_convert_to_linear_depth(l_sample2_depth, g_near_plane, g_far_plane);
		const float l_sample1_view_z = depth_to_view_z(l_sample1_depth);
		const float l_sample2_view_z = depth_to_view_z(l_sample2_depth);

		//calculate_occlusion(float3(l_sample1_offset, l_sample1_view_z - l_view_z), l_view_normal, l_occlusion, l_weight);
		//calculate_occlusion(float3(l_sample2_offset, l_sample2_view_z - l_view_z), l_view_normal, l_occlusion, l_weight);
		const float l_sample1_diff = max(l_view_z - l_sample1_view_z, 0);
		const float l_sample2_diff = max(l_view_z - l_sample2_view_z, 0);
		l_occlusion += 1.f - saturate(l_sample1_diff / g_radius);
		l_occlusion += 1.f - saturate(l_sample2_diff / g_radius);
	}

	return l_occlusion / g_num_samples;
	//return (1.0 - l_occlusion * g_intensity / max(l_weight, 0.0001));
}