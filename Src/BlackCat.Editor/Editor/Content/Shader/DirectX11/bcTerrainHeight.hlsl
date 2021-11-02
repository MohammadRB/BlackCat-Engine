// [03/01/2017 MRB]

#include "bcRegister.hlsli"

#define THREAD_GROUP_SIZE 32

// == Resource ===================================================================================

RWTexture2D<float> g_heightmap		: register(BC_COMPUTE_STATE_U0);

cbuffer g_cb_parameter				: register(BC_COMPUTE_STATE_CB0)
{
    uint g_width					: packoffset(c0.x);
    uint g_height					: packoffset(c0.y);
    uint g_chunk_size				: packoffset(c0.z);
    uint g_xz_multiplier			: packoffset(c0.w);
    float g_y_multiplier			: packoffset(c1.x);
	float g_physics_y_scale			: packoffset(c1.y);
    uint g_distance_detail			: packoffset(c1.z);
    uint g_height_detail			: packoffset(c1.w);
};

cbuffer g_cb_parameter1				: register(BC_COMPUTE_STATE_CB1)
{
    uint g_tool_center_x			: packoffset(c0.x);
    uint g_tool_center_z			: packoffset(c0.y);
    uint g_tool_radius				: packoffset(c0.z);
    int g_tool_height				: packoffset(c0.w);
};

// == Helper ======================================================================================

float get_height(uint2 p_texcoord)
{
	// Scale value from float to int based on physics y scale
    float l_height = g_heightmap[p_texcoord];
	int l_physx_height = l_height / g_physics_y_scale;
	l_height = l_physx_height * g_physics_y_scale;

	return l_height;
}

void set_height(uint2 p_texcoord, float p_height)
{
	int l_physx_height = p_height / g_physics_y_scale;
	float l_height = l_physx_height * g_physics_y_scale;

    g_heightmap[p_texcoord] = l_height;
}

// == Shader =====================================================================================

[numthreads(THREAD_GROUP_SIZE, THREAD_GROUP_SIZE, 1)]
void main(uint3 p_group_id : SV_GroupID,
    uint p_group_index : SV_GroupIndex,
    uint3 p_group_thread_id : SV_GroupThreadID,
    uint3 p_dispatch_thread_id : SV_DispatchThreadID)
{
    int2 l_tool_center = int2(g_tool_center_x, g_tool_center_z);
    int2 l_circle_coord = int2(p_dispatch_thread_id.x - g_tool_radius, p_dispatch_thread_id.y - g_tool_radius);
    int2 l_global_coord = l_tool_center + l_circle_coord;
    float l_center_distance = length(l_tool_center - l_global_coord);

    if (l_center_distance > g_tool_radius)
    {
        return;
    }

    float l_height_ratio = 1 - pow(l_center_distance / g_tool_radius, 2);
    float l_height = get_height(l_global_coord);

    l_height += l_height_ratio * g_tool_height;

    set_height(l_global_coord, l_height);
}