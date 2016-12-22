// [12/10/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Physics/Shape/bcShapeGeometry.h"
#include "Physics/Shape/bcShapeSphere.h"
#include "Physics/Shape/bcShapeBox.h"
#include "Physics/Shape/bcShapeCapsule.h"
#include "Physics/Shape/bcShapePlane.h"
#include "Physics/Shape/bcShapeConvexMesh.h"
#include "Physics/Shape/bcShapeTriangleMesh.h"
#include "Physics/Shape/bcShapeHeightField.h"
#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/Shape/bcConvexMesh.h"
#include "PhysicsImp/Shape/bcTriangleMesh.h"
#include "PhysicsImp/Shape/bcHeightField.h"

namespace black_cat
{
	namespace physics
	{
		template< class TBcType, class TPxType, class TInitializer >
		void bc_overwrite_output_array(TBcType* p_buffer, bcUINT32 p_element_count, TInitializer p_initializer)
		{
			auto* l_px_types = reinterpret_cast< TPxType** >(p_buffer);

			// Initialize objects in reverse order to prevent data overwrite(in case of wrapper objects are bigger) 
			// when writing to the array
			for (bcUINT32 i = p_element_count - 1; i >= 0; --i)
			{
				TPxType* l_px_type = l_px_types[i];
				TBcType* l_bc_type = &p_buffer[i];

				p_initializer(l_bc_type, l_px_type);
			}
		}

		physx::PxHeightFieldDesc bc_convert_to_px_height_field(const bc_height_field_desc& p_desc);

		/*class bc_px_geometry_holder
		{
		public:
			explicit bc_px_geometry_holder(const bc_shape_geometry& p_geometry)
			{
				switch (p_geometry.get_type())
				{
				case bc_shape_type::sphere:
					from_sphere(static_cast< const bc_shape_sphere& >(p_geometry));
					m_geometry = &m_sphere;
					break;
				case bc_shape_type::plane:
					from_box(static_cast< const bc_shape_box& >(p_geometry));
					m_geometry = &m_box;
					break;
				case bc_shape_type::capsule:
					from_capsule(static_cast< const bc_shape_capsule& >(p_geometry));
					m_geometry = &m_capsule;
					break;
				case bc_shape_type::box:
					from_plane(static_cast< const bc_shape_plane& >(p_geometry));
					m_geometry = &m_plane;
					break;
				case bc_shape_type::convexmesh:
					from_convex_mesh(static_cast< const bc_shape_convex_mesh& >(p_geometry));
					m_geometry = &m_convex_mesh;
					break;
				case bc_shape_type::trianglemesh:
					from_triangle_mesh(static_cast< const bc_shape_triangle_mesh& >(p_geometry));
					m_geometry = &m_triangle_mesh;
					break;
				case bc_shape_type::heightfield:
					from_height_field(static_cast< const bc_shape_height_field& >(p_geometry));
					m_geometry = &m_height_field;
					break;
				default:
					break;
				}
			}

			void from_sphere(const bc_shape_sphere& p_sphere) noexcept
			{
				m_sphere = physx::PxSphereGeometry(p_sphere.m_radius);
			}

			void from_box(const bc_shape_box& p_box) noexcept
			{
				m_box = physx::PxBoxGeometry(physx::PxVec3
				(
					p_box.m_half_extends.x,
					p_box.m_half_extends.y,
					p_box.m_half_extends.z
				));
			}

			void from_capsule(const bc_shape_capsule& p_capsule) noexcept
			{
				m_capsule = physx::PxCapsuleGeometry(p_capsule.m_radius, p_capsule.m_half_height);
			}

			void from_plane(const bc_shape_plane& p_plane) noexcept
			{
				m_plane = physx::PxPlaneGeometry();
			}

			void from_convex_mesh(const bc_shape_convex_mesh& p_convex_mesh) noexcept
			{
				m_convex_mesh = physx::PxConvexMeshGeometry
				(
					static_cast< physx::PxConvexMesh* >(static_cast< bc_physics_reference& >
					(
						const_cast< bc_convex_mesh& >(p_convex_mesh.m_convex)
					).get_platform_pack().m_px_object),
					physx::PxMeshScale(p_convex_mesh.m_scale.m_scale)
				);
			}

			void from_triangle_mesh(const bc_shape_triangle_mesh& p_triangle_mesh) noexcept
			{
				m_triangle_mesh = physx::PxTriangleMeshGeometry
				(
					static_cast< physx::PxTriangleMesh* >(static_cast< bc_physics_reference& >
					(
						const_cast< bc_triangle_mesh& >(p_triangle_mesh.m_mesh)
					).get_platform_pack().m_px_object),
					physx::PxMeshScale(p_triangle_mesh.m_scale.m_scale)
				);
			}

			void from_height_field(const bc_shape_height_field& p_height_field) noexcept
			{
				m_height_field = physx::PxHeightFieldGeometry
				(
					static_cast< physx::PxHeightField* >(static_cast< bc_physics_reference& >
					(
						const_cast< bc_height_field& >(p_height_field.m_height_field)
					).get_platform_pack().m_px_object),
					physx::PxMeshGeometryFlags(),
					p_height_field.m_y_scale,
					p_height_field.m_xz_scale,
					p_height_field.m_xz_scale
				);
			}

			physx::PxGeometry& get() const noexcept
			{
				return *m_geometry;
			}

		protected:

		private:
			physx::PxGeometry* m_geometry;
			union
			{
				physx::PxSphereGeometry m_sphere;
				physx::PxBoxGeometry m_box;
				physx::PxCapsuleGeometry m_capsule;
				physx::PxPlaneGeometry m_plane;
				physx::PxConvexMeshGeometry m_convex_mesh;
				physx::PxTriangleMeshGeometry m_triangle_mesh;
				physx::PxHeightFieldGeometry m_height_field;
			};
		};*/
	}
}