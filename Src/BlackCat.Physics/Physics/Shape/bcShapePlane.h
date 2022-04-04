// [11/29/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Core/Math/bcVector3f.h"
#include "Physics/bcPhysicsApi.h"
#include "Physics/Shape/bcShapeGeometry.h"
#include "Physics/Fundation/bcTransform.h"

namespace black_cat
{
	namespace physics
	{
		template<bc_physics_api TApi>
		struct bc_platform_shape_plane_pack
		{
		};

		template<bc_physics_api TApi>
		class bc_platform_shape_plane : public bc_platform_shape_geometry<TApi>
		{
		public:
			using platform_pack = bc_platform_shape_plane_pack<TApi>;

		public:
			bc_platform_shape_plane() noexcept;

			explicit bc_platform_shape_plane(platform_pack& p_pack) noexcept;

			bc_platform_shape_plane(const core::bc_vector3f& p_normal, bcFLOAT p_distance) noexcept;

			bc_platform_shape_plane(const core::bc_vector3f& p_normal, const core::bc_vector3f& p_point_on_plane) noexcept;

			bc_platform_shape_plane(const core::bc_vector3f& p_point0,
				const core::bc_vector3f& p_point1,
				const core::bc_vector3f& p_point2) noexcept;

			bc_platform_shape_plane(const bc_platform_shape_plane&) noexcept;

			~bc_platform_shape_plane() override;

			bc_platform_shape_plane& operator=(const bc_platform_shape_plane&) noexcept;

			bcFLOAT distance(const core::bc_vector3f& p_point) const noexcept;

			core::bc_vector3f get_normal() const noexcept;

			bcFLOAT get_distance() const noexcept;

			bc_transform as_transform() const noexcept;

			bc_shape_type get_type() const noexcept override
			{
				return bc_shape_type::plane;
			}

			platform_pack& get_platform_pack() noexcept override
			{
				return m_pack;
			}

			const platform_pack& get_platform_pack() const noexcept override
			{
				return m_pack;
			}

		private:
			platform_pack m_pack;
		};

		using bc_shape_plane = bc_platform_shape_plane<g_current_physics_api>;
	}
}