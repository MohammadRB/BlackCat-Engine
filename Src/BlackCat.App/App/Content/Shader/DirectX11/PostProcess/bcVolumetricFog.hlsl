// [12/08/2023 MRB]

#include "../bcRegister.hlsli"
#include "../bcHelper.hlsli"

struct bc_fog_instance
{
	float4x4 m_world_view_projection;
};

SamplerState g_sam_point							: register(BC_RENDER_PASS_STATE_S0);

Texture2D g_tex2d_depth								: register(BC_RENDER_PASS_STATE_T0);
StructuredBuffer<bc_fog_instance> g_buf_instances	: register(BC_RENDER_PASS_STATE_T1);

cbuffer g_cb_params									: register(BC_RENDER_PASS_STATE_CB1)
{
	// ordered by top-left top-right bottom-right bottom-left
	float4 g_frustum_vectors[4]						: packoffset(c0);
}

struct bc_vs_input
{
	float3 m_position								: POSITION0;
};

struct bc_vs_output
{
	float4 m_position								: SV_POSITION;
	float3 m_view_frustum_vector					: FRUSTUM_VECTOR;
};

float2 snap_to_texel(float2 p_uv, float2 p_screen_coords)
{
	return round(p_uv * p_screen_coords) * rcp(p_screen_coords);
}

float3 get_world_position(float p_depth, float3 p_frustum_vector)
{
	return bc_reconstruct_world_position(p_depth, p_frustum_vector, g_camera_position, g_near_plane, g_far_plane);
}

bc_vs_output vol_fog_vs(bc_vs_input p_input, uint p_instance_index : SV_InstanceID)
{
	bc_vs_output l_output;
	const bc_fog_instance l_instance = g_buf_instances.Load(p_instance_index);

	l_output.m_position = mul(float4(p_input.m_position, 1), l_instance.m_world_view_projection);
	l_output.m_position = mul(l_output.m_position, g_view_projection);

	return l_output;
}

float4 vol_fog_ps(bc_vs_output p_input) : SV_Target0
{
	return 0.5;
}