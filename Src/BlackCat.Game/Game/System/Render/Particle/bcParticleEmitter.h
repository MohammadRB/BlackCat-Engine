// [11/11/2020 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Core/Container/bcVector.h"
#include "Core/Utility/bcRefCountPtr.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_particle_manager_container;
		class bc_external_particle_emitter;
		
		struct bc_particle_emitter_trait
		{
			core::bc_vector3f m_position;
			core::bc_vector3f m_direction;
			bcFLOAT m_lifetime_seconds;
			bcFLOAT m_force;
			bcFLOAT m_mass;
			bcUINT16 m_direction_deviation;
			bcUINT16 m_emission_deviation;
			core::bc_vector3f m_emission_deviation_force;
			bcUINT16 m_sprite_index;
			bcUINT16 m_velocity_curve_index;
			bcFLOAT m_velocity_curve_duration;
			core::bc_vector3f m_particles_color;
			bcFLOAT m_particles_color_intensity;
			bcUINT16 m_particles_count_per_second;
			bcUINT16 m_particles_total_count;
			bcFLOAT m_particles_lifetime;
			bcFLOAT m_particles_force;
			bcFLOAT m_particles_mass;
			bcFLOAT m_particles_start_size;
			bcFLOAT m_particles_end_size;
			bcFLOAT m_particles_rotation_per_second;
			bcFLOAT m_particles_velocity_curve_duration;
			bcUINT16 m_particles_velocity_curve_index;
			bcINT8 m_particles_velocity_reverse_direction;
			bcUINT16 m_particles_size_curve_index;
			bcUINT16 m_particles_fade_curve_index;
		};

		class BC_GAME_DLL _bc_particle_emitter_ptr_deleter
		{
		public:
			_bc_particle_emitter_ptr_deleter();

			_bc_particle_emitter_ptr_deleter(bc_particle_manager_container* p_manager);

			void operator()(bc_external_particle_emitter* p_emitter);

		private:
			bc_particle_manager_container* m_container;
		};

		class bc_external_particle_emitter : public core::bc_ref_count
		{
		public:
			void set_positions(const core::bc_vector3f& p_position)
			{
				for(bc_particle_emitter_trait* l_emitter : m_emitters)
				{
					l_emitter->m_position = p_position;
				}
			}

			void set_directions(const core::bc_vector3f& p_direction)
			{
				for (bc_particle_emitter_trait* l_emitter : m_emitters)
				{
					l_emitter->m_direction = p_direction;
				}
			}

			void set_particle_counts_per_second(bcUINT32 p_count_per_second)
			{
				for (bc_particle_emitter_trait* l_emitter : m_emitters)
				{
					l_emitter->m_particles_count_per_second = p_count_per_second;
				}
			}
			
			core::bc_vector_movable<bc_particle_emitter_trait*> m_emitters;
		};
		
		using bc_particle_emitter_ptr = core::bc_ref_count_ptr<bc_external_particle_emitter, _bc_particle_emitter_ptr_deleter>;

		struct bc_particle_emitter_state
		{
			core::bc_vector3f m_prev_position;
			core::bc_vector3f m_position;
			core::bc_vector3f m_emission_direction;
			bcFLOAT m_energy;
			bcUINT32 m_emission_deviation;
			core::bc_vector3f m_emission_deviation_force;
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