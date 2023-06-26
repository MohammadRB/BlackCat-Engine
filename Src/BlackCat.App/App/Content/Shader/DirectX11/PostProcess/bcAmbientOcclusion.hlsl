// [03/04/2023 MRB]

#include "../bcRegister.hlsli"
#include "../bcHelper.hlsli"

SamplerState g_sam_point				: register(BC_RENDER_PASS_STATE_S0);

Texture2D g_tex2d_depth					: register(BC_RENDER_PASS_STATE_T0);
Texture2D g_tex2d_normal				: register(BC_RENDER_PASS_STATE_T1);

cbuffer g_cb_params						: register(BC_RENDER_PASS_STATE_CB1)
{
	uint g_num_rays						: packoffset(c0.x);
	uint g_steps_per_ray				: packoffset(c0.y);
	float g_strength_per_ray			: packoffset(c0.z);
	float g_radius						: packoffset(c0.w);
	float g_attenuation					: packoffset(c1.x);
	float g_bias						: packoffset(c1.y);
	float g_tan_fov						: packoffset(c1.z);
	float4 g_frustum_vectors[4]			: packoffset(c2); // ordered by top-left top-right bottom-right bottom-left
	float4 g_sin_cos[32]				: packoffset(c6);
}

static const float2 g_quad[] =
{
	float2(-1, 1), float2(1, 1), float2(-1, -1), float2(1, 1), float2(1, -1), float2(-1, -1)
};
static const float2 g_quad_tex[] =
{
	float2(0, 0), float2(1, 0), float2(0, 1), float2(1, 0), float2(1, 1), float2(0, 1)
};
static const uint g_view_frustum_vector_indexes[] = 
{
	0, 1, 3, 1, 2, 3
};

struct bc_vs_output
{
	float4 m_position					: SV_POSITION;
	float2 m_texcoord					: TEXCOORD0;
	float3 m_view_frustum_vector		: FRUSTUM_VECTOR;
};

float depth_to_view_z(float p_depth_value)
{
	return g_projection[3][2] / (p_depth_value - g_projection[2][2]);
}

float2 snap_to_texel(float2 p_uv, float2 p_screen_coords)
{
	return round(p_uv * p_screen_coords) * rcp(p_screen_coords);
}

float calculate_screen_space_distance(float p_geometry_size, float p_view_distance, float p_screen_height)
{
	//return (p_distance * g_cot_fov) / p_view_z;
	const float l_view_height_at_near_clip = g_tan_fov;
	const float l_view_height = g_tan_fov * p_view_distance;
	const float l_view_height_ratio = p_geometry_size / l_view_height;
	const float l_geometry_size_on_near_clip = l_view_height_at_near_clip * l_view_height_ratio;
	return l_geometry_size_on_near_clip * p_screen_height;
}

float2 rotate_vector(float2 p_vector, uint p_rotation_index)
{
	const float l_sin = g_sin_cos[p_rotation_index].x;
	const float l_cos = g_sin_cos[p_rotation_index].y;

	// float2(p_vector.x * l_cos - p_vector.y * l_sin, p_vector.x * l_sin + p_vector.y * l_cos)
	const float4 l_multiply = float4(p_vector.x, p_vector.y, p_vector.x, p_vector.y) * float4(l_cos, l_sin, l_sin, l_cos);
	return float2(l_multiply.x - l_multiply.y, l_multiply.z + l_multiply.w);
}

float3 get_world_position(float p_depth, float3 p_frustum_vector)
{
	const float l_linear_depth = bc_convert_to_linear_depth(p_depth, g_near_plane, g_far_plane);
	const float l_scaled_linear_depth = l_linear_depth * (g_far_plane - g_near_plane);
	const float3 l_world_pos = g_camera_position + p_frustum_vector * l_scaled_linear_depth;

	return l_world_pos;
}

float3 get_world_position(float2 p_uv, float2 p_frustum_diff)
{
	const float l_depth = g_tex2d_depth.Sample(g_sam_point, p_uv);
	const float3 l_frustum_vector = float3(g_frustum_vectors[0].xy + p_uv * p_frustum_diff, 1.0);
	return get_world_position(l_depth, l_frustum_vector);
}

