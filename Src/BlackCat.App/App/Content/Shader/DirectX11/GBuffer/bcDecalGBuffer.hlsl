// [29/02/2021 MRB]

#include "../bcRegister.hlsli"
#include "../bcHelper.hlsli"

struct bc_decal_instance
{
	float4x4 m_world_inv;
	float4x4 m_world_view_projection;
	float m_u0;
	float m_v0;
	float m_u1;
	float m_v1;
	uint m_group;
};

SamplerState g_sam_point								: register(BC_RENDER_PASS_STATE_S0);
SamplerState g_sam_linear								: register(BC_RENDER_PASS_STATE_S1);

Texture2D<float> g_tex2d_depth							: register(BC_RENDER_PASS_STATE_T0);
Texture2D<uint2> g_tex2d_stencil						: register(BC_RENDER_PASS_STATE_T1);
StructuredBuffer<bc_decal_instance> g_buf_instances		: register(BC_RENDER_PASS_STATE_T2);

Texture2D g_tex2d_diffuse								: register(BC_RENDER_STATE_T0);
Texture2D g_tex2d_normal								: register(BC_RENDER_STATE_T1);
Texture2D g_tex2d_specular								: register(BC_RENDER_STATE_T2);

struct bc_vs_input
{
	float3 m_position			: POSITION0;
};

struct bc_vs_output
{
	float4 m_position			: SV_POSITION;
	float4 m_cs_position		: TEXCOORD0;
	float4x4 m_world_inv		: TEXCOORD1;
	float m_u0					: TEXCOORD5;
	float m_v0					: TEXCOORD6;
	float m_u1					: TEXCOORD7;
	float m_v1					: TEXCOORD8;
	uint m_group				: TEXCOORD9;
};

struct bc_ps_output
{
	float4 m_diffuse			: SV_Target0;
	float4 m_normal				: SV_Target1;
	float4 m_specular			: SV_Target2;
};

// https://mtnphil.wordpress.com/2014/05/24/decals-deferred-rendering
float3x3 cotangent_frame(float3 p_normal, float3 p_world, float2 p_uv)
{
	// get edge vectors of the pixel triangle
	const float3 l_dp1 = ddx(p_world);
	const float3 l_dp2 = ddy(p_world);
	const float2 l_duv1 = ddx(p_uv);
	const float2 l_duv2 = ddy(p_uv);

	// solve the linear system
	const float3 l_dp2_perp = cross(l_dp2, p_normal);
	const float3 l_dp1_perp = cross(p_normal, l_dp1);
	const float3 l_t = l_dp2_perp * l_duv1.x + l_dp1_perp * l_duv2.x;
	const float3 l_b = l_dp2_perp * l_duv1.y + l_dp1_perp * l_duv2.y;

	// construct a scale-invariant frame
	const float l_invmax = rsqrt(max(dot(l_t, l_t), dot(l_b, l_b)));
	return float3x3(l_t * l_invmax, l_b * l_invmax, p_normal);
}

// https://www.slideshare.net/philiphammer/bindless-deferred-decals-in-the-surge-2
bool is_edge(float2 p_texcoord)
{
	// w z
	// x y
	const float4 l_depth_quad0 = g_tex2d_depth.GatherRed(g_sam_point, p_texcoord);
	const float4 l_depth_quad1 = g_tex2d_depth.GatherRed(g_sam_point, p_texcoord, int2(-1, -1));
	const float l_center_depth = l_depth_quad0.w;
	const float4 l_four_around_center = float4(l_depth_quad0.x, l_depth_quad0.z, l_depth_quad1.x, l_depth_quad1.z);
	return any(abs(l_center_depth - l_four_around_center) > 0.005f / g_global_scale);
}

bc_vs_output vs(bc_vs_input p_input, uint p_instance_index : SV_InstanceID)
{
	bc_vs_output l_output;
	const bc_decal_instance l_instance = g_buf_instances.Load(p_instance_index);
	
	l_output.m_position = mul(float4(p_input.m_position, 1), l_instance.m_world_view_projection);
	l_output.m_cs_position = l_output.m_position;
	l_output.m_world_inv = l_instance.m_world_inv;
	l_output.m_u0 = l_instance.m_u0;
	l_output.m_v0 = l_instance.m_v0;
	l_output.m_u1 = l_instance.m_u1;
	l_output.m_v1 = l_instance.m_v1;
	l_output.m_group = l_instance.m_group;
	
	return l_output;
}

