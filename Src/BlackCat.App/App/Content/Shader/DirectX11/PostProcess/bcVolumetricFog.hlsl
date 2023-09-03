// [12/08/2023 MRB]

#include "../bcRegister.hlsli"
#include "../bcHelper.hlsli"

struct bc_fog_instance
{
	float4x4 m_world;
	float3 m_extend;
	float3 m_min;
	float3 m_max;
	float3 m_color;
	float m_center_fade;
	float m_visibility_distance;
	float m_intensity;
};

struct bc_box
{
	float3 m_min;
	float3 m_max;
};

struct bc_ray
{
	float3 m_origin;
	float3 m_dir;
	float3 m_dir_inv;
};

struct bc_ray_intersect_result
{
	bool m_intersect;
	float m_tmin;
	float m_tmax;
};

SamplerState g_sam_point									: register(BC_RENDER_PASS_STATE_S0);
Texture2D g_tex2d_depth										: register(BC_RENDER_PASS_STATE_T0);
StructuredBuffer<bc_fog_instance> g_buf_instances			: register(BC_RENDER_PASS_STATE_T1);

cbuffer g_cb_params											: register(BC_RENDER_PASS_STATE_CB1)
{
	// ordered by top-left top-right bottom-right bottom-left
	float3 g_frustum_vectors[4] : packoffset(c0);
	float3 g_frustum_hor_diff : packoffset(c4);
	float3 g_frustum_ver_diff : packoffset(c5);
}

struct bc_vs_input
{
	float3 m_position : POSITION0;
};

struct bc_vs_output
{
	float4 m_position : SV_POSITION;
	float4 m_cs_position : TEXCOORD0;
	float3 m_fog_half_extend : TEXCOORD1;
	float3 m_fog_extend_min : TEXCOORD2;
	float3 m_fog_extend_max : TEXCOORD3;
	float3 m_fog_center : TEXCOORD4;
	float3 m_fog_color : TEXCOORD5;
	float m_fog_center_fade : TEXCOORD6;
	float m_fog_visibility_distance : TEXCOORD7;
	float m_fog_intensity : TEXCOORD8;
};

// https://tavianator.com/2022/ray_box_boundary.html
bc_ray_intersect_result bc_ray_box_intersect(const bc_ray p_ray, const bc_box p_fog_box)
{
	float l_tmin = 0.0, l_tmax = 1.#INF;

	for (int i = 0; i < 3; ++i)
	{
		float t1 = (p_fog_box.m_min[i] - p_ray.m_origin[i]) * p_ray.m_dir_inv[i];
		float t2 = (p_fog_box.m_max[i] - p_ray.m_origin[i]) * p_ray.m_dir_inv[i];

		l_tmin = min(max(t1, l_tmin), max(t2, l_tmin));
		l_tmax = max(min(t1, l_tmax), min(t2, l_tmax));
	}

	bc_ray_intersect_result l_result;
	l_result.m_intersect = l_tmin <= l_tmax;
	l_result.m_tmin = l_tmin;
	l_result.m_tmax = l_tmax;

	return l_result;
}

float3 bc_line_point_intersect(const float3 p_line_point, const float3 p_line_dir, const float3 p_point)
{
	const float3 l_line_to_point = p_point - p_line_point;
	const float l_t = dot(l_line_to_point, p_line_dir);
	const float3 l_point_on_line = p_line_point + l_t * p_line_dir;

	return l_point_on_line;
}

float3 interpolate_frustum_vector(float2 p_uv, float3 p_frustum_hor_diff, float3 p_frustum_ver_diff)
{
	const float3 l_frustum_vector = g_frustum_vectors[0] + (p_uv.x * p_frustum_hor_diff) + (p_uv.y * p_frustum_ver_diff);
	return normalize(l_frustum_vector);
}

float3 get_world_position(float p_depth, float3 p_frustum_vector)
{
	return bc_reconstruct_world_position(p_depth, p_frustum_vector, g_camera_position, g_camera_direction, g_near_plane, g_far_plane);
}

