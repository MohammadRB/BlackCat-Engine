// [10/10/2019 MRB]

#include "../bcRegister.hlsli"
#include "../GBuffer/bcVegetableAnimation.hlsli"

SamplerState g_sam_sampler		: register(BC_RENDER_PASS_STATE_S0);
Texture2D g_tex2d_diffuse		: register(BC_RENDER_STATE_T0);

struct bc_vs_input
{
    float3 m_position : POSITION0;
    float2 m_texcoord : TEXCOORD0;
    float3 m_normal : NORMAL0;
    float3 m_tangent : TANGENT0;
};

struct bc_vs_output
{
    float4 m_position : SV_POSITION;
    float2 m_texcoord : TEXCOORD0;
};

bc_vs_output vs(bc_vs_input p_input)
{
    bc_vs_output l_output;

    float3 l_position = bc_do_vegetable_animation
    (
        p_input.m_position,
        g_world,
        g_global_scale * 3,
        true,
        g_global_wind_direction,
        g_global_wind_power,
        g_total_elapsed_second
    );

    l_output.m_position = mul(float4(l_position, 1), g_view_projection);
    l_output.m_texcoord = p_input.m_texcoord;
	
    return l_output;
}

void ps(bc_vs_output p_input)
{
    float l_alpha_map = g_tex2d_diffuse.Sample(g_sam_sampler, p_input.m_texcoord).w;
    
    clip(l_alpha_map - 0.4);
}