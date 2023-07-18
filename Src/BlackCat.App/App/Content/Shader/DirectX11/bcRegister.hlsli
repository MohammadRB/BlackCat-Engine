// [15/07/2016 MRB]

// Default registers according to render system parameters

// RenderPassState registers
		
#define BC_RENDER_PASS_STATE_S0 s0
#define BC_RENDER_PASS_STATE_S1 s1
#define BC_RENDER_PASS_STATE_S2 s2
#define BC_RENDER_PASS_STATE_S3 s3
#define BC_RENDER_PASS_STATE_S4 s4
#define BC_RENDER_PASS_STATE_S5 s5
#define BC_RENDER_PASS_STATE_S6 s6
#define BC_RENDER_PASS_STATE_S7 s7
#define BC_RENDER_PASS_STATE_S8 s8
#define BC_RENDER_PASS_STATE_S9 s9

#define BC_RENDER_PASS_STATE_T0 t0
#define BC_RENDER_PASS_STATE_T1 t1
#define BC_RENDER_PASS_STATE_T2 t2
#define BC_RENDER_PASS_STATE_T3 t3
#define BC_RENDER_PASS_STATE_T4 t4
#define BC_RENDER_PASS_STATE_T5 t5
#define BC_RENDER_PASS_STATE_T6 t6
#define BC_RENDER_PASS_STATE_T7 t7
#define BC_RENDER_PASS_STATE_T8 t8
#define BC_RENDER_PASS_STATE_T9 t9

#define BC_RENDER_PASS_STATE_U0 u0
#define BC_RENDER_PASS_STATE_U1 u1
#define BC_RENDER_PASS_STATE_U2 u2
#define BC_RENDER_PASS_STATE_U3 u3
#define BC_RENDER_PASS_STATE_U4 u4
#define BC_RENDER_PASS_STATE_U5 u5
#define BC_RENDER_PASS_STATE_U6 u6
#define BC_RENDER_PASS_STATE_U7 u7

#define BC_RENDER_PASS_STATE_CB0 b0
#define BC_RENDER_PASS_STATE_CB1 b1
#define BC_RENDER_PASS_STATE_CB2 b2
#define BC_RENDER_PASS_STATE_CB3 b3

// RenderState registers

#define BC_RENDER_STATE_T0 t10
#define BC_RENDER_STATE_T1 t11
#define BC_RENDER_STATE_T2 t12
#define BC_RENDER_STATE_T3 t13
#define BC_RENDER_STATE_T4 t14
#define BC_RENDER_STATE_T5 t15
#define BC_RENDER_STATE_T6 t16
#define BC_RENDER_STATE_T7 t17
#define BC_RENDER_STATE_T8 t18
#define BC_RENDER_STATE_T9 t19
#define BC_RENDER_STATE_T10 t20
#define BC_RENDER_STATE_T11 t21
#define BC_RENDER_STATE_T12 t22
#define BC_RENDER_STATE_T13 t23
#define BC_RENDER_STATE_T14 t24
#define BC_RENDER_STATE_T15 t25
#define BC_RENDER_STATE_T16 t26
#define BC_RENDER_STATE_T17 t27
#define BC_RENDER_STATE_T18 t28
#define BC_RENDER_STATE_T19 t29

#define BC_RENDER_STATE_CB0 b4
#define BC_RENDER_STATE_CB1 b5
#define BC_RENDER_STATE_CB2 b6
#define BC_RENDER_STATE_CB3 b7

// ComputeState registers

#define BC_COMPUTE_STATE_S0 s0
#define BC_COMPUTE_STATE_S1 s1
#define BC_COMPUTE_STATE_S2 s2
#define BC_COMPUTE_STATE_S3 s3
#define BC_COMPUTE_STATE_S4 s4
#define BC_COMPUTE_STATE_S5 s5
#define BC_COMPUTE_STATE_S6 s6
#define BC_COMPUTE_STATE_S7 s7
#define BC_COMPUTE_STATE_S8 s8
#define BC_COMPUTE_STATE_S9 s9

