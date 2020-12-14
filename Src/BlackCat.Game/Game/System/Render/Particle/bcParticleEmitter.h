// [11/11/2020 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"

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
			bcUINT32 m_sprite_index;
			bcUINT32 m_velocity_curve_index;
			bcFLOAT m_velocity_curve_duration;
			core::bc_vector3f m_particles_color;
			bcFLOAT m_particles_color_intensity;
			bcUINT32 m_particles_total_count;
			bcFLOAT m_particles_lifetime;
			bcFLOAT m_particles_force;
			bcFLOAT m_particles_mass;
			bcFLOAT m_particles_start_size;
			bcFLOAT m_particles_end_size;
			bcFLOAT m_particles_rotation; // degree per second
			bcINT32 m_particles_velocity_reverse_direction;
			bcUINT32 m_particles_velocity_curve_index;
			bcFLOAT m_particles_velocity_curve_duration;
			bcUINT32 m_particles_size_curve_index;
			bcUINT32 m_particles_fade_curve_index;
		};

		struct bc_particle_emitter
		{
			core::bc_vector3f m_prev_position;
			core::bc_vector3f m_position;
			core::bc_vector3f m_emission_direction;
			bcFLOAT m_energy;
			bcUINT32 m_emission_deviation;
			bcUINT32 m_sprite_index;
			core::bc_vector3f m_particles_color;
			bcFLOAT m_particles_color_intensity;
			bcUINT32 m_particles_count;
			bcFLOAT m_particles_lifetime;
			bcFLOAT m_particles_force;
			bcFLOAT m_particles_mass;
			bcFLOAT m_particles_start_size;
			bcFLOAT m_particles_end_size;
			bcFLOAT m_particles_rotation;
			bcUINT32 m_particles_velocity_curve_index;
			bcFLOAT m_particles_velocity_curve_duration;
			bcUINT32 m_particles_size_curve_index;
			bcUINT32 m_particles_fade_curve_index;
		};
	}
}