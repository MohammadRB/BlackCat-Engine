// [03/23/2019 MRB]

#include "..\bcRegister.hlsli"
#include "..\bcHelper.hlsli"

#define THREAD_GROUP_SIZE 16

struct direct_light
{
	float3	m_min_bound;
	float3	m_max_bound;
	float3	m_direction;
	float3	m_color;
	float	m_intensity;
	float3	m_ambient_color;
	float	m_ambient_intensity;
};

struct point_light
{
	float3	m_min_bound;
	float3	m_max_bound;
	float3	m_position;
	float	m_radius;
	float3	m_color;
	float	m_intensity;
};

struct spot_light
{
	float3	m_min_bound;
	float3	m_max_bound;
	float3	m_position;
	float	m_angle;
	float3	m_direction;
	float	m_length;
	float3	m_color;
	float	m_intensity;
    float   m_angle_cos;
};

Texture2D<float4> g_depth_map						: register(BC_COMPUTE_STATE_T0);
Texture2D<float4> g_diffuse_map						: register(BC_COMPUTE_STATE_T1);
Texture2D<float4> g_normal_map						: register(BC_COMPUTE_STATE_T2);

StructuredBuffer<direct_light> g_direct_lights		: register(BC_COMPUTE_STATE_T3);
StructuredBuffer<point_light> g_point_lights		: register(BC_COMPUTE_STATE_T4);
StructuredBuffer<spot_light> g_spot_lights			: register(BC_COMPUTE_STATE_T5);

RWTexture2D<float4> g_output_texture                : register(BC_COMPUTE_STATE_U0);

cbuffer g_cb_parameters                             : register(BC_COMPUTE_STATE_CB1)
{
    uint g_direct_lights_count                      : packoffset(c0.x);
    uint g_point_lights_count                       : packoffset(c0.y);
    uint g_spot_lights_count                        : packoffset(c0.z);
    float4x4 g_view_proj_inv                        : packoffset(c1.x);
};

float4 read_texture(Texture2D<float4> p_texture, int2 p_texcoord)
{
    return p_texture.Load(int3(p_texcoord, 0));
}

void write_output(int2 p_texcoord, float4 p_output)
{
	g_output_texture[p_texcoord] = p_output;
}

bool do_ranges_intersect(float2 p_range1, float2 p_range2)
{
    if
    (
        !(
            (p_range1.x >= p_range2.x && p_range1.x <= p_range2.y)
            ||
            (p_range1.y >= p_range2.x && p_range1.y <= p_range2.y)
            ||
            (p_range2.x >= p_range1.x && p_range2.y <= p_range1.y)
        )
    )
    {
        return false;
    }

    return true;
}

bool is_light_in_sight(float3 p_world_pos, float3 p_light_top_left_min_z, float3 p_light_bottom_right_max_z, uint2 p_group_id, float p_group_min_z, float p_group_max_z)
{
    if (!do_ranges_intersect(float2(p_light_top_left_min_z.z, p_light_bottom_right_max_z.z), float2(p_group_min_z, p_group_max_z)))
    {
        return false;
    }
    
    float2 l_group_top_left = float2(p_group_id * THREAD_GROUP_SIZE);
    float2 l_group_bottom_right = l_group_top_left + (THREAD_GROUP_SIZE - 1);
    float2 l_group_top_left_ss = bc_to_screen_space_texcoord(l_group_top_left, g_screen_width, g_screen_height);
    float2 l_group_bottom_right_ss = bc_to_screen_space_texcoord(l_group_bottom_right, g_screen_width, g_screen_height);
    float2 l_light_top_left = p_light_top_left_min_z.xy;
    float2 l_light_bottom_right = p_light_bottom_right_max_z.xy;

    if
    (
        !(
            do_ranges_intersect(float2(l_light_top_left.x, l_light_bottom_right.x), float2(l_group_top_left_ss.x, l_group_bottom_right_ss.x))
            &&
            do_ranges_intersect(float2(l_light_top_left.y, l_light_bottom_right.y), float2(l_group_top_left_ss.y, l_group_bottom_right_ss.y))
        )
    )
    {
        return false;
    }

    return true;
}

bool is_light_in_tile(float p_tile_min_z, float p_tile_max_z, float p_light_min_z, float p_light_max_z)
{
    return do_ranges_intersect(float2(p_tile_min_z, p_tile_max_z), float2(p_light_min_z, p_light_max_z));
}

