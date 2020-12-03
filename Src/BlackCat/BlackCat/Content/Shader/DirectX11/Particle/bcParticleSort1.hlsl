// [11/19/2020 MRB]

#include "..\bcRegister.hlsli"
#include "..\bcHelper.hlsli"
#include "bcParticle.hlsli"

#define THREAD_GROUP_SIZE 1024

Buffer<uint> g_draw_call_args							: register(BC_COMPUTE_STATE_T0);
RWStructuredBuffer<alive_particle> g_alive_indices		: register(BC_COMPUTE_STATE_U0);
cbuffer g_sort_cbuffer									: register(BC_COMPUTE_STATE_CB1)
{
	uint g_num_particles								: packoffset(c0.x);
	uint g_sort_array_size								: packoffset(c0.y);
	uint g_sort_array_size_mask							: packoffset(c0.z);
	uint g_matrix_width									: packoffset(c1.x);
	uint g_matrix_height								: packoffset(c1.y);
};

alive_particle select_bitonic(alive_particle p1, alive_particle p2, uint p_sort_dir)
{
	if (p_sort_dir == 1 && p1.m_distance < p2.m_distance)
	{
		return p1;
	}
	if (p_sort_dir == -1 && p1.m_distance > p2.m_distance)
	{
		return p1;
	}

	return p2;
};

[numthreads(THREAD_GROUP_SIZE, 1, 1)]
void bitonic_sort(uint3 p_group_id : SV_GroupID, uint p_group_index : SV_GroupIndex, uint3 p_group_thread_id : SV_GroupThreadID, uint3 p_dispatch_thread_id : SV_DispatchThreadID)
{
	const uint l_sort_compare_directions[] = { 1, -1 };
	
	for (uint l_bitonic_array_size = 2; l_bitonic_array_size <= g_num_particles; l_bitonic_array_size *= 2)
	{
		for (uint l_bitonic_compare = l_bitonic_array_size / 2; l_bitonic_compare > 0; l_bitonic_compare /= 2)
		{
			uint l_compare_direction = l_sort_compare_directions[(p_dispatch_thread_id.x / l_bitonic_compare) % 2];
			uint l_sort_direction = l_sort_compare_directions[(p_dispatch_thread_id.x / l_bitonic_array_size) % 2]; // 1 asc, -1 desc
			uint l_read_index = p_dispatch_thread_id.x;
			uint l_compare_index = l_read_index + (l_bitonic_compare * l_compare_direction);
			
			alive_particle l_read_particle = g_alive_indices[l_read_index];
			alive_particle l_compare_particle = g_alive_indices[l_compare_index];
			alive_particle l_bitonic_particle = select_bitonic(l_read_particle, l_compare_particle, l_sort_direction * l_compare_direction);
			
			g_alive_indices[l_read_index] = l_bitonic_particle;
		}
	}
}