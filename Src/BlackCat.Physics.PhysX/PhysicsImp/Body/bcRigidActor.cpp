// [12/14/2016 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "Core/Memory/bcAlloc.h"
#include "PhysicsImp/bcExport.h"
#include "PhysicsImp/bcUtility.h"
#include "PhysicsImp/Body/bcRigidActor.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_rigid_actor< g_api_physx >::bc_platform_rigid_actor() noexcept
			: bc_platform_actor()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_rigid_actor< g_api_physx >::bc_platform_rigid_actor(const bc_platform_rigid_actor& p_other) noexcept
			: bc_platform_actor(p_other)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_rigid_actor< g_api_physx >::~bc_platform_rigid_actor()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_rigid_actor< g_api_physx >& bc_platform_rigid_actor< g_api_physx >::operator=(const bc_platform_rigid_actor& p_other)
		{
			bc_platform_actor::operator=(p_other);

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_transform bc_platform_rigid_actor< g_api_physx >::get_global_pose() const noexcept
		{
			auto* l_px_actor = static_cast< physx::PxRigidActor* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_rigid_actor& >(*this)).get_platform_pack().m_px_object
			);

			bc_transform l_transform;
			l_transform.get_platform_pack().m_px_transform = l_px_actor->getGlobalPose();

			return l_transform;
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_rigid_actor< g_api_physx >::set_global_pose(const bc_transform& p_transform) noexcept
		{
			auto* l_px_actor = static_cast< physx::PxRigidActor* >
			(
				static_cast< bc_platform_physics_reference& >(*this).get_platform_pack().m_px_object
			);

			l_px_actor->setGlobalPose(const_cast< bc_transform& >(p_transform).get_platform_pack().m_px_transform);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_shape bc_platform_rigid_actor< g_api_physx >::create_shape(const bc_shape_geometry& p_shape, const bc_material& p_material, bc_shape_flag p_flags)
		{
			auto* l_px_actor = static_cast< physx::PxRigidActor* >
			(
				static_cast< bc_platform_physics_reference& >(*this).get_platform_pack().m_px_object
			);
			auto* l_px_material = static_cast< physx::PxMaterial* >
			(
				static_cast< bc_physics_reference& >(const_cast< bc_material& >(p_material)).get_platform_pack().m_px_object
			);
			bc_shape l_shape;

			static_cast< bc_physics_reference& >(l_shape).get_platform_pack().m_px_object = l_px_actor->createShape
			(
				*const_cast<bc_shape_geometry&>(p_shape).get_platform_pack().m_px_geometry,
				*l_px_material,
				static_cast< physx::PxShapeFlag::Enum >(static_cast< physx::PxShapeFlags::InternalType >(p_flags))
			);

			return l_shape;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_shape bc_platform_rigid_actor< g_api_physx >::create_shape(const bc_shape_geometry& p_shape, const bc_material* p_materials, bcUINT32 p_material_count, bc_shape_flag p_flags)
		{
			auto* l_px_actor = static_cast< physx::PxRigidActor* >
			(
				static_cast< bc_platform_physics_reference& >(*this).get_platform_pack().m_px_object
			);

			auto** l_buffer = static_cast< physx::PxMaterial** >
			(
				BC_ALLOC(sizeof(physx::PxMaterial*) * p_material_count, core::bc_alloc_type::frame)
			);

			for (bcUINT32 i = 0; i < p_material_count; ++i)
			{
				l_buffer[i] = static_cast< physx::PxMaterial* >
				(
					static_cast< bc_physics_reference& >(const_cast< bc_material& >(p_materials[i]))
					.get_platform_pack().m_px_object
				);
			}

			bc_shape l_shape;
			static_cast< bc_physics_reference& >(l_shape).get_platform_pack().m_px_object =
				l_px_actor->createShape
				(
					*const_cast<bc_shape_geometry&>(p_shape).get_platform_pack().m_px_geometry,
					l_buffer,
					p_material_count,
					static_cast< physx::PxShapeFlag::Enum >(static_cast< physx::PxShapeFlags::InternalType >(p_flags))
				);

			BC_FREE(l_buffer);

			return l_shape;
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_rigid_actor< g_api_physx >::attach_shape(bc_shape& p_shape) noexcept
		{
			auto* l_px_actor = static_cast< physx::PxRigidActor* >
			(
				static_cast< bc_platform_physics_reference& >(*this).get_platform_pack().m_px_object
			);
			auto* l_px_shape = static_cast< physx::PxShape* >
			(
				static_cast< bc_physics_reference& >(p_shape).get_platform_pack().m_px_object
			);

			l_px_actor->attachShape(*l_px_shape);
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_rigid_actor< g_api_physx >::detach_shape(bc_shape& p_shape) noexcept
		{
			auto* l_px_actor = static_cast< physx::PxRigidActor* >
			(
				static_cast< bc_platform_physics_reference& >(*this).get_platform_pack().m_px_object
			);
			auto* l_px_shape = static_cast< physx::PxShape* >
			(
				static_cast< bc_physics_reference& >(p_shape).get_platform_pack().m_px_object
			);

			l_px_actor->detachShape(*l_px_shape);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT32 bc_platform_rigid_actor< g_api_physx >::get_shape_count() const noexcept
		{
			auto* l_px_actor = static_cast< physx::PxRigidActor* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_rigid_actor& >(*this)).get_platform_pack().m_px_object
			);

			return l_px_actor->getNbShapes();
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT32 bc_platform_rigid_actor< g_api_physx >::get_shapes(bc_shape* p_buffer, bcUINT32 p_buffer_size, bcUINT32 p_start_index) const noexcept
		{
			auto* l_px_actor = static_cast< physx::PxRigidActor* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_rigid_actor& >(*this)).get_platform_pack().m_px_object
			);

			const bcUINT32 l_written_count = l_px_actor->getShapes(reinterpret_cast< physx::PxShape** >(p_buffer), p_buffer_size, p_start_index);

			bc_overwrite_output_array< bc_shape, physx::PxShape* >(p_buffer, l_written_count, [](bc_shape& p_shape, physx::PxShape*& p_px_shape)
			{
				static_cast< bc_platform_physics_reference& >(p_shape).get_platform_pack().m_px_object = p_px_shape;
			});

			return l_written_count;
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_rigid_actor< g_api_physx >::set_collision_group(bc_collision_filter p_filter) noexcept
		{
			auto l_buffer_size = get_shape_count();
			auto* l_buffer = static_cast< bc_shape* >
			(
				BC_ALLOC(sizeof(bc_shape*) * l_buffer_size, core::bc_alloc_type::frame)
			);

			get_shapes(l_buffer, l_buffer_size);

			for (bcUINT32 i = 0; i < l_buffer_size; ++i)
			{
				l_buffer[i].set_collision_group(p_filter);
			}

			BC_FREE(l_buffer);
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_rigid_actor< g_api_physx >::set_query_group(bc_query_group p_filter) noexcept
		{
			auto l_buffer_size = get_shape_count();
			auto* l_buffer = static_cast< bc_shape* >
			(
				BC_ALLOC(sizeof(bc_shape*) * l_buffer_size, core::bc_alloc_type::frame)
			);

			get_shapes(l_buffer, l_buffer_size);

			for (bcUINT32 i = 0; i < l_buffer_size; ++i)
			{
				l_buffer[i].set_query_group(p_filter);
			}

			BC_FREE(l_buffer);
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_rigid_actor< g_api_physx >::set_notify_flag(bc_shape_notify_flag p_flag, bool p_value) noexcept
		{
			auto l_buffer_size = get_shape_count();
			auto* l_buffer = static_cast< bc_shape* >
				(
					BC_ALLOC(sizeof(bc_shape*) * l_buffer_size, core::bc_alloc_type::frame)
					);

			get_shapes(l_buffer, l_buffer_size);

			for (bcUINT32 i = 0; i < l_buffer_size; ++i)
			{
				l_buffer[i].set_notify_flag(p_flag, p_value);
			}

			BC_FREE(l_buffer);
		}
	}
}