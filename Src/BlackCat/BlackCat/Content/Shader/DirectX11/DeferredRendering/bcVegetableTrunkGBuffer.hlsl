// [09/09/2019 MRB]

#include "..\bcRegister.hlsli"

SamplerState g_sam_sampler			: register(BC_RENDER_PASS_STATE_S0);

Texture2D g_tex2d_diffuse			: register(BC_RENDER_STATE_T0);
Texture2D g_tex2d_normal				: register(BC_RENDER_STATE_T1);
Texture2D g_tex2d_specular			: register(BC_RENDER_STATE_T2);

cbuffer g_cb_material				: register(BC_RENDER_STATE_CB1)
{
    float4 g_diffuse					: packoffset(c0);
    float g_specular_intency			: packoffset(c1.x);
    float g_specular_power			: packoffset(c1.y);
    bool g_has_normal_map			: packoffset(c2);
};

struct bc_vs_input
{
    float3 m_position	: POSITION0;
    float2 m_texcoord	: TEXCOORD0;
    float3 m_normal		: NORMAL0;
    float3 m_tangent		: TANGENT0;
};

struct bc_vs_output
{
    float4 m_position	: SV_POSITION;
    float2 m_texcoord	: TEXCOORD0;
    float3 m_normal		: NORMAL0;
    float3 m_tangent		: TANGENT0;
    float3 m_binormal	: BINORMAL0;
};

struct bc_ps_output
{
    float4 m_diffuse		: SV_Target0;
    float4 m_normal		: SV_Target1;
};

bc_vs_output gbuffer_vegetable_trunk_vs(bc_vs_input p_input)
{
    bc_vs_output l_output;

    l_output.m_position = mul(float4(p_input.m_position, 1), g_world_view_projection);
    l_output.m_texcoord = p_input.m_texcoord;
    l_output.m_normal = normalize(mul(p_input.m_normal, (float3x3) g_world));

    if (g_has_normal_map)
    {
        l_output.m_tangent = normalize(mul(p_input.m_tangent, (float3x3) g_world));
        l_output.m_binormal = normalize(cross(l_output.m_normal, l_output.m_tangent));
    }

    return l_output;
}