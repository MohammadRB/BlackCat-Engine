// [02/14/2021 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/Shape/bcHeightFieldSampleArray.h"
#include "PhysicsImp/bcExport.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_height_field_sample_array<g_api_physx>::bc_platform_height_field_sample_array()
		{
			m_pack.m_num_rows = 0;
			m_pack.m_num_columns = 0;
			m_pack.m_buffer = nullptr;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_height_field_sample_array<g_api_physx>::bc_platform_height_field_sample_array(bc_platform_height_field_sample_array&& p_other) noexcept
		{
			operator=(std::move(p_other));
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_height_field_sample_array<g_api_physx>::~bc_platform_height_field_sample_array()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_height_field_sample_array<g_api_physx>& bc_platform_height_field_sample_array<g_api_physx>::operator=(bc_platform_height_field_sample_array&& p_other) noexcept
		{
			m_pack.m_num_rows = p_other.m_pack.m_num_rows;
			m_pack.m_num_columns = p_other.m_pack.m_num_columns;
			m_pack.m_buffer = std::move(p_other.m_pack.m_buffer);
			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT32 bc_platform_height_field_sample_array<g_api_physx>::get_num_row() const noexcept
		{
			return m_pack.m_num_rows;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT32 bc_platform_height_field_sample_array<g_api_physx>::get_num_column() const noexcept
		{
			return m_pack.m_num_columns;
		}

		template<>
		BC_PHYSICSIMP_DLL
		std::tuple<bcINT16, bc_material_index> bc_platform_height_field_sample_array<g_api_physx>::get_sample_from_top_left(bcINT32 p_row, bcINT32 p_column) const noexcept
		{
			auto& l_sample = m_pack.m_buffer.get()[p_row * m_pack.m_num_rows + p_column];
			return std::make_tuple(l_sample.height, static_cast<bcUBYTE>(l_sample.materialIndex1));
		}

		template<>
		BC_PHYSICSIMP_DLL
		std::tuple<bcINT16, bc_material_index> bc_platform_height_field_sample_array<g_api_physx>::get_sample_from_bottom_left(bcINT32 p_row, bcINT32 p_column) const noexcept
		{
			p_row = (m_pack.m_num_rows - 1) - p_row;
			return get_sample_from_top_left(p_row, p_column);
		}
	}
}