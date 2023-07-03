// [19/11/2020 MRB]

#include "../bcRegister.hlsli"
#include "../bcHelper.hlsli"
#include "bcParticle.hlsli"

#define THREAD_GROUP_SIZE 1
#define PARTICLE_RANDOM_COEFFICIENT .2f

StructuredBuffer<emitter> g_emitters				: register(BC_COMPUTE_STATE_T0);
RWStructuredBuffer<particle> g_particles			: register(BC_COMPUTE_STATE_U0);
ConsumeStructuredBuffer<uint> g_dead_indices		: register(BC_COMPUTE_STATE_U1);

float randomize(float p_value, float p_rnd, float p_coefficient)
{
	return p_value + (p_rnd * (p_coefficient * p_value));
}

float randomize_normal(float p_value, float p_rnd, float p_coefficient)
{
	return p_value + (p_rnd * p_coefficient);
}

[numthreads(THREAD_GROUP_SIZE, 1, 1)]
void main(uint3 p_group_id : SV_GroupID, uint p_group_index : SV_GroupIndex, uint3 p_group_thread_id : SV_GroupThreadID, uint3 p_dispatch_thread_id : SV_DispatchThreadID)
{
	const uint l_emitter_index = p_dispatch_thread_id.x;
	const emitter l_emitter = g_emitters[l_emitter_index];
	const float l_particles_deviation = l_emitter.m_emission_direction_deviation / 180.f;
	//const float l_emitter_position_length = length(l_emitter.m_position);
	
	for (uint l_ite = 0; l_ite < l_emitter.m_particles_count; ++l_ite)
	{
		const float l_randoms[] = { bc_random((l_ite + 3) + g_total_elapsed), bc_random((l_ite + 7) + g_total_elapsed), bc_random((l_ite + 5) + g_total_elapsed) };
		const float l_randoms_range[] = { (l_randoms[0] - .5f) * 2, (l_randoms[1] - .5f) * 2, (l_randoms[2] - .5f) * 2 };
		const float l_random = l_randoms_range[l_ite % 3];

		const float3 l_new_particle_position = float3(l_randoms_range[0], l_randoms_range[1], l_randoms_range[2]) * l_emitter.m_emission_position_deviation_plane * l_emitter.m_emission_position_deviation;
		const float3 l_new_particle_dir = normalize
		(
			float3
			(
				randomize_normal(l_emitter.m_emission_direction.x, l_randoms_range[0], l_particles_deviation),
				randomize_normal(l_emitter.m_emission_direction.y, l_randoms_range[1], l_particles_deviation),
				randomize_normal(l_emitter.m_emission_direction.z, l_randoms_range[2], l_particles_deviation)
			)
		);
		const float3 l_new_particle_color = l_emitter.m_particles_color;// + (bc_noise((g_total_elapsed_second + i + l_emitter_position_length) / 30, 0.05f) * min(1, l_emitter.m_particles_color_intensity));
		const float l_emitter_energy = min(1, l_emitter.m_energy + 0.5f);

		particle l_new_particle;
		l_new_particle.m_position = lerp(l_emitter.m_prev_position, l_emitter.m_position, (l_ite + 1.f) / l_emitter.m_particles_count) + l_new_particle_position;
		l_new_particle.m_direction = l_new_particle_dir;
		l_new_particle.m_color = l_new_particle_color;
		l_new_particle.m_color_intensity = l_emitter.m_particles_color_intensity;
		l_new_particle.m_lifetime = randomize(l_emitter.m_particles_lifetime, l_random, PARTICLE_RANDOM_COEFFICIENT);
		l_new_particle.m_age = 0;
		l_new_particle.m_force = randomize(l_emitter.m_particles_force, l_random, PARTICLE_RANDOM_COEFFICIENT);
		l_new_particle.m_mass = randomize(l_emitter.m_particles_mass, l_random, PARTICLE_RANDOM_COEFFICIENT) * (1 / l_emitter_energy);
		l_new_particle.m_start_size = randomize(l_emitter.m_particles_start_size, l_random, PARTICLE_RANDOM_COEFFICIENT) * l_emitter_energy;
		l_new_particle.m_end_size = randomize(l_emitter.m_particles_end_size, l_random, PARTICLE_RANDOM_COEFFICIENT) * l_emitter_energy;
		l_new_particle.m_size = l_new_particle.m_start_size;
		l_new_particle.m_fade = 1;
		l_new_particle.m_rotation = randomize(l_emitter.m_particles_rotation, l_random, PARTICLE_RANDOM_COEFFICIENT);
		l_new_particle.m_sprite_index = l_emitter.m_sprite_index;
		l_new_particle.m_velocity_curve_index = l_emitter.m_particles_velocity_curve_index;
		l_new_particle.m_velocity_curve_duration = l_emitter.m_particles_velocity_curve_duration;
		l_new_particle.m_size_curve_index = l_emitter.m_particles_size_curve_index;
		l_new_particle.m_fade_curve_index = l_emitter.m_particles_fade_curve_index;

		const uint l_dead_index = g_dead_indices.Consume();
		g_particles[l_dead_index] = l_new_particle;
	}
}