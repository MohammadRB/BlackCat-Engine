// [03/22/2022 MRB]

#pragma once

#include "Game/System/Render/Particle/bcParticleBuilder.h"

namespace box
{
	using namespace black_cat;

	class bx_falling_leaf_particle
	{
	public:
		game::bc_particle_builder operator()() const;
	};

	inline game::bc_particle_builder bx_falling_leaf_particle::operator()() const
	{
		return game::bc_particle_builder()
			.emitter({ 0, 0, 0 }, { 0, 1, 0 }, std::numeric_limits<bcFLOAT>::max(), 0, 0)
			.with_emission_position_deviation(5)
			.with_emission_direction_deviation(180)
			.with_texture(11)
			.with_particle_velocity_curve(game::bc_particle_builder::s_curve_fast_step1, 1.f)
			.with_particles_color({ .3f, .6f, .3f }, 1.2f)
			.with_particle_size(.3f, .3f)
			.with_particles_rotation(50)
			.with_particle_fade_curve(game::bc_particle_builder::s_curve_constant)
			.emit_particles_with_per_second_count(1, 15, 7, 0.08f)
			.duplicate_last({ 0, 0, 0 }, { 0, 1, 0 }, std::numeric_limits<bcFLOAT>::max(), 0, 0)
			.with_texture(12)
			.emit_particles_with_per_second_count(1, 15, 10, 0.09f);
	}
}
