// [03/04/2023 MRB]

#include "../bcRegister.hlsli"
#include "../bcHelper.hlsli"

static const uint g_sin_cos_array_size = 32;
static const uint g_max_ray_count = g_sin_cos_array_size / 2;

SamplerState g_sam_point					: register(BC_RENDER_PASS_STATE_S0);

Texture2D g_tex2d_depth						: register(BC_RENDER_PASS_STATE_T0);
Texture2D g_tex2d_normal					: register(BC_RENDER_PASS_STATE_T1);

cbuffer g_cb_params							: register(BC_RENDER_PASS_STATE_CB1)
{
	uint g_num_rays							: packoffset(c0.x);
	uint g_steps_per_ray					: packoffset(c0.y);
	float g_strength						: packoffset(c0.z);
	float g_radius							: packoffset(c0.w);
	float g_attenuation						: packoffset(c1.x);
	float g_bias							: packoffset(c1.y);
	float g_tan_fov							: packoffset(c1.z);
	float4 g_sin_cos[g_sin_cos_array_size]	: packoffset(c2);
}

cbuffer g_cb_frustum_params					: register(BC_RENDER_PASS_STATE_CB2)
{
	// ordered by top-left top-right bottom-right bottom-left
	float4 g_frustum_vectors[4]				: packoffset(c0);
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

float2 snap_to_texel(float2 p_uv, float2 p_screen_coords)
{
	return round(p_uv * p_screen_coords) * rcp(p_screen_coords);
}

float calculate_screen_space_distance(float p_geometry_size, float p_view_distance, float p_screen_height)
{
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

float3 interpolate_frustum_vector(float2 p_uv, float2x3 p_frustum_diff)
{
	const float3 l_frustum_vector = g_frustum_vectors[0].xyz + (p_uv.x * p_frustum_diff[0]) + (p_uv.y * p_frustum_diff[1]);
	return normalize(l_frustum_vector);
}

float3 get_world_position(float p_depth, float3 p_frustum_vector)
{
	return bc_reconstruct_world_position(p_depth, p_frustum_vector, g_camera_position, g_camera_direction, g_near_plane, g_far_plane);
}

float3 get_world_position(float2 p_uv, float2x3 p_frustum_diff)
{
	const float l_depth = g_tex2d_depth.Sample(g_sam_point, p_uv);
	const float3 l_frustum_vector = interpolate_frustum_vector(p_uv, p_frustum_diff);
	return get_world_position(l_depth, l_frustum_vector);
}

float calculate_sample_occlusion(float2 p_uv,
                                 float3 p_frustum_vector,
                                 float3 p_center_pos,
                                 float3 p_center_normal,
								 float3 p_center_tangent,
								 float3 p_view_vector,
                                 inout float p_top_occlusion)
{
	const float l_sample_depth = g_tex2d_depth.SampleLevel(g_sam_point, p_uv, 0);
	const float3 l_sample_pos = get_world_position(l_sample_depth, p_frustum_vector);

	float3 l_horizon_vector = l_sample_pos - p_center_pos;
	const float l_horizon_vector_length = length(l_horizon_vector);
	l_horizon_vector /= l_horizon_vector_length;
	
	const float l_occlusion = dot(p_center_normal, l_horizon_vector);
	const float l_occlusion_increase = max(l_occlusion - p_top_occlusion, 0);
	p_top_occlusion = max(l_occlusion, p_top_occlusion);
	
	const float l_tangent_view_angle = abs(dot(p_center_tangent, p_view_vector));
	const float l_attenuation = g_attenuation * (max(0., l_tangent_view_angle - .6) * 2 + 1);
	float l_attenuation_factor = saturate(l_horizon_vector_length / l_attenuation);
	l_attenuation_factor = 1.0 - l_attenuation_factor * l_attenuation_factor;

	return l_occlusion_increase * l_attenuation_factor;
}

float calculate_ray_occlusion(float2 p_origin_uv,
                              float2 p_direction,
                              float2 p_max_screen_coords,
                              float2 p_ss_radius,
                              uint p_num_steps_per_ray,
                              float3 p_center_pos,
                              float3 p_center_normal,
							  float3 p_view_vector,
                              float2x3 p_frustum_diff)
{
	// Sample nearest neighbor
	float2 l_texel_direction = p_direction * rcp(float2(g_screen_width, g_screen_height));
	float3 l_center_tangent = get_world_position(p_origin_uv + l_texel_direction, p_frustum_diff) - p_center_pos;
	l_center_tangent -= dot(p_center_normal, l_center_tangent) * p_center_normal;
	l_center_tangent = normalize(l_center_tangent);

	const float2 l_step_uv = snap_to_texel(l_texel_direction * p_ss_radius / p_num_steps_per_ray, p_max_screen_coords);
	float2 l_start_uv = p_origin_uv + l_step_uv;

	const float3 l_frustum_vector_step = interpolate_frustum_vector(l_step_uv, p_frustum_diff) - g_frustum_vectors[0].xyz;
	float3 l_frustum_vector = interpolate_frustum_vector(l_start_uv, p_frustum_diff);

	float l_top_occlusion = g_bias;
	float l_occlusion = 0.0;

	for (uint l_step = 0; l_step < g_steps_per_ray; ++l_step)
	{
		l_occlusion += calculate_sample_occlusion
		(
			l_start_uv,
			l_frustum_vector,
			p_center_pos,
			p_center_normal,
			l_center_tangent,
			p_view_vector,
			l_top_occlusion
		);

		l_start_uv += l_step_uv;
		l_frustum_vector += l_frustum_vector_step;
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
	const float2x3 l_frustum_diff = float2x3
	(
		g_frustum_vectors[1].xyz - g_frustum_vectors[0].xyz,
		g_frustum_vectors[3].xyz - g_frustum_vectors[0].xyz
	);

	const float l_depth = g_tex2d_depth.SampleLevel(g_sam_point, p_input.m_texcoord, 0);
	const float l_linear_depth = bc_convert_to_scaled_linear_depth(l_depth, g_near_plane, g_far_plane);
	const float3 l_world_pos = get_world_position(l_depth, p_input.m_view_frustum_vector);
	const float3 l_world_normal = bc_reconstruct_world_normal(l_world_pos);
	const float3 l_view_vector = normalize(g_camera_position - l_world_pos);

	const uint2 l_texcoord = p_input.m_texcoord * l_max_screen_coords;
	const float l_random1 = bc_random(l_texcoord.x);
	const float l_random2 = bc_random(l_texcoord.y);
	const uint l_rotation_array_skip_per_ray = g_max_ray_count / g_num_rays;
	const uint l_rotation_array_start = (l_random1 + l_random2) / 2 * g_max_ray_count;

	const float l_ss_radios = max(1, calculate_screen_space_distance(g_radius, l_linear_depth, g_screen_height));
	const uint l_steps_per_ray = min(g_steps_per_ray, l_ss_radios);
	float l_occlusion = 0;
	
	for (uint i = 0; i < g_num_rays; ++i)
	{
		// The sin-cos array contains two set of values repeated to prevent out of bound index
		const uint l_rotation_index = l_rotation_array_start + (i * l_rotation_array_skip_per_ray);

		const float2 l_sample_dir = rotate_vector(float2(1, 0), l_rotation_index);
		l_occlusion += calculate_ray_occlusion
		(
			p_input.m_texcoord,
			l_sample_dir,
			l_max_screen_coords,
			l_ss_radios,
			l_steps_per_ray,
			l_world_pos,
			l_world_normal,
			l_view_vector,
			l_frustum_diff
		);
	}

	return 1.0 - saturate(g_strength / g_num_rays * l_occlusion);
}