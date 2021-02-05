// [11/29/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Physics/bcPhysicsApi.h"
#include "Physics/Shape/bcShapeGeometry.h"

namespace black_cat
{
	namespace physics
	{
		template<bc_physics_api TApi>
		struct bc_platform_shape_capsule_pack
		{
		};

		/**
		 * \brief Capsule shapes are aligned along x axis
		 */
		template<bc_physics_api TApi>
		class bc_platform_shape_capsule : public bc_platform_shape_geometry<TApi>
		{
		public:
			using platform_pack = bc_platform_shape_capsule_pack<TApi>;

		public:
			bc_platform_shape_capsule(platform_pack& p_pack);

			/**
			 * \brief Constructor to initialzie capsule with passed radius and half height.
			 * \param p_half_height Half of the capsule's height, measured between the centers of the hemispherical ends.
			 * \param p_radius 
			 */
			bc_platform_shape_capsule(bcFLOAT p_half_height, bcFLOAT p_radius);

			bc_platform_shape_capsule(const bc_platform_shape_capsule&);

			~bc_platform_shape_capsule();

			bc_platform_shape_capsule& operator=(const bc_platform_shape_capsule&);

			bcFLOAT get_half_height() const noexcept;

			bcFLOAT get_radius() const noexcept;

			bc_shape_type get_type() const noexcept override
			{
				return bc_shape_type::capsule;
			}

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		private:
			platform_pack m_pack;
		};

		using bc_shape_capsule = bc_platform_shape_capsule<g_current_physics_api>;
	}
}