// [02/22/2019 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"

namespace black_cat
{
	namespace game
	{
		class bc_direct_light
		{
		public:
			bc_direct_light(const core::bc_vector3f& p_direction,
				const core::bc_vector3f& p_color,
				bcFLOAT p_intensity,
				const core::bc_vector3f& p_ambient_color,
				bcFLOAT p_ambient_intensity) noexcept;

			bc_direct_light(const bc_direct_light&) = default;

			~bc_direct_light() = default;

			bc_direct_light& operator=(const bc_direct_light&) = default;

			const core::bc_vector3f& get_direction() const noexcept;

			void set_direction(const core::bc_vector3f& p_direction) noexcept;

			const core::bc_vector3f& get_color() const noexcept;

			void set_color(const core::bc_vector3f& p_color) noexcept;

			bcFLOAT get_intensity() const noexcept;

			void set_intensity(bcFLOAT p_intensity) noexcept;

			const core::bc_vector3f& get_ambient_color() const noexcept;

			void set_ambient_color(const core::bc_vector3f& p_color) noexcept;

			bcFLOAT get_ambient_intensity() const noexcept;

			void set_ambient_intensity(bcFLOAT p_intensity) noexcept;

		private:
			core::bc_vector3f m_direction;
			core::bc_vector3f m_color;
			bcFLOAT m_intensity;
			core::bc_vector3f m_ambient_color;
			bcFLOAT m_ambient_intensity;
		};

		inline bc_direct_light::bc_direct_light(const core::bc_vector3f& p_direction,
			const core::bc_vector3f& p_color,
			bcFLOAT p_intensity,
			const core::bc_vector3f& p_ambient_color,
			bcFLOAT p_ambient_intensity) noexcept
			: m_direction(p_direction),
			m_color(p_color),
			m_intensity(p_intensity),
			m_ambient_color(p_ambient_color),
			m_ambient_intensity(p_ambient_intensity)
		{
		}

		inline const core::bc_vector3f& bc_direct_light::get_direction() const noexcept
		{
			return m_direction;
		}

		inline void bc_direct_light::set_direction(const core::bc_vector3f& p_direction) noexcept
		{
			m_direction = p_direction;
		}

		inline const core::bc_vector3f& bc_direct_light::get_color() const noexcept
		{
			return m_color;
		}

		inline void bc_direct_light::set_color(const core::bc_vector3f& p_color) noexcept
		{
			m_color = p_color;
		}

		inline bcFLOAT bc_direct_light::get_intensity() const noexcept
		{
			return m_intensity;
		}

		inline void bc_direct_light::set_intensity(bcFLOAT p_intensity) noexcept
		{
			m_intensity = p_intensity;
		}

		inline const core::bc_vector3f& bc_direct_light::get_ambient_color() const noexcept
		{
			return m_ambient_color;
		}

		inline void bc_direct_light::set_ambient_color(const core::bc_vector3f& p_color) noexcept
		{
			m_ambient_color = p_color;
		}

		inline bcFLOAT bc_direct_light::get_ambient_intensity() const noexcept
		{
			return m_ambient_intensity;
		}

		inline void bc_direct_light::set_ambient_intensity(bcFLOAT p_intensity) noexcept
		{
			m_ambient_intensity = p_intensity;
		}
	}
}