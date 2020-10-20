// [01/11/2019 MRB]

#pragma once

#include "Core/Container/bcArray.h"
#include "PhysicsImp/Shape/bcBoundBox.h"
#include "PhysicsImp/Shape/bcShapePlane.h"
#include "Game/bcExport.h"
#include "Game/System/Input/bcCamera.h"
#include "Game/System/Input/bcCameraInstance.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_camera_frustum
		{
		public:
			explicit bc_camera_frustum(const bc_icamera& p_camera) noexcept;

			explicit bc_camera_frustum(const bc_camera_instance& p_camera) noexcept;

			bc_camera_frustum(const bc_camera_frustum&) noexcept;

			~bc_camera_frustum();

			bc_camera_frustum& operator=(const bc_camera_frustum&) noexcept;

			bool intersects(const physics::bc_bound_box& p_box) const noexcept;

		private:
			void _construct(const bc_icamera::extend& p_extends);
			
			// Ordered by: near, far, left, top, right, bottom
			core::bc_array<physics::bc_shape_plane, 6> m_planes;
		};
	}
}