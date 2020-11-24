// [11/19/2020 MRB]

#include "..\bcRegister.hlsli"
#include "..\bcHelper.hlsli"
#include "bcParticle.hlsli"

#define THREAD_GROUP_SIZE 16

RWStructuredBuffer<alive_particle> g_alive_indices	: register(BC_COMPUTE_STATE_U0);
Buffer<uint> g_draw_call_args						: register(BC_COMPUTE_STATE_T0);

void swap(alive_particle p1, alive_particle p2)
{
	uint l_index = p1.m_index;
	float l_distance = p1.m_distance;
	
	p1.m_index = p2.m_index;
	p1.m_distance = p2.m_distance;
	p2.m_index = l_index;
	p2.m_distance = l_distance;
};

void sort(alive_particle p1, alive_particle p2, uint p_dir)
{
	if (p_dir == 0 && p1.m_distance > p2.m_distance)
	{
		swap(p1, p2);
	}
	else if (p_dir == 1 && p1.m_distance < p2.m_distance)
	{
		swap(p1, p2);
	}
}

[numthreads(THREAD_GROUP_SIZE, 1, 1)]
void main(uint3 p_group_id : SV_GroupID, uint p_group_index : SV_GroupIndex, uint3 p_group_thread_id : SV_GroupThreadID, uint3 p_dispatch_thread_id : SV_DispatchThreadID)
{
	uint l_dispatch_index = p_dispatch_thread_id.x;
	uint l_num_alive_particles = g_draw_call_args[1];
	uint l_num_particles;
	uint l_particles_stride;
	g_alive_indices.GetDimensions(l_num_particles, l_particles_stride);
	
	for (uint l_bitonic_array_size = 2; l_bitonic_array_size < l_num_particles; l_bitonic_array_size *= 2)
	{
		for (uint l_bitonic_compare = l_bitonic_array_size / 2; l_bitonic_compare > 0; l_bitonic_compare /= 2)
		{
			uint l_read_index = l_dispatch_index + ((l_dispatch_index / l_bitonic_compare) * l_bitonic_compare);
			uint l_compare_index = l_read_index + l_bitonic_compare;
			uint l_sort_direction = (l_read_index / l_bitonic_array_size) % 2; // 0 asc, 1 desc
			
			alive_particle l_read_particle = g_alive_indices[l_read_index];
			alive_particle l_compare_particle = g_alive_indices[l_compare_index];
					
			uint l_read_is_out_of_bound = clamp(l_read_index - l_num_alive_particles - 1, 0, 1);
			uint l_compare_is_out_of_bound = clamp(l_compare_index - l_num_alive_particles - 1, 0, 1);
			
			// if read or compare index is bigger than l_num_alive_particles distance must be max
			l_read_particle.m_distance += (3.402823466e+38F - l_read_particle.m_distance) * l_read_is_out_of_bound;
			l_compare_particle.m_distance += (3.402823466e+38F - l_compare_particle.m_distance) * l_compare_is_out_of_bound;
			
			sort(l_read_particle, l_compare_particle, l_sort_direction);
			
			DeviceMemoryBarrierWithGroupSync();
		}
	}
}