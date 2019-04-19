// [03/23/2019 MRB]

#include "..\bcRegister.hlsli"
#include "..\bcHelper.hlsli"

#define THREAD_GROUP_SIZE 16

struct direct_light
{
	float3	m_top_left_min_z;
	float3	m_bottom_right_max_z;
	float3	m_direction;
	float3	m_color;
	float	m_intensity;
	float3	m_ambient_color;
	float	m_ambient_intensity;
};

struct point_light
{
	float3	m_top_left_min_z;
	float3	m_bottom_right_max_z;
	float3	m_position;
	float	m_radius;
	float3	m_color;
	float	m_intensity;
};

struct spot_light
{
	float3	m_top_left_min_z;
	float3	m_bottom_right_max_z;
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
    int g_direct_lights_count                       : packoffset(c0.x);
    int g_point_lights_count                        : packoffset(c0.y);
    int g_spot_lights_count                         : packoffset(c0.z);
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

float2 to_screen_space_texcoord(float2 p_texcoord)
{
    return float2(p_texcoord.x * 1.0 / g_screen_width, p_texcoord.y * 1.0 / g_screen_height);
}

float4 direct_light_shading(direct_light p_light, float3 p_camera_pos, float3 p_position, float3 p_normal, float p_specular_intensity, float p_specular_power)
{
    p_specular_power = p_specular_power * 255;
    
    float3 l_light_vector = -normalize(p_light.m_direction);
    float l_dot = max(0.0f, dot(p_normal, l_light_vector));
    float3 l_diffuse_light = p_light.m_color * l_dot;
    
    float3 l_reflection_vector = normalize(reflect(-l_light_vector, p_normal));
    // camera-to-surface vector
    float3 l_direction_to_camera = normalize(p_camera_pos - p_position);
    // compute specular light
    float l_specular_light = p_specular_intensity * pow(saturate(dot(l_reflection_vector, l_direction_to_camera)), p_specular_power);

    // output the two lights
    return p_light.m_intensity * float4(l_diffuse_light, l_specular_light);
}

float4 point_light_shading(point_light p_light, float3 p_camera_pos, float3 p_position, float3 p_normal, float p_specular_intensity, float p_specular_power)
{
    p_specular_power = p_specular_power * 255;
    
    // surface-to-light vector
    float3 l_light_vector = normalize(p_light.m_position - p_position);
    float l_dot = max(0.0f, dot(p_normal, l_light_vector));
    float3 l_diffuse_light = p_light.m_color * l_dot;
    
    float3 l_reflection_vector = normalize(reflect(-l_light_vector, p_normal));
    // camera-to-surface vector
    float3 l_direction_to_camera = normalize(p_camera_pos - p_position);
    // compute specular light
    float l_specular_light = p_specular_intensity * pow(saturate(dot(l_reflection_vector, l_direction_to_camera)), p_specular_power);

    // compute attenuation based on distance - linear attenuation
    float l_attenuation = saturate(1.0f - length(l_light_vector) / p_light.m_radius);

    // output the two lights
    return l_attenuation * p_light.m_intensity * float4(l_diffuse_light, l_specular_light);
}

float4 spot_light_shading(spot_light p_light, float3 p_camera_pos, float3 p_position, float3 p_normal, float p_specular_intensity, float p_specular_power)
{
    p_specular_power = p_specular_power * 255;
    
    // surface-to-light vector
    float3 l_light_vector = normalize(p_light.m_position - p_position);
    float l_light_surface_angle = dot(p_light.m_direction, -l_light_vector);

    float4 l_shading = 0;
    if (l_light_surface_angle >= p_light.m_angle_cos)
    {
        float l_dot = max(0.0f, dot(p_normal, l_light_vector));
        float3 l_diffuse_light = p_light.m_color * l_dot;
    
        float3 l_reflection_vector = normalize(reflect(-l_light_vector, p_normal));
        // camera-to-surface vector
        float3 l_direction_to_camera = normalize(p_camera_pos - p_position);
        // compute specular light
        float l_specular_light = p_specular_intensity * pow(saturate(dot(l_reflection_vector, l_direction_to_camera)), p_specular_power);

        // compute attenuation based on distance - linear attenuation
        float l_attenuation = saturate(1.0f - length(l_light_vector) / p_light.m_length);
        float l_angle_attenuation = pow(l_light_surface_angle, 2);

        l_shading = l_attenuation * l_angle_attenuation * p_light.m_intensity * float4(l_diffuse_light, l_specular_light);
    }

    return l_shading;
}

groupshared uint gs_min_z;
groupshared uint gs_max_z;

[numthreads(THREAD_GROUP_SIZE, THREAD_GROUP_SIZE, 1)]
void main(uint3 p_group_id : SV_GroupID, uint p_group_index : SV_GroupIndex, uint3 p_group_thread_id : SV_GroupThreadID, uint3 p_dispatch_thread_id : SV_DispatchThreadID)
{
	int2 l_global_texcoord = p_dispatch_thread_id.xy;

    float l_depth = read_texture(g_depth_map, l_global_texcoord).x;
    float4 l_diffuse_map = read_texture(g_diffuse_map, l_global_texcoord);
    float4 l_normal_map = read_texture(g_normal_map, l_global_texcoord);

    float4 l_world_position = bc_reconstruct_world_space(to_screen_space_texcoord(l_global_texcoord), l_depth, g_view_proj_inv);
    float3 l_diffuse = l_diffuse_map.xyz;
    float3 l_normal = bc_to_decoded_normal(l_normal_map.xyz);
    float l_specular_intensity = l_diffuse_map.w;
    float l_specular_power = l_normal_map.w;

    uint l_depth_int = (uint) floor(l_world_position.z);
    gs_min_z = 0xffffffff;
    gs_max_z = 0x0;

    GroupMemoryBarrierWithGroupSync();

    InterlockedMin(gs_min_z, l_depth_int);
    InterlockedMax(gs_max_z, l_depth_int);

    GroupMemoryBarrierWithGroupSync();

    float4 l_light_map = 0;

    for (uint i = 0; i < g_direct_lights_count; ++i)
    {
        l_light_map += direct_light_shading(g_direct_lights[i], g_camera_position, l_world_position, l_normal, l_specular_intensity, l_specular_power);
    }

    float4 l_output = length(l_world_position.xyz - g_camera_position) / 3000.0;

    write_output(l_global_texcoord, l_light_map);
}