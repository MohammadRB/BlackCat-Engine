// [12/12/2016 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/bcExport.h"
#include "PhysicsImp/Shape/bcTriangleMesh.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_triangle_mesh< g_api_physx >::bc_platform_triangle_mesh() noexcept
			: bc_platform_physics_reference()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_triangle_mesh< g_api_physx >::bc_platform_triangle_mesh(const bc_platform_triangle_mesh& p_other) noexcept
			: bc_platform_physics_reference(p_other)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_triangle_mesh< g_api_physx >::~bc_platform_triangle_mesh()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_triangle_mesh< g_api_physx >& bc_platform_triangle_mesh< g_api_physx >::operator=(const bc_platform_triangle_mesh& p_other) noexcept
		{
			bc_platform_physics_reference::operator=(p_other);

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT32 bc_platform_triangle_mesh< g_api_physx >::get_num_vertices() const noexcept
		{
			auto* l_px_mesh = static_cast< physx::PxTriangleMesh* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_triangle_mesh& >(*this)).get_platform_pack().m_px_object
			);

			return l_px_mesh->getNbVertices();
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT32 bc_platform_triangle_mesh< g_api_physx >::get_num_triangles() const noexcept
		{
			auto* l_px_mesh = static_cast< physx::PxTriangleMesh* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_triangle_mesh& >(*this)).get_platform_pack().m_px_object
			);

			return l_px_mesh->getNbTriangles();
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_bounded_strided_data bc_platform_triangle_mesh< g_api_physx >::get_vertices() const
		{
			auto* l_px_mesh = static_cast< physx::PxTriangleMesh* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_triangle_mesh& >(*this)).get_platform_pack().m_px_object
			);

			return bc_bounded_strided_data(l_px_mesh->getVertices(), sizeof(physx::PxVec3), get_num_vertices());
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_bounded_strided_data bc_platform_triangle_mesh< g_api_physx >::get_indices() const
		{
			auto* l_px_mesh = static_cast< physx::PxTriangleMesh* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_triangle_mesh& >(*this)).get_platform_pack().m_px_object
			);

			return bc_bounded_strided_data
			(
				l_px_mesh->getTriangles(),
				l_px_mesh->getTriangleMeshFlags() & physx::PxTriangleMeshFlag::e16_BIT_INDICES ? sizeof(physx::PxU16) : sizeof(physx::PxU32),
				get_num_triangles() * 3
			);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_bound_box bc_platform_triangle_mesh< g_api_physx >::get_local_bound() const
		{
			auto* l_px_mesh = static_cast< physx::PxTriangleMesh* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_triangle_mesh& >(*this)).get_platform_pack().m_px_object
			);

			bc_bound_box l_bound;
			l_bound.get_platform_pack().m_bound = l_px_mesh->getLocalBounds();

			return l_bound;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bool bc_platform_triangle_mesh< g_api_physx >::is_valid() const noexcept
		{
			return static_cast< bc_platform_physics_reference& >
				(
					const_cast< bc_platform_triangle_mesh& >(*this)
				)
				.get_platform_pack().m_px_object != nullptr;
		}
	}
}