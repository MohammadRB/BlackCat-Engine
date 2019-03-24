// [03/23/2019 MRB]

#include "..\bcRegister.hlsli"

#define THREAD_GROUP_SIZE 16

struct direct_light
{
	float3	m_top_left_min_z;
	float3	m_bottom_right_max_z;
	float3	m_direction;
	float3	m_color;
	float	m_intensity;
	float3	m_ambient_color;
	float	m_ambient_intensity;
};

struct point_light
{
	float3	m_top_left_min_z;
	float3	m_bottom_right_max_z;
	float3	m_position;
	float	m_radius;
	float3	m_color;
	float	m_intensity;
};

struct spot_light
{
	float3	m_top_left_min_z;
	float3	m_bottom_right_max_z;
	float3	m_position;
	float	m_angle;
	float3	m_direction;
	float	m_length;
	float3	m_color;
	float	m_intensity;
};

Texture2D<float4> g_depth_map						: register(BC_COMPUTE_STATE_T0);
Texture2D<float4> g_diffuse_map						: register(BC_COMPUTE_STATE_T1);
Texture2D<float4> g_normal_map						: register(BC_COMPUTE_STATE_T2);

StructuredBuffer<direct_light> g_direct_lights		: register(BC_COMPUTE_STATE_T3);
StructuredBuffer<point_light> g_direct_lights		: register(BC_COMPUTE_STATE_T4);
StructuredBuffer<spot_light> g_direct_lights		: register(BC_COMPUTE_STATE_T5);

RWTexture2D<float4> g_output_texture				: register(BC_COMPUTE_STATE_U0);

float4 read_buffer(Texture2D<float> p_buffer, int2 p_texcoord)
{
	return p_buffer.load(p_texcoord);
}

void write_output(int2 p_texcoord, float4 p_output)
{
	g_output_texture[p_texcoord] = p_output;
}

[numthreads(THREAD_GROUP_SIZE, THREAD_GROUP_SIZE, 1)]
void main(uint3 p_group_id : SV_GroupID, uint p_group_index : SV_GroupIndex, uint3 p_group_thread_id : SV_GroupThreadID, uint3 p_dispatch_thread_id : SV_DispatchThreadID)
{
	int2 l_global_texcoord = p_dispatch_thread_id.xy;

	float l_depth = read_buffer(g_depth_map, l_global_texcoord).x;
	write_output(l_global_texcoord, float4(l_depth));
}