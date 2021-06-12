// [12/17/2016 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "Core/Memory/bcAlloc.h"
#include "Core/Utility/bcLogger.h"
#include "Physics/bcException.h"
#include "PhysicsImp/bcExport.h"
#include "PhysicsImp/bcUtility.h"
#include "PhysicsImp/Fundation/bcPhysics.h"
#include "PhysicsImp/Fundation/bcSceneBuilder.h"
#include "PhysicsImp/Fundation/bcScene.h"
#include "PhysicsImp/Fundation/bcTransform.h"
#include "PhysicsImp/Fundation/bcMemoryBuffer.h"
#include "PhysicsImp/Fundation/bcSerializeBuffer.h"
#include "PhysicsImp/Fundation/bcMaterial.h"
#include "PhysicsImp/Shape/bcShapeGeometry.h"
#include "PhysicsImp/Shape/bcTriangleMesh.h"
#include "PhysicsImp/Shape/bcConvexMesh.h"
#include "PhysicsImp/Shape/bcHeightField.h"
#include "PhysicsImp/Shape/bcShape.h"
#include "PhysicsImp/Body/bcActor.h"
#include "PhysicsImp/Body/bcRigidActor.h"
#include "PhysicsImp/Body/bcRigidStatic.h"
#include "PhysicsImp/Body/bcRigidDynamic.h"
#include "PhysicsImp/Body/bcAggregate.h"
#include "PhysicsImp/Joint/bcJoint.h"
#include "PhysicsImp/Joint/bcD6Joint.h"
#include "PhysicsImp/Joint/bcDistanceJoint.h"
#include "PhysicsImp/Joint/bcFixedJoint.h"
#include "PhysicsImp/Joint/bcPrismaticJoint.h"
#include "PhysicsImp/Joint/bcRevoluteJoint.h"
#include "PhysicsImp/Joint/bcSphericalJoint.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_physics< g_api_physx >::bc_platform_physics()
		{
			m_pack.m_px_physics = nullptr;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_physics< g_api_physx >::bc_platform_physics(bc_platform_physics&& p_other) noexcept
		{
			operator=(std::move(p_other));
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_physics< g_api_physx >::~bc_platform_physics()
		{
			if (is_initialized())
			{
				destroy();
			}
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_physics< g_api_physx >& bc_platform_physics< g_api_physx >::operator=(bc_platform_physics&& p_other) noexcept
		{
			m_pack.m_px_foundation = p_other.m_pack.m_px_foundation;
#ifdef BC_DEBUG
			m_pack.m_px_pvd = p_other.m_pack.m_px_pvd;
#endif
			m_pack.m_px_physics = p_other.m_pack.m_px_physics;
			m_pack.m_px_cooking = p_other.m_pack.m_px_cooking;
			m_pack.m_allocator = std::move(p_other.m_pack.m_allocator);
			m_pack.m_logger = std::move(p_other.m_pack.m_logger);

			p_other.m_pack.m_px_foundation = nullptr;
#ifdef BC_DEBUG
			p_other.m_pack.m_px_pvd = nullptr;
#endif
			p_other.m_pack.m_px_physics = nullptr;
			p_other.m_pack.m_px_cooking = nullptr;

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_scene_ref bc_platform_physics< g_api_physx >::create_scene(bc_scene_builder p_desc)
		{
			p_desc.get_platform_pack().m_px_desc.cpuDispatcher = m_pack.m_task_dispatcher.get();

			_bc_px_scene_pack_data l_scene_pack_data;
			l_scene_pack_data.m_px_scene = m_pack.m_px_physics->createScene(p_desc.get_platform_pack().m_px_desc);
			l_scene_pack_data.m_controller_manager = PxCreateControllerManager(*l_scene_pack_data.m_px_scene);
			l_scene_pack_data.m_allocator = m_pack.m_allocator->m_imp.get();
			l_scene_pack_data.m_task_dispatcher = m_pack.m_task_dispatcher->m_imp.get();
			l_scene_pack_data.m_simulation_callback = std::move(p_desc.get_platform_pack().m_simulation_callback);
			l_scene_pack_data.m_contact_filter_callback = std::move(p_desc.get_platform_pack().m_contact_filter_callback);
			l_scene_pack_data.m_contact_modify_callback = std::move(p_desc.get_platform_pack().m_contact_modify_callback);
			l_scene_pack_data.m_filter_shader_data = std::move(p_desc.get_platform_pack().m_filter_shader_data);

			//l_scene_pack_data.m_controller_manager->setOverlapRecoveryModule(true);
			//l_scene_pack_data.m_controller_manager->setPreciseSweeps(false);
			
			bc_scene::platform_pack l_pack;
			l_pack.m_data = core::bc_make_shared<_bc_px_scene_pack_data>(std::move(l_scene_pack_data));

			return bc_scene_ref(bc_scene(l_pack));
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT32 bc_platform_physics< g_api_physx >::get_scene_count() const noexcept
		{
			return m_pack.m_px_physics->getNbScenes();
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_rigid_static_ref bc_platform_physics< g_api_physx >::create_rigid_static(const bc_transform& p_pose)
		{
			bc_rigid_static::platform_pack l_pack;
			l_pack.m_px_object = m_pack.m_px_physics->createRigidStatic(p_pose.get_platform_pack().m_px_transform);

			return bc_rigid_static_ref(bc_rigid_static(l_pack));
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_rigid_dynamic_ref bc_platform_physics< g_api_physx >::create_rigid_dynamic(const bc_transform& p_pose)
		{
			bc_rigid_dynamic::platform_pack l_pack;
			l_pack.m_px_object = m_pack.m_px_physics->createRigidDynamic(p_pose.get_platform_pack().m_px_transform);
						
			return bc_rigid_dynamic_ref(bc_rigid_dynamic(l_pack));
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_shape_ref bc_platform_physics< g_api_physx >::create_shape(const bc_shape_geometry& p_geometry,
			const bc_material& p_material,
			bool p_is_exclusive)
		{
			return create_shape
			(
				p_geometry,
				p_material,
				bc_shape_flag::default_v,
				p_is_exclusive
			);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_shape_ref bc_platform_physics< g_api_physx >::create_shape(const bc_shape_geometry& p_geometry,
			const bc_material& p_material,
			bc_shape_flag p_shape_flags,
			bool p_is_exclusive)
		{
			bc_shape::platform_pack l_pack;
			l_pack.m_px_object = m_pack.m_px_physics->createShape
			(
				*p_geometry.get_platform_pack().m_px_geometry,
				*static_cast<physx::PxMaterial*>(p_material.get_platform_pack().m_px_object),
				p_is_exclusive,
				static_cast<physx::PxShapeFlag::Enum>(p_shape_flags)
			);

			return bc_shape_ref(bc_shape(l_pack));
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_shape_ref bc_platform_physics< g_api_physx >::create_shape(const bc_shape_geometry& p_geometry,
			bc_material* const* p_materials,
			bcUINT16 p_material_count,
			bool p_is_exclusive)
		{
			return create_shape
			(
				p_geometry,
				p_materials,
				p_material_count,
				bc_shape_flag::default_v,
				p_is_exclusive
			);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_shape_ref bc_platform_physics< g_api_physx >::create_shape(const bc_shape_geometry& p_geometry,
			bc_material* const* p_materials,
			bcUINT16 p_material_count,
			bc_shape_flag p_shape_flags,
			bool p_is_exclusive)
		{
			auto** l_px_material_buffer = static_cast< physx::PxMaterial** >(BC_ALLOC(sizeof(physx::PxMaterial*) * p_material_count, core::bc_alloc_type::frame));

			for (bcUINT32 i = 0; i < p_material_count; ++i)
			{
				l_px_material_buffer[i] = static_cast<physx::PxMaterial*>(p_materials[i]->get_platform_pack().m_px_object);
			}

			bc_shape::platform_pack l_shape_pack;
			l_shape_pack.m_px_object = m_pack.m_px_physics->createShape
			(
				*p_geometry.get_platform_pack().m_px_geometry,
				l_px_material_buffer,
				p_material_count,
				p_is_exclusive,
				static_cast<physx::PxShapeFlag::Enum>(p_shape_flags)
			);

			BC_FREE(l_px_material_buffer);

			return bc_shape_ref(bc_shape(l_shape_pack));
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT32 bc_platform_physics< g_api_physx >::get_shape_count() const noexcept
		{
			return m_pack.m_px_physics->getNbShapes();
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_material_ref bc_platform_physics< g_api_physx >::create_material(bcFLOAT p_static_friction,
			bcFLOAT p_dynamic_friction,
			bcFLOAT p_restitution)
		{
			bc_material::platform_pack l_pack;
			l_pack.m_px_object = m_pack.m_px_physics->createMaterial(p_static_friction, p_dynamic_friction, p_restitution);

			return bc_material_ref(bc_material(l_pack));
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT32 bc_platform_physics< g_api_physx >::get_material_count() const noexcept
		{
			return m_pack.m_px_physics->getNbMaterials();
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_aggregate_ref bc_platform_physics< g_api_physx >::create_aggregate(bcUINT32 p_max_size, bool p_enable_self_collision)
		{
			bc_aggregate::platform_pack l_pack;
			l_pack.m_px_object = m_pack.m_px_physics->createAggregate(p_max_size, p_enable_self_collision);

			return bc_aggregate_ref(bc_aggregate(l_pack));
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_memory_buffer bc_platform_physics< g_api_physx >::create_convex_mesh(const bc_convex_mesh_desc& p_desc)
		{
			core::bc_unique_ptr< physx::PxVec3 > l_vertex_buffer(static_cast< physx::PxVec3* >
			(
				BC_ALLOC(sizeof(physx::PxVec3) * p_desc.m_points.m_count, core::bc_alloc_type::frame)
			));

			physx::PxConvexMeshDesc l_px_desc = bc_convert_to_px_convex_mesh(p_desc, l_vertex_buffer.get());
			l_px_desc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;

			bc_memory_buffer l_buffer;
			physx::PxConvexMeshCookingResult::Enum l_px_result_flag;
			
			if (m_pack.m_px_cooking->cookConvexMesh(l_px_desc, *l_buffer.get_platform_pack().m_px_stream, &l_px_result_flag))
			{
				l_buffer.get_platform_pack().m_is_valid = true;
				return l_buffer;
			}

			// If failed try with eINFLATE_CONVEX flag
			l_px_desc.flags |= physx::PxConvexFlag::eINFLATE_CONVEX;

			if (m_pack.m_px_cooking->cookConvexMesh(l_px_desc, *l_buffer.get_platform_pack().m_px_stream, &l_px_result_flag))
			{
				l_buffer.get_platform_pack().m_is_valid = true;
			}

			return l_buffer;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_convex_mesh_ref bc_platform_physics< g_api_physx >::create_convex_mesh(bc_memory_buffer& p_buffer)
		{
			BC_ASSERT(p_buffer.is_valid());

			bc_convex_mesh::platform_pack l_pack;

			physx::PxDefaultMemoryInputData input
			(
				static_cast< physx::PxU8* >(p_buffer.get_buffer_pointer()),
				p_buffer.get_buffer_size()
			);
			l_pack.m_px_object = m_pack.m_px_physics->createConvexMesh(input);

			return bc_convex_mesh_ref(bc_convex_mesh(l_pack));
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT32 bc_platform_physics< g_api_physx >::get_convex_mesh_count() const noexcept
		{
			return m_pack.m_px_physics->getNbConvexMeshes();
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_memory_buffer bc_platform_physics< g_api_physx >::create_triangle_mesh(const bc_triangle_mesh_desc& p_desc)
		{
			const bool l_16_bit_index = core::bc_enum::has(p_desc.m_flag, bc_triangle_mesh_flag::use_16bit_index);
			const core::bc_unique_ptr< physx::PxVec3 > l_vertex_buffer
			(
				static_cast< physx::PxVec3* >
				(
					BC_ALLOC(sizeof(physx::PxVec3) * p_desc.m_points.m_count, core::bc_alloc_type::frame)
				)
			);
			const core::bc_unique_ptr< bcBYTE > l_index_buffer
			(
				static_cast< bcBYTE* >
				(
					BC_ALLOC(p_desc.m_indices.m_count * (l_16_bit_index ? sizeof(bcUINT16) : sizeof(bcUINT32)), core::bc_alloc_type::frame)
				)
			);

			const physx::PxTriangleMeshDesc l_px_desc = bc_convert_to_px_triangle_mesh(p_desc, l_vertex_buffer.get(), l_index_buffer.get());
			bc_memory_buffer l_buffer;
			
			if (m_pack.m_px_cooking->cookTriangleMesh(l_px_desc, *l_buffer.get_platform_pack().m_px_stream))
			{
				l_buffer.get_platform_pack().m_is_valid = true;
			}

			return l_buffer;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_triangle_mesh_ref bc_platform_physics< g_api_physx >::create_triangle_mesh(bc_memory_buffer& p_buffer)
		{
			BC_ASSERT(p_buffer.is_valid());

			physx::PxDefaultMemoryInputData l_input
			(
				static_cast< physx::PxU8* >(p_buffer.get_buffer_pointer()),
				p_buffer.get_buffer_size()
			);
			bc_triangle_mesh::platform_pack l_pack;
			l_pack.m_px_object = m_pack.m_px_physics->createTriangleMesh(l_input);

			return bc_triangle_mesh_ref(bc_triangle_mesh(l_pack));
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_triangle_mesh_ref bc_platform_physics< g_api_physx >::create_runtime_triangle_mesh(const bc_triangle_mesh_desc& p_desc)
		{
			const physx::PxTolerancesScale l_scale;
			physx::PxCookingParams l_params(l_scale);
			// disable mesh cleaning - perform mesh validation on development configurations
			l_params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
			// disable edge precompute, edges are set for each triangle, slows contact generation
			l_params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
			// lower hierarchy for internal mesh
			l_params.meshCookingHint = physx::PxMeshCookingHint::eCOOKING_PERFORMANCE;

			m_pack.m_px_cooking->setParams(l_params);

			const bool l_is_16_bit_index = core::bc_enum::has(p_desc.m_flag, bc_triangle_mesh_flag::use_16bit_index);
			const core::bc_unique_ptr< physx::PxVec3 > l_vertex_buffer
			(
				static_cast<physx::PxVec3*>
				(
					BC_ALLOC(sizeof(physx::PxVec3) * p_desc.m_points.m_count, core::bc_alloc_type::frame)
				)
			);
			const core::bc_unique_ptr< bcBYTE > l_index_buffer
			(
				static_cast<bcBYTE*>
				(
					BC_ALLOC(p_desc.m_indices.m_count * (l_is_16_bit_index ? sizeof(bcUINT16) : sizeof(bcUINT32)), core::bc_alloc_type::frame)
				)
			);

			const physx::PxTriangleMeshDesc l_px_desc = bc_convert_to_px_triangle_mesh(p_desc, l_vertex_buffer.get(), l_index_buffer.get());

#ifdef BC_DEBUG
			// mesh should be validated before cooked without the mesh cleaning
			const bool l_is_mesh_valid = m_pack.m_px_cooking->validateTriangleMesh(l_px_desc);
			if(!l_is_mesh_valid)
			{
				core::bc_log(core::bc_log_type::debug, bcL("Triangle mesh is not valid for cooking"));
			}
#endif

			physx::PxTriangleMesh* l_px_triangle_mesh = m_pack.m_px_cooking->createTriangleMesh(l_px_desc, m_pack.m_px_physics->getPhysicsInsertionCallback());

			bc_triangle_mesh::platform_pack l_triangle_pack;
			l_triangle_pack.m_px_object = l_px_triangle_mesh;

			return bc_triangle_mesh_ref(bc_triangle_mesh(l_triangle_pack));
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT32 bc_platform_physics< g_api_physx >::get_triangle_mesh_count() const noexcept
		{
			return m_pack.m_px_physics->getNbTriangleMeshes();
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_memory_buffer bc_platform_physics< g_api_physx >::create_height_field(const bc_height_field_desc& p_desc)
		{
			const core::bc_unique_ptr< physx::PxHeightFieldSample > l_sample_buffer(static_cast< physx::PxHeightFieldSample* >
			(
				BC_ALLOC(sizeof(physx::PxHeightFieldSample) * (p_desc.m_num_row * p_desc.m_num_column), core::bc_alloc_type::frame)
			));

			const physx::PxHeightFieldDesc l_px_desc = bc_convert_to_px_height_field(p_desc, l_sample_buffer.get());
			bc_memory_buffer l_buffer;
			
			if (m_pack.m_px_cooking->cookHeightField(l_px_desc, *l_buffer.get_platform_pack().m_px_stream))
			{
				l_buffer.get_platform_pack().m_is_valid = true;
			}

			return l_buffer;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_height_field_ref bc_platform_physics< g_api_physx >::create_height_field(bc_memory_buffer& p_buffer)
		{
			BC_ASSERT(p_buffer.is_valid());

			physx::PxDefaultMemoryInputData input
			(
				static_cast< physx::PxU8* >(p_buffer.get_buffer_pointer()),
				p_buffer.get_buffer_size()
			);
			bc_height_field::platform_pack l_pack;
			l_pack.m_px_object = m_pack.m_px_physics->createHeightField(input);

			return bc_height_field_ref(bc_height_field(l_pack));
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT32 bc_platform_physics< g_api_physx >::get_height_field_count() const noexcept
		{
			return m_pack.m_px_physics->getNbHeightFields();
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_fixed_joint_ref bc_platform_physics< g_api_physx >::create_fixed_joint(bc_rigid_actor* p_actor0,
			const bc_transform& p_local_frame0,
			bc_rigid_actor* p_actor1,
			const bc_transform& p_local_frame1)
		{
			bc_fixed_joint::platform_pack l_pack;
			l_pack.m_px_object = physx::PxFixedJointCreate
			(
				*m_pack.m_px_physics,
				static_cast< physx::PxRigidActor* >(p_actor0->get_platform_pack().m_px_object),
				p_local_frame0.get_platform_pack().m_px_transform,
				static_cast< physx::PxRigidActor* >(p_actor1->get_platform_pack().m_px_object),
				p_local_frame1.get_platform_pack().m_px_transform
			);

			return bc_fixed_joint_ref(bc_fixed_joint(l_pack));
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_distance_joint_ref bc_platform_physics< g_api_physx >::create_distance_joint(bc_rigid_actor* p_actor0,
			const bc_transform& p_local_frame0,
			bc_rigid_actor* p_actor1,
			const bc_transform& p_local_frame1)
		{
			bc_distance_joint::platform_pack l_pack;
			l_pack.m_px_object = physx::PxDistanceJointCreate
			(
				*m_pack.m_px_physics,
				static_cast< physx::PxRigidActor* >(p_actor0->get_platform_pack().m_px_object),
				p_local_frame0.get_platform_pack().m_px_transform,
				static_cast< physx::PxRigidActor* >(p_actor1->get_platform_pack().m_px_object),
				p_local_frame1.get_platform_pack().m_px_transform
			);

			return bc_distance_joint_ref(bc_distance_joint(l_pack));
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_prismatic_joint_ref bc_platform_physics< g_api_physx >::create_prismatic_joint(bc_rigid_actor* p_actor0,
			const bc_transform& p_local_frame0,
			bc_rigid_actor* p_actor1,
			const bc_transform& p_local_frame1)
		{
			bc_prismatic_joint::platform_pack l_pack;

			l_pack.m_px_object = physx::PxPrismaticJointCreate
			(
				*m_pack.m_px_physics,
				static_cast<physx::PxRigidActor*>(p_actor0->get_platform_pack().m_px_object),
				p_local_frame0.get_platform_pack().m_px_transform,
				static_cast<physx::PxRigidActor*>(p_actor1->get_platform_pack().m_px_object),
				p_local_frame1.get_platform_pack().m_px_transform
			);

			return bc_prismatic_joint_ref(bc_prismatic_joint(l_pack));
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_revolute_joint_ref bc_platform_physics< g_api_physx >::create_revolute_joint(bc_rigid_actor* p_actor0,
			const bc_transform& p_local_frame0,
			bc_rigid_actor* p_actor1,
			const bc_transform& p_local_frame1)
		{
			bc_revolute_joint::platform_pack l_pack;

			l_pack.m_px_object = physx::PxRevoluteJointCreate
			(
				*m_pack.m_px_physics,
				static_cast<physx::PxRigidActor*>(p_actor0->get_platform_pack().m_px_object),
				p_local_frame0.get_platform_pack().m_px_transform,
				static_cast<physx::PxRigidActor*>(p_actor1->get_platform_pack().m_px_object),
				p_local_frame1.get_platform_pack().m_px_transform
			);

			return bc_revolute_joint_ref(bc_revolute_joint(l_pack));
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_spherical_joint_ref bc_platform_physics< g_api_physx >::create_spherical_joint(bc_rigid_actor* p_actor0,
			const bc_transform& p_local_frame0,
			bc_rigid_actor* p_actor1,
			const bc_transform& p_local_frame1)
		{
			bc_spherical_joint::platform_pack l_pack;
			l_pack.m_px_object = physx::PxSphericalJointCreate
			(
				*m_pack.m_px_physics,
				static_cast<physx::PxRigidActor*>(p_actor0->get_platform_pack().m_px_object),
				p_local_frame0.get_platform_pack().m_px_transform,
				static_cast<physx::PxRigidActor*>(p_actor1->get_platform_pack().m_px_object),
				p_local_frame1.get_platform_pack().m_px_transform
			);

			return bc_spherical_joint_ref(bc_spherical_joint(l_pack));
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_d6_joint_ref bc_platform_physics< g_api_physx >::create_d6_joint(bc_rigid_actor* p_actor0,
			const bc_transform& p_local_frame0,
			bc_rigid_actor* p_actor1,
			const bc_transform& p_local_frame1)
		{
			bc_d6_joint::platform_pack l_pack;
			l_pack.m_px_object = physx::PxD6JointCreate
			(
				*m_pack.m_px_physics,
				static_cast<physx::PxRigidActor*>(p_actor0->get_platform_pack().m_px_object),
				p_local_frame0.get_platform_pack().m_px_transform,
				static_cast<physx::PxRigidActor*>(p_actor1->get_platform_pack().m_px_object),
				p_local_frame1.get_platform_pack().m_px_transform
			);

			return bc_d6_joint_ref(bc_d6_joint(l_pack));
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_memory_buffer bc_platform_physics< g_api_physx >::read_to_memory_buffer(void* p_data, bcUINT32 p_size)
		{
			bc_memory_buffer l_buffer;
			l_buffer.get_platform_pack().m_px_stream->write(p_data, p_size);
			l_buffer.get_platform_pack().m_is_valid = true;

			return l_buffer;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_serialize_buffer bc_platform_physics< g_api_physx >::create_serialize_buffer()
		{
			bc_serialize_buffer::platform_pack l_pack;
			l_pack.m_registry = physx::PxSerialization::createSerializationRegistry(*m_pack.m_px_physics);
			l_pack.m_collection = PxCreateCollection();

			return bc_serialize_buffer(l_pack);
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_memory_buffer bc_platform_physics< g_api_physx >::serialize(bc_serialize_buffer& p_buffer)
		{
			auto l_output_stream = core::bc_make_unique<physx::PxDefaultMemoryOutputStream>();

			physx::PxSerialization::complete
			(
				*p_buffer.get_platform_pack().m_collection,
				*p_buffer.get_platform_pack().m_registry
			);
			physx::PxSerialization::serializeCollectionToBinary
			(
				*l_output_stream.get(), 
				*p_buffer.get_platform_pack().m_collection,
				*p_buffer.get_platform_pack().m_registry
			);

			bc_memory_buffer l_buffer;
			l_buffer.get_platform_pack().m_px_stream = std::move(l_output_stream);
			l_buffer.get_platform_pack().m_is_valid = true;

			return l_buffer;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_serialize_buffer bc_platform_physics< g_api_physx >::deserialize(bc_memory_buffer& p_buffer)
		{
			void* l_aligned_buffer = BC_ALIGNED_ALLOC_THROW(p_buffer.get_buffer_size(), 128, core::bc_alloc_type::unknown);
			
			std::memcpy(l_aligned_buffer, p_buffer.get_buffer_pointer(), p_buffer.get_buffer_size());

			bc_serialize_buffer::platform_pack l_buffer_pack;
			l_buffer_pack.m_registry = physx::PxSerialization::createSerializationRegistry(*m_pack.m_px_physics);
			l_buffer_pack.m_collection = physx::PxSerialization::createCollectionFromBinary(l_aligned_buffer, *l_buffer_pack.m_registry);
			l_buffer_pack.m_collection_deserialize_buffer = l_aligned_buffer;

			return bc_serialize_buffer(l_buffer_pack);;
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_physics< g_api_physx >::_initialize(core::bc_unique_ptr< bci_allocator > p_allocator,
			core::bc_unique_ptr< bci_task_dispatcher > p_task_dispatcher,
			core::bc_unique_ptr< bci_logger > p_logger)
		{
			const physx::PxTolerancesScale l_px_scale;

			m_pack.m_allocator = core::bc_make_unique< bc_px_allocator >(std::move(p_allocator));
			m_pack.m_task_dispatcher = core::bc_make_unique< bc_px_task_dispatcher >(std::move(p_task_dispatcher));
			m_pack.m_logger = core::bc_make_unique< bc_px_logger >(std::move(p_logger));

			m_pack.m_px_foundation = PxCreateFoundation(PX_FOUNDATION_VERSION, *m_pack.m_allocator, *m_pack.m_logger);

			if (!m_pack.m_px_foundation)
			{
				throw bc_physics_exception(0, "Failed to create PhysX foundation");
			}

#ifdef BC_DEBUG
			m_pack.m_px_pvd = physx::PxCreatePvd(*m_pack.m_px_foundation);
			m_pack.m_px_pvd_transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
			m_pack.m_px_pvd->connect(*m_pack.m_px_pvd_transport, physx::PxPvdInstrumentationFlag::eDEBUG);

			m_pack.m_px_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pack.m_px_foundation, l_px_scale, true, m_pack.m_px_pvd);
#else
			m_pack.m_px_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pack.m_px_foundation, l_px_scale, false, nullptr);
#endif

			if (!m_pack.m_px_physics)
			{
				throw bc_physics_exception(0, "Failed to create PhysX physics");
			}

			m_pack.m_px_cooking = PxCreateCooking
			(
				PX_PHYSICS_VERSION,
				*m_pack.m_px_foundation,
				physx::PxCookingParams(l_px_scale)
			);

			if (!m_pack.m_px_cooking)
			{
				throw bc_physics_exception(0, "Failed to create PhysX cooking");
			}

#ifdef BC_DEBUG
			if (!PxInitExtensions(*m_pack.m_px_physics, m_pack.m_px_pvd))
#else
			if (!PxInitExtensions(*m_pack.m_px_physics, nullptr))
#endif
			{
				throw bc_physics_exception(0, "Failed to create PhysX extensions");
			}
			
			PxRegisterHeightFields(*m_pack.m_px_physics);
		}

		template<>
		BC_PHYSICSIMP_DLL
		void bc_platform_physics< g_api_physx >::_destroy()
		{
			m_pack.m_px_cooking->release();
			m_pack.m_px_physics->release();
#ifdef BC_DEBUG
			m_pack.m_px_pvd->release();
			if (m_pack.m_px_pvd_transport)
			{
				m_pack.m_px_pvd_transport->release();
			}
#endif
			m_pack.m_px_foundation->release();

			m_pack.m_allocator.reset(nullptr);
			m_pack.m_task_dispatcher.reset(nullptr);
			m_pack.m_logger.reset(nullptr);
		}
	}
}