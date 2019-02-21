// [11/30/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Core/Math/bcVector3f.h"
#include "Physics/bcPhysicsApi.h"
#include "Physics/bcPhysicsReference.h"
#include "Physics/bcPhysicsRef.h"
#include "Physics/bcUtility.h"
#include "Physics/Fundation/bcMaterial.h"

namespace black_cat
{
	namespace physics
	{
		template<bc_physics_api>
		class bc_platform_shape;

		using bc_shape = bc_platform_shape<g_current_physics_api>;

		/**
		 * \brief Represent required data to create a height field in physics api.
		 * Height samples must be defined from top-left corner.
		 */
		struct bc_height_field_desc
		{
			bc_height_field_desc(bcUINT32 p_num_row, 
				bcUINT32 p_num_column, 
				const bc_bounded_strided_typed_data<bcINT16>& p_samples, 
				const bc_bounded_strided_typed_data<bc_material_index>& p_samples_material)
				: m_num_row(p_num_row),
				m_num_column(p_num_column),
				m_samples(p_samples),
				m_samples_material(p_samples_material)
			{
			}

			const bcUINT32 m_num_row;
			const bcUINT32 m_num_column;
			/**
			 * \brief must have m_num_row * m_num_column sample that each one map to a bcINT16.
			 */
			const bc_bounded_strided_typed_data<bcINT16> m_samples;
			/**
			 * \brief must have m_num_row * m_num_column sample that each one map to a bc_material_index witch 
			 * will be used as material index for each cell.
			 */
			const bc_bounded_strided_typed_data<bc_material_index> m_samples_material;
		};

		template< bc_physics_api TApi >
		struct bc_platform_height_field_pack
		{
		};

		template< bc_physics_api TApi >
		class bc_platform_height_field : public bc_platform_physics_reference<TApi>
		{
		public:
			using platform_pack = bc_platform_height_field_pack< TApi >;

		public:
			bc_platform_height_field();

			bc_platform_height_field(const bc_platform_height_field&) noexcept;

			~bc_platform_height_field();

			bc_platform_height_field& operator=(const bc_platform_height_field&) noexcept;

			bcUINT32 get_num_row() const noexcept;

			bcUINT32 get_num_column() const noexcept;

			/**
			 * \brief Get height at given coordinate or 0 if indices are out of range
			 * \param p_x 
			 * \param p_z 
			 * \return 
			 */
			bcFLOAT get_height(bcFLOAT p_x, bcFLOAT p_z);

			bc_material_index get_triangle_material(bcUINT32 p_triangle_index);

			core::bc_vector3f get_triangle_normal(bcUINT32 p_triangle_index);

			/**
			 * \brief Replaces a rectangular sub-field in the sample data array.
			 * Source samples that are out of range of target height-field will be clipped with no error.
			 * \param p_row First row in the destination height-field to be modified.Can be negative.
			 * \param p_column First cell in the destination height-field to be modified.Can be negative.
			 * \param p_desc
			 * \param p_height_field_shapes Shapes that are created with this height field.
			 * \param p_shape_count Number of shapes associated with this height field.
			 * \return 
			 */
			bool modify_samples(bcUINT32 p_row, 
				bcUINT32 p_column, 
				const bc_height_field_desc& p_desc, 
				bc_shape* p_height_field_shapes, 
				bcUINT32 p_shape_count);

			bool is_valid() const noexcept override;

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		protected:

		private:
			platform_pack m_pack;
		};

		using bc_height_field = bc_platform_height_field< g_current_physics_api >;
		using bc_height_field_ref = bc_physics_ref< bc_height_field >;
	}
}