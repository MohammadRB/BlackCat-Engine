// [04/03/2021 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"

namespace black_cat
{
	namespace game
	{
		class bc_bullet_manager;
		
		class bc_bullet
		{
			friend class bc_bullet_manager;
			
		public:
			bc_bullet(const core::bc_vector3f& p_position, const core::bc_vector3f& p_dir, bcFLOAT p_mass, bcFLOAT p_speed, bcFLOAT p_radius = 0);

			bc_bullet(const bc_bullet&) noexcept = default;

			~bc_bullet() = default;

			bc_bullet& operator=(const bc_bullet&) noexcept = default;

			const core::bc_vector3f& get_position() const noexcept;
			
			const core::bc_vector3f& get_direction() const noexcept;

			bcFLOAT get_mass() const noexcept;
			
			bcFLOAT get_speed() const noexcept;

			bcFLOAT get_radius() const noexcept;
		
		private:
			core::bc_vector3f m_position;
			core::bc_vector3f m_direction;
			bcFLOAT m_mass;
			bcFLOAT m_speed;
			bcFLOAT m_radius;
		};

		inline bc_bullet::bc_bullet(const core::bc_vector3f& p_position,
			const core::bc_vector3f& p_dir,
			bcFLOAT p_mass,
			bcFLOAT p_speed,
			bcFLOAT p_radius)
			: m_position(p_position),
			m_direction(p_dir),
			m_mass(p_mass),
			m_speed(p_speed),
			m_radius(p_radius)
		{
		}

		inline const core::bc_vector3f& bc_bullet::get_position() const noexcept
		{
			return m_position;
		}

		inline const core::bc_vector3f& bc_bullet::get_direction() const noexcept
		{
			return m_direction;
		}

		inline bcFLOAT bc_bullet::get_mass() const noexcept
		{
			return m_mass;
		}

		inline bcFLOAT bc_bullet::get_speed() const noexcept
		{
			return m_speed;
		}

		inline bcFLOAT bc_bullet::get_radius() const noexcept
		{
			return m_radius;
		}
	}	
}