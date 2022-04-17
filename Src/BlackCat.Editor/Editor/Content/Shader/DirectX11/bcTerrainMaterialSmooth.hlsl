// [03/24/2017 MRB]

#include "bcRegister.hlsli"

#define THREAD_GROUP_SIZE 32

// == Resource ===================================================================================

Texture2D<uint4> g_texture_map_srv			: register(BC_COMPUTE_STATE_T0);
RWTexture2D<uint4> g_texture_map_uav		: register(BC_COMPUTE_STATE_U0);

cbuffer g_cb_parameter						: register(BC_COMPUTE_STATE_CB0)
{
    uint g_width							: packoffset(c0.x);
    uint g_height							: packoffset(c0.y);
    uint g_chunk_size						: packoffset(c0.z);
    uint g_xz_multiplier					: packoffset(c0.w);
    float g_y_multiplier					: packoffset(c1.x);
	float g_physics_y_scale					: packoffset(c1.y);
    uint g_distance_detail					: packoffset(c1.z);
    uint g_height_detail					: packoffset(c1.w);
	uint g_texture_map_width				: packoffset(c2.x);
	uint g_texture_map_height				: packoffset(c2.y);
};

cbuffer g_cb_parameter1						: register(BC_COMPUTE_STATE_CB1)
{
    uint g_tool_center_x					: packoffset(c0.x);
    uint g_tool_center_z					: packoffset(c0.y);
    uint g_tool_radius						: packoffset(c0.z);
    uint g_tool_smooth						: packoffset(c0.w);
};

// == Helper ======================================================================================

uint4 get_texture_map(uint2 p_texcoord)
{
    return g_texture_map_srv[p_texcoord];
}

void set_texture_map(uint2 p_texcoord, uint4 p_value)
{
    g_texture_map_uav[p_texcoord] = p_value;
}

// == Shader =====================================================================================

[numthreads(THREAD_GROUP_SIZE, THREAD_GROUP_SIZE, 1)]
void main(uint3 p_group_id : SV_GroupID, uint p_group_index : SV_GroupIndex, uint3 p_group_thread_id : SV_GroupThreadID, uint3 p_dispatch_thread_id : SV_DispatchThreadID)
{
	const int2 l_tool_center = int2(g_tool_center_x, g_tool_center_z);
	const int2 l_circle_coord = int2(p_dispatch_thread_id.x - g_tool_radius, p_dispatch_thread_id.y - g_tool_radius);
    const int2 l_global_coord = l_tool_center + l_circle_coord;
	const float l_center_distance = length(l_tool_center - l_global_coord);

    if (l_center_distance > g_tool_radius)
    {
        return;
    }

    uint l_unpacked_values[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

	const uint l_min_x = max(0, l_global_coord.x - (int)g_tool_smooth);
	const uint l_min_z = max(0, l_global_coord.y - (int)g_tool_smooth);
	const uint l_max_x = min(g_texture_map_width, l_global_coord.x + g_tool_smooth);
	const uint l_max_z = min(g_texture_map_height, l_global_coord.y + g_tool_smooth);
    uint l_num_sample = 0;

    for (int x = l_min_x; x <= l_max_x; ++x)
    {
        for (int z = l_min_z; z <= l_max_z; ++z)
        {
	        const uint2 l_texcoord = uint2(x, z);

            uint4 l_value = get_texture_map(l_texcoord);
            
            l_unpacked_values[0] += (l_value.x >> 8);
            l_unpacked_values[1] += (l_value.x & 255);
            l_unpacked_values[2] += (l_value.y >> 8);
            l_unpacked_values[3] += (l_value.y & 255);
            l_unpacked_values[4] += (l_value.z >> 8);
            l_unpacked_values[5] += (l_value.z & 255);
            l_unpacked_values[6] += (l_value.w >> 8);
            l_unpacked_values[7] += (l_value.w & 255);

            ++l_num_sample;
        }
    }
    
    for (uint i = 0; i < 8; ++i)
    {
        l_unpacked_values[i] /= l_num_sample;
    }

    // Normalize values
    uint l_sum = 0;
    uint l_diff;

    for (uint i = 0; i < 8; ++i)
    {
        l_sum += l_unpacked_values[i];
    }

    l_diff = 255 - l_sum;

    for (uint i = 0; i < 8; ++i)
    {
	    const float l_multiplier = l_unpacked_values[i] / 255.0;
	    const uint l_normalization = l_multiplier * l_diff;

        l_unpacked_values[i] += l_normalization;
    }

    uint4 l_final_value = 0;
    l_final_value.x = (l_unpacked_values[0] << 8) + (l_unpacked_values[1]);
    l_final_value.y = (l_unpacked_values[2] << 8) + (l_unpacked_values[3]);
    l_final_value.z = (l_unpacked_values[4] << 8) + (l_unpacked_values[5]);
    l_final_value.w = (l_unpacked_values[6] << 8) + (l_unpacked_values[7]);

    set_texture_map(l_global_coord, l_final_value);
}