// [09/09/2019 MRB]

#include "../bcRegister.hlsli"
#include "bcVegetableAnimation.hlsli"

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
    float3 m_position		: POSITION0;
    float2 m_texcoord		: TEXCOORD0;
    float3 m_normal			: NORMAL0;
    float3 m_tangent		: TANGENT0;
};

struct bc_vs_output
{
    float4 m_position		: SV_POSITION;
    float2 m_texcoord		: TEXCOORD0;
    float3 m_normal			: NORMAL0;
    float3 m_tangent		: TANGENT0;
    float3 m_binormal		: BINORMAL0;
};

struct bc_ps_output
{
    float4 m_diffuse		: SV_Target0;
    float4 m_normal			: SV_Target1;
    float4 m_specular		: SV_Target2;
};

bc_vs_output gbuffer_vegetable_leaf_vs(bc_vs_input p_input)
{
    bc_vs_output l_output;

    float3 l_position = bc_do_vegetable_animation(p_input.m_position, g_world, g_vegetable_max_height, true, g_global_wind_direction, g_global_wind_power, g_total_elapsed_second);
	
    l_output.m_position = mul(float4(l_position, 1), g_view_projection);
    l_output.m_texcoord = p_input.m_texcoord;
    l_output.m_normal = normalize(mul(p_input.m_normal, (float3x3) g_world));

    if (g_has_normal_map)
    {
        l_output.m_tangent = normalize(mul(p_input.m_tangent, (float3x3) g_world));
        l_output.m_binormal = normalize(cross(l_output.m_normal, l_output.m_tangent));
    }

    return l_output;
}

bc_ps_output gbuffer_vegetable_leaf_ps(bc_vs_output p_input)
{
    bc_ps_output l_output;

    float4 l_diffuse_map = g_tex2d_diffuse.Sample(g_sam_sampler, p_input.m_texcoord);
    
	clip(l_diffuse_map.w - 0.4);

    float4 l_specular_map = g_tex2d_specular.Sample(g_sam_sampler, p_input.m_texcoord);
    float3 l_normal = p_input.m_normal;

    float2 l_ddx = ddx(p_input.m_texcoord);
    float2 l_ddy = ddy(p_input.m_texcoord);

	[branch]
    if (g_has_normal_map)
    {
        float4 l_normal_map = g_tex2d_normal.SampleGrad(g_sam_sampler, p_input.m_texcoord, l_ddx, l_ddy);

        float3x3 l_tbn;
        l_tbn[0] = p_input.m_tangent;
        l_tbn[1] = p_input.m_binormal;
        l_tbn[2] = p_input.m_normal;

        float3 l_in_range_normal = (l_normal_map.xyz - 0.5) * 2;
        l_normal = mul(l_in_range_normal, l_tbn);
    }

    float3 l_final_normal = (l_normal + 1) / 2.0f;

    l_output.m_diffuse = float4(l_diffuse_map.xyz, 1);
    l_output.m_normal = float4(l_final_normal, 1);
	l_output.m_specular = float4(l_specular_map.x, g_specular_power, 0, 1);

    return l_output;
}