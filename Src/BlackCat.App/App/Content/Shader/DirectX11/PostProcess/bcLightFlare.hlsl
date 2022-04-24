// [04/30/2021 MRB]

#include "../bcRegister.hlsli"

#define MASK_COLOR_OFF 0.09f;

struct light_flare
{
	float3 m_position;
	float3 m_color;
	float m_size;
	float m_intensity;
	int m_texture_index;
	float m_u0;
	float m_v0;
	float m_u1;
	float m_v1;
};

StructuredBuffer<light_flare> g_flares		: register(BC_RENDER_PASS_STATE_T0);

SamplerState g_linear_sampler				: register(BC_RENDER_PASS_STATE_S0);

Texture2D g_tex2d_mask0						: register(BC_RENDER_STATE_T0);
Texture2D g_tex2d_mask1						: register(BC_RENDER_STATE_T1);
Texture2D g_tex2d_mask2						: register(BC_RENDER_STATE_T2);
Texture2D g_tex2d_mask3						: register(BC_RENDER_STATE_T3);
Texture2D g_tex2d_mask4						: register(BC_RENDER_STATE_T4);
Texture2D g_tex2d_mask5						: register(BC_RENDER_STATE_T5);
Texture2D g_tex2d_mask6						: register(BC_RENDER_STATE_T6);
Texture2D g_tex2d_mask7						: register(BC_RENDER_STATE_T7);
Texture2D g_tex2d_mask8						: register(BC_RENDER_STATE_T8);
Texture2D g_tex2d_mask9						: register(BC_RENDER_STATE_T9);

static const float2 g_quad[] = 
{
	float2(-1, 1), float2(1, 1), float2(-1, -1), float2(1, -1)
};
static const float2 g_quad_tex[] = 
{
	float2(0, 0), float2(1, 0), float2(0, 1), float2(1, 1)
};

struct bc_query_vs_input
{
	float3 m_position						: POSITION0;
};

struct bc_query_vs_output
{
	float4 m_position						: SV_POSITION;
};

struct bc_vs_output
{
	uint m_instance_index					: TEXCOORD0;
};

struct bc_gs_output
{
	float4 m_pos							: SV_Position;
	float2 m_texcoord						: TEXCOORD0;
	float3 m_color							: TEXCOORD1;
	float m_intensity						: TEXCOORD2;
	int m_texture_index						: TEXCOORD3;
	float m_u0								: TEXCOORD4;
	float m_v0								: TEXCOORD5;
	float m_u1								: TEXCOORD6;
	float m_v1								: TEXCOORD7;
};

struct bc_ps_output
{
	float4 m_color							: SV_Target;
};

float4 sample_mask_texture(uint p_texture_index, float2 p_texcoord)
{
	switch (p_texture_index)
	{
	case 0:
		return g_tex2d_mask0.Sample(g_linear_sampler, p_texcoord);
	case 1:
		return g_tex2d_mask1.Sample(g_linear_sampler, p_texcoord);
	case 2:
		return g_tex2d_mask2.Sample(g_linear_sampler, p_texcoord);
	case 3:
		return g_tex2d_mask3.Sample(g_linear_sampler, p_texcoord);
	case 4:
		return g_tex2d_mask4.Sample(g_linear_sampler, p_texcoord);
	case 5:
		return g_tex2d_mask5.Sample(g_linear_sampler, p_texcoord);
	case 6:
		return g_tex2d_mask6.Sample(g_linear_sampler, p_texcoord);
	case 7:
		return g_tex2d_mask7.Sample(g_linear_sampler, p_texcoord);
	case 8:
		return g_tex2d_mask8.Sample(g_linear_sampler, p_texcoord);
	case 9:
		return g_tex2d_mask9.Sample(g_linear_sampler, p_texcoord);
	default:
		return 0;
	}
}

bc_query_vs_output light_flare_query_vs(bc_query_vs_input p_input)
{
	bc_query_vs_output l_output;

	l_output.m_position = mul(float4(p_input.m_position, 1), g_world_view_projection);

	return l_output;
}

bc_vs_output light_flare_vs(uint p_instance_index : SV_InstanceID)
{
	bc_vs_output l_output;
	l_output.m_instance_index = p_instance_index;

	return l_output;
}

[maxvertexcount(4)]
void light_flare_gs(point bc_vs_output p_input[1], inout TriangleStream<bc_gs_output> p_stream)
{
	const light_flare l_light_flare = g_flares[p_input[0].m_instance_index];
	
	bc_gs_output l_output_vertices[4];
	float3 l_billboard_right = normalize(float3(g_view._11, g_view._21, g_view._31));
	float3 l_billboard_up = normalize(float3(g_view._12, g_view._22, g_view._32));

	[unroll]
	for (uint i = 0; i < 4; ++i)
	{
		float2 l_quad = g_quad[i];
		l_quad *= l_light_flare.m_size;

		l_output_vertices[i].m_pos = float4(l_light_flare.m_position, 1) + float4(l_billboard_right, 0) * l_quad.x + float4(l_billboard_up, 0) * l_quad.y;
		l_output_vertices[i].m_pos = mul(l_output_vertices[i].m_pos, g_view_projection);
		l_output_vertices[i].m_texcoord = g_quad_tex[i];
		l_output_vertices[i].m_color = l_light_flare.m_color;
		l_output_vertices[i].m_intensity = l_light_flare.m_intensity;
		l_output_vertices[i].m_texture_index = l_light_flare.m_texture_index;
		l_output_vertices[i].m_u0 = l_light_flare.m_u0;
		l_output_vertices[i].m_v0 = l_light_flare.m_v0;
		l_output_vertices[i].m_u1 = l_light_flare.m_u1;
		l_output_vertices[i].m_v1 = l_light_flare.m_v1;
		
		p_stream.Append(l_output_vertices[i]);
	}
}

float4 light_flare_ps(bc_gs_output p_input) : SV_Target0
{
	p_input.m_texcoord = float2
	(
		p_input.m_u0 + (p_input.m_u1 - p_input.m_u0) * p_input.m_texcoord.x,
		p_input.m_v0 + (p_input.m_v1 - p_input.m_v0) * p_input.m_texcoord.y
	);
	
	const float4 l_mask = sample_mask_texture(p_input.m_texture_index, p_input.m_texcoord) - MASK_COLOR_OFF;
	float3 l_color = l_mask.xyz * p_input.m_color * p_input.m_intensity;
	
	return float4(l_color, 1);
}