// [03/27/2019 MRB]

#include "bcRegister.hlsli"

SamplerState g_sam_sampler : register(BC_RENDER_PASS_STATE_S0);

Texture2D g_tex2d_input_texture : register(BC_RENDER_PASS_STATE_T0);

struct bc_vs_input
{
    float3 m_position : POSITION0;
    float2 m_texcoord : TEXCOORD0;
};

struct bc_vs_output
{
    float4 m_position : SV_POSITION;
    float2 m_texcoord : TEXCOORD0;
};

struct bc_ps_output
{
    float4 m_color : SV_Target0;
};

bc_vs_output bb_write_vs(bc_vs_input p_input)
{
    bc_vs_output l_output;

    l_output.m_position = float4(p_input.m_position, 1);
    l_output.m_texcoord = p_input.m_texcoord;

    //// Offset the position by half a pixel to correctly align texels to pixels
    //l_output.m_position.x = l_output.m_position.x - (1.0f / g_screen_width);
    //l_output.m_position.y = l_output.m_position.y + (1.0f / g_screen_height);

    return l_output;
}

bc_ps_output bb_write_ps(bc_vs_output p_input)
{
    bc_ps_output l_output;

    l_output.m_color = g_tex2d_input_texture.Sample(g_sam_sampler, p_input.m_texcoord);

    return l_output;
}