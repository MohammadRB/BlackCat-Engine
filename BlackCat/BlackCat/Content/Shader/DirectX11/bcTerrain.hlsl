// [09/02/2016 MRB]

#include "bcRegister.hlsli"

// == Resource ===================================================================================

SamplerState g_heightmap_sampler        : register(BC_RENDER_PASS_STATE_S0);

cbuffer g_cb_render_pass_parameter      : register(BC_RENDER_PASS_STATE_CB1)
{
    float4 g_frustum_planes[6] : packoffset(c0.x);
};

Texture2D g_heightmap                   : register(BC_RENDER_STATE_T0);
StructuredBuffer<int3> g_chunk_info     : register(BC_RENDER_STATE_T1);

cbuffer g_cb_parameter                  : register(BC_RENDER_STATE_CB1)
{
    uint g_width                        : packoffset(c0.x);
    uint g_height                       : packoffset(c0.y);
    uint g_chunk_size                   : packoffset(c0.z);
    uint g_xz_Multiplier                : packoffset(c0.w);
    uint g_y_multiplier                 : packoffset(c1.x);
    uint g_distance_detail              : packoffset(c1.y); // Distance from camera that render will happen with full detail
    uint g_height_detail                : packoffset(c1.z); // Lower values result in higher details
};

// == Input / Output ==============================================================================

struct bc_vs_input
{
    float3 m_position   : POSITION;
    float2 m_texcoord   : TEXCOORD0;
};

struct bc_vs_output
{
    float3 m_position   : POSITION;
    float2 m_texcoord   : TEXCOORD0;
};

struct bc_constant_hs_output
{
    float m_edges[4]    : SV_TessFactor;
    float m_inside[2]   : SV_InsideTessFactor;
};

struct bc_hs_output
{
    float3 m_position   : POSITION;
    float2 m_texcoord   : TEXCOORD0;
};

struct bc_ds_output
{
    float4 m_position   : SV_POSITION;
    float2 m_texcoord   : TEXCOORD0;
    float3 m_pos_w      : TEXCOORD1;
};

struct bc_ps_output
{
    float4 m_color      : SV_Target0;
};

// == Helper =======================================================================================

float get_height(float2 p_texcoord)
{
    return g_heightmap.SampleLevel(g_heightmap_sampler, p_texcoord, 0) * g_y_multiplier;
}

float calc_tess_factor(float3 p_patch_center, int p_patch_diff_height)
{
    float l_max_dis = sqrt(pow(g_width * g_xz_Multiplier, 2) + pow(g_height * g_xz_Multiplier, 2));
    float l_min_dis = (g_chunk_size * g_xz_Multiplier) / 2;
    float l_dis = distance(p_patch_center, g_camera_position) - g_distance_detail;
    float l_tess_factor = 1 - saturate((l_dis - l_min_dis) / (l_max_dis - l_min_dis));
    l_tess_factor = pow(l_tess_factor, 16);
    l_tess_factor *= g_chunk_size;
    l_tess_factor += (p_patch_diff_height + max(0, p_patch_diff_height - int(g_height_detail)));

    return min(g_chunk_size, max(l_tess_factor, 2));
}

bool aabb_inside_plane(float3 p_center, float3 p_extents, float4 p_plane)
{
	// Compute the distance to the center of the box.
    float l_dist = dot(float4(p_center.xyz, 1), p_plane);

    // Project the axes of the box onto the normal of the plane.  Half the
    // length of the projection (sometime called the "radius") is equal to
    // h(u) * abs(n dot b(u))) + h(v) * abs(n dot b(v)) + h(w) * abs(n dot b(w))
    // where h(i) are extents of the box, n is the plane normal, and b(i) are the 
    // axes of the box. In this case b(i) = [(1,0,0), (0,1,0), (0,0,1)].
    float l_radius = dot(p_extents, abs(p_plane.xyz));

    // Outside the plane?
    bool l_outside = (l_dist > l_radius) ? 1 : 0;

    // Fully inside the plane?
    bool l_inside = (l_dist < -l_radius) ? 1 : 0;

    if (l_outside == true)
    {
        return false; // outside
    }
    else if (l_inside == true)
    {
        return true; // inside
    }
    
    return true; // intersect
}

