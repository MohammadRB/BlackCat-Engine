// [11/25/2020 MRB]

#include "..\bcRegister.hlsli"
#include "..\bcHelper.hlsli"
#include "bcParticle.hlsli"

StructuredBuffer<particle> g_particles				: register(BC_RENDER_PASS_STATE_T0);
StructuredBuffer<alive_particle> g_alive_indices	: register(BC_RENDER_PASS_STATE_T1);

struct vertex_output
{
	uint m_vertex_index : TEXCOORD0;
};

struct geometry_output
{
	float4 m_pos : SV_Position;
	float2 m_tex_coord : TEXCOORD0;
};

vertex_output vs(uint p_vertex_index : SV_VertexID)
{
	vertex_output l_output;
	l_output.m_vertex_index = p_vertex_index;
	
	return l_output;
}

[maxvertexcount(4)]
void gs(point vertex_output p_input[1], inout TriangleStream<geometry_output> p_stream)
{
	uint l_particle_index = g_alive_indices[p_input[0].m_vertex_index].m_index;
	particle l_particle = g_particles[l_particle_index];
	
	float3 l_camera_direction = normalize(g_camera_position - l_particle.m_position);
	float3 l_billboard_right = cross(float3(0, 1, 0), l_camera_direction);
	float3 l_billboard_up = cross(l_billboard_right, l_camera_direction);
	
	geometry_output l_vertex0;
	geometry_output l_vertex1;
	geometry_output l_vertex2;
	geometry_output l_vertex3;
	
	l_vertex0.m_pos = float4(l_particle.m_position, 1) - float4(l_billboard_right, 1) + float4(l_billboard_up, 1);
	l_vertex0.m_tex_coord = float2(0, 0);
	l_vertex1.m_pos = float4(l_particle.m_position, 1) + float4(l_billboard_right, 1) + float4(l_billboard_up, 1);
	l_vertex1.m_tex_coord = float2(0, 1);
	l_vertex2.m_pos = float4(l_particle.m_position, 1) + float4(l_billboard_right, 1) - float4(l_billboard_up, 1);
	l_vertex2.m_tex_coord = float2(1, 1);
	l_vertex3.m_pos = float4(l_particle.m_position, 1) - float4(l_billboard_right, 1) - float4(l_billboard_up, 1);
	l_vertex3.m_tex_coord = float2(1, 0);
	
	l_vertex0.m_pos = mul(g_view_projection, l_vertex0.m_pos);
	l_vertex1.m_pos = mul(g_view_projection, l_vertex1.m_pos);
	l_vertex2.m_pos = mul(g_view_projection, l_vertex2.m_pos);
	l_vertex3.m_pos = mul(g_view_projection, l_vertex3.m_pos);
	
	p_stream.Append(l_vertex0);
	p_stream.Append(l_vertex1);
	p_stream.Append(l_vertex2);
	p_stream.Append(l_vertex3);
}

float4 ps(geometry_output p_input) : SV_Target
{
	return float4(0.5, 0.5, 0.5, 0.5);
}