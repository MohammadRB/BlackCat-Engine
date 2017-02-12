// [12/11/2016 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/bcExport.h"
#include "PhysicsImp/bcUtility.h"
#include "PhysicsImp/Shape/bcHeightField.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_height_field< g_api_physx >::bc_platform_height_field()
			: bc_platform_physics_reference()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_height_field< g_api_physx >::bc_platform_height_field(const bc_platform_height_field& p_other) noexcept
			: bc_platform_physics_reference(p_other)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_height_field< g_api_physx >::~bc_platform_height_field()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_height_field< g_api_physx >& bc_platform_height_field< g_api_physx >::operator=(const bc_platform_height_field& p_other) noexcept
		{
			bc_platform_physics_reference::operator=(p_other);

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT32 bc_platform_height_field< g_api_physx >::get_num_row() const noexcept
		{
			physx::PxHeightField* l_px_height_field = static_cast< physx::PxHeightField* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_height_field& >(*this)).get_platform_pack().m_px_object
			);

			return l_px_height_field->getNbRows();
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT32 bc_platform_height_field< g_api_physx >::get_num_column() const noexcept
		{
			physx::PxHeightField* l_px_height_field = static_cast< physx::PxHeightField* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_height_field& >(*this)).get_platform_pack().m_px_object
			);

			return l_px_height_field->getNbColumns();
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcFLOAT bc_platform_height_field< g_api_physx >::get_height(bcFLOAT p_x, bcFLOAT p_z)
		{
			physx::PxHeightField* l_px_height_field = static_cast< physx::PxHeightField* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_height_field& >(*this)).get_platform_pack().m_px_object
			);

			return l_px_height_field->getHeight(p_x, p_z);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_material_index bc_platform_height_field< g_api_physx >::get_triangle_material(bcUINT32 p_triangle_index)
		{
			physx::PxHeightField* l_px_height_field = static_cast< physx::PxHeightField* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_height_field& >(*this)).get_platform_pack().m_px_object
			);

			return l_px_height_field->getTriangleMaterialIndex(p_triangle_index);
		}

		template<>
		BC_PHYSICSIMP_DLL
		core::bc_vector3f bc_platform_height_field< g_api_physx >::get_triangle_normal(bcUINT32 p_triangle_index)
		{
			physx::PxHeightField* l_px_height_field = static_cast< physx::PxHeightField* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_height_field& >(*this)).get_platform_pack().m_px_object
			);

			auto l_px_vec3 = l_px_height_field->getTriangleNormal(p_triangle_index);

			return bc_to_game_hand(l_px_vec3);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bool bc_platform_height_field< g_api_physx >::modify_samples(bcUINT32 p_row, bcUINT32 p_column, const bc_height_field_desc& p_desc)
		{
			physx::PxHeightField* l_px_height_field = static_cast< physx::PxHeightField* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_height_field& >(*this)).get_platform_pack().m_px_object
			);
			physx::PxHeightFieldSample* l_px_samples = static_cast< physx::PxHeightFieldSample* >
			(
				bcAlloc(sizeof(physx::PxHeightFieldSample) * (p_desc.m_num_row * p_desc.m_num_column), core::bc_alloc_type::frame)
			);

			physx::PxHeightFieldDesc l_px_height_desc = bc_convert_to_px_height_field(p_desc, l_px_samples);
			bool l_result = l_px_height_field->modifySamples(p_row, p_column, l_px_height_desc);

			// TODO Is it required to free samples
			bcFree(l_px_samples);

			return l_result;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bool bc_platform_height_field< g_api_physx >::is_valid() const noexcept
		{
			return static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_height_field& >(*this)).get_platform_pack().m_px_object != nullptr;
		}
	}
}