bool is_pixel_in_light_range(float3 p_position, float3 p_light_min_bound, float3 p_light_max_bound)
{
    bool l_is_in_light_box = (p_position.x >= p_light_min_bound.x) &&
    (p_position.y >= p_light_min_bound.y) &&
    (p_position.z >= p_light_min_bound.z) &&
    (p_position.x <= p_light_max_bound.x) &&
    (p_position.y <= p_light_max_bound.y) &&
    (p_position.z <= p_light_max_bound.z);

    return l_is_in_light_box;
}

float4 direct_light_shading(direct_light p_light, float3 p_camera_pos, float3 p_position, float3 p_normal, float p_specular_intensity, float p_specular_power)
{
    float3 l_light_vector = -normalize(p_light.m_direction);
    float l_dot = max(0.0f, dot(p_normal, l_light_vector));
    float3 l_diffuse_light = p_light.m_color * l_dot;
    
    float3 l_reflection_vector = normalize(reflect(-l_light_vector, p_normal));
    // camera-to-surface vector
    float3 l_direction_to_camera = normalize(p_camera_pos - p_position);
    // compute specular light
    float l_specular_light = p_specular_intensity * saturate(pow(dot(l_reflection_vector, l_direction_to_camera), p_specular_power));

    // output the two lights
    return p_light.m_intensity * float4(l_diffuse_light, l_specular_light);
}

float4 point_light_shading(point_light p_light, float3 p_camera_pos, float3 p_position, float3 p_normal, float p_specular_intensity, float p_specular_power)
{
    // surface-to-light vector
    float3 l_light_vector = normalize(p_light.m_position - p_position);
    float l_dot = max(0.0f, dot(p_normal, l_light_vector));
    float3 l_diffuse_light = p_light.m_color * l_dot;
    
    float3 l_reflection_vector = normalize(reflect(-l_light_vector, p_normal));
    // camera-to-surface vector
    float3 l_direction_to_camera = normalize(p_camera_pos - p_position);
    // compute specular light
    float l_specular_light = p_specular_intensity * saturate(pow(dot(l_reflection_vector, l_direction_to_camera), p_specular_power));

    // compute attenuation based on distance - linear attenuation
    float l_attenuation = 1.0f - saturate(length(p_light.m_position - p_position) / p_light.m_radius);

    // output the two lights
    return l_attenuation * p_light.m_intensity * float4(l_diffuse_light, l_specular_light);
}

float4 spot_light_shading(spot_light p_light, float3 p_camera_pos, float3 p_position, float3 p_normal, float p_specular_intensity, float p_specular_power)
{
    // surface-to-light vector
    float3 l_light_vector = normalize(p_light.m_position - p_position);
    float l_light_surface_angle = max(0.0f, dot(p_light.m_direction, -l_light_vector));

    float4 l_shading = 0;
    if (l_light_surface_angle >= p_light.m_angle_cos)
    {
        float l_dot = max(0.0f, dot(p_normal, l_light_vector));
        float3 l_diffuse_light = p_light.m_color * l_dot;
    
        float3 l_reflection_vector = normalize(reflect(-l_light_vector, p_normal));
        // camera-to-surface vector
        float3 l_direction_to_camera = normalize(p_camera_pos - p_position);
        // compute specular light
        float l_specular_light = p_specular_intensity * saturate(pow(dot(l_reflection_vector, l_direction_to_camera), p_specular_power));

        // compute attenuation based on distance - linear attenuation
        float l_attenuation = 1.0f - saturate(length(p_light.m_position - p_position) / p_light.m_length);
        float l_angle_attenuation = l_light_surface_angle - p_light.m_angle_cos;
        l_angle_attenuation *= (1.0 / (1 - p_light.m_angle_cos));
        l_angle_attenuation = pow(l_angle_attenuation, 1.5);
        
        l_shading = l_attenuation * l_angle_attenuation * p_light.m_intensity * float4(l_diffuse_light, l_specular_light);

    }

    return l_shading;
}

groupshared int gs_tile_min_z = 2147483647;
groupshared int gs_tile_max_z = -2147483648;
groupshared uint gs_number_of_visible_point_lights = 0;
groupshared uint gs_number_of_visible_spot_lights = 0;
groupshared uint gs_visible_point_light_indices[512];
groupshared uint gs_visible_spot_light_indices[512];

