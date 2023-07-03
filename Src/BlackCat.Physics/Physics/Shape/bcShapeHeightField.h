// [30/11/2016 MRB]

#pragma once

#include "Physics/Shape/bcHeightField.h"
#include "Physics/Shape/bcShapeGeometry.h"

namespace black_cat
{
	namespace physics
	{
		template<bc_physics_api TApi>
		struct bc_platform_shape_height_field_pack
		{
		};

		template<bc_physics_api TApi>
		class bc_platform_shape_height_field : public bc_platform_shape_geometry<TApi>
		{
		public:
			using platform_pack = bc_platform_shape_height_field_pack<TApi>;

		public:
			bc_platform_shape_height_field() noexcept;

			explicit bc_platform_shape_height_field(platform_pack& p_pack) noexcept;

			bc_platform_shape_height_field(const bc_height_field& p_height_field,
				bcUINT16 p_xz_scale,
				bcFLOAT p_y_scale) noexcept;

			bc_platform_shape_height_field(const bc_platform_shape_height_field&) noexcept;

			~bc_platform_shape_height_field() override;

			bc_platform_shape_height_field& operator=(const bc_platform_shape_height_field&) noexcept;

			bc_height_field get_height_field() const noexcept;

			bcUINT16 get_xz_scale() const noexcept;

			bcUINT16 get_y_scale() const noexcept;

			bc_shape_type get_type() const noexcept override
			{
				return bc_shape_type::height_field;
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

		using bc_shape_height_field = bc_platform_shape_height_field<g_current_physics_api>;
	}
}