bc_ps_output ps(bc_vs_output p_input)
{
	bc_ps_output l_output;

	const float4 l_clip_space = p_input.m_cs_position / p_input.m_cs_position.w;
	const float2 l_texcoord = bc_clip_space_to_texcoord(p_input.m_cs_position);
	const float l_depth = g_tex2d_depth.Sample(g_sam_point, l_texcoord).r;

	if (!CAMERA_INSIDE)
	{
		clip(l_depth - (l_clip_space.z));
	}

	//l_output.m_diffuse = float4(1, 1, 1, 0.5);
	//l_output.m_normal = float4(0, 0, 0, 0);
	//l_output.m_specular = float4(0, 0, 0, 0);
	//return l_output;
	
	const uint l_stencil = g_tex2d_stencil.Load(int3(l_texcoord * uint2(g_screen_width, g_screen_height), 0)).g;

	clip((l_stencil & p_input.m_group) - 1.0f);
	
	const float3 l_pixel_world = bc_reconstruct_world_space(l_clip_space.xy, l_depth, g_view_projection_inv);
	const float3 l_pixel_local = mul(float4(l_pixel_world, 1), p_input.m_world_inv).xyz;

	clip(0.5 - abs(l_pixel_local));
	
	float2 l_decal_texcoord = l_pixel_local.xz + 0.5;
	l_decal_texcoord = float2
	(
		p_input.m_u0 + (p_input.m_u1 - p_input.m_u0) * l_decal_texcoord.x,
		p_input.m_v0 + (p_input.m_v1 - p_input.m_v0) * l_decal_texcoord.y
	);
	
	float4 l_diffuse_map;
	float4 l_normal_map;
	float4 l_specular_map;

	if (!is_edge(l_texcoord))
	{
		l_diffuse_map = g_tex2d_diffuse.Sample(g_sam_linear, l_decal_texcoord);
		l_normal_map = g_tex2d_normal.Sample(g_sam_linear, l_decal_texcoord);
		l_specular_map = g_tex2d_specular.Sample(g_sam_linear, l_decal_texcoord);
	}
	else
	{
		l_diffuse_map = g_tex2d_diffuse.SampleLevel(g_sam_linear, l_decal_texcoord, 0);
		l_normal_map = g_tex2d_normal.SampleLevel(g_sam_linear, l_decal_texcoord, 0);
		l_specular_map = g_tex2d_specular.SampleLevel(g_sam_linear, l_decal_texcoord, 0);
	}
	
	clip(l_diffuse_map.a - 0.005);
	
	const float3 l_ddx_pixel_world = ddx(l_pixel_world);
	const float3 l_ddy_pixel_world = ddy(l_pixel_world);
	const float3 l_pixel_normal = normalize(cross(l_ddx_pixel_world, l_ddy_pixel_world));
	const float3x3 l_tangent_space = cotangent_frame(l_pixel_normal, l_pixel_world, l_decal_texcoord);

	const float3 l_world_normal = normalize(mul(bc_to_decoded_normal(l_normal_map.rgb), l_tangent_space));
	
	l_output.m_diffuse = l_diffuse_map;
	l_output.m_normal = float4(bc_to_encoded_normal(l_world_normal), l_diffuse_map.a);
	l_output.m_specular = float4(l_specular_map.r, 0.1, 0, l_diffuse_map.a);
	
	return l_output;
}

bc_ps_output debug_ps(bc_vs_output p_input)
{
	bc_ps_output l_output;

	const float4 l_clip_space = p_input.m_cs_position / p_input.m_cs_position.w;
	const float2 l_texcoord = bc_clip_space_to_texcoord(p_input.m_cs_position);
	const float l_depth = g_tex2d_depth.Sample(g_sam_point, l_texcoord).r;

	clip(l_depth - l_clip_space.z);
	
	l_output.m_diffuse = float4(1, 1, 1, 0.5);
	l_output.m_normal = float4(0, 0, 0, 0);
	l_output.m_specular = float4(0, 0, 0, 0);
	
	return l_output;
}