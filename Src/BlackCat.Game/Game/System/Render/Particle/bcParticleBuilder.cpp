// [11/14/2020 MRB]

#include "Game/GamePCH.h"

#include "Game/System/Render/Particle/bcParticleBuilder.h"

namespace black_cat
{
	namespace game
	{
		const bc_particle_curve bc_particle_builder::curve_fast_steady = { core::bc_vector2f{0.f, 0.f}, { .4f,.8f }, { 1.f, 1.f } };
		const bc_particle_curve bc_particle_builder::curve_linear = { core::bc_vector2f{0.f, 0.f}, { .5f,.5f }, { 1.f, 1.f } };

		bc_particle_builder1::bc_particle_builder1(bc_particle_builder& p_builder, bc_particle_emitter_trait& p_emitter)
			: m_builder(p_builder),
			m_emitter(p_emitter)
		{
		}

		bc_particle_builder1& bc_particle_builder1::with_deviation(bcUINT32 p_angle)
		{
			m_emitter.m_deviation_angle = p_angle;
			return *this;
		}

		bc_particle_builder1& bc_particle_builder1::with_texture(bcUINT32 p_texture_index, bcUINT32 p_sprite_index) noexcept
		{
			m_emitter.m_texture_index = p_texture_index;
			m_emitter.m_sprite_index = p_sprite_index;
			return *this;
		}

		bc_particle_builder& bc_particle_builder1::emit(bcUINT32 p_particles_total_count, 
			bcFLOAT p_particles_lifetime, 
			bcFLOAT p_particles_force, 
			bcFLOAT p_particles_mass)
		{
			m_emitter.m_particles_total_count = p_particles_total_count;
			m_emitter.m_particles_lifetime = p_particles_lifetime;
			m_emitter.m_particles_energy = p_particles_force;
			m_emitter.m_particles_mass = p_particles_mass;
			return m_builder;
		}

		bc_particle_builder::bc_particle_builder() = default;

		bc_particle_builder1 bc_particle_builder::emitter(const core::bc_vector3f& p_position, const core::bc_vector3f& p_direction)
		{
			return emitter(p_position, p_direction, 0, 0, 0);
		}

		bc_particle_builder1 bc_particle_builder::emitter(const core::bc_vector3f& p_position,
			const core::bc_vector3f& p_direction,
			bcFLOAT p_lifetime,
			bcFLOAT p_force,
			bcFLOAT p_mass)
		{
			bc_particle_emitter_trait l_emitter;
			l_emitter.m_position = p_position;
			l_emitter.m_lifetime = p_lifetime;
			l_emitter.m_direction = p_direction;
			l_emitter.m_force = p_force;
			l_emitter.m_mass = p_mass;
			l_emitter.m_deviation_angle = 45;
			l_emitter.m_texture_index = 0;
			l_emitter.m_sprite_index = 0;
			l_emitter.m_particles_total_count = 0;
			l_emitter.m_particles_lifetime = 0;
			l_emitter.m_particles_energy = 0;
			l_emitter.m_particles_mass = 0;

			m_emitters.push_back(l_emitter);

			return bc_particle_builder1(*this, *m_emitters.rbegin());
		}
	}
}