float calculate_sample_occlusion(float2 p_uv,
                                 float3 p_frustum_vector,
                                 float3 p_center_pos,
                                 float3 p_center_normal,
                                 float3 p_center_tangent,
                                 inout float p_top_occlusion)
{
	const float l_sample_depth = g_tex2d_depth.SampleLevel(g_sam_point, p_uv, 0);
	const float3 l_sample_pos = get_world_position(l_sample_depth, p_frustum_vector);
	const float3 l_horizon_vector = l_sample_pos - p_center_pos;
	const float l_horizon_vector_length = length(l_horizon_vector);

	if (dot(p_center_tangent, l_horizon_vector) < 0.0)
	{
		return 0.5;
	}

	const float l_occlusion = dot(p_center_normal, l_horizon_vector) / l_horizon_vector_length;
	const float l_diff = max(l_occlusion - p_top_occlusion, 0);
	p_top_occlusion = max(l_occlusion, p_top_occlusion);

	float l_attenuation_factor = saturate(l_horizon_vector_length / g_attenuation);
	l_attenuation_factor = 1.0 - l_attenuation_factor * l_attenuation_factor;

	return l_diff * l_attenuation_factor;
}

float calculate_ray_occlusion(float2 p_origin_uv,
                              float2 p_direction,
                              float p_jitter,
                              float2 p_max_screen_coords,
                              float2 p_ss_radius,
                              uint p_num_steps_per_ray,
                              float3 p_center_pos,
                              float3 p_center_normal,
                              float2 p_frustum_diff)
{
	float2 l_texel_sized_step = p_direction * rcp(float2(g_screen_width, g_screen_height));
	float3 l_center_tangent = get_world_position(p_origin_uv + l_texel_sized_step, p_frustum_diff) - p_center_pos;
	l_center_tangent -= dot(p_center_normal, l_center_tangent) * p_center_normal;

	const float2 l_step_uv = snap_to_texel(p_direction * p_ss_radius / (p_num_steps_per_ray - 1), p_max_screen_coords);
	float2 l_start_uv = p_origin_uv;

	const float2 l_frustum_vector_step = l_step_uv * p_frustum_diff;
	float3 l_frustum_vector = float3(g_frustum_vectors[0].xy + l_start_uv * p_frustum_diff, 1.0);

	float l_top_occlusion = g_bias;
	float l_occlusion = 0.0;

	for (uint l_step = 0; l_step < g_steps_per_ray; ++l_step)
	{
		l_occlusion += calculate_sample_occlusion(l_start_uv, l_frustum_vector, p_center_pos, p_center_normal, l_center_tangent, l_top_occlusion);

		l_start_uv += l_step_uv;
		l_frustum_vector.xy += l_frustum_vector_step.xy;
	}

	return l_occlusion;
}

bc_vs_output ao_vs(uint p_vertex_id : SV_VertexID)
{
	bc_vs_output l_output;
	l_output.m_position = float4(g_quad[p_vertex_id], 0, 1);
	l_output.m_texcoord = g_quad_tex[p_vertex_id];
	l_output.m_view_frustum_vector = g_frustum_vectors[g_view_frustum_vector_indexes[p_vertex_id]].xyz;

	return l_output;
}

float4 ao_ps(bc_vs_output p_input) : SV_Target0
{
	const float2 l_max_screen_coords = float2(g_screen_width, g_screen_height) - 1;
	const float l_depth = g_tex2d_depth.SampleLevel(g_sam_point, p_input.m_texcoord, 0);
	const float3 l_normal = bc_to_decoded_normal(g_tex2d_normal.SampleLevel(g_sam_point, p_input.m_texcoord, 0).xyz);

	const float l_linear_depth = bc_convert_to_linear_depth(l_depth, g_near_plane, g_far_plane);
	const float l_scaled_linear_depth = l_linear_depth * (g_far_plane - g_near_plane);
	const float3 l_world_pos = g_camera_position + p_input.m_view_frustum_vector * l_scaled_linear_depth;

	const float2 l_frustum_diff = abs(float2(g_frustum_vectors[0].x - g_frustum_vectors[1].x, g_frustum_vectors[0].y - g_frustum_vectors[3].y));
	const float l_ss_radios = calculate_screen_space_distance(g_radius, l_scaled_linear_depth, g_screen_height);

	if (l_ss_radios < 1.0)
	{
		return 1.0;
	}

	const uint l_steps_per_ray = max(g_steps_per_ray, l_ss_radios);

	float l_occlusion = 0;

	for (uint i = 0; i < g_num_rays; ++i)
	{
		const float2 l_sample_dir = normalize(rotate_vector(float2(1, 0), i));
		l_occlusion += calculate_ray_occlusion
		(
			p_input.m_texcoord,
			l_sample_dir,
			0,
			l_max_screen_coords,
			l_ss_radios,
			l_steps_per_ray,
			l_world_pos,
			l_normal,
			l_frustum_diff
		);
	}

	return 1.0 - saturate(g_strength_per_ray * l_occlusion);
}