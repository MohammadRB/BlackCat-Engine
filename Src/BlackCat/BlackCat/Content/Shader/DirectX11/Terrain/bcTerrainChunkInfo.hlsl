// [09/06/2016 MRB]

#include "..//bcRegister.hlsli"

#define THREAD_GROUP_SIZE 32

// == Resource ===================================================================================

Texture2D<float> g_heightmap				: register(BC_COMPUTE_STATE_T0);
RWStructuredBuffer<int3> g_chunk_info		: register(BC_COMPUTE_STATE_U0);

cbuffer g_cb_parameter						: register(BC_COMPUTE_STATE_CB0)
{
    uint g_width							: packoffset(c0.x);
    uint g_height							: packoffset(c0.y);
    uint g_chunk_size						: packoffset(c0.z);
    uint g_xz_multiplier					: packoffset(c0.w);
    float g_y_multiplier					: packoffset(c1.x);
	float g_physics_y_scale					: packoffset(c1.y);
    uint g_distance_detail					: packoffset(c1.z);		// Distance from camera that render will happen with full detail
    uint g_height_detail					: packoffset(c1.w);		// Lower values result in higher details
};

// == Helper ======================================================================================

float4 create_plane_from_point_and_normal(float3 p_point, float3 p_normal)
{
    return float4(p_normal, /*-*/(p_normal.x * p_point.x + p_normal.y * p_point.y + p_normal.z * p_point.z));
}

float compute_distance_from_plane(float4 p_plane, float3 p_position)
{
    return dot(p_plane.xyz, p_position) - p_plane.w;
}

float get_height(uint2 p_texcoord)
{
    p_texcoord.y = g_height - p_texcoord.y; // In directx TexCoord (0,0) is top-left but we begin from bottom-left
    return g_heightmap.Load(int3(p_texcoord, 0));
}

uint2 get_chunk_id(uint2 p_dispatch_thread_id)
{
    uint2 l_chunk_id = uint2(p_dispatch_thread_id.x / g_chunk_size, p_dispatch_thread_id.y / g_chunk_size);
    
    return l_chunk_id;
}

uint get_chunk_index(uint2 p_dispatch_thread_id)
{
    uint2 l_width_chunk_count = g_width / g_chunk_size;
    uint2 l_chunk_id = get_chunk_id(p_dispatch_thread_id);
    
    return l_chunk_id.y * l_width_chunk_count + l_chunk_id.x;
}

// == Shader ======================================================================================

groupshared float3 gs_height[THREAD_GROUP_SIZE][THREAD_GROUP_SIZE]; // (min, max, height)
groupshared float3 gs_corners[4];
groupshared float3 gs_normals[4];
groupshared float4 gs_plane;

