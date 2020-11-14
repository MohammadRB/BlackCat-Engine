// [11/12/2020 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Game/System/Render/Particle/bcParticle.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_particle_builder
		{
		public:
			bc_particle_builder();

			bc_particle_builder& randomize_sphere(const core::bc_vector3f& p_center,
				float p_radius,
				const core::bc_vector3f& p_average_velocity,
				float p_average_age,
				bcUINT32 p_texture_index,
				bcUINT32 p_sprite_index);

		private:
			core::bc_vector_frame<bc_particle> m_particles;
		};
	}
}