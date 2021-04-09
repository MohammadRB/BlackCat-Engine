// [11/29/2016 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Core/Math/bcMatrix3f.h"
#include "Core/Container/bcArray.h"
#include "Physics/bcPhysicsApi.h"
#include "Physics/Fundation/bcTransform.h"

namespace black_cat
{
	namespace physics
	{
		template< bc_physics_api TApi >
		struct bc_platform_bound_box_pack
		{
		};

		template< bc_physics_api TApi >
		class bc_platform_bound_box
		{
		public:
			using platform_pack = bc_platform_bound_box_pack<TApi>;

		public:
			/**
			 * \brief Initialize as empty bound box
			 */
			bc_platform_bound_box() noexcept;

			bc_platform_bound_box(const core::bc_vector3f& p_center, const core::bc_vector3f& p_half_extend) noexcept;

			explicit bc_platform_bound_box(platform_pack& p_pack) noexcept;
			
			bc_platform_bound_box(const bc_platform_bound_box&) noexcept;

			~bc_platform_bound_box();

			bc_platform_bound_box& operator=(const bc_platform_bound_box&) noexcept;

			core::bc_vector3f get_center() const noexcept;

			core::bc_vector3f get_half_extends() const noexcept;

			void expand(const core::bc_vector3f& p_point) noexcept;

			void expand(const bc_platform_bound_box& p_box) noexcept;

			bool intersect(const bc_platform_bound_box& p_box) const noexcept;

			bool contains(const core::bc_vector3f& p_point)  const noexcept;

			bool contains(const bc_platform_bound_box& p_box) const noexcept;

			void scale(bcFLOAT p_scale) noexcept;

			bool is_empty() const noexcept;

			void set_empty() noexcept;

			void transform(const bc_transform& p_transform) noexcept;

			/**
			 * \brief Get 8 points of box. Start from -x,+z counter clockwise to +x,+z, from +y to -y
			 * \param p_result 
			 */
			void get_points(core::bc_array<core::bc_vector3f, 8>& p_result) const noexcept;

			static bc_platform_bound_box from_center_extends(const core::bc_vector3f& p_center, const core::bc_vector3f& p_half_extend);

			static bc_platform_bound_box from_min_max(const core::bc_vector3f& p_min, const core::bc_vector3f& p_max);

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}
			
		private:
			platform_pack m_pack;
		};

		using bc_bound_box = bc_platform_bound_box< g_current_physics_api >;
	}
}