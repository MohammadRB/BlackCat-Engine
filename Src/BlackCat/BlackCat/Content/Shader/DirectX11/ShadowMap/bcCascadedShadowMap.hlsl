// [06/16/2019 MRB]

#include "..\bcRegister.hlsli"

//cbuffer g_bc_parameters				: register(BC_RENDER_PASS_STATE_CB1)
//{
//	uint m_shadow_map_size				: packoffset(c0.x);
//	uint m_shadow_map_cascade_count		: packoffset(c0.y);
//	uint m_shadow_map_current_cascade	: packoffset(c0.z);
//};

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
};

bc_vs_output csm_vs(bc_vs_input p_input)
{
	bc_vs_output l_output;

	l_output.m_position = mul(float4(p_input.m_position, 1), g_world_view_projection);

	//l_output.m_position.x = (l_output.m_position.x + 1) / 2.0;
	//l_output.m_position.x /= m_shadow_map_cascade_count;
	//l_output.m_position.x += (m_shadow_map_current_cascade * (1.0 / m_shadow_map_cascade_count));
	//l_output.m_position.x = (l_output.m_position.x - 0.5) * 2;

	return l_output;
}