// [11/11/2020 MRB]

#pragma once

#include "Core/Math/bcVector2f.h"
#include "Core/Math/bcVector3f.h"
#include "Core/Container/bcArray.h"

namespace black_cat
{
	namespace game
	{
		using bc_particle_curve = core::bc_array<core::bc_vector2f, 3>;
		
		struct bc_particle_emitter_trait
		{
			core::bc_vector3f m_position;
			bcFLOAT m_lifetime;					// lifetime in seconds
			core::bc_vector3f m_direction;
			bcFLOAT m_force;
			bcFLOAT m_mass;
			bcUINT32 m_deviation_angle;			// Randomize particles around emitter direction
			bcUINT32 m_texture_index;
			bcUINT32 m_sprite_index;
			bcUINT32 m_particles_total_count;
			bcFLOAT m_particles_lifetime;
			bcFLOAT m_particles_energy;
			bcFLOAT m_particles_mass;
		};

		struct bc_particle_emitter
		{
			core::bc_vector3f m_position;
			core::bc_vector3f m_emission_direction;
			bcUINT32 m_emission_deviation;
			bcUINT32 m_texture_index;
			bcUINT32 m_sprite_index;
			bcUINT32 m_particles_count;
			bcFLOAT m_particles_lifetime;
			bcFLOAT m_particles_force;
			bcFLOAT m_particles_mass;
		};
	}
}