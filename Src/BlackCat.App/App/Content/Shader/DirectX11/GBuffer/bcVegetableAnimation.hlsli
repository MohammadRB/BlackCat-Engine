#include "../bcHelper.hlsli"

static const float g_vegetable_max_wind_power = 5;
static const float g_vegetable_max_height = 20;

float bc_saturate_wind_power(float p_wind_power)
{
    return min(p_wind_power, g_vegetable_max_wind_power);
}

float3 bc_vegetable_animation(float3 p_local_pos, float3 p_world_pos, float p_height, bool p_leaf, float3 p_wind_dir, float p_wind_power, float p_time)
{
	const float l_wind_power = bc_saturate_wind_power(p_wind_power);
	const float l_wind_power_scale = l_wind_power / g_vegetable_max_wind_power;

	const float l_time_frequency = p_time * l_wind_power_scale;
    float l_noise = bc_noise(l_time_frequency / 13 + length(p_world_pos / 10), 0.2f);
    l_noise = (l_noise / 2.0f + 0.5f) - 0.3f;

	const float l_xz_length = length(p_local_pos.xz);
	const float l_xyz_length = length(p_local_pos.xyz);

	const float l_wind_power_influence = l_xz_length + l_wind_power * 1.1;
	const float l_height_influence = pow(saturate(p_local_pos.y / p_height), 2);
	const float l_vertex_moment_length = l_noise * l_wind_power_influence * l_height_influence;
	const float3 l_vertex_moment = normalize(p_wind_dir) * l_vertex_moment_length;
    float3 l_new_dir = normalize(p_local_pos + l_vertex_moment);
    float3 l_new_pos = l_new_dir * l_xyz_length;
	
    if (p_leaf)
    {
        float l_noise_leaf = bc_noise(l_time_frequency / 3 + l_xz_length, 0.1);
        const float l_center_offset_influence = pow(saturate(l_xz_length / 2), 2);

        l_noise_leaf *= l_wind_power_scale;
        l_noise_leaf *= l_center_offset_influence;
        
		float3 l_noise_dir = 0;
        l_noise_dir.xz = 1 - abs(normalize(p_local_pos.xz));
        l_new_pos += l_noise_dir * l_noise_leaf;
    }

    return l_new_pos - p_local_pos;
}

float3 bc_do_vegetable_animation(float3 p_local_pos, float4x4 p_world, float p_height, bool p_leaf, float3 p_wind_dir, float p_wind_power, float p_time)
{
    float3 l_local_position = mul(p_local_pos, (float3x3) p_world);
    const float3 l_world_position = float3(p_world._41, p_world._42, p_world._43);
    const float3 l_animation = bc_vegetable_animation(l_local_position, l_world_position, p_height, p_leaf, p_wind_dir, p_wind_power, p_time);
	
    l_local_position += l_animation;
    l_local_position += l_world_position;

    return l_local_position;
}