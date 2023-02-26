// [11/25/2020 MRB]

#include "../bcRegister.hlsli"
#include "../bcHelper.hlsli"
#include "bcParticle.hlsli"

#define NUM_HORIZONTAL_SPRITE 4
#define NUM_VERTICAL_SPRITE 4
#define NUM_LIGHTS 10

StructuredBuffer<particle> g_particles				: register(BC_RENDER_PASS_STATE_T0);
StructuredBuffer<alive_particle> g_alive_indices	: register(BC_RENDER_PASS_STATE_T1);
Texture2D<float> g_depth_buffer						: register(BC_RENDER_PASS_STATE_T2);
Texture2D<float4> g_sprites							: register(BC_RENDER_PASS_STATE_T3);
Texture2D<float4> g_render_buffer					: register(BC_RENDER_PASS_STATE_T4);

SamplerState g_linear_sampler						: register(BC_RENDER_PASS_STATE_S0);
SamplerState g_point_sampler						: register(BC_RENDER_PASS_STATE_S1);

cbuffer g_lights_cbuffer							: register(BC_RENDER_PASS_STATE_CB1)
{
	uint g_lights_count								: packoffset(c0);
	float4 g_lights[NUM_LIGHTS * 2]					: packoffset(c1);
}

static const float2 g_billboard_quad[] = { float2(-1, 1), float2(1, 1), float2(-1, -1), float2(1, -1) };
static const float2 g_billboard_quad_tex[] = { float2(0, 0), float2(1, 0), float2(0, 1), float2(1, 1) };
static const float2 g_full_screen_quad[] = { float2(-1, 1), float2(1, 1), float2(-1, -1), float2(1, 1), float2(1, -1), float2(-1, -1) };
static const float2 g_full_screen_quad_tex[] = { float2(0, 0), float2(1, 0), float2(0, 1), float2(1, 0), float2(1, 1), float2(0, 1) };

struct render_vertex_output
{
	uint m_instance_index							: TEXCOORD0;
};

struct render_geometry_output
{
	float4 m_pos									: SV_Position;
	float4 m_cs_position							: TEXCOORD0;
	float4 m_color									: TEXCOORD1;
	float4 m_light_shading							: TEXCOORD2;
	float3 m_direct_light_shading					: TEXCOORD3;
	float3 m_ambient_shading						: TEXCOORD4;
	float2 m_texcoord								: TEXCOORD5;
	float m_linear_depth							: TEXCOORD6;
	float m_fade									: TEXCOORD7;
	int m_sprite_index								: TEXCOORD8;
};

struct merge_vs_output
{
	float4 m_position								: SV_POSITION;
	float2 m_texcoord								: TEXCOORD0;
};

float3 direct_light_shading(float3 p_normal)
{
	const float l_dot = max(0.2f, dot(p_normal, -g_global_light_direction));
	const float3 l_diffuse_light = g_global_light_color * (g_global_light_intensity * .35f) * min(0.7f, l_dot);

	return l_diffuse_light;
}

render_vertex_output render_vs(uint p_instance_index : SV_InstanceID)
{
	render_vertex_output l_output;
	l_output.m_instance_index = p_instance_index;
	
	return l_output;
}

