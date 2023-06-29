// [03/02/2022 MRB]

#include "../bcRegister.hlsli"

struct bullet
{
	float3 m_position;
	float3 m_direction;
};

cbuffer g_cb_params							: register(BC_RENDER_PASS_STATE_CB1)
{
	float g_length							: packoffset(c0.x);
	float g_width							: packoffset(c0.y);
	float g_intensity						: packoffset(c0.z);
}

StructuredBuffer<bullet> g_bullets			: register(BC_RENDER_PASS_STATE_T0);
Texture2D g_texture							: register(BC_RENDER_PASS_STATE_T1);
SamplerState g_linear_sampler				: register(BC_RENDER_PASS_STATE_S0);

static const float2 g_bullet_vertices[] =
{
	float2(1, -1), float2(1, 1), float2(-1, -1), float2(-1, 1)
};
static const float2 g_bullet_texcoords[] =
{
	float2(0, 1), float2(0, 0), float2(1, 1), float2(1, 0) // Horizontal texture
};

struct bc_vs_output
{
	uint m_instance_index		: TEXCOORD0;
};

struct bc_gs_output
{
	float4 m_pos				: SV_Position;
	float2 m_texcoord			: TEXCOORD0;
};

struct bc_ps_output
{
	float4 m_color				: SV_Target;
};

bc_vs_output bullet_trail_vs(uint p_instance_index : SV_InstanceID)
{
	bc_vs_output l_output;
	l_output.m_instance_index = p_instance_index;

	return l_output;
}

[maxvertexcount(4)]
void bullet_trail_gs(point bc_vs_output p_input[1], inout TriangleStream<bc_gs_output> p_stream)
{
	const bullet l_bullet = g_bullets[p_input[0].m_instance_index];

	bc_gs_output l_output_vertices[4];
	const float3 l_billboard_forward = l_bullet.m_direction;
	const float3 l_billboard_right = normalize(cross(l_billboard_forward, normalize(g_camera_position - l_bullet.m_position)));
	const float3 l_billboard_center = l_bullet.m_position - l_billboard_forward * (g_length / 2);

	[unroll]
	for (uint i = 0; i < 4; ++i)
	{
		float2 l_quad = g_bullet_vertices[i];
		l_quad *= float2(g_length, g_width);

		l_output_vertices[i].m_pos = float4(l_billboard_center, 1) + float4(l_billboard_forward, 0) * l_quad.x + float4(l_billboard_right, 0) * l_quad.y;
		l_output_vertices[i].m_pos = mul(l_output_vertices[i].m_pos, g_view_projection);
		l_output_vertices[i].m_texcoord = g_bullet_texcoords[i];

		p_stream.Append(l_output_vertices[i]);
	}
}

float4 bullet_trail_ps(bc_gs_output p_input) : SV_Target0
{
	float4 l_mask = g_texture.Sample(g_linear_sampler, p_input.m_texcoord);
	l_mask = max(l_mask - 0.1f, 0.f);
	l_mask *= 1.1f;

	return l_mask * g_intensity;
}