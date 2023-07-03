// [11/12/2016 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/bcExport.h"
#include "PhysicsImp/Shape/bcConvexMesh.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_convex_mesh<g_api_physx>::bc_platform_convex_mesh() noexcept
			: bc_platform_physics_reference(),
			m_pack()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_convex_mesh<g_api_physx>::bc_platform_convex_mesh(platform_pack& p_pack) noexcept
			: bc_platform_physics_reference(),
			m_pack(p_pack)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_convex_mesh<g_api_physx>::bc_platform_convex_mesh(const bc_platform_convex_mesh& p_other) noexcept
			: bc_platform_physics_reference(p_other),
			m_pack(p_other.m_pack)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_convex_mesh<g_api_physx>::~bc_platform_convex_mesh()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_convex_mesh<g_api_physx>& bc_platform_convex_mesh<g_api_physx>::operator=(const bc_platform_convex_mesh& p_other) noexcept
		{
			bc_platform_physics_reference::operator=(p_other);
			m_pack = p_other.m_pack;

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT32 bc_platform_convex_mesh< g_api_physx >::get_num_vertices() const noexcept
		{
			physx::PxConvexMesh* l_px_convex_mesh = static_cast<physx::PxConvexMesh*>(m_pack.m_px_object);

			return l_px_convex_mesh->getNbVertices();
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT32 bc_platform_convex_mesh<g_api_physx>::get_num_polygons() const noexcept
		{
			physx::PxConvexMesh* l_px_convex_mesh = static_cast<physx::PxConvexMesh*>(m_pack.m_px_object);

			return l_px_convex_mesh->getNbPolygons();
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_bounded_strided_data bc_platform_convex_mesh<g_api_physx>::get_vertices() const
		{
			physx::PxConvexMesh* l_px_convex_mesh = static_cast<physx::PxConvexMesh*>(m_pack.m_px_object);
			bc_bounded_strided_data l_data
			(
				reinterpret_cast< const core::bc_vector3f* >(l_px_convex_mesh->getVertices()), 
				sizeof(physx::PxVec3), 
				l_px_convex_mesh->getNbVertices()
			);

			return l_data;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_strided_data bc_platform_convex_mesh<g_api_physx>::get_indices() const
		{
			physx::PxConvexMesh* l_px_convex_mesh = static_cast<physx::PxConvexMesh*>(m_pack.m_px_object);
			bc_strided_data l_data(reinterpret_cast<const bcUINT16*>(l_px_convex_mesh->getIndexBuffer()), sizeof(bcUINT16));

			return l_data;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_convex_mesh_polygon bc_platform_convex_mesh<g_api_physx>::get_polygon(bcINT32 p_index) const
		{
			physx::PxConvexMesh* l_px_convex_mesh = static_cast<physx::PxConvexMesh*>(m_pack.m_px_object);
			physx::PxHullPolygon l_px_polygon;
			l_px_convex_mesh->getPolygonData(p_index, l_px_polygon);

			return bc_convex_mesh_polygon(l_px_polygon.mNbVerts, l_px_polygon.mIndexBase);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_bound_box bc_platform_convex_mesh<g_api_physx>::get_local_bound() const
		{
			physx::PxConvexMesh* l_px_convex_mesh = static_cast<physx::PxConvexMesh*>(m_pack.m_px_object);

			bc_platform_bound_box<g_api_physx>::platform_pack l_pack;
			l_pack.m_bound = l_px_convex_mesh->getLocalBounds();
			
			return bc_platform_bound_box<g_api_physx>(l_pack);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bool bc_platform_convex_mesh<g_api_physx>::is_valid() const noexcept
		{
			return m_pack.m_px_object != nullptr;
		}
	}
}