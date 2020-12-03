// [11/25/2020 MRB]

#include "..\bcRegister.hlsli"
#include "..\bcHelper.hlsli"
#include "bcParticle.hlsli"

StructuredBuffer<particle> g_particles				: register(BC_RENDER_PASS_STATE_T0);
StructuredBuffer<alive_particle> g_alive_indices	: register(BC_RENDER_PASS_STATE_T1);

struct vertex_output
{
	uint m_instance_index : TEXCOORD0;
};

struct geometry_output
{
	float4 m_pos : SV_Position;
	float2 m_texcoord : TEXCOORD0;
	float m_normalized_age : TEXCOORD1;
	float m_alpha_multiplier : TEXCOORD2;
	int m_texure_index : TEXCOORD3;
	int m_sprite_index : TEXCOORD4;
};

vertex_output vs(uint p_instance_index : SV_InstanceID)
{
	vertex_output l_output;
	l_output.m_instance_index = p_instance_index;
	
	return l_output;
}

[maxvertexcount(4)]
void gs(point vertex_output p_input[1], inout TriangleStream<geometry_output> p_stream)
{
	uint l_particle_index = g_alive_indices[p_input[0].m_instance_index].m_index;
	particle l_particle = g_particles[l_particle_index];
	
	float3 l_billboard_normal = normalize(g_camera_position - l_particle.m_position);
	float3 l_billboard_right = normalize(cross(l_billboard_normal, float3(0, 1, 0)));
	float3 l_billboard_up = normalize(cross(l_billboard_right, l_billboard_normal));
	
	float2 l_texcoords[] = { float2(0, 0), float2(1, 0), float2(0, 1), float2(1, 1) };
	geometry_output l_vertices[4];
	
	l_vertices[0].m_pos = float4(l_particle.m_position, 1) - float4(l_billboard_right, 0) + float4(l_billboard_up, 0);
	l_vertices[1].m_pos = float4(l_particle.m_position, 1) + float4(l_billboard_right, 0) + float4(l_billboard_up, 0);
	l_vertices[2].m_pos = float4(l_particle.m_position, 1) - float4(l_billboard_right, 0) - float4(l_billboard_up, 0);
	l_vertices[3].m_pos = float4(l_particle.m_position, 1) + float4(l_billboard_right, 0) - float4(l_billboard_up, 0);
	
	const float l_age_alpha_reduce = 0.15;
	float l_normalized_age = l_particle.m_age / l_particle.m_lifetime;
	float l_alpha_multiplier = 1 - max(0, l_normalized_age - (1 - l_age_alpha_reduce)) * 1 / l_age_alpha_reduce;
	
	[unroll]
	for (uint i = 0; i < 4; ++i)
	{
		l_vertices[i].m_pos = mul(l_vertices[i].m_pos, g_view_projection);
		l_vertices[i].m_texcoord = l_texcoords[i];
		l_vertices[i].m_normalized_age = l_normalized_age;
		l_vertices[i].m_alpha_multiplier = l_alpha_multiplier;
		l_vertices[i].m_texure_index = l_particle.m_texture_index;
		l_vertices[i].m_sprite_index = l_particle.m_sprite_index;
		p_stream.Append(l_vertices[i]);
	}
}

float4 ps(geometry_output p_input) : SV_Target0
{
	return float4(0.8, 0.5, 0.5, 0.7 * p_input.m_alpha_multiplier);
}