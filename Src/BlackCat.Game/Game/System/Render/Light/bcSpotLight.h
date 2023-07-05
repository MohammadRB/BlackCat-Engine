// [22/02/2019 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Core/Math/bcMatrix4f.h"
#include "Core/Utility/bcNullable.h"
#include "Game/System/Render/Light/bcLightFlare.h"
#include "Game/bcExport.h"

namespace black_cat::game
{
	class BC_GAME_DLL bc_spot_light
	{
	public:
		bc_spot_light(const core::bc_vector3f& p_position, 
		              const core::bc_vector3f& p_direction, 
		              bcFLOAT p_length, 
		              bcFLOAT p_angle, 
		              const core::bc_vector3f& p_color, 
		              bcFLOAT p_intensity,
		              const bc_light_flare* p_flare = nullptr);

		bc_spot_light(const bc_spot_light&) = default;

		~bc_spot_light() = default;

		bc_spot_light& operator=(const bc_spot_light&) = default;

		core::bc_vector3f get_position() const noexcept;
			
		core::bc_vector3f get_position(const core::bc_matrix4f& p_transformation) const noexcept;

		void set_position(const core::bc_vector3f& p_position) noexcept;

		core::bc_vector3f get_direction() const noexcept;
			
		core::bc_vector3f get_direction(const core::bc_matrix4f& p_transformation) const noexcept;

		void set_direction(const core::bc_vector3f& p_direction) noexcept;

		bcFLOAT get_length() const noexcept;

		void set_length(bcFLOAT p_length) noexcept;

		bcFLOAT get_angle() const noexcept;

		void set_angle(bcFLOAT p_angle) noexcept;

		const core::bc_vector3f& get_color() const noexcept;

		void set_color(const core::bc_vector3f& p_color) noexcept;

		bcFLOAT get_intensity() const noexcept;

		void set_intensity(bcFLOAT p_intensity) noexcept;

		const bc_light_flare* get_flare() const noexcept;

		void set_flare(bc_light_flare p_flare) noexcept;

		void set_flare_intensity(bcFLOAT p_intensity) noexcept;

		bcFLOAT calculate_intensity(const core::bc_vector3f& p_view_position, bcFLOAT p_fade_point) const noexcept;
		
	private:
		core::bc_vector3f m_position;
		core::bc_vector3f m_direction;
		bcFLOAT m_length;
		bcFLOAT m_angle;
		core::bc_vector3f m_color;
		bcFLOAT m_intensity;
		core::bc_nullable<bc_light_flare> m_flare;
	};

	inline bc_spot_light::bc_spot_light(const core::bc_vector3f& p_position, 
	                                    const core::bc_vector3f& p_direction,
	                                    bcFLOAT p_length, 
	                                    bcFLOAT p_angle, 
	                                    const core::bc_vector3f& p_color, 
	                                    bcFLOAT p_intensity,
	                                    const bc_light_flare* p_flare)
		: m_position(p_position),
		  m_direction(p_direction),
		  m_length(p_length),
		  m_angle(p_angle),
		  m_color(p_color),
		  m_intensity(p_intensity)
	{
		m_direction.normalize();
		if(p_flare)
		{
			m_flare = *p_flare;
		}
	}

	inline core::bc_vector3f bc_spot_light::get_position() const noexcept
	{
		return m_position;
	}

	inline core::bc_vector3f bc_spot_light::get_position(const core::bc_matrix4f& p_transformation) const noexcept
	{
		return m_position + p_transformation.get_translation();
	}

	inline void bc_spot_light::set_position(const core::bc_vector3f& p_position) noexcept
	{
		m_position = p_position;
	}

	inline core::bc_vector3f bc_spot_light::get_direction() const noexcept
	{
		return m_direction;
	}

	inline core::bc_vector3f bc_spot_light::get_direction(const core::bc_matrix4f& p_transformation) const noexcept
	{
		return (p_transformation * core::bc_vector4f(m_direction, 0)).xyz();
	}

	inline void bc_spot_light::set_direction(const core::bc_vector3f& p_direction) noexcept
	{
		m_direction = p_direction;
	}

	inline bcFLOAT bc_spot_light::get_length() const noexcept
	{
		return m_length;
	}

	inline void bc_spot_light::set_length(bcFLOAT p_length) noexcept
	{
		m_length = p_length;
	}

	inline bcFLOAT bc_spot_light::get_angle() const noexcept
	{
		return m_angle;
	}

	inline void bc_spot_light::set_angle(bcFLOAT p_angle) noexcept
	{
		m_angle = p_angle;
	}

	inline const core::bc_vector3f& bc_spot_light::get_color() const noexcept
	{
		return m_color;
	}

	inline void bc_spot_light::set_color(const core::bc_vector3f& p_color) noexcept
	{
		m_color = p_color;
	}

	inline bcFLOAT bc_spot_light::get_intensity() const noexcept
	{
		return m_intensity;
	}

	inline void bc_spot_light::set_intensity(bcFLOAT p_intensity) noexcept
	{
		m_intensity = p_intensity;
	}

	inline const bc_light_flare* bc_spot_light::get_flare() const noexcept
	{
		return m_flare.get();
	}

	inline void bc_spot_light::set_flare(bc_light_flare p_flare) noexcept
	{
		m_flare = std::move(p_flare);
	}

	inline void bc_spot_light::set_flare_intensity(bcFLOAT p_intensity) noexcept
	{
		if (m_flare.has_value())
		{
			m_flare->set_intensity(p_intensity);
		}
	}
}