float3 get_world_position(float p_depth, float2 p_uv, float3 p_frustum_hor_diff, float3 p_frustum_ver_diff)
{
	const float3 l_frustum_vector = interpolate_frustum_vector(p_uv, p_frustum_hor_diff, p_frustum_ver_diff);
	return get_world_position(p_depth, l_frustum_vector);
}

bc_vs_output vol_fog_vs(bc_vs_input p_input, uint p_instance_index : SV_InstanceID)
{
	bc_vs_output l_output;
	const bc_fog_instance l_instance = g_buf_instances.Load(p_instance_index);

	l_output.m_position = mul(mul(float4(p_input.m_position, 1), l_instance.m_world), g_view_projection);
	l_output.m_cs_position = l_output.m_position;
	l_output.m_fog_half_extend = l_instance.m_extend / 2;
	l_output.m_fog_extend_min = l_instance.m_min;
	l_output.m_fog_extend_max = l_instance.m_max;
	l_output.m_fog_center = (l_instance.m_min + l_instance.m_max) / 2;
	l_output.m_fog_color = l_instance.m_color;
	l_output.m_fog_center_fade = l_instance.m_center_fade;
	l_output.m_fog_visibility_distance = l_instance.m_visibility_distance;
	l_output.m_fog_intensity = l_instance.m_intensity;

	return l_output;
}

float4 vol_fog_ps(bc_vs_output p_input) : SV_Target0
{
	const float2 l_texcoord = bc_clip_space_to_texcoord(p_input.m_cs_position);
	const float l_pixel_depth = g_tex2d_depth.Sample(g_sam_point, l_texcoord);

	const float3 l_view_ray = interpolate_frustum_vector(l_texcoord, g_frustum_hor_diff, g_frustum_ver_diff);
	const float3 l_pixel_world_pos = get_world_position(l_pixel_depth, l_view_ray);
	
	bc_ray l_ray;
	l_ray.m_origin = g_camera_position;
	l_ray.m_dir = l_view_ray;
	l_ray.m_dir_inv = rcp(l_ray.m_dir);

	bc_box l_box;
	l_box.m_min = p_input.m_fog_extend_min;
	l_box.m_max = p_input.m_fog_extend_max;
	
	const bc_ray_intersect_result l_intersect_result = bc_ray_box_intersect(l_ray, l_box);
	
	const float l_pixel_distance = length(l_pixel_world_pos - l_ray.m_origin);
	const float l_tmin = max(l_intersect_result.m_tmin, 0);
	const float l_tmax = min(l_intersect_result.m_tmax, l_pixel_distance);
	
	clip(l_tmax - l_tmin);

	const float l_ray_length = l_tmax - l_tmin;
	const float l_ray_length_ratio = min(1, l_ray_length / p_input.m_fog_visibility_distance);

	const float3 l_ray_middle_point = l_ray.m_origin + l_ray.m_dir * ((l_tmin + l_tmax) * .5);
	const float3 l_ray_middle_box_center_distance = abs(l_ray_middle_point - p_input.m_fog_center);

	const float3 l_center_fade_distance = p_input.m_fog_half_extend * p_input.m_fog_center_fade;
	const float3 l_center_fade_distance_inv = p_input.m_fog_half_extend * (1. - p_input.m_fog_center_fade);
	float3 l_center_distance_fade = max(0, l_ray_middle_box_center_distance - l_center_fade_distance);
	l_center_distance_fade /= l_center_fade_distance_inv;
	l_center_distance_fade = pow(l_center_distance_fade, 2);
	l_center_distance_fade = 1. - l_center_distance_fade;

	const float l_center_distance_ratio = min
	(
		min(l_center_distance_fade.x, l_center_distance_fade.y), l_center_distance_fade.z
	);
	
	const float l_fog = l_ray_length_ratio * l_center_distance_ratio * p_input.m_fog_intensity;
	const float3 l_fog_color = p_input.m_fog_color * g_global_light_ambient_color * g_global_light_ambient_intensity;
	
	return float4(l_fog_color, l_fog);
}