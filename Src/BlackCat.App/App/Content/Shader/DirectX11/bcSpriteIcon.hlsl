// [01/24/2022 MRB]

#include "bcRegister.hlsli"

struct bc_icon_struct
{
	float3 m_position;
	uint m_size;
	float m_u0;
	float m_v0;
	float m_u1;
	float m_v1;
};

StructuredBuffer<bc_icon_struct> g_icons	: register(BC_RENDER_PASS_STATE_T0);
Texture2D g_texture							: register(BC_RENDER_PASS_STATE_T1);
SamplerState g_linear_sampler				: register(BC_RENDER_PASS_STATE_S0);

static const float2 g_quad[] =
{
	float2(-1, 1), float2(1, 1), float2(-1, -1), float2(1, -1)
};
static const float2 g_quad_tex[] =
{
	float2(0, 0), float2(1, 0), float2(0, 1), float2(1, 1)
};

struct bc_vs_output
{
	uint m_instance_index					: TEXCOORD0;
};

struct bc_gs_output
{
	float4 m_pos							: SV_Position;
	float2 m_texcoord						: TEXCOORD0;
};

struct bc_ps_output
{
	float4 m_color							: SV_Target;
};

bc_vs_output sprite_icon_vs(uint p_instance_index : SV_InstanceID)
{
	bc_vs_output l_output;
	l_output.m_instance_index = p_instance_index;

	return l_output;
}

[maxvertexcount(4)]
void sprite_icon_gs(point bc_vs_output p_input[1], inout TriangleStream<bc_gs_output> p_stream)
{
	bc_gs_output l_output_vertices[4];
	const bc_icon_struct l_icon = g_icons[p_input[0].m_instance_index];

	const float l_distance = length(l_icon.m_position - g_camera_position);
	const float l_distance_multiplier = max(0.5f, 1 - min(1.f, l_distance / 1000.f));
	const float l_icon_ndc_width = l_distance_multiplier * l_icon.m_size * (1. / g_screen_width);
	const float l_icon_ndc_height = l_distance_multiplier * l_icon.m_size * (1. / g_screen_height);
	const float2 l_icon_uvs[4] = { {l_icon.m_u0, l_icon.m_v0}, {l_icon.m_u1, l_icon.m_v0}, {l_icon.m_u0, l_icon.m_v1}, {l_icon.m_u1, l_icon.m_v1} };
	const float4 l_icon_ss_position = mul(float4(l_icon.m_position, 1), g_view_projection);
	const float4 l_icon_ndc_position = l_icon_ss_position / l_icon_ss_position.w;

	[unroll]
	for (uint i = 0; i < 4; ++i)
	{
		float2 l_quad = g_quad[i];
		l_quad.x *= l_icon_ndc_width;
		l_quad.y *= l_icon_ndc_height;

		l_output_vertices[i].m_pos = l_icon_ndc_position + float4(l_quad.x, l_quad.y, 0, 0);
		l_output_vertices[i].m_pos *= l_icon_ss_position.w;
		l_output_vertices[i].m_texcoord = l_icon_uvs[i];

		p_stream.Append(l_output_vertices[i]);
	}
}

float4 sprite_icon_ps(bc_gs_output p_input) : SV_Target0
{
	const float4 l_color = g_texture.Sample(g_linear_sampler, p_input.m_texcoord);
	return float4(l_color);
}