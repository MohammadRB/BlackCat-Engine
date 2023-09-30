// [30/11/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Core/Math/bcVector3f.h"
#include "Physics/Fundation/bcMaterial.h"
#include "Physics/Shape/bcHeightFieldSampleArray.h"
#include "Physics/bcPhysicsReference.h"
#include "Physics/bcPhysicsRef.h"
#include "Physics/bcPhysicsApi.h"
#include "Physics/bcUtility.h"

namespace black_cat::physics
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

	struct bc_height_field_sample
	{
		/**
		 * \brief The height of the heightfield sample.
		 * This value is scaled by bc_physics_system::get_height_field_y_scale.
		 */
		bcINT16 m_height;
		/**
		 * \brief The triangle material index of the quad's lower triangle.
		 */
		bc_material_index m_material_index_0;
		/**
		 * \brief The triangle material index of the quad's upper triangle.
		 */
		bc_material_index m_material_index_1;
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
		bc_platform_height_field() noexcept;

		explicit bc_platform_height_field(platform_pack& p_pack) noexcept;

		bc_platform_height_field(const bc_platform_height_field&) noexcept;

		~bc_platform_height_field() override;

		bc_platform_height_field& operator=(const bc_platform_height_field&) noexcept;

		bcUINT32 get_num_row() const noexcept;

		bcUINT32 get_num_column() const noexcept;

		/**
		 * \brief Returns sample of given row and column
		 * \param p_x
		 * \param p_z
		 * \return
		 */
		bc_height_field_sample get_sample(bcUINT32 p_x, bcUINT32 p_z) const noexcept;

		/**
		 * \brief Get height at given coordinate or 0 if indices are out of range
		 * \param p_x 
		 * \param p_z 
		 * \return 
		 */
		bcFLOAT get_height(bcFLOAT p_x, bcFLOAT p_z) const noexcept;

		bc_material_index get_triangle_material(bcUINT32 p_triangle_index) const noexcept;

		core::bc_vector3f get_triangle_normal(bcUINT32 p_triangle_index) const noexcept;

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

		/**
		 * \brief Get internal array of height field samples
		 * \return 
		 */
		bc_height_field_sample_array get_sample_array(core::bc_alloc_type p_alloc_type) const;
			
		bool is_valid() const noexcept override;

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

	using bc_height_field = bc_platform_height_field< g_current_physics_api >;
	using bc_height_field_ref = bc_physics_ref< bc_height_field >;
}
