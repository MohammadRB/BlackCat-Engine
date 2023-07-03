// [10/03/2019 MRB]

#include "../bcRegister.hlsli"

SamplerState g_sam_sampler			: register(BC_RENDER_PASS_STATE_S0);

Texture2D g_tex2d_diffuse			: register(BC_RENDER_STATE_T0);
Texture2D g_tex2d_normal			: register(BC_RENDER_STATE_T1);
Texture2D g_tex2d_specular			: register(BC_RENDER_STATE_T2);

cbuffer g_cb_material				: register(BC_RENDER_STATE_CB1)
{
	float4 g_diffuse				: packoffset(c0);
	float g_specular_intensity		: packoffset(c1.x);
	float g_specular_power			: packoffset(c1.y);
	bool g_has_normal_map			: packoffset(c1.z);
};

struct bc_vs_input
{
	float3 m_position				: POSITION0;
	float2 m_texcoord				: TEXCOORD0;
	float3 m_normal					: NORMAL0;
	float3 m_tangent				: TANGENT0;
	int4 m_bone_indices				: BLENDINDICES0;
	float4 m_bone_weights			: BLENDWEIGHT0;
};

struct bc_vs_output
{
	float4 m_position				: SV_POSITION;
	float2 m_texcoord				: TEXCOORD0;
	float3 m_normal					: NORMAL0;
	float3 m_tangent				: TANGENT0;
	float3 m_binormal				: BINORMAL0;
};

struct bc_ps_output
{
	float4 m_diffuse : SV_Target0;
	float4 m_normal : SV_Target1;
};

bc_vs_output gbuffer_skinned_vs(bc_vs_input p_input)
{
	bc_vs_output l_output;

	float4x4 l_transform = g_bone_transforms[p_input.m_bone_indices.x] * p_input.m_bone_weights[0];
	l_transform += g_bone_transforms[p_input.m_bone_indices.y] * p_input.m_bone_weights[1];
	l_transform += g_bone_transforms[p_input.m_bone_indices.z] * p_input.m_bone_weights[2];
	l_transform += g_bone_transforms[p_input.m_bone_indices.w] * p_input.m_bone_weights[3];
	
	l_output.m_position = mul(float4(p_input.m_position.xyz, 1), mul(l_transform, g_view_projection));
	l_output.m_texcoord = p_input.m_texcoord;
	l_output.m_normal = normalize(mul(p_input.m_normal, (float3x3) l_transform));

	if (g_has_normal_map)
	{
		l_output.m_tangent = normalize(mul(p_input.m_tangent, (float3x3) l_transform));
		l_output.m_binormal = normalize(cross(l_output.m_normal, l_output.m_tangent));
	}

	return l_output;
}