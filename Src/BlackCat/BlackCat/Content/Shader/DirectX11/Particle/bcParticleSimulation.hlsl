// [11/19/2020 MRB]

#include "..\bcRegister.hlsli"
#include "..\bcHelper.hlsli"
#include "bcParticle.hlsli"

#define THREAD_GROUP_SIZE 1024
#define CURVE_COUNT 11
#define CURVE_SAMPLE_COUNT 10
cbuffer g_curves_cbuffer : register(BC_COMPUTE_STATE_CB1)
{
	float g_curves[CURVE_COUNT][CURVE_SAMPLE_COUNT];
};

RWStructuredBuffer<particle> g_particles				: register(BC_COMPUTE_STATE_U0);
RWStructuredBuffer<alive_particle> g_alive_indices		: register(BC_COMPUTE_STATE_U1);
AppendStructuredBuffer<uint> g_dead_indices				: register(BC_COMPUTE_STATE_U2);

float sample_curve(uint p_curve_index, float p_time)
{
	p_time = min(1.0f, p_time);
	const float l_time = p_time * (CURVE_SAMPLE_COUNT - 1);
	const uint l_lower_band = floor(l_time);
	const uint l_upper_band = ceil(l_time);

	const float l_lower_band_time = (float) l_lower_band / (CURVE_SAMPLE_COUNT - 1);
	const float l_lower_band_sample = g_curves[p_curve_index][l_lower_band];
	const float l_upper_band_time = (float) l_upper_band / (CURVE_SAMPLE_COUNT - 1);
	const float l_upper_band_sample = g_curves[p_curve_index][l_upper_band];

	const float l_normalized_time_between_samples = (p_time - l_lower_band_time) / (l_upper_band_time - l_lower_band_time + .001f);
	return l_normalized_time_between_samples * (l_upper_band_sample - l_lower_band_sample) + l_lower_band_sample;
}

[numthreads(THREAD_GROUP_SIZE, 1, 1)]
void main(uint3 p_group_id : SV_GroupID, uint p_group_index : SV_GroupIndex, uint3 p_group_thread_id : SV_GroupThreadID, uint3 p_dispatch_thread_id : SV_DispatchThreadID)
{
	uint l_particle_index = p_dispatch_thread_id.x;
	particle l_particle = g_particles[l_particle_index];
	
	// Reset alive particles
	g_alive_indices[l_particle_index].m_index = 0;
	g_alive_indices[l_particle_index].m_distance = MAX_FLOAT;

	DeviceMemoryBarrierWithGroupSync();
	
	l_particle.m_age += g_elapsed_second;
	if (l_particle.m_age > l_particle.m_lifetime)
	{
		// If it is not already dead
		if (l_particle.m_age - g_elapsed_second < l_particle.m_lifetime)
		{
			l_particle.m_age = MAX_FLOAT;
			g_particles[l_particle_index] = l_particle;
			
			g_dead_indices.Append(l_particle_index);
		}

		return;
	}
	
	//float l_acceleration = l_particle.m_force / l_particle.m_mass;
	//float l_normalized_age = sample_curve(l_particle.m_curve_index, l_particle.m_age / l_particle.m_lifetime);
	//float l_normalized_age_reversed = 1 - l_normalized_age;
	//float l_velocity_to_apply = l_normalized_age_reversed * l_acceleration;
	//float l_velocity_to_wind_power = g_wind_power / max(l_velocity_to_apply * 2, 1.f);
	
	//l_particle.m_position += (l_particle.m_direction * l_velocity_to_apply * g_elapsed_second) + (g_wind_dir * l_velocity_to_wind_power * g_elapsed_second);
	//l_particle.m_current_size = l_particle.m_size + (l_normalized_age * l_particle.m_size);
	//l_particle.m_force -= l_particle.m_mass * g_elapsed_second;

	float l_velocity_curve_value = sample_curve(l_particle.m_velocity_curve_index, l_particle.m_age / l_particle.m_velocity_curve_duration);
	float l_acceleration = l_particle.m_force / l_particle.m_mass * l_velocity_curve_value;

	l_particle.m_position += (l_particle.m_direction * l_acceleration * g_elapsed_second) +
			(g_wind_dir * g_wind_power * g_elapsed_second) +
			(g_gravity_dir * g_gravity_force * l_particle.m_mass * g_elapsed_second);
	l_particle.m_current_size = l_particle.m_size + ((1 - l_velocity_curve_value) * l_particle.m_size);
	l_particle.m_force = max(0, l_particle.m_force - l_particle.m_mass * g_elapsed_second);
	
	g_particles[l_particle_index] = l_particle;
	
	alive_particle l_alive_particle;
	l_alive_particle.m_index = l_particle_index;
	l_alive_particle.m_distance = length(l_particle.m_position - g_camera_position);
	
	uint l_alive_index = g_alive_indices.IncrementCounter();
	g_alive_indices[l_alive_index] = l_alive_particle;
}