[numthreads(THREAD_GROUP_SIZE, THREAD_GROUP_SIZE, 1)]
void main(uint3 p_group_id : SV_GroupID, uint p_group_index : SV_GroupIndex, uint3 p_group_thread_id : SV_GroupThreadID, uint3 p_dispatch_thread_id : SV_DispatchThreadID)
{
	int2 l_global_texcoord = p_dispatch_thread_id.xy;

    float l_depth = read_texture(g_depth_map, l_global_texcoord).x;
    float4 l_diffuse_map = read_texture(g_diffuse_map, l_global_texcoord);
    float4 l_normal_map = read_texture(g_normal_map, l_global_texcoord);

    float3 l_world_position = bc_reconstruct_world_space(bc_to_screen_space_texcoord(l_global_texcoord, g_screen_width, g_screen_height), l_depth, g_view_proj_inv);
    float3 l_diffuse = l_diffuse_map.xyz;
    float3 l_normal = bc_to_decoded_normal(l_normal_map.xyz);
    float l_specular_intensity = l_diffuse_map.w;
    float l_specular_power = l_normal_map.w * 20;
    
    int l_world_pos_min_z = floor(l_world_position.z);
    int l_world_pos_max_z = ceil(l_world_position.z);

    GroupMemoryBarrierWithGroupSync();

    InterlockedMin(gs_tile_min_z, l_world_pos_min_z);
    InterlockedMax(gs_tile_max_z, l_world_pos_max_z);

    GroupMemoryBarrierWithGroupSync();
    
    uint l_thread_count = THREAD_GROUP_SIZE * THREAD_GROUP_SIZE;
    uint l_point_light_serving_count = (g_point_lights_count + l_thread_count - 1) / l_thread_count;
    uint l_spot_light_serving_count = (g_spot_lights_count + l_thread_count - 1) / l_thread_count;

    for (uint l_p; l_p < l_point_light_serving_count; ++l_p)
    {
        uint l_light_index = l_p + (p_group_index * l_point_light_serving_count);
        if(l_light_index >= g_point_lights_count)
        {
            continue;
        }

        point_light l_light = g_point_lights[l_light_index];
        if (is_light_in_tile(gs_tile_min_z, gs_tile_max_z, l_light.m_min_bound.z, l_light.m_max_bound.z))
        {
            uint l_offset;
            InterlockedAdd(gs_number_of_visible_point_lights, 1, l_offset);

            gs_visible_point_light_indices[l_offset] = l_light_index;
        }
    }

    for (uint l_s; l_s < l_spot_light_serving_count; ++l_s)
    {
        uint l_light_index = l_s + (p_group_index * l_spot_light_serving_count);
        if (l_light_index >= g_spot_lights_count)
        {
            continue;
        }

        spot_light l_light = g_spot_lights[l_light_index];
        if (is_light_in_tile(gs_tile_min_z, gs_tile_max_z, l_light.m_min_bound.z, l_light.m_max_bound.z))
        {
            uint l_offset;
            InterlockedAdd(gs_number_of_visible_spot_lights, 1, l_offset);

            gs_visible_spot_light_indices[l_offset] = l_light_index;
        }
    }

    GroupMemoryBarrierWithGroupSync();

    float4 l_light_map = 0;
    float3 l_ambient_map = 0;

    for (uint l_d = 0; l_d < g_direct_lights_count; ++l_d)
    {
        direct_light l_light = g_direct_lights[l_d];
        l_light_map += direct_light_shading(l_light, g_camera_position, l_world_position, l_normal, l_specular_intensity, l_specular_power);
        l_ambient_map += (l_light.m_ambient_color * l_light.m_ambient_intensity);
    }

    for (uint l_p = 0; l_p < gs_number_of_visible_point_lights; ++l_p)
    {
        uint l_point_light_index = gs_visible_point_light_indices[l_p];
        point_light l_light = g_point_lights[l_point_light_index];

        if (!is_pixel_in_light_range(l_world_position, l_light.m_min_bound, l_light.m_max_bound))
        {
            continue;
        }

        l_light_map += point_light_shading(l_light, g_camera_position, l_world_position, l_normal, l_specular_intensity, l_specular_power);
    }

    for (uint l_s = 0; l_s < gs_number_of_visible_spot_lights; ++l_s)
    {
        uint l_spot_light_index = gs_visible_spot_light_indices[l_s];
        spot_light l_light = g_spot_lights[l_spot_light_index];

        if (!is_pixel_in_light_range(l_world_position, l_light.m_min_bound, l_light.m_max_bound))
        {
            continue;
        }

        l_light_map += spot_light_shading(l_light, g_camera_position, l_world_position, l_normal, l_specular_intensity, l_specular_power);
    }

    float4 l_final_light_map = 0;
    l_final_light_map.xyz = l_light_map.xyz + l_ambient_map;
    l_final_light_map.w = l_light_map.w;

    float4 l_shaded_diffuse = 1;
    l_shaded_diffuse.xyz = (l_diffuse.xyz * l_final_light_map.xyz);
    l_shaded_diffuse.xyz += l_final_light_map.w;

    write_output(l_global_texcoord, l_shaded_diffuse);
}