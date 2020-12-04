// [11/19/2020 MRB]

#include "..\bcRegister.hlsli"
#include "..\bcHelper.hlsli"
#include "bcParticle.hlsli"

#define THREAD_GROUP_SIZE 1024

//Buffer<uint> g_draw_call_args							: register(BC_COMPUTE_STATE_T0);
RWStructuredBuffer<alive_particle> g_alive_particles	: register(BC_COMPUTE_STATE_U0);
cbuffer g_sort_cbuffer									: register(BC_COMPUTE_STATE_CB1)
{
	uint g_num_particles								: packoffset(c0.x);
	uint g_sort_array_size								: packoffset(c0.y);
	uint g_sort_array_size_mask							: packoffset(c0.z);
	uint g_matrix_width									: packoffset(c1.x);
	uint g_matrix_height								: packoffset(c1.y);
};

groupshared alive_particle sort_shared_data[THREAD_GROUP_SIZE];

[numthreads(THREAD_GROUP_SIZE, 1, 1)]
void bitonic_sort(uint3 p_group_id : SV_GroupID, uint p_group_index : SV_GroupIndex, uint3 p_group_thread_id : SV_GroupThreadID, uint3 p_dispatch_thread_id : SV_DispatchThreadID)
{
	sort_shared_data[p_group_index] = g_alive_particles[p_dispatch_thread_id.x];
	
	GroupMemoryBarrierWithGroupSync();

	for (uint l_bitonic_compare = g_sort_array_size >> 1; l_bitonic_compare > 0; l_bitonic_compare >>= 1)
	{
		bool l_condition1 = (sort_shared_data[p_group_index & ~l_bitonic_compare].m_distance <= sort_shared_data[p_group_index | l_bitonic_compare].m_distance);
		bool l_condition2 = (bool) (g_sort_array_size_mask & p_dispatch_thread_id.x);

		alive_particle l_result_subset[] = { sort_shared_data[p_group_index ^ l_bitonic_compare], sort_shared_data[p_group_index] };
		uint l_result_index = (l_condition1 == l_condition2) ? 0 : 1;
		
		GroupMemoryBarrierWithGroupSync();
		
		sort_shared_data[p_group_index] = l_result_subset[l_result_index];
		
		GroupMemoryBarrierWithGroupSync();
	}
	
	g_alive_particles[p_dispatch_thread_id.x] = sort_shared_data[p_group_index];
}

//alive_particle select_bitonic(alive_particle p1, alive_particle p2, uint p_sort_dir)
//{
//	if (p_sort_dir == 1 && p1.m_distance < p2.m_distance)
//	{
//		return p1;
//	}
//	else if (p_sort_dir == -1 && p1.m_distance > p2.m_distance)
//	{
//		return p1;
//	}
	
//	return p2;
//};

//groupshared alive_particle sort_shared_data[THREAD_GROUP_SIZE];

//[numthreads(THREAD_GROUP_SIZE, 1, 1)]
//void bitonic_sort(uint3 p_group_id : SV_GroupID, uint p_group_index : SV_GroupIndex, uint3 p_group_thread_id : SV_GroupThreadID, uint3 p_dispatch_thread_id : SV_DispatchThreadID)
//{
//	uint l_dispatch_index = p_dispatch_thread_id.x;
//	uint l_num_alive_particles = g_draw_call_args[1];
	
//	alive_particle l_dispatch_particle = g_alive_particles[l_dispatch_index];
	
//	//// if read or compare index is bigger than l_num_alive_particles distance must be max
//	//uint l_read_is_out_of_bound = clamp((int) l_dispatch_index - (int) l_num_alive_particles - 1, 0, 1);
//	//l_dispatch_particle.m_distance += (MAX_FLOAT - l_dispatch_particle.m_distance) * l_read_is_out_of_bound;
	
//	sort_shared_data[p_group_index] = l_dispatch_particle;
	
//	GroupMemoryBarrierWithGroupSync();
	
//	uint l_sort_compare_directions[] = { 1, -1 };
	
//	for (uint l_bitonic_array_size = g_sort_array_start; l_bitonic_array_size <= g_sort_array_size; l_bitonic_array_size *= 2)
//	{
//		for (uint l_bitonic_compare = l_bitonic_array_size / 2; l_bitonic_compare > 0; l_bitonic_compare /= 2)
//		{
//			uint l_sort_direction = l_sort_compare_directions[(p_group_index / l_bitonic_array_size) % 2]; // 1 asc, 1 desc
//			uint l_compare_direction = l_sort_compare_directions[(p_group_index / l_bitonic_compare) % 2];
//			uint l_read_index = p_group_index;
//			uint l_compare_index = p_group_index + (l_bitonic_compare * l_compare_direction);
			
//			alive_particle l_read_particle = sort_shared_data[l_read_index];
//			alive_particle l_compare_particle = sort_shared_data[l_compare_index];
//			alive_particle l_bitonic_particle = select_bitonic(l_read_particle, l_compare_particle, l_sort_direction * l_compare_direction);
			
//			GroupMemoryBarrierWithGroupSync();
			
//			sort_shared_data[l_read_index] = l_bitonic_particle;
//			//sort_shared_data[l_read_index].m_distance = l_bitonic_particle.m_distance;
			
//			GroupMemoryBarrierWithGroupSync();
//		}
//	}

//	GroupMemoryBarrierWithGroupSync();
	
//	g_alive_particles[l_dispatch_index] = sort_shared_data[p_group_index];
//}