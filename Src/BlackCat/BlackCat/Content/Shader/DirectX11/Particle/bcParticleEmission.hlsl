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

float randomize_normal(float p_value, float p_rnd, float p_coefficent)
{
	return p_value + (p_rnd * p_coefficent);
}

[numthreads(THREAD_GROUP_SIZE, 1, 1)]
void main(uint3 p_group_id : SV_GroupID, uint p_group_index : SV_GroupIndex, uint3 p_group_thread_id : SV_GroupThreadID, uint3 p_dispatch_thread_id : SV_DispatchThreadID)
{
	uint l_emitter_index = p_dispatch_thread_id.x;
	emitter l_emitter = g_emitters[l_emitter_index];
	float l_particles_deviation = l_emitter.m_emission_deviation / 180.f;
	
	for (uint i = 0; i < l_emitter.m_particles_count; ++i)
	{
		float l_random[] = { bc_random((i + 3) + g_total_elapsed), bc_random((i + 7) + g_total_elapsed), bc_random((i + 5) + g_total_elapsed) };
		float l_random_range[] = { (l_random[0] - .5f) * 2, (l_random[1] - .5f) * 2, (l_random[2] - .5f) * 2 };
		particle l_new_particle;
		
		float3 l_new_particle_dir = normalize
		(
			float3
			(
				randomize_normal(l_emitter.m_emission_direction.x, l_random_range[0], l_particles_deviation),
				randomize_normal(l_emitter.m_emission_direction.y, l_random_range[1], l_particles_deviation),
				randomize_normal(l_emitter.m_emission_direction.z, l_random_range[2], l_particles_deviation)
			)
		);
		
		l_new_particle.m_position = lerp(l_emitter.m_prev_position, l_emitter.m_position, (i + 1) / l_emitter.m_particles_count);
		l_new_particle.m_lifetime = randomize(l_emitter.m_particles_lifetime, l_random_range[i % 3], PARTICLE_RANDOM_COEFFICIENT);
		l_new_particle.m_direction = l_new_particle_dir;
		l_new_particle.m_age = 0;
		l_new_particle.m_force = randomize(l_emitter.m_particles_force, l_random_range[i % 3], PARTICLE_RANDOM_COEFFICIENT);
		l_new_particle.m_mass = randomize(l_emitter.m_particles_mass, l_random_range[i % 3], PARTICLE_RANDOM_COEFFICIENT);
		l_new_particle.m_size = randomize(l_emitter.m_particles_size, l_random_range[i % 3], PARTICLE_RANDOM_COEFFICIENT);
		l_new_particle.m_current_size = l_new_particle.m_size /= 2;
		l_new_particle.m_texture_index = l_emitter.m_texture_index;
		l_new_particle.m_sprite_index = l_emitter.m_sprite_index;
		l_new_particle.m_curve_index = l_emitter.m_particles_curve_index;
		
		uint l_dead_index = g_dead_indices.Consume();
		g_particles[l_dead_index] = l_new_particle;
	}
}