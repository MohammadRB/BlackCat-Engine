// [08/05/2020 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Core/Math/bcMatrix4f.h"
#include "Game/System/Input/bcCamera.h"

namespace black_cat::game
{
	class bc_camera_instance
	{
	public:
		bc_camera_instance() noexcept = default;

		explicit bc_camera_instance(const bci_camera& p_camera);

		bc_camera_instance(const bc_camera_instance&) noexcept = default;

		~bc_camera_instance() = default;

		bc_camera_instance& operator=(const bc_camera_instance&) noexcept = default;

		bcUINT16 get_screen_width() const noexcept;

		bcUINT16 get_screen_height() const noexcept;

		bcFLOAT get_near_clip() const noexcept;

		bcFLOAT get_far_clip() const noexcept;

		core::bc_vector3f get_position() const noexcept;

		core::bc_vector3f get_direction() const noexcept;
			
		core::bc_vector3f get_forward() const noexcept;

		core::bc_vector3f get_back() const noexcept;

		core::bc_vector3f get_up() const noexcept;

		core::bc_vector3f get_down() const noexcept;

		core::bc_vector3f get_right() const noexcept;

		core::bc_vector3f get_left() const noexcept;
			
		const core::bc_matrix4f& get_view() const noexcept;

		const core::bc_matrix4f& get_projection() const noexcept;

		const bci_camera::extend& get_extends() const noexcept;
			
	private:
		bcUINT16 m_screen_width;
		bcUINT16 m_screen_height;
		bcFLOAT m_near_clip;
		bcFLOAT m_far_clip;
		core::bc_vector3f m_position;
		core::bc_vector3f m_direction;
		core::bc_vector3f m_forward;
		core::bc_vector3f m_up;
		core::bc_vector3f m_right;
		core::bc_matrix4f m_view;
		core::bc_matrix4f m_projection;
		bci_camera::extend m_extends;
	};

	inline bc_camera_instance::bc_camera_instance(const bci_camera& p_camera)
		: m_screen_width(p_camera.get_screen_width()),
		  m_screen_height(p_camera.get_screen_height()),
		  m_near_clip(p_camera.get_near_clip()),
		  m_far_clip(p_camera.get_far_clip()),
		  m_position(p_camera.get_position()),
		  m_direction(p_camera.get_direction()),
		  m_forward(p_camera.get_forward()),
		  m_up(p_camera.get_up()),
		  m_right(p_camera.get_right()),
		  m_view(p_camera.get_view()),
		  m_projection(p_camera.get_projection())
	{
		p_camera.get_extend_points(m_extends);
	}
		
	inline bcUINT16 bc_camera_instance::get_screen_width() const noexcept
	{
		return m_screen_width;
	}

	inline bcUINT16 bc_camera_instance::get_screen_height() const noexcept
	{
		return m_screen_height;
	}

	inline bcFLOAT bc_camera_instance::get_near_clip() const noexcept
	{
		return m_near_clip;
	}

	inline bcFLOAT bc_camera_instance::get_far_clip() const noexcept
	{
		return m_far_clip;
	}

	inline core::bc_vector3f bc_camera_instance::get_position() const noexcept
	{
		return m_position;
	}

	inline core::bc_vector3f bc_camera_instance::get_direction() const noexcept
	{
		return m_direction;
	}
		
	inline core::bc_vector3f bc_camera_instance::get_forward() const noexcept
	{
		return m_forward;
	}

	inline core::bc_vector3f bc_camera_instance::get_back() const noexcept
	{
		return -get_forward();
	}

	inline core::bc_vector3f bc_camera_instance::get_up() const noexcept
	{
		return m_up;
	}

	inline core::bc_vector3f bc_camera_instance::get_down() const noexcept
	{
		return -get_up();
	}

	inline core::bc_vector3f bc_camera_instance::get_right() const noexcept
	{
		return m_right;
	}

	inline core::bc_vector3f bc_camera_instance::get_left() const noexcept
	{
		return -get_right();
	}

	inline const core::bc_matrix4f& bc_camera_instance::get_view() const noexcept
	{
		return m_view;
	}

	inline const core::bc_matrix4f& bc_camera_instance::get_projection() const noexcept
	{
		return m_projection;
	}

	inline const bci_camera::extend& bc_camera_instance::get_extends() const noexcept
	{
		return m_extends;
	}
}
