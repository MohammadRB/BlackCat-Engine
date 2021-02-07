// [03/10/2019 MRB]

#include "../bcRegister.hlsli"

struct bc_vs_input
{
	float3 m_position : POSITION0;
	float2 m_texcoord : TEXCOORD0;
	float3 m_normal : NORMAL0;
	float3 m_tangent : TANGENT0;
	int4 m_bone_indices : BLENDINDICES0;
	float4 m_bone_weights : BLENDWEIGHT0;
};

struct bc_vs_output
{
	float4 m_position : SV_POSITION;
};

bc_vs_output vs(bc_vs_input p_input)
{
	bc_vs_output l_output;

	float4x4 l_transform = g_bone_transforms[p_input.m_bone_indices.x] * p_input.m_bone_weights[0];
	l_transform += g_bone_transforms[p_input.m_bone_indices.y] * p_input.m_bone_weights[1];
	l_transform += g_bone_transforms[p_input.m_bone_indices.z] * p_input.m_bone_weights[2];
	l_transform += g_bone_transforms[p_input.m_bone_indices.w] * p_input.m_bone_weights[3];
	
	l_output.m_position = mul(float4(p_input.m_position.xyz, 1), mul(l_transform, g_view_projection));

	return l_output;
}