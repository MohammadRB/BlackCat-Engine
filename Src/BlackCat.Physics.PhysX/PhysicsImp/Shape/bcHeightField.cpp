// [12/11/2016 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/bcExport.h"
#include "PhysicsImp/bcUtility.h"
#include "PhysicsImp/Shape/bcHeightField.h"
#include "PhysicsImp/Shape/bcShape.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_height_field<g_api_physx>::bc_platform_height_field() noexcept
			: bc_platform_physics_reference(),
			m_pack()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_height_field<g_api_physx>::bc_platform_height_field(platform_pack& p_pack) noexcept
			: bc_platform_physics_reference(),
			m_pack(p_pack)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_height_field< g_api_physx >::bc_platform_height_field(const bc_platform_height_field& p_other) noexcept
			: bc_platform_physics_reference(p_other),
			m_pack(p_other.m_pack)
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
			m_pack = p_other.m_pack;

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT32 bc_platform_height_field< g_api_physx >::get_num_row() const noexcept
		{
			physx::PxHeightField* l_px_height_field = static_cast<physx::PxHeightField*>(m_pack.m_px_object);

			return l_px_height_field->getNbRows();
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT32 bc_platform_height_field< g_api_physx >::get_num_column() const noexcept
		{
			physx::PxHeightField* l_px_height_field = static_cast<physx::PxHeightField*>(m_pack.m_px_object);

			return l_px_height_field->getNbColumns();
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcFLOAT bc_platform_height_field< g_api_physx >::get_height(bcFLOAT p_x, bcFLOAT p_z) const noexcept
		{
			physx::PxHeightField* l_px_height_field = static_cast<physx::PxHeightField*>(m_pack.m_px_object);

			return l_px_height_field->getHeight(p_x, p_z);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_material_index bc_platform_height_field< g_api_physx >::get_triangle_material(bcUINT32 p_triangle_index) const noexcept
		{
			physx::PxHeightField* l_px_height_field = static_cast<physx::PxHeightField*>(m_pack.m_px_object);

			return l_px_height_field->getTriangleMaterialIndex(p_triangle_index);
		}

		template<>
		BC_PHYSICSIMP_DLL
		core::bc_vector3f bc_platform_height_field< g_api_physx >::get_triangle_normal(bcUINT32 p_triangle_index) const noexcept
		{
			physx::PxHeightField* l_px_height_field = static_cast<physx::PxHeightField*>(m_pack.m_px_object);

			const auto l_px_vec3 = l_px_height_field->getTriangleNormal(p_triangle_index);

			return bc_to_game_hand(l_px_vec3);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bool bc_platform_height_field< g_api_physx >::modify_samples(bcUINT32 p_row, 
			bcUINT32 p_column, 
			const bc_height_field_desc& p_desc, 
			bc_shape* p_height_field_shapes, 
			bcUINT32 p_shape_count)
		{
			auto* l_px_height_field = static_cast<physx::PxHeightField*>(m_pack.m_px_object);
			core::bc_unique_ptr< physx::PxHeightFieldSample > l_px_samples(static_cast< physx::PxHeightFieldSample* >
			(
				BC_ALLOC(sizeof(physx::PxHeightFieldSample) * (p_desc.m_num_row * p_desc.m_num_column), core::bc_alloc_type::frame)
			));

			const physx::PxHeightFieldDesc l_px_height_desc = bc_convert_to_px_height_field(p_desc, l_px_samples.get());
			const bool l_result = l_px_height_field->modifySamples(p_row, p_column, l_px_height_desc);
			
			// Update associated shapes to this height field
			for (bcUINT32 i = 0; i < p_shape_count; ++i)
			{
				auto* l_px_shape = static_cast<physx::PxShape*>(p_height_field_shapes[i].get_platform_pack().m_px_object);

				auto& l_px_height_field_geo = l_px_shape->getGeometry().heightField();
				l_px_shape->setGeometry(physx::PxHeightFieldGeometry
				(
					l_px_height_field,
					l_px_height_field_geo.heightFieldFlags,
					l_px_height_field_geo.heightScale,
					l_px_height_field_geo.rowScale,
					l_px_height_field_geo.columnScale
				));
			}

			return l_result;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_height_field_sample_array bc_platform_height_field< g_api_physx >::get_sample_array(core::bc_alloc_type p_alloc_type) const
		{
			bc_height_field_sample_array l_result;
			auto& l_result_pack = l_result.get_platform_pack();
			const auto l_buffer_size = get_num_row() * get_num_column() * sizeof(physx::PxHeightFieldSample);
			
			l_result_pack.m_num_rows = get_num_row();
			l_result_pack.m_num_columns = get_num_column();
			l_result_pack.m_buffer.reset(static_cast< physx::PxHeightFieldSample* >(BC_ALLOC(l_buffer_size, p_alloc_type)));

			auto* l_px_height_field = static_cast<physx::PxHeightField*>
			(
				static_cast<bc_platform_physics_reference&>(const_cast<bc_platform_height_field&>(*this)).get_platform_pack().m_px_object
			);
			l_px_height_field->saveCells(l_result_pack.m_buffer.get(), l_buffer_size);

			return l_result;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bool bc_platform_height_field< g_api_physx >::is_valid() const noexcept
		{
			return m_pack.m_px_object != nullptr;
		}
	}
}