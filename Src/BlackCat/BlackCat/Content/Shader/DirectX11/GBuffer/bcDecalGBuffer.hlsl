// [02/29/2021 MRB]

#include "../bcRegister.hlsli"
#include "../bcHelper.hlsli"

struct bc_decal_instance
{
	float4x4 m_world_inv;
	float4x4 m_world_view_projection;
	float m_u0;
	float m_v0;
	float m_u1;
	float m_v1;
	uint m_group;
};

SamplerState g_sam_point								: register(BC_RENDER_PASS_STATE_S0);
SamplerState g_sam_linear								: register(BC_RENDER_PASS_STATE_S1);

Texture2D g_tex2d_depth									: register(BC_RENDER_PASS_STATE_T0);
StructuredBuffer<bc_decal_instance> g_buf_instances		: register(BC_RENDER_PASS_STATE_T1);

Texture2D g_tex2d_diffuse								: register(BC_RENDER_STATE_T0);
Texture2D g_tex2d_normal								: register(BC_RENDER_STATE_T1);
Texture2D g_tex2d_specular								: register(BC_RENDER_STATE_T2);

struct bc_vs_input
{
	float3 m_position : POSITION0;
};

struct bc_vs_output
{
	float4 m_position : SV_POSITION;
	float4 m_cs_position : TEXCOORD0;
	float4x4 m_world_inv : TEXCOORD1;
};

struct bc_ps_output
{
	float4 m_diffuse : SV_Target0;
	float4 m_normal : SV_Target1;
};

bc_vs_output vs(bc_vs_input p_input, uint p_instance_index : SV_InstanceID)
{
	bc_vs_output l_output;
	const bc_decal_instance l_instance = g_buf_instances.Load(p_instance_index);
	
	l_output.m_position = mul(float4(p_input.m_position, 1), l_instance.m_world_view_projection);
	l_output.m_cs_position = l_output.m_position;
	l_output.m_world_inv = l_instance.m_world_inv;
	
	return l_output;
}

bc_ps_output ps(bc_vs_output p_input)
{
	bc_ps_output l_output;

	const float4 l_clip_space = p_input.m_cs_position / p_input.m_cs_position.w;
	const float2 l_texcoord = bc_clip_space_to_texcoord(p_input.m_cs_position);
	const float l_depth = g_tex2d_depth.Sample(g_sam_point, l_texcoord).x;

	if (!CAMERA_INSIDE)
	{
		clip(l_depth - (l_clip_space.z));
	}

	const float3 l_pixel_world = bc_reconstruct_world_space(l_clip_space.xy, l_depth, g_view_projection_inv);
	const float3 l_pixel_local = mul(float4(l_pixel_world, 1), p_input.m_world_inv).xyz;

	clip(0.5 - abs(l_pixel_local));
	
	l_output.m_diffuse = float4(1, 1, 1, 0.5);
	l_output.m_normal = float4(0, 0, 0, 0);
	
	return l_output;
}