#define BC_COMPUTE_STATE_T0 t0
#define BC_COMPUTE_STATE_T1 t1
#define BC_COMPUTE_STATE_T2 t2
#define BC_COMPUTE_STATE_T3 t3
#define BC_COMPUTE_STATE_T4 t4
#define BC_COMPUTE_STATE_T5 t5
#define BC_COMPUTE_STATE_T6 t6
#define BC_COMPUTE_STATE_T7 t7
#define BC_COMPUTE_STATE_T8 t8
#define BC_COMPUTE_STATE_T9 t9
#define BC_COMPUTE_STATE_T10 t10
#define BC_COMPUTE_STATE_T11 t11
#define BC_COMPUTE_STATE_T12 t12
#define BC_COMPUTE_STATE_T13 t13
#define BC_COMPUTE_STATE_T14 t14
#define BC_COMPUTE_STATE_T15 t15
#define BC_COMPUTE_STATE_T16 t16
#define BC_COMPUTE_STATE_T17 t17
#define BC_COMPUTE_STATE_T18 t18
#define BC_COMPUTE_STATE_T19 t19

#define BC_COMPUTE_STATE_U0 u0
#define BC_COMPUTE_STATE_U1 u1
#define BC_COMPUTE_STATE_U2 u2
#define BC_COMPUTE_STATE_U3 u3
#define BC_COMPUTE_STATE_U4 u4
#define BC_COMPUTE_STATE_U5 u5
#define BC_COMPUTE_STATE_U6 u6
#define BC_COMPUTE_STATE_U7 u7

#define BC_COMPUTE_STATE_CB0 b0
#define BC_COMPUTE_STATE_CB1 b1
#define BC_COMPUTE_STATE_CB2 b2
#define BC_COMPUTE_STATE_CB3 b3
#define BC_COMPUTE_STATE_CB4 b4
#define BC_COMPUTE_STATE_CB5 b5
#define BC_COMPUTE_STATE_CB6 b6
#define BC_COMPUTE_STATE_CB7 b7

// Bound to vertex, hull, domain, geometry, pixel and compute shader
cbuffer g_cb_global_state					: register(BC_RENDER_PASS_STATE_CB0)
{
    float4x4 g_view							: packoffset(c0);
    float4x4 g_view_inv						: packoffset(c4);
    float4x4 g_projection					: packoffset(c8);
    float4x4 g_view_projection				: packoffset(c12);
    float4x4 g_view_projection_inv			: packoffset(c16);
    int g_screen_width						: packoffset(c20.x);
    int g_screen_height						: packoffset(c20.y);
    float g_near_plane						: packoffset(c20.z);
    float g_far_plane						: packoffset(c20.w);
    float3 g_camera_position				: packoffset(c21);
    double g_total_elapsed					: packoffset(c22.x);
    double g_total_elapsed_second			: packoffset(c22.z);
    float g_elapsed							: packoffset(c23.x);
    float g_elapsed_second					: packoffset(c23.y);
	float g_global_scale					: packoffset(c24.x);
	float3 g_global_light_direction			: packoffset(c24.y);
	float3 g_global_light_color				: packoffset(c25.x);
	float g_global_light_intensity			: packoffset(c25.w);
	float3 g_global_light_ambient_color		: packoffset(c26.x);
	float g_global_light_ambient_intensity	: packoffset(c26.w);
	float3 g_global_wind_direction			: packoffset(c27.x);
	float g_global_wind_power				: packoffset(c27.w);
}

// Bound to vertex, hull, domain shader
cbuffer g_cb_object_state					: register(BC_RENDER_STATE_CB0)
{
	float4x4 g_world_view_projection		: packoffset(c0);
    float4x4 g_world						: packoffset(c4);
	float4x4 g_bone_transforms[90]			: packoffset(c8);
}

static const float g_specular_power_scale = 32;
static const float3 g_gravity_dir = float3(0, -1, 0);
static const float g_gravity_force = 9.8;