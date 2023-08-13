// [12/08/2023 MRB]

#include "../bcRegister.hlsli"
#include "../bcHelper.hlsli"

SamplerState g_sam_point					: register(BC_RENDER_PASS_STATE_S0);

Texture2D g_tex2d_depth						: register(BC_RENDER_PASS_STATE_T0);

cbuffer g_cb_params							: register(BC_RENDER_PASS_STATE_CB1)
{
	// ordered by top-left top-right bottom-right bottom-left
	float4 g_frustum_vectors[4]				: packoffset(c0);
}

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

float3 get_world_position(float p_depth, float3 p_frustum_vector)
{
	return bc_reconstruct_world_position(p_depth, p_frustum_vector, g_camera_position, g_near_plane, g_far_plane);
}

bc_vs_output vol_fog_vs(uint p_vertex_id : SV_VertexID)
{
	bc_vs_output l_output;
	l_output.m_position = float4(g_ss_quad[p_vertex_id], 0, 1);
	l_output.m_texcoord = g_ss_quad_tex[p_vertex_id];
	l_output.m_view_frustum_vector = g_frustum_vectors[g_view_frustum_vector_indexes[p_vertex_id]].xyz;

	return l_output;
}

float4 vol_fog_ps(bc_vs_output p_input) : SV_Target0
{
	return 0.5;
}