// [05/04/2022 MRB]

#include "../bcRegister.hlsli"
#include "../bcHelper.hlsli"

SamplerState g_sam_point				: register(BC_RENDER_PASS_STATE_S0);

Texture2D g_tex2d_depth					: register(BC_RENDER_PASS_STATE_T0);
Texture2D g_tex2d_diffuse				: register(BC_RENDER_PASS_STATE_T1);
Texture2D g_tex2d_normal				: register(BC_RENDER_PASS_STATE_T2);

cbuffer g_cb_params						: register(BC_RENDER_PASS_STATE_CB1)
{
	float g_min_depth_threshold			: packoffset(c0.x);
	float g_max_depth_threshold			: packoffset(c0.y);
	float g_normal_threshold			: packoffset(c0.z);
	uint g_sample_count					: packoffset(c0.w);
}

static const float2 g_quad[] =
{
	float2(-1, 1), float2(1, 1), float2(-1, -1), float2(1, 1), float2(1, -1), float2(-1, -1)
};
static const float2 g_quad_tex[] =
{
	float2(0, 0), float2(1, 0), float2(0, 1), float2(1, 0), float2(1, 1), float2(0, 1)
};
static const float2 g_offsets[5] = {
	// center
	float2(0.0, 0.0),
	// top
	float2(0.0, -1.0),
	// right
	float2(1.0, 0.0),
	// bottom
	float2(0.0, 1.0),
	// left
	float2(-1.0, 0.0)
};
static const float2 g_diffuse_offsets[8] = {
	// top
	float2(0.0, -1.0),
	// right
	float2(1.0, 0.0),
	// bottom
	float2(0.0, 1.0),
	// left
	float2(-1.0, 0.0),
	// top-right
	float2(1.0, -1.0),
	// bottom-right
	float2(1.0, 1.0),
	// bottom-left
	float2(-1.0, 1.0),
	// top-left
	float2(-1.0, -1.0),
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

float get_depth_edge_factor(float2 p_texcoord, float2 l_pixel_size)
{
	float l_depth[5];
	
	for (int l_i = 0; l_i < 5; ++l_i)
	{
		const float2 l_uv = p_texcoord + g_offsets[l_i] * l_pixel_size;
		l_depth[l_i] = bc_convert_to_linear_depth(g_tex2d_depth.Sample(g_sam_point, l_uv).x, g_near_plane, g_far_plane);
	}

	// Compute deltas in depth
	float2 l_deltas1;
	float2 l_deltas2;
	l_deltas1.x = l_depth[1];
	l_deltas1.y = l_depth[2];
	l_deltas2.x = l_depth[3];
	l_deltas2.y = l_depth[4];

	// Compute absolute gradients from center
	l_deltas1 = abs(l_deltas1 - l_depth[0]);
	l_deltas2 = abs(l_depth[0] - l_deltas2);

	const float l_depth_threshold = lerp(g_min_depth_threshold, g_max_depth_threshold, l_depth[0]);

	// Find min and max gradient, ensuring min != 0
	const float2 l_max_deltas = max(l_deltas1, l_deltas2);
	const float2 l_min_deltas = max(min(l_deltas1, l_deltas2), l_depth_threshold / (g_far_plane - g_near_plane));

	// Compare change in gradients, flagging ones that change significantly.
	// The constant number is to avoid false depth detection on equal min and max deltas on flat surfaces
	float2 l_depth_results = step(l_min_deltas * 1.5f, l_max_deltas);
	return (l_depth_results.x + l_depth_results.y) * 0.5f;
}

float get_normal_edge_factor(float2 p_texcoord, float2 l_pixel_size)
{
	float3 l_normals[5];

	for (int l_i = 0; l_i < 5; ++l_i)
	{
		const float2 l_uv = p_texcoord + g_offsets[l_i] * l_pixel_size;
		l_normals[l_i] = bc_to_decoded_normal(g_tex2d_normal.Sample(g_sam_point, l_uv).xyz);
	}

	float2 l_deltas1;
	float2 l_deltas2;

	// Compute change in the cosine of the angle between normals.
	l_deltas1.x = dot(l_normals[1], l_normals[0]);
	l_deltas1.y = dot(l_normals[2], l_normals[0]);

	l_deltas2.x = dot(l_normals[3], l_normals[0]);
	l_deltas2.y = dot(l_normals[4], l_normals[0]);

	l_deltas1 = abs(l_deltas1 - l_deltas2);

	// Compare change in the cosine of the angles, flagging changes above some constant threshold.
	float2 l_normal_results = step(g_normal_threshold, l_deltas1);
	return (l_normal_results.x + l_normal_results.y) * 0.5;
}

bc_vs_output edge_aa_vs(uint p_vertex_id : SV_VertexID)
{
	bc_vs_output l_output;
	l_output.m_position = float4(g_quad[p_vertex_id], 0, 1);
	l_output.m_texcoord = g_quad_tex[p_vertex_id];

	return l_output;
}

bc_ps_output edge_aa_ps(bc_vs_output p_input)
{
	bc_ps_output l_output;
	const float2 l_pixel_size = rcp(float2(g_screen_width, g_screen_height));

	const float	l_depth_factor = get_depth_edge_factor(p_input.m_texcoord, l_pixel_size);
	const float	l_normal_factor = get_normal_edge_factor(p_input.m_texcoord, l_pixel_size);
	const float l_edge_factor = max(l_depth_factor, l_normal_factor);

	const float4 l_color = g_tex2d_diffuse.Sample(g_sam_point, p_input.m_texcoord);
	if (l_edge_factor <= 0.0)
	{
		l_output.m_color = l_color;
		return l_output;
	}
	
	float4 l_neighbors_color = l_color;

	for (int l_i = 0; l_i < g_sample_count; ++l_i)
	{
		const float2 l_uv = p_input.m_texcoord + g_diffuse_offsets[l_i] * l_pixel_size;
		l_neighbors_color += g_tex2d_diffuse.Sample(g_sam_point, l_uv);
	}

	l_neighbors_color /= g_sample_count + 1;

	l_output.m_color.xyz = (1 - l_edge_factor) * l_color + l_edge_factor * l_neighbors_color;
	//l_output.m_color = l_edge_factor;

	return l_output;
}