// [12/06/2020 MRB]

#include "..\bcRegister.hlsli"
#include "..\bcHelper.hlsli"
#include "bcParticle.hlsli"

#define THREAD_GROUP_SIZE 1024

RWStructuredBuffer<alive_particle> g_alive_particles	: register(BC_COMPUTE_STATE_U0);

groupshared alive_particle sort_shared_data[THREAD_GROUP_SIZE];

[numthreads(THREAD_GROUP_SIZE, 1, 1)]
void bitonic_group_sort(uint3 p_group_id : SV_GroupID, uint p_group_index : SV_GroupIndex, uint3 p_group_thread_id : SV_GroupThreadID, uint3 p_dispatch_thread_id : SV_DispatchThreadID)
{
	sort_shared_data[p_group_index] = g_alive_particles[p_dispatch_thread_id.x];
	
	GroupMemoryBarrierWithGroupSync();

	for (uint l_bitonic_array_size = 2; l_bitonic_array_size <= THREAD_GROUP_SIZE; l_bitonic_array_size <<= 1)
	{
		for (uint l_bitonic_compare = l_bitonic_array_size >> 1; l_bitonic_compare > 0; l_bitonic_compare >>= 1)
		{
			bool l_condition1 = (sort_shared_data[p_group_index & ~l_bitonic_compare].m_distance >= sort_shared_data[p_group_index | l_bitonic_compare].m_distance);
			bool l_condition2 = (bool) (l_bitonic_array_size & p_dispatch_thread_id.x);

			alive_particle l_result_subset[] = { sort_shared_data[p_group_index ^ l_bitonic_compare], sort_shared_data[p_group_index] };
			uint l_result_index = (l_condition1 == l_condition2) ? 0 : 1;
			
			GroupMemoryBarrierWithGroupSync();
		
			sort_shared_data[p_group_index] = l_result_subset[l_result_index];
		
			GroupMemoryBarrierWithGroupSync();
		}
	}
	
	g_alive_particles[p_dispatch_thread_id.x] = sort_shared_data[p_group_index];
}