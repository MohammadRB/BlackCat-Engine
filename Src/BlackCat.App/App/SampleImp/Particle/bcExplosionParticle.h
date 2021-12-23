// [04/06/2021 MRB]

#pragma once

#include "Core/Utility/bcRandom.h"
#include "Core/bcUtility.h"
#include "Game/System/Render/Particle/bcParticleBuilder.h"

namespace black_cat
{
	class bc_big_explosion_particle
	{
	public:
		game::bc_particle_builder operator()(core::bc_random& p_random) const;
	};

	inline game::bc_particle_builder bc_big_explosion_particle::operator()(core::bc_random& p_random) const
	{
		core::bc_array<core::bc_vector3f, 30> l_random_directions;
		bc_randomize_direction
		(
			p_random,
			core::bc_vector3f::up(),
			180,
			std::begin(l_random_directions),
			std::end(l_random_directions)
		);

		game::bc_particle_builder l_builder;
		
		l_builder.emitter(core::bc_vector3f(0), core::bc_vector3f::up())
			.with_emission_deviation(180)
			.with_texture(4)
			.with_particles_color({ 0.2f, 0.2f, 0.2f })
			.with_particle_size(2, 15)
			.with_particle_size_curve(game::bc_particle_builder::s_curve_fast_step2)
			.with_particle_velocity_curve(game::bc_particle_builder::s_curve_fast_step3, 0.15f)
			.with_particles_rotation(10)
			.emit_particles(100, 5, 700, 0.15f);
		l_builder.emitter(core::bc_vector3f(0, -8, 0), core::bc_vector3f::up())
			.with_emission_deviation(180, { 0, -0.9f, 0 })
			.with_texture(4)
			.with_particles_color({ 0.7f, 0.7f, 0.7f })
			.with_particle_size(7, 15)
			.with_particle_size_curve(game::bc_particle_builder::s_curve_fast_step2)
			.with_particle_velocity_curve(game::bc_particle_builder::s_curve_fast_step3, 0.09f)
			.with_particles_rotation(10)
			.emit_particles(100, 8, 2500, 0.05f)
			.duplicate_last(core::bc_vector3f(0, -8, 0), core::bc_vector3f::up())
			.emit_particles(50, 8, 2000, 0.05f);

		for (auto& l_direction : l_random_directions)
		{
			const auto l_position = core::bc_vector3f(l_direction.x, 0, l_direction.z) * 6;
			const auto l_emitter_energy = l_direction.y;

			l_builder.emitter(l_position, l_direction, 0.2f, 1500 * l_emitter_energy, 0.1f)
				.with_direction_deviation(20)
				.with_emission_deviation(180)
				.with_velocity_curve(game::bc_particle_builder::s_curve_fast_step4)
				.with_texture(7)
				.with_particles_color({ 0.4f, 0.4f, 0.4f }, 1.0)
				.with_particle_size(1.0f, 5.0f)
				.with_particle_size_curve(game::bc_particle_builder::s_curve_fast_step4)
				.with_particle_velocity_curve(game::bc_particle_builder::s_curve_fast_step3, 2)
				.emit_particles(30, 5, 4.0f * l_emitter_energy, 0.15f);
		}

		return l_builder;
	}
}