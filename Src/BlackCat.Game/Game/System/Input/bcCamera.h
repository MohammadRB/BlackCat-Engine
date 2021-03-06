// [06/28/2016 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Container/bcArray.h"
#include "Core/Messaging/Event/bcEvent.h"
#include "Core/Messaging/Event/bcEventListenerHandle.h"
#include "Core/Math/bcVector3f.h"
#include "Core/Math/bcMatrix3f.h"
#include "Core/Math/bcMatrix4f.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_orthographic_camera;
		class bc_perspective_camera;

		class BC_GAME_DLL bci_camera
		{
		public:
			using extend = core::bc_array< core::bc_vector3f, 8 >;

		public:
			virtual ~bci_camera() noexcept;

			bcUINT16 get_screen_width() const noexcept
			{
				return m_screen_width;
			}

			bcUINT16 get_screen_height() const noexcept
			{
				return m_screen_height;
			}

			bcFLOAT get_near_clip() const noexcept
			{
				return m_near;
			}

			bcFLOAT get_far_clip() const noexcept
			{
				return m_far;
			}

			core::bc_vector3f get_position() const noexcept
			{
				return m_position;
			}

			core::bc_vector3f get_direction() const noexcept
			{
				auto l_direction = (m_look_at - m_position);
				l_direction.normalize();

				return l_direction;
			}

			core::bc_vector3f get_look_at() const noexcept
			{
				return m_look_at;
			}
			
			core::bc_vector3f get_forward() const noexcept
			{
				return get_direction();
			}

			core::bc_vector3f get_back() const noexcept
			{
				return get_forward() * -1;
			}

			core::bc_vector3f get_up() const noexcept
			{
				const auto l_direction = get_direction();
				const auto l_right = core::bc_vector3f(0, 1, 0).cross(l_direction);
				const auto l_up = core::bc_vector3f::normalize(l_direction.cross(l_right));

				return l_up;
			}

			core::bc_vector3f get_down() const noexcept
			{
				return get_up() * -1;
			}

			core::bc_vector3f get_right() const noexcept
			{
				const auto l_direction = get_direction();
				const auto l_right = core::bc_vector3f::normalize(core::bc_vector3f(0, 1, 0).cross(l_direction));

				return l_right;
			}

			core::bc_vector3f get_left() const noexcept
			{
				return get_right() * -1;
			}

			core::bc_matrix4f get_view() const noexcept
			{
				return m_view;
			}

			core::bc_matrix4f get_projection() const noexcept
			{
				return m_projection;
			}

			/**
			 * \brief Get 8 point that describe camera frustum. First four points lay on near clip and second four points on far clip
			 * ordered by lower-left upper-left upper-right lower-right
			 * \param p_points
			 */
			virtual void get_extend_points(extend& p_points) const noexcept = 0;
			
			/**
			 * \brief Convert a point from screen space to a normalized ray in 3d space
			 * \param p_screen_width
			 * \param p_screen_height
			 * \param p_left Point offset from the left of screen
			 * \param p_top point offset from the top of screen
			 * \return
			 */
			core::bc_vector3f project_clip_point_to_3d_ray(bcUINT16 p_left, bcUINT16 p_top) const noexcept;

			void set_projection(bcUINT16 p_back_buffer_width, bcUINT16 p_back_buffer_height) noexcept;

			void set_projection(bcUINT16 p_back_buffer_width, bcUINT16 p_back_buffer_height, bcFLOAT p_near_clip, bcFLOAT p_far_clip) noexcept;
			
			virtual void update(const core_platform::bc_clock::update_param& p_clock) noexcept = 0;

		protected:
			bci_camera(bcUINT16 p_back_buffer_width, bcUINT16 p_back_buffer_height, bcFLOAT p_near_clip, bcFLOAT p_far_clip) noexcept;

			bci_camera(bci_camera&& p_other) noexcept;

			bci_camera& operator=(bci_camera&& p_other) noexcept;

			void set_look_at(const core::bc_vector3f& p_position, const core::bc_vector3f& p_look_at, const core::bc_vector3f& p_up = core::bc_vector3f(0, 1, 0)) noexcept;
			
			virtual core::bc_matrix4f create_view_matrix(const core::bc_vector3f& p_up = core::bc_vector3f(0, 1, 0)) noexcept;

			virtual core::bc_matrix4f create_projection_matrix() noexcept = 0;
			
		private:
			bcUINT16 m_screen_width;
			bcUINT16 m_screen_height;
			bcFLOAT m_near;
			bcFLOAT m_far;
			core::bc_vector3f m_position;
			core::bc_vector3f m_look_at;
			core::bc_matrix4f m_view;
			core::bc_matrix4f m_projection;
		};

		class BC_GAME_DLL bc_orthographic_camera : public bci_camera
		{
		public:
			using bci_camera::set_projection;

		public:
			virtual ~bc_orthographic_camera() override = default;

			bcFLOAT get_min_x() const noexcept
			{
				return m_min_x;
			}

			bcFLOAT get_max_x() const noexcept
			{
				return m_max_x;
			}

			bcFLOAT get_min_y() const noexcept
			{
				return m_min_y;
			}

			bcFLOAT get_max_y() const noexcept
			{
				return m_max_y;
			}

			void get_extend_points(extend& p_points) const noexcept override;
			
		protected:
			bc_orthographic_camera(bcUINT16 p_back_buffer_width, bcUINT16 p_back_buffer_height, bcFLOAT p_near_clip, bcFLOAT p_far_clip) noexcept;

			bc_orthographic_camera(bc_orthographic_camera&& p_other) noexcept;

			bc_orthographic_camera& operator=(bc_orthographic_camera&& p_other) noexcept;

			core::bc_matrix4f create_projection_matrix() noexcept override;

		private:
			bcFLOAT m_min_x;
			bcFLOAT m_max_x;
			bcFLOAT m_min_y;
			bcFLOAT m_max_y;
		};

		class BC_GAME_DLL bc_perspective_camera : public bci_camera
		{
		public:
			using bci_camera::set_projection;

		public:
			virtual ~bc_perspective_camera() override = default;
			
			bcFLOAT get_aspect_ratio() const noexcept
			{
				return m_aspect_ratio;
			}

			bcFLOAT get_field_of_view() const noexcept
			{
				return m_field_of_view;
			}

			void get_extend_points(extend& p_points) const noexcept override final;

			void set_projection(bcUINT16 p_back_buffer_width, bcUINT16 p_back_buffer_height, bcFLOAT p_height_fov, bcFLOAT p_near_clip, bcFLOAT p_far_clip) noexcept;
			
		protected:
			bc_perspective_camera(bcUINT16 p_back_buffer_width, bcUINT16 p_back_buffer_height, bcFLOAT p_height_fov, bcFLOAT p_near_clip, bcFLOAT p_far_clip) noexcept;

			bc_perspective_camera(bc_perspective_camera&& p_other) noexcept;

			bc_perspective_camera& operator=(bc_perspective_camera&& p_other) noexcept;

			core::bc_matrix4f create_projection_matrix() noexcept override;

		private:
			bcFLOAT m_aspect_ratio;
			bcFLOAT m_field_of_view;
		};
	}
}