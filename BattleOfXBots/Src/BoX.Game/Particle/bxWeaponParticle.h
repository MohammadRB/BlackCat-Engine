// [04/06/2021 MRB]

#pragma once

#include "Core/Utility/bcRandom.h"
#include "Game/System/Render/Particle/bcParticleBuilder.h"

namespace box
{
	using namespace black_cat;

	class bx_rifle_fire_particle
	{
	public:
		game::bc_particle_builder operator()() const;
	};

	class bx_bullet_terrain_particle
	{
	public:
		game::bc_particle_builder operator()() const;
	};

	class bx_bullet_soil_particle
	{
	public:
		game::bc_particle_builder operator()() const;
	};

	class bx_bullet_wood_particle
	{
	public:
		game::bc_particle_builder operator()() const;
	};

	class bx_bullet_iron_particle
	{
	public:
		game::bc_particle_builder operator()() const;
	};
	
	inline game::bc_particle_builder bx_rifle_fire_particle::operator()() const
	{
		game::bc_particle_builder l_builder;

		l_builder.emitter(core::bc_vector3f(0), core::bc_vector3f::up())
			.with_emission_direction_deviation(180)
			.with_texture(4)
			.with_particles_color({ 0.9f, 0.5f, 0.1f }, 3.0)
			.with_particle_size(0.3f, 0.9f)
			.with_particle_size_curve(game::bc_particle_builder::s_curve_fast_step4)
			.with_particle_velocity_curve(game::bc_particle_builder::s_curve_fast_step5, 0.08f)
			.emit_particles_with_total_count(10, 0.08f, 40, 0.0f)
			.duplicate_last(core::bc_vector3f(0), core::bc_vector3f::up(), 0.04f, 30, 0)
			.with_emission_direction_deviation(5)
			.with_particle_size(0.1f, 0.3f)
			.with_particle_velocity_curve(game::bc_particle_builder::s_curve_fast_step5, 0.08f)
			.emit_particles_with_total_count(20, 0.08f, 25, 0.0f);

		return l_builder;
	}
	
	inline game::bc_particle_builder bx_bullet_terrain_particle::operator()() const
	{
		const core::bc_array<core::bc_vector3f, 3> l_random_directions = 
		{
			core::bc_vector3f::normalize(core::bc_vector3f(.0, 1, .0)),
			core::bc_vector3f::normalize(core::bc_vector3f(-.3f, .8f, -.3f)),
			core::bc_vector3f::normalize(core::bc_vector3f(.3f, .8f, .3f))
		};
		
		game::bc_particle_builder l_builder;

		for (auto& l_direction : l_random_directions)
		{
			const auto l_position = core::bc_vector3f(l_direction.x, 0, l_direction.z) * 0.5f;
			const auto l_emitter_energy = l_direction.y;
			
			l_builder.emitter(l_position, l_direction, 0.025f, 100 * l_emitter_energy, 0)
				.with_direction_deviation(45)
				.with_emission_direction_deviation(25)
				.with_texture(8)
				.with_particles_color({ 0.7f, 0.7f, 0.7f })
				.with_particle_size(0.1f, 0.6f)
				.with_particles_rotation(5)
				.with_particle_size_curve(game::bc_particle_builder::s_curve_fast_step4)
				.with_particle_velocity_curve(game::bc_particle_builder::s_curve_fast_step5, 0.08f)
				.emit_particles_with_total_count(20, 1.5f * l_emitter_energy, 110 * l_emitter_energy, 0.045f);
		}

		l_builder.emitter(core::bc_vector3f(0, -0.1f, 0), core::bc_vector3f::up())
			.with_emission_position_deviation(1, { 1.f, .2f, 1.f })
			.with_emission_direction_deviation(180)
			.with_texture(4)
			.with_particles_color({ 0.7f, 0.7f, 0.7f })
			.with_particle_size(.9f, 1.5f)
			.with_particle_size_curve(game::bc_particle_builder::s_curve_fast_step2)
			.with_particle_velocity_curve(game::bc_particle_builder::s_curve_fast_step3, 0.09f)
			.with_particles_rotation(10)
			.emit_particles_with_total_count(8, 6, 70, 0.01f);

		return l_builder;
	}

