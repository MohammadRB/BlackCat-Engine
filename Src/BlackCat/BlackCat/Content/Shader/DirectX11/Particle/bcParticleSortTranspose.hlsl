// [11/30/2020 MRB]

#include "..\bcRegister.hlsli"
#include "..\bcHelper.hlsli"
#include "bcParticle.hlsli"

#define TRANSPOSE_BLOCK_SIZE 16

StructuredBuffer<alive_particle> g_alive_particles_r		: register(BC_COMPUTE_STATE_T1);
RWStructuredBuffer<alive_particle> g_alive_particles_w		: register(BC_COMPUTE_STATE_U0);
cbuffer g_sort_cbuffer										: register(BC_COMPUTE_STATE_CB0)
{
	uint g_num_particles									: packoffset(c0.x);
	uint g_sort_array_size									: packoffset(c0.y);
	uint g_sort_array_start									: packoffset(c0.z);
	uint g_matrix_width										: packoffset(c0.w);
	uint g_matrix_height									: packoffset(c1.x);
};

groupshared alive_particle transpose_shared_data[TRANSPOSE_BLOCK_SIZE * TRANSPOSE_BLOCK_SIZE];

[numthreads(TRANSPOSE_BLOCK_SIZE, TRANSPOSE_BLOCK_SIZE, 1)]
void bitonic_matrix_transpose(uint3 p_group_id : SV_GroupID, uint p_group_index : SV_GroupIndex, uint3 p_group_thread_id : SV_GroupThreadID, uint3 p_dispatch_thread_id : SV_DispatchThreadID)
{
	transpose_shared_data[p_group_index] = g_alive_particles_r[p_dispatch_thread_id.y * g_matrix_width + p_dispatch_thread_id.x];
	
	GroupMemoryBarrierWithGroupSync();
	
	uint2 XY = p_dispatch_thread_id.yx - p_group_thread_id.yx + p_group_thread_id.xy;
	g_alive_particles_w[XY.y * g_matrix_height + XY.x] = transpose_shared_data[p_group_thread_id.x * TRANSPOSE_BLOCK_SIZE + p_group_thread_id.y];
}