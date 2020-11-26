// [11/12/2020 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Game/System/Render/Particle/bcParticleEmitter.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_particle_manager;
		class bc_particle_builder;

		class BC_GAME_DLL bc_particle_builder1
		{
		public:
			bc_particle_builder1(bc_particle_builder& p_builder, bc_particle_emitter_trait& p_emitter);

			bc_particle_builder1& with_deviation(bcUINT32 p_angle);
			
			bc_particle_builder1& with_texture(bcUINT32 p_texture_index, bcUINT32 p_sprite_index) noexcept;

			bc_particle_builder& emit_particles(bcUINT32 p_particles_total_count, bcFLOAT p_particles_lifetime, bcFLOAT p_particles_force, bcFLOAT p_particles_mass);

		private:
			bc_particle_builder& m_builder;
			bc_particle_emitter_trait& m_emitter;
		};
		
		class BC_GAME_DLL bc_particle_builder
		{
			friend class bc_particle_manager;
			
		public:
			const static bc_particle_curve curve_fast_steady;
			const static bc_particle_curve curve_linear;
			
		public:
			bc_particle_builder();

			bc_particle_builder1 emitter(const core::bc_vector3f& p_position, const core::bc_vector3f& p_direction);
			
			bc_particle_builder1 emitter(const core::bc_vector3f& p_position, 
				const core::bc_vector3f& p_direction,
				bcFLOAT p_lifetime,
				bcFLOAT p_force,
				bcFLOAT p_mass);

		private:
			core::bc_vector_frame<bc_particle_emitter_trait> m_emitters;
		};
	}
}