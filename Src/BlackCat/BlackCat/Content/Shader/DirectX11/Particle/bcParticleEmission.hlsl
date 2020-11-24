// [11/19/2020 MRB]

#include "..\bcRegister.hlsli"
#include "..\bcHelper.hlsli"
#include "bcParticle.hlsli"

#define THREAD_GROUP_SIZE 1
#define PARTICLE_RANDOM_COEFFICIENT .1f

StructuredBuffer<emitter> g_emitters			: register(BC_COMPUTE_STATE_T0);
RWStructuredBuffer<particle> g_particles		: register(BC_COMPUTE_STATE_U0);
ConsumeStructuredBuffer<uint> g_dead_indices	: register(BC_COMPUTE_STATE_U1);

float randomize(float p_value, float p_rnd, float p_coefficent)
{
	return p_value + (p_rnd * (p_coefficent * p_value));
}

[numthreads(THREAD_GROUP_SIZE, 1, 1)]
void main(uint3 p_group_id : SV_GroupID, uint p_group_index : SV_GroupIndex, uint3 p_group_thread_id : SV_GroupThreadID, uint3 p_dispatch_thread_id : SV_DispatchThreadID)
{
	emitter l_emitter = g_emitters[p_dispatch_thread_id.x];
	float l_particles_deviation = l_emitter.m_emission_deviation / 180.f;
	
	for (uint i = 0; i < l_emitter.m_particles_count; ++i)
	{
		float l_random = bc_random(i + 1);
		float l_random_range = (l_random - .5f) * 2;
		
		uint l_dead_index = g_dead_indices.Consume();
		particle l_new_particle = g_particles[l_dead_index];
		
		float3 l_new_particle_dir = normalize(float3
		(
			randomize(l_emitter.m_emission_direction.x, l_random_range, l_particles_deviation),
			randomize(l_emitter.m_emission_direction.y, l_random_range, l_particles_deviation),
			randomize(l_emitter.m_emission_direction.z, l_random_range, l_particles_deviation)
		));
		
		l_new_particle.m_position = l_emitter.m_position;
		l_new_particle.m_lifetime = randomize(l_emitter.m_particles_lifetime, l_random_range, PARTICLE_RANDOM_COEFFICIENT);
		l_new_particle.m_direction = l_new_particle_dir;
		l_new_particle.m_force = randomize(l_emitter.m_particles_force, l_random_range, PARTICLE_RANDOM_COEFFICIENT);
		l_new_particle.m_mass = l_emitter.m_particles_mass;
		l_new_particle.m_texture_index = l_emitter.m_texture_index;
		l_new_particle.m_sprite_index = l_emitter.m_sprite_index;
	}
}