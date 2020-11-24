// [11/19/2020 MRB]

#include "..\bcRegister.hlsli"
#include "..\bcHelper.hlsli"
#include "bcParticle.hlsli"

#define THREAD_GROUP_SIZE 16

RWStructuredBuffer<particle> g_particles				: register(BC_COMPUTE_STATE_U0);
RWStructuredBuffer<alive_particle> g_alive_indices		: register(BC_COMPUTE_STATE_U1);
AppendStructuredBuffer<uint> g_dead_indices				: register(BC_COMPUTE_STATE_U2);
//RWStructuredBuffer<uint2> g_draw_call_args				: register(BC_COMPUTE_STATE_U3);

[numthreads(THREAD_GROUP_SIZE, 1, 1)]
void main(uint3 p_group_id : SV_GroupID, uint p_group_index : SV_GroupIndex, uint3 p_group_thread_id : SV_GroupThreadID, uint3 p_dispatch_thread_id : SV_DispatchThreadID)
{
	uint l_particle_index = p_dispatch_thread_id.x;
	particle l_particle = g_particles[l_particle_index];

	l_particle.m_age += g_elapsed;
	if (l_particle.m_age > l_particle.m_lifetime)
	{
		// If it was already dead
		if (l_particle.m_age - g_elapsed <= l_particle.m_lifetime)
		{
			g_dead_indices.Append(l_particle_index);
		}
		return;
	}

	float l_acceleration = l_particle.m_force / l_particle.m_mass;
	float l_normalized_age = 1 - pow(max(1.f, l_particle.m_age / l_particle.m_lifetime), 2);
	float l_velocity_to_apply = l_normalized_age * l_acceleration;
	float l_velocity_to_wind_power = g_wind_power / max(l_velocity_to_apply, 1.f);
	
	l_particle.m_position += (l_particle.m_direction * l_velocity_to_apply) + (l_velocity_to_wind_power * g_wind_dir * g_elapsed_second);
	l_particle.m_force *= l_normalized_age;
	
	alive_particle l_alive_particle;
	l_alive_particle.m_index = l_particle_index;
	l_alive_particle.m_distance = length(l_particle.m_position - g_camera_position);
	
	uint l_alive_count = g_alive_indices.IncrementCounter() + 1;
	g_alive_indices[l_alive_count] = l_alive_particle;
	
	//DeviceMemoryBarrierWithGroupSync();
	
	//if (l_particle_index == 0)
	//{
	//	uint l_alive_count = g_alive_indices.IncrementCounter();
	//	g_alive_indices.DecrementCounter();
		
	//	g_draw_call_args[0] = uint2(1, 0); // Number of alive particles will be written by copy_structure_count
	//	g_draw_call_args[1] = uint2(0, 0);
	//}
}