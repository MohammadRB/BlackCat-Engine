// [12/13/2016 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "Core/Memory/bcAlloc.h"
#include "PhysicsImp/bcExport.h"
#include "PhysicsImp/bcUtility.h"
#include "PhysicsImp/Shape/bcShape.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape< g_api_physx >::bc_platform_shape()
			: bc_platform_physics_reference()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape< g_api_physx >::bc_platform_shape(const bc_platform_shape& p_other) noexcept
			: bc_platform_physics_reference(p_other)
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape< g_api_physx >::~bc_platform_shape()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_shape< g_api_physx >& bc_platform_shape< g_api_physx >::operator=(const bc_platform_shape& p_other) noexcept
		{
			bc_platform_physics_reference::operator=(p_other);
			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_shape_type bc_platform_shape< g_api_physx >::get_type() const noexcept
		{
			auto* l_px_shape = static_cast< physx::PxShape* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_shape& >(*this)).get_platform_pack().m_px_object
			);

			return static_cast< bc_shape_type >(l_px_shape->getGeometryType());
		}

		template<>
		BC_PHYSICSIMP_DLL
		bool bc_platform_shape< g_api_physx >::as_sphere(bc_shape_sphere& p_sphere) const noexcept
		{
			auto* l_px_shape = static_cast< physx::PxShape* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_shape& >(*this)).get_platform_pack().m_px_object
			);

			physx::PxSphereGeometry l_geometry;
			bool l_result = l_px_shape->getSphereGeometry(l_geometry);

			if (l_result)
			{
				p_sphere = bc_shape_sphere(l_geometry.radius);
			}

			return l_result;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bool bc_platform_shape< g_api_physx >::as_box(bc_shape_box& p_box) const noexcept
		{
			auto* l_px_shape = static_cast< physx::PxShape* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_shape& >(*this)).get_platform_pack().m_px_object
			);

			physx::PxBoxGeometry l_geometry;
			bool l_result = l_px_shape->getBoxGeometry(l_geometry);

			if (l_result)
			{
				p_box = bc_shape_box(l_geometry.halfExtents.x, l_geometry.halfExtents.y, l_geometry.halfExtents.z);
			}

			return l_result;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bool bc_platform_shape< g_api_physx >::as_capsule(bc_shape_capsule& p_capsule) const noexcept
		{
			auto* l_px_shape = static_cast< physx::PxShape* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_shape& >(*this)).get_platform_pack().m_px_object
			);

			physx::PxCapsuleGeometry l_geometry;
			bool l_result = l_px_shape->getCapsuleGeometry(l_geometry);

			if (l_result)
			{
				p_capsule = bc_shape_capsule(l_geometry.halfHeight, l_geometry.radius);
			}

			return l_result;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bool bc_platform_shape< g_api_physx >::as_plane(bc_shape_plane& p_plane) const noexcept
		{
			auto* l_px_shape = static_cast< physx::PxShape* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_shape& >(*this)).get_platform_pack().m_px_object
			);

			physx::PxPlaneGeometry l_geometry;
			bool l_result = l_px_shape->getPlaneGeometry(l_geometry);

			if (l_result)
			{
				// TODO
			}

			return l_result;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bool bc_platform_shape< g_api_physx >::as_convex_mesh(bc_shape_convex_mesh& p_convex_mesh) const noexcept
		{
			auto* l_px_shape = static_cast< physx::PxShape* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_shape& >(*this)).get_platform_pack().m_px_object
			);

			physx::PxConvexMeshGeometry l_geometry;
			bool l_result = l_px_shape->getConvexMeshGeometry(l_geometry);

			if (l_result)
			{
				bc_convex_mesh l_convex_mesh;
				static_cast< bc_physics_reference& >(l_convex_mesh).get_platform_pack().m_px_object = l_geometry.convexMesh;

				p_convex_mesh = bc_shape_convex_mesh(bc_geometry_scale(l_geometry.scale.scale.x), l_convex_mesh);
			}

			return l_result;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bool bc_platform_shape< g_api_physx >::as_triangle_mesh(bc_shape_triangle_mesh& p_triangle_mesh) const noexcept
		{
			auto* l_px_shape = static_cast< physx::PxShape* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_shape& >(*this)).get_platform_pack().m_px_object
			);

			physx::PxTriangleMeshGeometry l_geometry;
			bool l_result = l_px_shape->getTriangleMeshGeometry(l_geometry);

			if (l_result)
			{
				bc_triangle_mesh l_triangle_mesh;
				static_cast< bc_physics_reference& >(l_triangle_mesh).get_platform_pack().m_px_object = l_geometry.triangleMesh;

				p_triangle_mesh = bc_shape_triangle_mesh(bc_geometry_scale(l_geometry.scale.scale.x), l_triangle_mesh);
			}

			return l_result;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bool bc_platform_shape< g_api_physx >::as_height_field(bc_shape_height_field& p_height_field) const noexcept
		{
			auto* l_px_shape = static_cast< physx::PxShape* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_shape& >(*this)).get_platform_pack().m_px_object
			);

			physx::PxHeightFieldGeometry l_geometry;
			bool l_result = l_px_shape->getHeightFieldGeometry(l_geometry);

			if (l_result)
			{
				bc_height_field l_height_field;
				static_cast< bc_physics_reference& >(l_height_field).get_platform_pack().m_px_object = l_geometry.heightField;

				p_height_field = bc_shape_height_field(l_height_field, l_geometry.rowScale, l_geometry.heightScale);
			}

			return l_result;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_transform bc_platform_shape< g_api_physx >::get_local_pose() const noexcept
		{
			auto* l_px_shape = static_cast< physx::PxShape* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_shape& >(*this)).get_platform_pack().m_px_object
			);
			bc_transform l_transform;
			l_transform.get_platform_pack().m_px_transform = l_px_shape->getLocalPose();

			return l_transform;
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_shape< g_api_physx >::set_local_pose(const bc_transform& p_pose) noexcept
		{
			auto* l_px_shape = static_cast< physx::PxShape* >
			(
				static_cast< bc_platform_physics_reference& >(*this).get_platform_pack().m_px_object
			);

			l_px_shape->setLocalPose(const_cast< bc_transform& >(p_pose).get_platform_pack().m_px_transform);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT32 bc_platform_shape< g_api_physx >::get_material_count() const noexcept
		{
			auto* l_px_shape = static_cast< physx::PxShape* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_shape& >(*this)).get_platform_pack().m_px_object
			);

			return l_px_shape->getNbMaterials();
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT32 bc_platform_shape< g_api_physx >::get_materials(bc_material* p_buffer, bcUINT32 p_buffer_size) const noexcept
		{
			auto* l_px_shape = static_cast< physx::PxShape* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_shape& >(*this)).get_platform_pack().m_px_object
			);

			bcUINT32 l_written_count = l_px_shape->getMaterials(reinterpret_cast< physx::PxMaterial** >(p_buffer), p_buffer_size);

			bc_overwrite_output_array< bc_material, physx::PxMaterial >(p_buffer, l_written_count, [](bc_material* p_material, physx::PxMaterial* p_px_material)
				{
					static_cast< bc_platform_physics_reference* >(p_material)->get_platform_pack().m_px_object = p_px_material;
				});

			return l_written_count;
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_shape< g_api_physx >::set_materials(const bc_material* p_materials, bcUINT32 p_count)
		{
			auto* l_px_shape = static_cast< physx::PxShape* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_shape& >(*this)).get_platform_pack().m_px_object
			);

			auto* l_buffer = static_cast< physx::PxMaterial** >
			(
				bcAlloc(sizeof(physx::PxMaterial*) * p_count, core::bc_alloc_type::frame)
			);

			for (bcUINT32 i = 0; i < p_count; ++i)
			{
				l_buffer[i] = static_cast< physx::PxMaterial* >
				(
					static_cast< bc_platform_physics_reference& >
					(
						const_cast< bc_material& >(p_materials[i])
					).get_platform_pack().m_px_object
				);
			}

			l_px_shape->setMaterials(l_buffer, p_count);

			bcFree(l_buffer);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_material bc_platform_shape< g_api_physx >::get_material_from_face(bcUINT32 p_face_index) const
		{
			auto* l_px_shape = static_cast< physx::PxShape* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_shape& >(*this)).get_platform_pack().m_px_object
			);

			bc_material l_material;
			static_cast< bc_physics_reference& >(l_material).get_platform_pack().m_px_object =
				l_px_shape->getMaterialFromInternalFaceIndex(p_face_index);

			return l_material;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcFLOAT bc_platform_shape< g_api_physx >::get_contact_offset() const noexcept
		{
			auto* l_px_shape = static_cast< physx::PxShape* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_shape& >(*this)).get_platform_pack().m_px_object
			);

			return l_px_shape->getContactOffset();
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_shape< g_api_physx >::set_contact_offset(bcFLOAT p_offset) noexcept
		{
			auto* l_px_shape = static_cast< physx::PxShape* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_shape& >(*this)).get_platform_pack().m_px_object
			);

			l_px_shape->setContactOffset(p_offset);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcFLOAT bc_platform_shape< g_api_physx >::get_rest_offset() const noexcept
		{
			auto* l_px_shape = static_cast< physx::PxShape* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_shape& >(*this)).get_platform_pack().m_px_object
			);

			return l_px_shape->getRestOffset();
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_shape< g_api_physx >::set_rest_offset(bcFLOAT p_offset) noexcept
		{
			auto* l_px_shape = static_cast< physx::PxShape* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_shape& >(*this)).get_platform_pack().m_px_object
			);

			l_px_shape->setRestOffset(p_offset);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_shape_flag bc_platform_shape< g_api_physx >::get_flags() const noexcept
		{
			auto* l_px_shape = static_cast< physx::PxShape* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_shape& >(*this)).get_platform_pack().m_px_object
			);

			return static_cast< bc_shape_flag >(static_cast< physx::PxShapeFlags::InternalType >(l_px_shape->getFlags()));
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_shape< g_api_physx >::set_flag(bc_shape_flag p_flag, bool p_value) noexcept
		{
			auto* l_px_shape = static_cast< physx::PxShape* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_shape& >(*this)).get_platform_pack().m_px_object
			);

			l_px_shape->setFlag(static_cast< physx::PxShapeFlag::Enum >(p_flag), p_value);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_collision_filter bc_platform_shape< g_api_physx >::get_collision_group() const noexcept
		{
			auto* l_px_shape = static_cast< physx::PxShape* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_shape& >(*this)).get_platform_pack().m_px_object
			);
			auto l_px_filter_data = l_px_shape->getSimulationFilterData();

			return bc_collision_filter
			(
				static_cast< bc_collision_group >(l_px_filter_data.word0),
				static_cast< bc_collision_group >(l_px_filter_data.word1)
			);
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_shape< g_api_physx >::set_collision_group(bc_collision_filter p_filter) noexcept
		{
			auto* l_px_shape = static_cast< physx::PxShape* >
			(
				static_cast< bc_platform_physics_reference& >(*this).get_platform_pack().m_px_object
			);
			auto l_px_filter_data = l_px_shape->getSimulationFilterData();

			l_px_shape->setSimulationFilterData(physx::PxFilterData
			(
				static_cast< physx::PxU32 >(p_filter.m_group),
				static_cast< physx::PxU32 >(p_filter.m_mask),
				l_px_filter_data.word2,
				l_px_filter_data.word3
			));
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_shape_notify_flag bc_platform_shape< g_api_physx >::get_notify_flags() const noexcept
		{
			auto* l_px_shape = static_cast< physx::PxShape* >
				(
					static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_shape& >(*this)).get_platform_pack().m_px_object
					);
			auto l_px_filter_data = l_px_shape->getSimulationFilterData();

			return static_cast< bc_shape_notify_flag >(l_px_filter_data.word2);
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_shape< g_api_physx >::set_notify_flag(bc_shape_notify_flag p_flag, bool p_value) noexcept
		{
			auto* l_px_shape = static_cast< physx::PxShape* >
			(
				static_cast< bc_platform_physics_reference& >(*this).get_platform_pack().m_px_object
			);
			auto l_px_filter_data = l_px_shape->getSimulationFilterData();

			l_px_shape->setSimulationFilterData(physx::PxFilterData
			(
				l_px_filter_data.word0,
				l_px_filter_data.word1,
				static_cast< physx::PxU32 >(core::bc_enum::set(static_cast< bc_shape_notify_flag >(l_px_filter_data.word2), p_flag, p_value)),
				l_px_filter_data.word3
			));
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_query_group bc_platform_shape< g_api_physx >::get_query_group() const noexcept
		{
			auto* l_px_shape = static_cast< physx::PxShape* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_shape& >(*this)).get_platform_pack().m_px_object
			);
			auto l_px_filter_data = l_px_shape->getQueryFilterData();

			return static_cast< bc_query_group >(l_px_filter_data.word0);
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_shape< g_api_physx >::set_query_group(bc_query_group p_filter) noexcept
		{
			auto* l_px_shape = static_cast< physx::PxShape* >
			(
				static_cast< bc_platform_physics_reference& >(*this).get_platform_pack().m_px_object
			);
			auto l_px_filter_data = l_px_shape->getQueryFilterData();

			l_px_shape->setQueryFilterData(physx::PxFilterData
			(
				static_cast< physx::PxU32 >(p_filter),
				l_px_filter_data.word1,
				l_px_filter_data.word2,
				l_px_filter_data.word3
			));
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_shape_query_flag bc_platform_shape< g_api_physx >::get_query_flags() const noexcept
		{
			auto* l_px_shape = static_cast< physx::PxShape* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_shape& >(*this)).get_platform_pack().m_px_object
			);
			auto l_px_filter_data = l_px_shape->getQueryFilterData();

			return static_cast< bc_shape_query_flag >(l_px_filter_data.word1);
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_shape<g_api_physx>::set_query_flag(bc_shape_query_flag p_flag, bool p_value) noexcept
		{
			auto* l_px_shape = static_cast< physx::PxShape* >
			(
				static_cast< bc_platform_physics_reference& >(*this).get_platform_pack().m_px_object
			);
			auto l_px_filter_data = l_px_shape->getQueryFilterData();

			l_px_shape->setQueryFilterData(physx::PxFilterData
			(
				l_px_filter_data.word0,
				static_cast< physx::PxU32 >(core::bc_enum::set(static_cast< bc_shape_query_flag >(l_px_filter_data.word1), p_flag, p_value)),
				l_px_filter_data.word2,
				l_px_filter_data.word3
			));
		}

		template<>
		BC_PHYSICSIMP_DLL
		bool bc_platform_shape< g_api_physx >::is_exclusive() const noexcept
		{
			auto* l_px_shape = static_cast< physx::PxShape* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_shape& >(*this)).get_platform_pack().m_px_object
			);

			return l_px_shape->isExclusive();
		}

		template<>
		BC_PHYSICSIMP_DLL
		bool bc_platform_shape< g_api_physx >::is_valid() const noexcept
		{
			return static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_shape& >(*this))
				.get_platform_pack().m_px_object != nullptr;
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_shape< g_api_physx >::set_data(void* user_data) noexcept
		{
			auto* l_px_shape = static_cast< physx::PxShape* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_shape& >(*this)).get_platform_pack().m_px_object
			);

			l_px_shape->userData = user_data;
		}

		template<>
		BC_PHYSICSIMP_DLL
		void* bc_platform_shape< g_api_physx >::get_data() const noexcept
		{
			auto* l_px_shape = static_cast< physx::PxShape* >
			(
				static_cast< bc_platform_physics_reference& >(const_cast< bc_platform_shape& >(*this)).get_platform_pack().m_px_object
			);

			return l_px_shape->userData;
		}
	}
}