// [04/06/2021 MRB]

#pragma once

#include "Game/System/Render/Particle/bcParticleBuilder.h"

namespace black_cat
{
	class bc_sample_rifle_fire_particle
	{
	public:
		game::bc_particle_builder operator()() const;
	};

	inline game::bc_particle_builder bc_sample_rifle_fire_particle::operator()() const
	{
		game::bc_particle_builder l_rifle_fire_builder;
		l_rifle_fire_builder.emitter(core::bc_vector3f(0), core::bc_vector3f::up())
			.with_emission_deviation(180)
			.with_texture(4)
			.with_particles_color({ 0.9f, 0.5f, 0.1f }, 3.0)
			.with_particle_size(0.3f, 0.9f)
			.with_particle_size_curve(game::bc_particle_builder::s_curve_fast_step4)
			.with_particle_velocity_curve(game::bc_particle_builder::s_curve_fast_step5, 0.08f)
			.emit_particles(10, 0.08f, 40, 0.0f)
			.duplicate_last(core::bc_vector3f(0), core::bc_vector3f::up(), 0.04f, 30, 0)
			.with_emission_deviation(5)
			.with_particle_size(0.1f, 0.3f)
			.with_particle_velocity_curve(game::bc_particle_builder::s_curve_fast_step5, 0.08f)
			.emit_particles(20, 0.08f, 20, 0.0f);

		return l_rifle_fire_builder;
	}
}
