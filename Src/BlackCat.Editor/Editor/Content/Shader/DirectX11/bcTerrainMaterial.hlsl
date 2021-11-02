// [03/01/2017 MRB]

#include "bcRegister.hlsli"

#define THREAD_GROUP_SIZE 32

// == Resource ===================================================================================

RWTexture2D<uint4> g_texturemap_uav			: register(BC_COMPUTE_STATE_U0);

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
	uint g_texturemap_width					: packoffset(c2.x);
	uint g_texturemap_height				: packoffset(c2.y);
};

cbuffer g_cb_parameter1						: register(BC_COMPUTE_STATE_CB1)
{
    uint g_tool_center_x					: packoffset(c0.x);
    uint g_tool_center_z					: packoffset(c0.y);
    uint g_tool_radius						: packoffset(c0.z);
    uint g_tool_material					: packoffset(c0.w);
};

// == Helper ======================================================================================

uint4 get_texture_map(uint2 p_texcoord)
{
    return g_texturemap_uav[p_texcoord];
}

void set_texture_map(uint2 p_texcoord, uint4 p_value)
{
    g_texturemap_uav[p_texcoord] = p_value;
}

uint4 set_material(int p_material_index)
{
    uint4 l_texturemap = 0;

    switch (p_material_index)
    {
        case 0:
        {
            const uint l_new_value = 255 << 8;
            l_texturemap.x = l_new_value;
            break;
        }
        case 1:
        {
	        const uint l_new_value = 255;
            l_texturemap.x = l_new_value;
            break;
        }
        case 2:
        {
            const uint l_new_value = 255 << 8;
            l_texturemap.y = l_new_value;
            break;
        }
        case 3:
        {
            const uint l_new_value = 255;
            l_texturemap.y = l_new_value;
            break;
        }
        case 4:
        {
            const uint l_new_value = 255 << 8;
            l_texturemap.z = l_new_value;
            break;
        }
        case 5:
        {
            const uint l_new_value = 255;
            l_texturemap.z = l_new_value;
            break;
        }
        case 6:
        {
            const uint l_new_value = 255 << 8;
            l_texturemap.w = l_new_value;
            break;
        }
        case 7:
        {
            const uint l_new_value = 255;
            l_texturemap.w = l_new_value;
            break;
        }
    }

    return l_texturemap;
}

// == Shader =====================================================================================

[numthreads(THREAD_GROUP_SIZE, THREAD_GROUP_SIZE, 1)]
void main(uint3 p_group_id : SV_GroupID,
    uint p_group_index : SV_GroupIndex,
    uint3 p_group_thread_id : SV_GroupThreadID,
    uint3 p_dispatch_thread_id : SV_DispatchThreadID)
{
	const int2 l_tool_center = int2(g_tool_center_x, g_tool_center_z);
	const int2 l_circle_coord = int2(p_dispatch_thread_id.x - g_tool_radius, p_dispatch_thread_id.y - g_tool_radius);
	const int2 l_global_coord = l_tool_center + l_circle_coord;
	const float l_center_distance = length(l_tool_center - l_global_coord);

    if (l_center_distance > g_tool_radius)
    {
        return;
    }

	const uint4 l_texture_map = set_material(g_tool_material);

    set_texture_map(l_global_coord, l_texture_map);
}