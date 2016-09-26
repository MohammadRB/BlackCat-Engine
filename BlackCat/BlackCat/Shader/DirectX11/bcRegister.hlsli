// [07/15/2016 MRB]

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

#define BC_RENDER_PASS_STATE_T0 t0
#define BC_RENDER_PASS_STATE_T1 t1
#define BC_RENDER_PASS_STATE_T2 t2
#define BC_RENDER_PASS_STATE_T3 t3

#define BC_RENDER_PASS_STATE_CB0 b0
#define BC_RENDER_PASS_STATE_CB1 b1
#define BC_RENDER_PASS_STATE_CB2 b2
#define BC_RENDER_PASS_STATE_CB3 b3

// RenderState registers

#define BC_RENDER_STATE_T0 t4
#define BC_RENDER_STATE_T1 t5
#define BC_RENDER_STATE_T2 t6
#define BC_RENDER_STATE_T3 t7

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

#define BC_COMPUTE_STATE_T0 t0
#define BC_COMPUTE_STATE_T1 t1
#define BC_COMPUTE_STATE_T2 t2
#define BC_COMPUTE_STATE_T3 t3
#define BC_COMPUTE_STATE_T4 t4
#define BC_COMPUTE_STATE_T5 t5
#define BC_COMPUTE_STATE_T6 t6
#define BC_COMPUTE_STATE_T7 t7

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

// Bound to vertex, hull, domain, geometry, pixel shader
cbuffer g_cb_global_state       : register(BC_RENDER_PASS_STATE_CB0)
{
    float4x4 g_view             : packoffset(c0);
    float4x4 g_projection       : packoffset(c4);
    float4x4 g_viewprojection   : packoffset(c8);
    float3 g_camera_position    : packoffset(c12);
    double g_total_elapsed      : packoffset(c13.x);
    float g_elapsed             : packoffset(c13.z);
    float g_elapsed_second      : packoffset(c13.w);
}

// Bound to vertex shader
cbuffer g_cb_object_state       : register(BC_RENDER_STATE_CB0)
{
    float4x4 g_world            : packoffset(c0);
}