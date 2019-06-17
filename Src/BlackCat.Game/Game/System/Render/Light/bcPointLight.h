// [02/22/2019 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Core/Math/bcMatrix4f.h"

namespace black_cat
{
	namespace game
	{
		class bc_point_light
		{
		public:
			bc_point_light(const core::bc_vector3f& p_position, bcFLOAT p_radius, const core::bc_vector3f& p_color, bcFLOAT p_intensity);

			bc_point_light(const bc_point_light&) = default;

			~bc_point_light() = default;

			bc_point_light& operator=(const bc_point_light&) = default;

			core::bc_vector3f get_position(const core::bc_matrix4f& p_transformation) const noexcept;

			void set_position(const core::bc_vector3f& p_position) noexcept;

			bcFLOAT get_radius() const noexcept;

			void set_radius(bcFLOAT p_radius) noexcept;

			const core::bc_vector3f& get_color() const noexcept;

			void set_color(const core::bc_vector3f& p_color) noexcept;

			const bcFLOAT& get_intensity() const noexcept;

			void set_intensity(bcFLOAT p_intensity) noexcept;

		private:
			core::bc_vector3f m_position;
			bcFLOAT m_radius;
			core::bc_vector3f m_color;
			bcFLOAT m_intensity;
		};

		inline bc_point_light::bc_point_light(const core::bc_vector3f& p_position,
			bcFLOAT p_radius,
			const core::bc_vector3f& p_color,
			bcFLOAT p_intensity)
			: m_position(p_position),
			m_radius(p_radius),
			m_color(p_color),
			m_intensity(p_intensity)
		{
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

		inline const bcFLOAT& bc_point_light::get_intensity() const noexcept
		{
			return m_intensity;
		}

		inline void bc_point_light::set_intensity(bcFLOAT p_intensity) noexcept
		{
			m_intensity = p_intensity;
		}
	}
}