	inline game::bc_particle_builder bx_bullet_soil_particle::operator()() const
	{
		const core::bc_array<core::bc_vector3f, 3> l_random_directions =
		{
			core::bc_vector3f::normalize(core::bc_vector3f(.0, 1.0f, .0)),
			core::bc_vector3f::normalize(core::bc_vector3f(-.3f, .8f, -.3f)),
			core::bc_vector3f::normalize(core::bc_vector3f(.3f, .8f, .3f))
		};

		game::bc_particle_builder l_builder;

		for (auto& l_direction : l_random_directions)
		{
			const auto l_position = core::bc_vector3f(l_direction.x, 0, l_direction.z) * 0.5f;
			const auto l_emitter_energy = l_direction.y;

			l_builder.emitter(l_position, l_direction, 0.025f, 80 * l_emitter_energy, 0)
				.with_direction_deviation(45)
				.with_emission_direction_deviation(25)
				.with_texture(8)
				.with_particles_color({ 0.7f, 0.7f, 0.7f })
				.with_particle_size(0.1f, 0.6f)
				.with_particles_rotation(5)
				.with_particle_size_curve(game::bc_particle_builder::s_curve_fast_step4)
				.with_particle_velocity_curve(game::bc_particle_builder::s_curve_fast_step5, 0.08f)
				.emit_particles_with_total_count(20, 1.5f * l_emitter_energy, 110 * l_emitter_energy, 0.06f);
		}

		l_builder.emitter(core::bc_vector3f(0, 0, 0), core::bc_vector3f::up())
			.with_emission_position_deviation(1, { 1.f, 0.2f, 1.f })
			.with_emission_direction_deviation(180)
			.with_texture(4)
			.with_particles_color({ 0.7f, 0.7f, 0.7f })
			.with_particle_size(.3f, 0.9f)
			.with_particle_size_curve(game::bc_particle_builder::s_curve_fast_step2)
			.with_particle_velocity_curve(game::bc_particle_builder::s_curve_fast_step3, 0.09f)
			.with_particles_rotation(10)
			.emit_particles_with_total_count(15, 1.5f, 70, 0.01f);

		return l_builder;
	}

	inline game::bc_particle_builder bx_bullet_wood_particle::operator()() const
	{
		const core::bc_array<core::bc_vector3f, 3> l_random_directions =
		{
			core::bc_vector3f::normalize(core::bc_vector3f(.0, 1.0f, .0)),
			core::bc_vector3f::normalize(core::bc_vector3f(-.3f, .8f, -.3f)),
			core::bc_vector3f::normalize(core::bc_vector3f(.3f, .8f, .3f))
		};

		game::bc_particle_builder l_builder;

		for (auto& l_direction : l_random_directions)
		{
			const auto l_position = core::bc_vector3f(l_direction.x, 0, l_direction.z) * 0.5f;
			const auto l_emitter_energy = l_direction.y;

			l_builder.emitter(l_position, l_direction, 0.025f, 80 * l_emitter_energy, 0)
				.with_direction_deviation(45)
				.with_emission_direction_deviation(25)
				.with_texture(8)
				.with_particles_color({ 0.62f, 0.41f, 0.25f })
				.with_particle_size(0.1f, 0.6f)
				.with_particles_rotation(5)
				.with_particle_size_curve(game::bc_particle_builder::s_curve_fast_step4)
				.with_particle_velocity_curve(game::bc_particle_builder::s_curve_fast_step5, 0.08f)
				.emit_particles_with_total_count(20, 1.5f * l_emitter_energy, 110 * l_emitter_energy, 0.06f);
		}

		l_builder.emitter(core::bc_vector3f(0, 0, 0), core::bc_vector3f::up())
			.with_emission_position_deviation(1, { 1.f, .2f, 1.f })
			.with_emission_direction_deviation(180)
			.with_texture(4)
			.with_particles_color({ 0.62f, 0.41f, 0.25f })
			.with_particle_size(.3f, 0.9f)
			.with_particle_size_curve(game::bc_particle_builder::s_curve_fast_step2)
			.with_particle_velocity_curve(game::bc_particle_builder::s_curve_fast_step3, 0.09f)
			.with_particles_rotation(10)
			.emit_particles_with_total_count(15, 1.5f, 70, 0.01f);

		return l_builder;
	}

	inline game::bc_particle_builder bx_bullet_iron_particle::operator()() const
	{
		const core::bc_array<core::bc_vector3f, 3> l_random_directions =
		{
			core::bc_vector3f::normalize(core::bc_vector3f(.0, 1.0f, .0)),
			core::bc_vector3f::normalize(core::bc_vector3f(-.2f, 0.7f, -.3f)),
			core::bc_vector3f::normalize(core::bc_vector3f(.3f, 0.6f, .2f))
		};
		
		game::bc_particle_builder l_builder;

		for (auto& l_direction : l_random_directions)
		{
			const auto l_emitter_energy = l_direction.y;
			
			l_builder.emitter(core::bc_vector3f(0), l_direction, 0.06f, 20 * l_emitter_energy, 0)
				.with_direction_deviation(30)
				.with_emission_direction_deviation(100)
				.with_texture(10)
				.with_particles_color({ 0.9f, 0.3f, 0.01f }, 4.0)
				.with_particle_size(0.02f, 0.07f)
				.with_particle_size_curve(game::bc_particle_builder::s_curve_fast_step3)
				.with_particle_velocity_curve(game::bc_particle_builder::s_curve_fast_step5, 0.09f)
				.emit_particles_with_total_count(10, 0.12f * l_emitter_energy, 200 * l_emitter_energy, 0.01f);
		}

		return l_builder;
	}
}