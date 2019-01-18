// [06/28/2016 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Container/bcArray.h"
#include "Core/Event/bcEvent.h"
#include "Core/Math/bcVector3f.h"
#include "Core/Math/bcMatrix3f.h"
#include "Core/Math/bcMatrix4f.h"
#include "PlatformImp/Application/bcHumanInterfaceDevice.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_icamera
		{
		public:
			using extend = core::bc_array<core::bc_vector3f, 8>;

		public:
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

			core::bc_vector3f get_position() const noexcept
			{
				return m_position;
			}

			void set_position(core::bc_vector3f p_position) noexcept;

			core::bc_vector3f get_direction() const noexcept;

			void set_direction(core::bc_vector3f p_direction) noexcept;

			core::bc_vector3f get_lookat() const noexcept
			{
				return m_lookat;
			}

			void set_lookat(core::bc_vector3f p_lookat) noexcept;

			void set_position_lookat(const core::bc_vector3f& p_position, const core::bc_vector3f& p_lookat);

			core::bc_vector3f get_forward() const noexcept;

			core::bc_vector3f get_back() const noexcept;

			core::bc_vector3f get_up() const noexcept;

			core::bc_vector3f get_down() const noexcept;

			core::bc_vector3f get_right() const noexcept;

			core::bc_vector3f get_left() const noexcept;

			/**
			 * \brief Get 8 point that describe camera frustum. First four points lay on near clip and two four points on far clip 
			 * ordered by lower-left upper-left upper-right lower-right
			 * \param p_points 
			 */
			virtual void get_extend_points(extend& p_points) const = 0;

			/**
			 * \brief Convert a point from screen space to a normalized ray in 3d space
			 * \param p_screen_width 
			 * \param p_screen_height 
			 * \param p_left Point offset from the left of screen
			 * \param p_top point offset from the top of screen
			 * \return 
			 */
			core::bc_vector3f project_clip_point_to_3d_ray(bcUINT16 p_screen_width, bcUINT16 p_screen_height, bcUINT16 p_left, bcUINT16 p_top) const;

			core::bc_matrix4f get_view() const noexcept
			{
				return m_view;
			}

			void set_view(const core::bc_matrix4f& p_view) noexcept
			{
				m_view = p_view;
			}

			core::bc_matrix4f get_projection() const noexcept
			{
				return m_projection;
			}

			void set_projection(const core::bc_matrix4f& p_proj) noexcept
			{
				m_projection = p_proj;
			}

			virtual void set_projection(bcUINT16 p_back_buffer_width, bcUINT16 p_back_buffer_height, bcFLOAT p_near_clip, bcFLOAT p_far_clip) noexcept = 0;

			void look_at(const core::bc_vector3f& p_position, const core::bc_vector3f& p_lookat, const core::bc_vector3f& up);

			virtual void update(core_platform::bc_clock::update_param p_clock_update_param,
				const platform::bc_pointing_device& p_pointing_device,
				const platform::bc_key_device& p_key_device) = 0;

		protected:
			bc_icamera(bcFLOAT p_near_clip, bcFLOAT p_far_clip) noexcept;

			bc_icamera(bc_icamera&&) = default;

			bc_icamera& operator=(bc_icamera&&) = default;

			void create_view_matrix(const core::bc_vector3f& p_up = core::bc_vector3f(0, 1, 0));

			virtual void create_projection_matrix() = 0;

			virtual bool on_key(core::bc_ievent& p_key_event) = 0;

			virtual bool on_pointing(core::bc_ievent& p_pointing_event) = 0;

		private:
			bcFLOAT m_near;
			bcFLOAT m_far;
			core::bc_vector3f m_position;
			core::bc_vector3f m_lookat;
			core::bc_matrix4f m_view;
			core::bc_matrix4f m_projection;

			core::bc_event_listener_handle m_key_listener_handle;
			core::bc_event_listener_handle m_pointing_listener_handle;
		};

		class BC_GAME_DLL bc_orthographic_camera : public bc_icamera
		{
		public:
			using bc_icamera::set_projection;

		public:
			virtual ~bc_orthographic_camera() = default;

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

			void set_projection(bcUINT16 p_back_buffer_width, bcUINT16 p_back_buffer_height, bcFLOAT p_near_clip, bcFLOAT p_far_clip) noexcept override final;

		protected:
			bc_orthographic_camera(bcFLOAT p_min_x,
				bcFLOAT p_min_y,
				bcFLOAT p_max_x,
				bcFLOAT p_max_y,
				bcFLOAT p_near_clip,
				bcFLOAT p_far_clip);

			bc_orthographic_camera(bc_orthographic_camera&&) = default;

			bc_orthographic_camera& operator=(bc_orthographic_camera&&) = default;

			void create_projection_matrix() override final;

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
			virtual ~bc_perspective_camera() = default;
			
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

			void get_extend_points(extend& p_points) const override final;

			void set_projection(bcUINT16 p_back_buffer_width, bcUINT16 p_back_buffer_height, bcFLOAT p_near_clip, bcFLOAT p_far_clip) noexcept override final;

		protected:
			bc_perspective_camera(bcFLOAT p_aspect_ratio, bcFLOAT p_height_fov, bcFLOAT p_near_clip, bcFLOAT p_far_clip);

			bc_perspective_camera(bc_perspective_camera&&) = default;

			bc_perspective_camera& operator=(bc_perspective_camera&&) = default;

			void create_projection_matrix() override final;

		private:
			bcFLOAT m_aspect_ratio;
			bcFLOAT m_field_of_view;
		};
	}
}