bool aabb_inside_frustum(float3 p_center, float3 p_extents, float4 p_frustum_planes[6])
{
    for (int i = 0; i < 6; ++i)
    {
        if (!aabb_inside_plane(p_center, p_extents, p_frustum_planes[i]))
        {
            return false;
        }
    }
	
    return true;
}

// == Shader =======================================================================================

bc_vs_output vs(bc_vs_input p_input, uint p_vertex_id : SV_VertexID)
{
    bc_vs_output l_output;

    l_output.m_position = p_input.m_position;
    l_output.m_texcoord = p_input.m_texcoord;
    //l_output.m_texcoord.y = 1 - l_output.m_texcoord.y;

    return l_output;
}

bc_constant_hs_output constant_hs(InputPatch<bc_vs_output, 4> p_input, uint p_patch_id : SV_PrimitiveID)
{
    bc_constant_hs_output l_output;

    float3 l_patch_info = g_chunk_info[p_patch_id];
    float3 l_left_patch_info;
    float3 l_right_patch_info;
    float3 l_top_patch_info;
    float3 l_bottom_patch_info;

    // Build axis-aligned bounding box.  patch[0] is lower-left corner and patch[2] is upper-right corner.
    float3 l_min_vertex = float3(p_input[0].m_position.x, l_patch_info.x, p_input[0].m_position.z);
    float3 l_max_vertex = float3(p_input[2].m_position.x, l_patch_info.y, p_input[2].m_position.z);
	
    float3 l_box_center = 0.5f * (l_min_vertex + l_max_vertex);
    float3 l_box_extents = 0.5f * (l_max_vertex - l_min_vertex);

    if (!aabb_inside_frustum(l_box_center, l_box_extents, g_frustum_planes))
    {
        l_output.m_edges[0] = 0.0f;
        l_output.m_edges[1] = 0.0f;
        l_output.m_edges[2] = 0.0f;
        l_output.m_edges[3] = 0.0f;
		
        l_output.m_inside[0] = 0.0f;
        l_output.m_inside[1] = 0.0f;
		
        return l_output;
    }

    uint l_width_chunk_count = g_width / g_chunk_size;
    uint l_height_chunk_count = g_height / g_chunk_size;
    uint l_chunk_size = g_chunk_size * g_xz_Multiplier;
    
    int2 l_patch_index = uint2(p_patch_id % l_width_chunk_count, p_patch_id / l_width_chunk_count);
    int2 l_left_patch_index = l_patch_index;
    l_left_patch_index.x = max(0, l_left_patch_index.x - 1);
    int2 l_right_patch_index = l_patch_index;
    l_right_patch_index.x = min(l_width_chunk_count - 1, l_right_patch_index.x + 1);
    int2 l_top_patch_index = l_patch_index;
    l_top_patch_index.y = min(l_height_chunk_count - 1, l_top_patch_index.y + 1);
    int2 l_bottom_patch_index = l_patch_index;
    l_bottom_patch_index.y = max(0, l_bottom_patch_index.y - 1);

    uint l_patch_id = l_patch_index.y * l_width_chunk_count + l_patch_index.x;
    uint l_left_patch_id = l_left_patch_index.y * l_width_chunk_count + l_left_patch_index.x;
    uint l_right_patch_id = l_right_patch_index.y * l_width_chunk_count + l_right_patch_index.x;
    uint l_top_patch_id = l_top_patch_index.y * l_width_chunk_count + l_top_patch_index.x;
    uint l_bottom_patch_id = l_bottom_patch_index.y * l_width_chunk_count + l_bottom_patch_index.x;

    l_left_patch_info = g_chunk_info[l_left_patch_id];
    l_right_patch_info = g_chunk_info[l_right_patch_id];
    l_top_patch_info = g_chunk_info[l_top_patch_id];
    l_bottom_patch_info = g_chunk_info[l_bottom_patch_id];

    float3 l_patch_center = float3(p_input[0].m_position.x, 0, p_input[0].m_position.z);
    l_patch_center.xz += (l_chunk_size / 2);
    l_patch_center.y = (l_patch_info.x + l_patch_info.y) / 2;
    float3 l_left_patch_center = l_patch_center;
    l_left_patch_center.x -= (l_chunk_size * abs(l_left_patch_index.x - l_patch_index.x));
    l_left_patch_center.y = (l_left_patch_info.x + l_left_patch_info.y) / 2;
    float3 l_right_patch_center = l_patch_center;
    l_right_patch_center.x += (l_chunk_size * (l_right_patch_index.x - l_patch_index.x));
    l_right_patch_center.y = (l_right_patch_info.x + l_right_patch_info.y) / 2;
    float3 l_top_patch_center = l_patch_center;
    l_top_patch_center.z += (l_chunk_size * (l_top_patch_index.y - l_patch_index.y));
    l_top_patch_center.y = (l_top_patch_info.x + l_top_patch_info.y) / 2;
    float3 l_bottom_patch_center = l_patch_center;
    l_bottom_patch_center.z -= (l_chunk_size * abs(l_bottom_patch_index.y - l_patch_index.y));
    l_bottom_patch_center.y = (l_bottom_patch_info.x + l_bottom_patch_info.y) / 2;

    int l_patch_height_diff = l_patch_info.z;
    int l_left_patch_height_diff = l_left_patch_info.z;
    int l_right_patch_height_diff = l_right_patch_info.z;
    int l_top_patch_height_diff = l_top_patch_info.z;
    int l_bottom_patch_height_diff = l_bottom_patch_info.z;

    float l_patch_tess_factor = calc_tess_factor(l_patch_center, l_patch_height_diff);
    float l_left_patch_tess_factor = calc_tess_factor(l_left_patch_center, l_left_patch_height_diff);
    float l_right_patch_tess_factor = calc_tess_factor(l_right_patch_center, l_right_patch_height_diff);
    float l_top_patch_tess_factor = calc_tess_factor(l_top_patch_center, l_top_patch_height_diff);
    float l_bottom_patch_tess_factor = calc_tess_factor(l_bottom_patch_center, l_bottom_patch_height_diff);

    l_output.m_edges[0] = min(l_patch_tess_factor, l_left_patch_tess_factor);
    l_output.m_edges[1] = min(l_patch_tess_factor, l_bottom_patch_tess_factor);
    l_output.m_edges[2] = min(l_patch_tess_factor, l_right_patch_tess_factor);
    l_output.m_edges[3] = min(l_patch_tess_factor, l_top_patch_tess_factor);
    l_output.m_inside[0] = l_output.m_inside[1] = l_patch_tess_factor;

    return l_output;
}

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(4)]
[patchconstantfunc("constant_hs")]
[maxtessfactor(64.0f)]
bc_hs_output hs(InputPatch<bc_vs_output, 4> p_input, uint i : SV_OutputControlPointID, uint p_patch_id : SV_PrimitiveID)
{
    bc_hs_output l_output;

    l_output.m_position = p_input[i].m_position;
    l_output.m_texcoord = p_input[i].m_texcoord;

    return l_output;
}

