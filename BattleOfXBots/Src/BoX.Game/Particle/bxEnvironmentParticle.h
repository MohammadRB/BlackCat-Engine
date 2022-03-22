// [03/22/2022 MRB]

#pragma once

#include "Core/Utility/bcRandom.h"
#include "Game/System/Render/Particle/bcParticleBuilder.h"

namespace box
{
	using namespace black_cat;

	class bx_falling_leaf_particle
	{
	public:
		game::bc_particle_builder operator()() const;
	};

	/*inline game::bc_particle_builder bx_falling_leaf_particle::operator()() const
	{
		return game::bc_particle_builder()
		       .emitter({ 0, 0, 0 }, { 0, -1, 0 }, std::numeric_limits<bcFLOAT>::max(), 0, 0)
		.with_emission_deviation(180)
		.with_texture(11)
		.with_particles_color({.3f, .6f, .3f})
		.with_particle_size(.1, .1)
		.with_particles_rotation(10)
		.emit_particles()
	}*/
}
