// [11/28/2016 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Core/Math/bcMatrix3f.h"
#include "Core/Math/bcMatrix4f.h"
#include "Physics/bcPhysicsApi.h"

namespace black_cat
{
	namespace physics
	{
		template< bc_physics_api TApi >
		struct bc_platform_transform_pack
		{
			
		};

		template< bc_physics_api TApi >
		class bc_platform_transform
		{
		public:
			using platform_pack = bc_platform_transform_pack< TApi >;

		public:
			bc_platform_transform() noexcept;

			explicit bc_platform_transform(const core::bc_vector3f& p_position) noexcept;

			explicit bc_platform_transform(const core::bc_matrix4f& p_transform) noexcept;

			bc_platform_transform(const core::bc_vector3f& p_position, const core::bc_matrix3f& p_rotation) noexcept;

			bc_platform_transform(const bc_platform_transform& p_other) noexcept;

			~bc_platform_transform();

			bc_platform_transform& operator=(const bc_platform_transform& p_other) noexcept;

			bc_platform_transform operator*(const bc_platform_transform& p_other) noexcept;

			bc_platform_transform& operator*=(const bc_platform_transform& p_other) noexcept;

			bc_platform_transform get_inverse() const noexcept;

			bc_platform_transform get_normalized() const noexcept;

			bc_platform_transform transform(const bc_platform_transform& p_other) const noexcept;
			
			core::bc_vector3f transform(const core::bc_vector3f& p_vector) const noexcept;

			core::bc_vector3f transform_inverse(const core::bc_vector3f& p_vector) const noexcept;

			core::bc_vector3f rotate(const core::bc_vector3f& p_vector) const noexcept;

			core::bc_vector3f rotate_inverse(const core::bc_vector3f& p_vector) const noexcept;

			bool is_finite() const noexcept;

			bool is_valid() const noexcept;

			core::bc_vector3f get_position() const noexcept;

			core::bc_matrix3f get_matrix3() const noexcept;

			core::bc_matrix4f get_matrix4() const noexcept;

			static bc_platform_transform identity() noexcept;

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		private:
			platform_pack m_pack;
		};

		using bc_transform = bc_platform_transform< g_current_physics_api >;
	}
}