[domain("quad")]
bc_ds_output ds(bc_constant_hs_output p_input, float2 UV : SV_DomainLocation, const OutputPatch<bc_hs_output, 4> p_quad)
{
    bc_ds_output l_output = (bc_ds_output) 0;

	//Interpolation to find each position the generated vertices
    float3 l_vertical_pos1 = lerp(p_quad[0].m_position, p_quad[1].m_position, UV.y);
    float3 l_vertical_pos2 = lerp(p_quad[3].m_position, p_quad[2].m_position, UV.y);
    float3 l_final_pos = lerp(l_vertical_pos1, l_vertical_pos2, UV.x);

	//Interpolation to find each texcoord the generated vertices
    float2 l_texcoord1 = lerp(p_quad[0].m_texcoord, p_quad[1].m_texcoord, UV.y);
    float2 l_texcoord2 = lerp(p_quad[3].m_texcoord, p_quad[2].m_texcoord, UV.y);
    float2 l_final_texcoord = lerp(l_texcoord1, l_texcoord2, UV.x);

    l_final_pos.y = get_height(l_final_texcoord);

    l_output.m_position = mul(float4(l_final_pos, 1), g_viewprojection);
    l_output.m_pos_w = l_final_pos;
    l_output.m_texcoord = l_final_texcoord;

    return l_output;
}

bc_ps_output ps(bc_ds_output p_input)
{
    bc_ps_output l_output = (bc_ps_output) 0;

    l_output.m_color = float4(1, 1, 1, 1);
	
    return l_output;
}