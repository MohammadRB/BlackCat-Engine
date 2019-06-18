// [01/11/2019 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "PhysicsImp/Shape/bcBoundBox.h"
#include "PhysicsImp/Shape/bcShapePlane.h"
#include "Game/bcExport.h"
#include "Game/System/Input/bcCamera.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_camera_frustum
		{
		public:
			explicit bc_camera_frustum(const bc_icamera& p_camera);

			bc_camera_frustum(bc_camera_frustum&&) noexcept;

			~bc_camera_frustum();

			bc_camera_frustum& operator=(bc_camera_frustum&&) noexcept;

			bool intersects(const physics::bc_bound_box& p_box) const noexcept;

		protected:

		private:
			// Ordered by: near, far, left, top, right, bottom
			core::bc_vector<physics::bc_shape_plane> m_planes;
		};
	}
}