[maxvertexcount(4)]
void render_gs(point render_vertex_output p_input[1], inout TriangleStream<render_geometry_output> p_stream)
{
	const uint l_particle_index = g_alive_indices[p_input[0].m_instance_index].m_index;
	particle l_particle = g_particles[l_particle_index];
	
	float3 l_lights_color = 0;
	float l_lights_intensity = 0;
	for (uint i = 0; i < g_lights_count; ++i)
	{
		const float4 l_light_pos = g_lights[i * 2];
		const float4 l_light_color = g_lights[i * 2 + 1];
		const float l_light_distance = length(l_light_pos.xyz - l_particle.m_position);
		const float l_light_attenuation = 1 - min(1, l_light_distance / l_light_pos.w);
		const float l_light_intensity = pow(l_light_attenuation, 4) * l_light_color.w;
		
		l_lights_color += l_light_color.xyz * l_light_intensity * l_particle.m_fade;
		l_lights_intensity += l_light_intensity;
	}

	const float l_rotation = l_particle.m_rotation * l_particle.m_age;
	float l_sin, l_cos;
	sincos(l_rotation, l_sin, l_cos);
	
	const float3 l_billboard_right = normalize(float3(g_view._11, g_view._21, g_view._31));
	const float3 l_billboard_up = normalize(float3(g_view._12, g_view._22, g_view._32));

	const float3 l_direct_light_shading = direct_light_shading(l_particle.m_direction);
	// multiply ambient with color intensity to allow particles with high intensity reflect additive color in dark ambient
	const float3 l_ambient_shading = g_global_light_ambient_color * g_global_light_ambient_intensity * max(l_particle.m_color_intensity, 1.f);
	
	float2 l_quad[4];
	render_geometry_output l_output_vertices[4];
	
	[unroll]
	for (uint i = 0; i < 4; ++i)
	{
		l_quad[i].x = l_cos * g_billboard_quad[i].x - l_sin * g_billboard_quad[i].y;
		l_quad[i].y = l_sin * g_billboard_quad[i].x + l_cos * g_billboard_quad[i].y;
		l_quad[i] *= l_particle.m_size;

		l_output_vertices[i].m_pos = float4(l_particle.m_position, 1) + float4(l_billboard_right, 0) * l_quad[i].x + float4(l_billboard_up, 0) * l_quad[i].y;
		l_output_vertices[i].m_pos = mul(l_output_vertices[i].m_pos, g_view_projection);
		l_output_vertices[i].m_cs_position = l_output_vertices[i].m_pos;
		l_output_vertices[i].m_color = float4(l_particle.m_color * l_particle.m_color_intensity, l_particle.m_color_intensity);
		l_output_vertices[i].m_light_shading = float4(l_lights_color, l_lights_intensity);
		l_output_vertices[i].m_direct_light_shading = l_direct_light_shading;
		l_output_vertices[i].m_ambient_shading = l_ambient_shading;
		l_output_vertices[i].m_texcoord = bc_to_sprite_texcoord(g_billboard_quad_tex[i], l_particle.m_sprite_index, NUM_HORIZONTAL_SPRITE, NUM_VERTICAL_SPRITE);
		l_output_vertices[i].m_linear_depth = bc_convert_to_linear_depth(l_output_vertices[i].m_pos.z / l_output_vertices[i].m_pos.w, g_near_plane, g_far_plane) * (g_far_plane - g_near_plane);
		l_output_vertices[i].m_fade = l_particle.m_fade;
		l_output_vertices[i].m_sprite_index = l_particle.m_sprite_index;
		p_stream.Append(l_output_vertices[i]);
	}
}

float4 render_ps(render_geometry_output p_input) : SV_Target0
{
	const float2 l_ss_texcoord = bc_clip_space_to_texcoord(p_input.m_cs_position);

	const float l_alpha = g_sprites.Sample(g_linear_sampler, p_input.m_texcoord).r;
	clip(l_alpha - 0.05f);

	const float l_depth_buffer = g_depth_buffer.Sample(g_point_sampler, l_ss_texcoord);
	clip(l_depth_buffer - (p_input.m_cs_position.z / p_input.m_cs_position.w));

	const float l_depth_buffer_linear = bc_convert_to_linear_depth(l_depth_buffer, g_near_plane, g_far_plane) * (g_far_plane - g_near_plane);
	const float l_depth_diff = l_depth_buffer_linear - p_input.m_linear_depth;
	const float l_soft_fade = saturate(l_depth_diff / (g_global_scale / 2.f));
	
	const float4 l_light_shading = p_input.m_light_shading * l_alpha;
	const float3 l_color_diffuse = p_input.m_color.rgb * p_input.m_ambient_shading + p_input.m_direct_light_shading;\
	const float3 l_final_diffuse = l_color_diffuse + l_light_shading.rgb;

	// it is important to multiply alpha with color intensity to simulate additive particles
	return float4(l_final_diffuse, l_alpha * p_input.m_fade * l_soft_fade * p_input.m_color.a);
}

merge_vs_output merge_vs(uint p_vertex_id : SV_VertexID)
{
	merge_vs_output l_output;
	l_output.m_position = float4(g_full_screen_quad[p_vertex_id], 0, 1);
	l_output.m_texcoord = g_full_screen_quad_tex[p_vertex_id];

	return l_output;
}

float4 merge_ps(merge_vs_output p_input) : SV_Target0
{
	const float4 l_diffuse = g_render_buffer.Sample(g_linear_sampler, p_input.m_texcoord);
	return l_diffuse;
}