// [06/28/2016 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "PlatformImp/Application/bcHumanInterfaceDevice.h"
#include "Graphic/Math/bcVector3f.h"
#include "Graphic/Math/bcMatrix3f.h"
#include "Graphic/Math/bcMatrix4f.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_icamera
		{
		public:
			bc_icamera(bcFLOAT p_near_clip, bcFLOAT p_far_clip) noexcept;

			virtual ~bc_icamera();

			bcFLOAT get_far_clip() const noexcept
			{
				return m_far;
			}

			void set_far_clip(bcFLOAT p_far_clip) noexcept;

			bcFLOAT get_near_clip() const noexcept
			{
				return m_near;
			}

			void set_near_clip(bcFLOAT p_near_clip) noexcept;

			graphic::bc_vector3f get_position() const noexcept
			{
				return m_position;
			}

			void set_position(graphic::bc_vector3f p_position) noexcept;

			graphic::bc_vector3f get_direction() const noexcept;

			void set_direction(graphic::bc_vector3f p_direction) noexcept;

			graphic::bc_vector3f get_lookat() const noexcept
			{
				return m_lookat;
			}

			void set_lookat(graphic::bc_vector3f p_lookat) noexcept;

			graphic::bc_vector3f get_forward() const noexcept;

			graphic::bc_vector3f get_back() const noexcept;

			graphic::bc_vector3f get_up() const noexcept;

			graphic::bc_vector3f get_down() const noexcept;

			graphic::bc_vector3f get_right() const noexcept;

			graphic::bc_vector3f get_left() const noexcept;

			graphic::bc_matrix4f get_view() const noexcept
			{
				return m_view;
			}

			void set_view(const graphic::bc_matrix4f& p_view) noexcept
			{
				m_view = p_view;
			}

			graphic::bc_matrix4f get_projection() const noexcept
			{
				return m_projection;
			}

			void set_projection(const graphic::bc_matrix4f& p_proj) noexcept
			{
				m_projection = p_proj;
			}

			virtual void set_projection(bcUINT16 p_back_buffer_width, bcUINT16 p_back_buffer_height, bcFLOAT p_near_clip, bcFLOAT p_far_clip) noexcept = 0;

			void look_at(const graphic::bc_vector3f& p_position, const graphic::bc_vector3f& p_lookat, const graphic::bc_vector3f& up);

			virtual void update(core_platform::bc_clock::update_param p_clock_update_param,
				const platform::bc_pointing_device& p_pointing_device,
				const platform::bc_key_device& p_key_device) = 0;

		protected:
			bc_icamera(const bc_icamera&) = default;

			bc_icamera& operator=(const bc_icamera&) = default;

			void create_view_matrix(const graphic::bc_vector3f& p_up = graphic::bc_vector3f(0, 1, 0));

			virtual void create_projection_matrix() = 0;

		private:
			bcFLOAT m_near;
			bcFLOAT m_far;
			graphic::bc_vector3f m_position;
			graphic::bc_vector3f m_lookat;
			graphic::bc_matrix4f m_view;
			graphic::bc_matrix4f m_projection;
		};

		class BC_GAME_DLL bc_orthographic_camera : public bc_icamera
		{
		public:
			using bc_icamera::set_projection;

		public:
			bc_orthographic_camera(bcFLOAT p_min_x, 
				bcFLOAT p_min_y, 
				bcFLOAT p_max_x, 
				bcFLOAT p_max_y, 
				bcFLOAT p_near_clip, 
				bcFLOAT p_far_clip);

			bc_orthographic_camera(const bc_orthographic_camera&) = default;

			virtual ~bc_orthographic_camera() = default;

			bc_orthographic_camera& operator=(const bc_orthographic_camera&) = default;

			bcFLOAT get_min_x() const noexcept
			{
				return m_min_x;
			}

			void set_min_x(bcFLOAT p_min_x) noexcept;

			bcFLOAT get_max_x() const noexcept
			{
				return m_max_x;
			}

			void set_max_x(bcFLOAT p_max_x) noexcept;

			bcFLOAT get_min_y() const noexcept
			{
				return m_min_y;
			}

			void set_min_y(bcFLOAT p_min_y) noexcept;

			bcFLOAT get_max_y() const noexcept
			{
				return m_max_y;
			}

			void set_max_y(bcFLOAT p_max_y) noexcept;

			void set_projection(bcUINT16 p_back_buffer_width, bcUINT16 p_back_buffer_height, bcFLOAT p_near_clip, bcFLOAT p_far_clip) noexcept override;

		protected:
			void create_projection_matrix() override;

		private:
			bcFLOAT m_min_x;
			bcFLOAT m_max_x;
			bcFLOAT m_min_y;
			bcFLOAT m_max_y;
		};

		class BC_GAME_DLL bc_perspective_camera : public bc_icamera
		{
		public:
			using bc_icamera::set_projection;

		public:
			bc_perspective_camera(bcFLOAT p_aspect_ratio, bcFLOAT p_field_of_view, bcFLOAT p_near_clip, bcFLOAT p_far_clip);

			bc_perspective_camera(const bc_perspective_camera&) = default;

			virtual ~bc_perspective_camera() = default;

			bc_perspective_camera& operator=(const bc_perspective_camera&) = default;
			
			bcFLOAT get_aspect_ratio() const noexcept
			{
				return m_aspect_ratio;
			}

			void set_aspect_ratio(bcFLOAT p_aspect_ratio) noexcept;

			bcFLOAT get_field_of_view() const noexcept
			{
				return m_field_of_view;
			}

			void set_field_of_view(bcFLOAT p_field_of_view) noexcept;

			void set_projection(bcUINT16 p_back_buffer_width, bcUINT16 p_back_buffer_height, bcFLOAT p_near_clip, bcFLOAT p_far_clip) noexcept override;

		protected:
			void create_projection_matrix() override;

		private:
			bcFLOAT m_aspect_ratio;
			bcFLOAT m_field_of_view;
		};
	}
}
