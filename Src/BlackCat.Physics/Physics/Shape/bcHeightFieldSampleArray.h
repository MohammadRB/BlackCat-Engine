// [02/14/2021 MRB]

#pragma once

#include "Physics/Fundation/bcMaterial.h"
#include "Physics/bcPhysicsApi.h"

namespace black_cat
{
	namespace physics
	{
		template< bc_physics_api TApi >
		struct bc_platform_height_field_sample_array_pack
		{
		};

		template< bc_physics_api TApi >
		class bc_platform_height_field_sample_array
		{
		public:
			using platform_pack = bc_platform_height_field_sample_array_pack< TApi >;
			
		public:
			bc_platform_height_field_sample_array();

			bc_platform_height_field_sample_array(bc_platform_height_field_sample_array&&) noexcept;

			~bc_platform_height_field_sample_array();

			bc_platform_height_field_sample_array& operator=(bc_platform_height_field_sample_array&&) noexcept;

			bcUINT32 get_num_row() const noexcept;

			bcUINT32 get_num_column() const noexcept;
			
			/**
			 * \brief Any index out of height field boundaries are remapped
			 * \param p_row 
			 * \param p_column 
			 * \return 
			 */
			std::tuple<bcINT16, bc_material_index> get_sample_from_top_left(bcINT32 p_row, bcINT32 p_column) const noexcept;

			/**
			 * \brief Any index out of height field boundaries are remapped
			 * \param p_row
			 * \param p_column
			 * \return
			 */
			std::tuple<bcINT16, bc_material_index> get_sample_from_bottom_left(bcINT32 p_row, bcINT32 p_column) const noexcept;
			
			platform_pack& get_platform_pack()
			{
				return m_pack;
			}
		
		private:
			platform_pack m_pack;
		};

		using bc_height_field_sample_array = bc_platform_height_field_sample_array< g_current_physics_api >;
	}	
}