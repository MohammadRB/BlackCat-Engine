// [11/11/2020 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"

namespace black_cat
{
	namespace game
	{
		struct bc_particle
		{
		public:
			bc_particle(const core::bc_vector3f& p_position, 
				const core::bc_vector3f& p_velocity, 
				float p_age, 
				bcUINT32 p_texture_index, 
				bcUINT32 p_sprite_index)
				: m_position(p_position),
				m_velocity(p_velocity),
				m_age(p_age),
				m_texture_index(p_texture_index),
				m_sprite_index(p_sprite_index)
			{
			}
			
			core::bc_vector3f m_position;
			core::bc_vector3f m_velocity;
			float m_age;
			bcUINT32 m_texture_index;
			bcUINT32 m_sprite_index;
		};
	}
}