// [29/11/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Physics/bcPhysicsApi.h"
#include "Physics/Shape/bcShapeGeometry.h"

namespace black_cat::physics
{
	template<bc_physics_api TApi>
	struct bc_platform_shape_sphere_pack
	{
	};

	template<bc_physics_api TApi>
	class bc_platform_shape_sphere : public bc_platform_shape_geometry<TApi>
	{
	public:
		using platform_pack = bc_platform_shape_sphere_pack<TApi>;

	public:
		bc_platform_shape_sphere() noexcept;

		explicit bc_platform_shape_sphere(platform_pack& p_pack) noexcept;

		explicit bc_platform_shape_sphere(bcFLOAT p_radius) noexcept;

		bc_platform_shape_sphere(const bc_platform_shape_sphere&) noexcept;

		~bc_platform_shape_sphere() noexcept override;

		bc_platform_shape_sphere& operator=(const bc_platform_shape_sphere&) noexcept;

		bcFLOAT get_radius() const noexcept;

		bc_shape_type get_type() const noexcept override
		{
			return bc_shape_type::sphere;
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

	using bc_shape_sphere = bc_platform_shape_sphere<g_current_physics_api>;
}
