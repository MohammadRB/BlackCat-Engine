// [11/11/2020 MRB]

#pragma once

#include "Core/Math/bcVector2f.h"
#include "Core/Math/bcVector3f.h"
#include "Core/Container/bcArray.h"

namespace black_cat
{
	namespace game
	{
		struct bc_particle_emitter_trait
		{
			core::bc_vector3f m_position;
			bcFLOAT m_lifetime; // lifetime in seconds
			core::bc_vector3f m_direction;
			bcFLOAT m_force;
			bcFLOAT m_mass;
			bcUINT32 m_deviation_angle;
			bcUINT32 m_texture_index;
			bcUINT32 m_sprite_index;
			bcUINT32 m_velocity_curve_index;
			bcFLOAT m_velocity_curve_duration;
			bcUINT32 m_particles_total_count;
			bcFLOAT m_particles_lifetime;
			bcFLOAT m_particles_force;
			bcFLOAT m_particles_mass;
			bcFLOAT m_particles_size;
			bcUINT32 m_particles_curve_index;
			bcFLOAT m_particles_velocity_curve_duration;
			bcINT32 m_particles_velocity_reverse_direction;
		};

		struct bc_particle_emitter
		{
			core::bc_vector3f m_prev_position;
			core::bc_vector3f m_position;
			core::bc_vector3f m_emission_direction;
			bcUINT32 m_emission_deviation;
			bcUINT32 m_texture_index;
			bcUINT32 m_sprite_index;
			bcUINT32 m_particles_count;
			bcFLOAT m_particles_lifetime;
			bcFLOAT m_particles_force;
			bcFLOAT m_particles_mass;
			bcFLOAT m_particles_size;
			bcUINT32 m_particles_velocity_curve_index;
			bcFLOAT m_particles_velocity_curve_duration;
		};
	}
}