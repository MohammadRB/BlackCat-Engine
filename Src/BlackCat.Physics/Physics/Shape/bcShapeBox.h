// [11/29/2016 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Core/Container/bcArray.h"
#include "Physics/bcPhysicsApi.h"
#include "Physics/Shape/bcShapeGeometry.h"

namespace black_cat
{
	namespace physics
	{
		template<bc_physics_api TApi>
		struct bc_platform_shape_box_pack
		{
		};

		template<bc_physics_api TApi>
		class bc_platform_shape_box : public bc_platform_shape_geometry<TApi>
		{
		public:
			using platform_pack = bc_platform_shape_box_pack<TApi>;

		public:
			bc_platform_shape_box(platform_pack& p_pack);

			/**
			 * \brief Constructor to initialize half extents
			 * \param p_x_half half of x extend
			 * \param p_y_half half of y extend
			 * \param p_z_half half of z extend
			 */
			bc_platform_shape_box(bcFLOAT p_x_half, bcFLOAT p_y_half, bcFLOAT p_z_half);

			explicit bc_platform_shape_box(const core::bc_vector3f& p_half_extends);

			bc_platform_shape_box(const bc_platform_shape_box&);

			~bc_platform_shape_box();

			bc_platform_shape_box& operator=(const bc_platform_shape_box&);

			core::bc_vector3f get_half_extends() const noexcept;

			void get_points(core::bc_array<core::bc_vector3f, 8>& p_result) const noexcept;

			bc_shape_type get_type() const noexcept override
			{
				return bc_shape_type::box;
			}

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		private:
			platform_pack m_pack;
		};

		using bc_shape_box = bc_platform_shape_box<g_current_physics_api>;
	}
}