[numthreads(THREAD_GROUP_SIZE, THREAD_GROUP_SIZE, 1)]
void main(uint3 p_group_id : SV_GroupID, 
    uint p_group_index : SV_GroupIndex, 
    uint3 p_group_thread_id : SV_GroupThreadID,
    uint3 p_dispatch_thread_id : SV_DispatchThreadID)
{
    // Load all heights to group shared memory
    float l_height = get_height(p_dispatch_thread_id.xy);
    gs_height[p_group_thread_id.x][p_group_thread_id.y].z = l_height;

    // Wait for all threads to load
    GroupMemoryBarrierWithGroupSync();
    
    uint l_step = 1;
    uint l_divider = 2;
    
    // Divide shared memory into n*n blocks (n is power of two, starting from 2 and increase upto thread group size) 
    // and compute min max values in every block and store in top-left entry of block and continue by merging results
    // from prev step
    while (THREAD_GROUP_SIZE / l_divider >= 1)
    {
        if (p_group_thread_id.x % l_divider == 0 && p_group_thread_id.y % l_divider == 0)
        {
            float l_height1 = gs_height[p_group_thread_id.x][p_group_thread_id.y].z;
            float l_height2 = gs_height[p_group_thread_id.x + l_step][p_group_thread_id.y].z;
            float l_height3 = gs_height[p_group_thread_id.x][p_group_thread_id.y + l_step].z;
            float l_height4 = gs_height[p_group_thread_id.x + l_step][p_group_thread_id.y + l_step].z;

            float l_min = min(min(l_height1, l_height2), min(l_height3, l_height4));
            float l_max = max(max(l_height1, l_height2), max(l_height3, l_height4));

            gs_height[p_group_thread_id.x][p_group_thread_id.y].xy = float2(l_min, l_max);
        }

        l_step = l_divider;
        l_divider *= 2;
    }

    // Use first thread in group to write min max result. There is no need to use memory barrier here because
    // first thread is the last thread that leave while loop
    if (p_group_thread_id.x == 0 && p_group_thread_id.y == 0)
    {
        uint l_chunk_index = get_chunk_index(p_dispatch_thread_id.xy);
         // Convert float min max to int becuase interlocked operations only work with ints
        int2 l_thread_group_minmax = int2(floor(gs_height[0][0].x), ceil(gs_height[0][0].y));

        InterlockedMin(g_chunk_info[l_chunk_index].x, l_thread_group_minmax.x);
        InterlockedMax(g_chunk_info[l_chunk_index].y, l_thread_group_minmax.y);
    }

    // Use four idle thread that didn't used in min-max calculation
    if (p_group_thread_id.x == 1 && p_group_thread_id.y == 0)
    {
        uint2 l_chunk_id = get_chunk_id(p_dispatch_thread_id.xy);
        uint2 l_texcoord = uint2(l_chunk_id.x * g_chunk_size, l_chunk_id.y * g_chunk_size);

        float l_height = get_height(l_texcoord);

        gs_corners[0] = float3(l_texcoord.x, l_height, l_texcoord.y);
    }
    if (p_group_thread_id.x == 3 && p_group_thread_id.y == 0)
    {
        uint2 l_chunk_id = get_chunk_id(p_dispatch_thread_id.xy);
        uint2 l_texcoord = uint2(l_chunk_id.x * g_chunk_size, l_chunk_id.y * g_chunk_size);
        l_texcoord.y += g_chunk_size - 1;

        float l_height = get_height(l_texcoord);

        gs_corners[1] = float3(l_texcoord.x, l_height, l_texcoord.y);
    }
    if (p_group_thread_id.x == 5 && p_group_thread_id.y == 0)
    {
        uint2 l_chunk_id = get_chunk_id(p_dispatch_thread_id.xy);
        uint2 l_texcoord = uint2(l_chunk_id.x * g_chunk_size, l_chunk_id.y * g_chunk_size);
        l_texcoord.xy += g_chunk_size - 1;

        float l_height = get_height(l_texcoord);

        gs_corners[2] = float3(l_texcoord.x, l_height, l_texcoord.y);
    }
    if (p_group_thread_id.x == 7 && p_group_thread_id.y == 0)
    {
        uint2 l_chunk_id = get_chunk_id(p_dispatch_thread_id.xy);
        uint2 l_texcoord = uint2(l_chunk_id.x * g_chunk_size, l_chunk_id.y * g_chunk_size);
        l_texcoord.x += g_chunk_size - 1;

        float l_height = get_height(l_texcoord);

        gs_corners[3] = float3(l_texcoord.x, l_height, l_texcoord.y);
    }

    // Wait for all four corner be ready
    GroupMemoryBarrierWithGroupSync();

    if (p_group_thread_id.x == 1 && p_group_thread_id.y == 0)
    {
        gs_normals[0] = normalize(cross
                            (
                                gs_corners[1] - gs_corners[0],
                                gs_corners[3] - gs_corners[0]
                            ));
    }
    if (p_group_thread_id.x == 3 && p_group_thread_id.y == 0)
    {
        gs_normals[1] = normalize(cross
                            (
                                gs_corners[2] - gs_corners[1],
                                gs_corners[0] - gs_corners[1]
                            ));
    }
    if (p_group_thread_id.x == 5 && p_group_thread_id.y == 0)
    {
        gs_normals[2] = normalize(cross
                            (
                                gs_corners[3] - gs_corners[2],
                                gs_corners[1] - gs_corners[2]
                            ));
    }
    if (p_group_thread_id.x == 7 && p_group_thread_id.y == 0)
    {
        gs_normals[3] = normalize(cross
                            (
                                gs_corners[0] - gs_corners[3],
                                gs_corners[2] - gs_corners[3]
                            ));
    }

    // Wait for all four normal be ready
    GroupMemoryBarrierWithGroupSync();

    if (p_group_thread_id.x == 1 && p_group_thread_id.y == 0)
    {
        float3 l_point0 = gs_corners[0];
        float3 l_point1 = gs_corners[1];
        float3 l_point2 = gs_corners[2];
        float3 l_point3 = gs_corners[3];
        float3 l_point = l_point0;
        l_point = l_point1.y < l_point.y ? l_point1 : l_point;
        l_point = l_point2.y < l_point.y ? l_point2 : l_point;
        l_point = l_point3.y < l_point.y ? l_point3 : l_point;

        float3 l_normal = normalize
            (
                gs_normals[0] +
                gs_normals[1] +
                gs_normals[2] +
                gs_normals[3]
            );

        gs_plane = create_plane_from_point_and_normal(l_point, l_normal);
    }

    // Wait until plane equation is ready
    GroupMemoryBarrierWithGroupSync();

    float l_distance_from_plane = compute_distance_from_plane(gs_plane, float3(p_dispatch_thread_id.x, l_height, p_dispatch_thread_id.y));

    gs_height[p_group_thread_id.x][p_group_thread_id.y].z = abs(l_distance_from_plane);

    // Wait for all threads to write their distance
    GroupMemoryBarrierWithGroupSync();

    l_step = 1;
    l_divider = 2;
    
    while (THREAD_GROUP_SIZE / l_divider >= 1)
    {
        if (p_group_thread_id.x % l_divider == 0 && p_group_thread_id.y % l_divider == 0)
        {
            float l_distance1 = gs_height[p_group_thread_id.x][p_group_thread_id.y].z;
            float l_distance2 = gs_height[p_group_thread_id.x + l_step][p_group_thread_id.y].z;
            float l_distance3 = gs_height[p_group_thread_id.x][p_group_thread_id.y + l_step].z;
            float l_distance4 = gs_height[p_group_thread_id.x + l_step][p_group_thread_id.y + l_step].z;

            float l_distance = (l_distance1 + l_distance2 + l_distance3 + l_distance4) / 4;

            gs_height[p_group_thread_id.x][p_group_thread_id.y].z = l_distance;
        }

        l_step = l_divider;
        l_divider *= 2;
    }

    // Use first thread in group to write plane distance result
    if (p_group_thread_id.x == 0 && p_group_thread_id.y == 0)
    {
        uint l_chunk_index = get_chunk_index(p_dispatch_thread_id.xy);
         // Convert float min max to int becuase interlocked operations only work with ints
        float l_thread_group_distance = gs_height[0][0].z;

        uint l_num_thread_group_per_chunk = (g_chunk_size / THREAD_GROUP_SIZE);
        l_num_thread_group_per_chunk *= l_num_thread_group_per_chunk;

        l_thread_group_distance *= (1.0 / l_num_thread_group_per_chunk);

        InterlockedAdd(g_chunk_info[l_chunk_index].z, round(l_thread_group_distance));
    }
}