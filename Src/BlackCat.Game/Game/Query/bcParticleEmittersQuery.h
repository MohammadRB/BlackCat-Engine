// [11/16/2020 MRB]

#pragma once

#include "Core/bcConstant.h"
#include "Core/Container/bcVector.h"
#include "Core/Messaging/Query/bcQuery.h"
#include "Game/System/Render/Particle/bcParticleEmitter.h"

namespace black_cat
{
	namespace game
	{
		struct bc_particle_emitter_state
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
		
		class bc_particle_emitters_query_context : public core::bc_query_context
		{
		public:
			explicit bc_particle_emitters_query_context(core::bc_vector_movable<bc_particle_emitter_state> p_emitters)
				: m_emitters(std::move(p_emitters))
			{
			}

			core::bc_vector_movable<bc_particle_emitter_state> m_emitters;
		};
		
		class bc_particle_emitter_query : public core::bc_query<bc_particle_emitters_query_context>
		{
			BC_QUERY(ptc_emt)
			
		public:
			explicit bc_particle_emitter_query()
				: bc_query(message_name())
			{
			}

			bc_particle_emitter_query(bc_particle_emitter_query&&) = default;

			~bc_particle_emitter_query() = default;

			bc_particle_emitter_query& operator=(bc_particle_emitter_query&&) = default;

			core::bc_vector_movable<bc_particle_emitter_state> get_emitters() noexcept
			{
				return std::move(m_emitters);
			}
			
			void execute(const bc_particle_emitters_query_context& p_context) noexcept override
			{
				m_emitters = p_context.m_emitters;
			}
			
		private:
			core::bc_vector_movable<bc_particle_emitter_state> m_emitters;
		};
	}
}