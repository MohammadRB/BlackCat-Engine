// [22/02/2019 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Core/Math/bcMatrix4f.h"
#include "Core/Utility/bcNullable.h"
#include "Game/System/Render/Light/bcLightFlare.h"

namespace black_cat
{
	namespace game
	{
		class bc_point_light
		{
		public:
			bc_point_light(const core::bc_vector3f& p_position, 
				bcFLOAT p_radius, 
				const core::bc_vector3f& p_color, 
				bcFLOAT p_intensity,
				const bc_light_flare* p_flare = nullptr);

			bc_point_light(const core::bc_vector3f& p_position, 
				bcFLOAT p_radius, 
				const core::bc_vector3f& p_color, 
				bcFLOAT p_intensity,
				bool p_particle_cast,
				bcFLOAT p_particle_intensity,
				const bc_light_flare* p_flare = nullptr);

			bc_point_light(const bc_point_light&) = default;

			~bc_point_light() = default;

			bc_point_light& operator=(const bc_point_light&) = default;

			core::bc_vector3f get_position() const noexcept;
			
			core::bc_vector3f get_position(const core::bc_matrix4f& p_transformation) const noexcept;

			void set_position(const core::bc_vector3f& p_position) noexcept;

			bcFLOAT get_radius() const noexcept;

			void set_radius(bcFLOAT p_radius) noexcept;

			const core::bc_vector3f& get_color() const noexcept;

			void set_color(const core::bc_vector3f& p_color) noexcept;

			bcFLOAT get_intensity() const noexcept;

			void set_intensity(bcFLOAT p_intensity) noexcept;

			bool get_particle_cast() const noexcept;

			void set_particle_cast(bool p_particle_cast) noexcept;

			bcFLOAT get_particle_intensity() const noexcept;

			void set_particle_intensity(bcFLOAT p_intensity) noexcept;

			const bc_light_flare* get_flare() const noexcept;

			void set_flare(bc_light_flare p_flare) noexcept;

			void set_flare_intensity(bcFLOAT p_intensity) noexcept;
		
		private:
			core::bc_vector3f m_position;
			bcFLOAT m_radius;
			core::bc_vector3f m_color;
			bcFLOAT m_intensity;
			bool m_particle_cast;
			bcFLOAT m_particle_intensity;
			core::bc_nullable<bc_light_flare> m_flare;
		};

		inline bc_point_light::bc_point_light(const core::bc_vector3f& p_position,
			bcFLOAT p_radius,
			const core::bc_vector3f& p_color,
			bcFLOAT p_intensity,
			const bc_light_flare* p_flare)
			: bc_point_light
			(
				p_position,
				p_radius,
				p_color,
				p_intensity,
				false,
				0,
				p_flare
			)
		{
		}

		inline bc_point_light::bc_point_light(const core::bc_vector3f& p_position,
			bcFLOAT p_radius,
			const core::bc_vector3f& p_color,
			bcFLOAT p_intensity,
			bool p_particle_cast,
			bcFLOAT p_particle_intensity,
			const bc_light_flare* p_flare)
			: m_position(p_position),
			m_radius(p_radius),
			m_color(p_color),
			m_intensity(p_intensity),
			m_particle_cast(p_particle_cast),
			m_particle_intensity(p_particle_intensity)
		{
			if(p_flare)
			{
				m_flare = *p_flare;
			}
		}

		inline core::bc_vector3f bc_point_light::get_position() const noexcept
		{
			return m_position;
		}

		inline core::bc_vector3f bc_point_light::get_position(const core::bc_matrix4f& p_transformation) const noexcept
		{
			return m_position + p_transformation.get_translation();
		}

		inline void bc_point_light::set_position(const core::bc_vector3f& p_position) noexcept
		{
			m_position = p_position;
		}

		inline bcFLOAT bc_point_light::get_radius() const noexcept
		{
			return m_radius;
		}

		inline void bc_point_light::set_radius(bcFLOAT p_radius) noexcept
		{
			m_radius = p_radius;
		}

		inline const core::bc_vector3f& bc_point_light::get_color() const noexcept
		{
			return m_color;
		}

		inline void bc_point_light::set_color(const core::bc_vector3f& p_color) noexcept
		{
			m_color = p_color;
		}

		inline bcFLOAT bc_point_light::get_intensity() const noexcept
		{
			return m_intensity;
		}

		inline void bc_point_light::set_intensity(bcFLOAT p_intensity) noexcept
		{
			m_intensity = p_intensity;
		}

		inline bool bc_point_light::get_particle_cast() const noexcept
		{
			return m_particle_cast;
		}

		inline void bc_point_light::set_particle_cast(bool p_particle_cast) noexcept
		{
			m_particle_cast = p_particle_cast;
		}

		inline bcFLOAT bc_point_light::get_particle_intensity() const noexcept
		{
			return m_particle_intensity;
		}

		inline void bc_point_light::set_particle_intensity(bcFLOAT p_intensity) noexcept
		{
			m_particle_intensity = p_intensity;
		}

		inline const bc_light_flare* bc_point_light::get_flare() const noexcept
		{
			return m_flare.get();
		}

		inline void bc_point_light::set_flare(bc_light_flare p_flare) noexcept
		{
			m_flare = std::move(p_flare);
		}

		inline void bc_point_light::set_flare_intensity(bcFLOAT p_intensity) noexcept
		{
			if(m_flare.has_value())
			{
				m_flare->set_intensity(p_intensity);
			}
		}
	}
}