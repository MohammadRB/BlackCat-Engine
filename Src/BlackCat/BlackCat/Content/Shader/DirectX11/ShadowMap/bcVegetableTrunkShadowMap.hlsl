// [10/10/2019 MRB]

#include "..\bcRegister.hlsli"
#include "..\DeferredRendering\bcVegetableAnimation.hlsli"

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
};

bc_vs_output vs(bc_vs_input p_input)
{
    bc_vs_output l_output;

    float3 l_position = bc_do_vegetable_animation(p_input.m_position, g_world, g_vegetable_max_height, false, g_global_wind_direction, g_global_wind_power, g_total_elapsed_second);

    l_output.m_position = mul(float4(l_position, 1), g_view_projection);
	
    return l_output;
}