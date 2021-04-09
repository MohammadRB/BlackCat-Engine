// [11/28/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Core/Memory/bcPtr.h"
#include "Core/Utility/bcInitializable.h"
#include "Physics/bcPhysicsApi.h"
#include "Physics/bcPhysicsRef.h"
#include "Physics/Fundation/bcFundation.h"

namespace black_cat
{
	namespace physics
	{
		template< bc_physics_api TApi >
		class bc_platform_scene_builder;
		using bc_scene_builder = bc_platform_scene_builder< g_current_physics_api >;

		template< bc_physics_api TApi >
		class bc_platform_scene;
		using bc_scene = bc_platform_scene< g_current_physics_api >;
		using bc_scene_ref = bc_physics_ref<bc_scene>;

		template< bc_physics_api TApi >
		class bc_platform_transform;
		using bc_transform = bc_platform_transform< g_current_physics_api >;

		struct bc_triangle_mesh_desc;

		template< bc_physics_api TApi >
		class bc_platform_triangle_mesh;
		using bc_triangle_mesh = bc_platform_triangle_mesh< g_current_physics_api >;
		using bc_triangle_mesh_ref = bc_physics_ref< bc_triangle_mesh >;

		struct bc_convex_mesh_desc;

		template< bc_physics_api TApi >
		class bc_platform_convex_mesh;
		using bc_convex_mesh = bc_platform_convex_mesh< g_current_physics_api >;
		using bc_convex_mesh_ref = bc_physics_ref< bc_convex_mesh >;

		struct bc_height_field_desc;

		template< bc_physics_api TApi >
		class bc_platform_height_field;
		using bc_height_field = bc_platform_height_field< g_current_physics_api >;
		using bc_height_field_ref = bc_physics_ref< bc_height_field >;

		template< bc_physics_api TApi >
		class bc_platform_shape_geometry;
		using bc_shape_geometry = bc_platform_shape_geometry< g_current_physics_api >;

		enum class bc_shape_flag;

		template< bc_physics_api TApi >
		class bc_platform_shape;
		using bc_shape = bc_platform_shape< g_current_physics_api >;
		using bc_shape_ref = bc_physics_ref<bc_shape>;

		template< bc_physics_api TApi >
		class bc_platform_material;
		using bc_material = bc_platform_material< g_current_physics_api >;
		using bc_material_ref = bc_physics_ref< bc_material >;

		template< bc_physics_api TApi >
		class bc_platform_actor;
		using bc_actor = bc_platform_actor< g_current_physics_api >;
		using bc_actor_ref = bc_physics_ref< bc_actor >;

		template< bc_physics_api TApi >
		class bc_platform_rigid_actor;
		using bc_rigid_actor = bc_platform_rigid_actor< g_current_physics_api >;
		using bc_rigid_actor_ref = bc_physics_ref< bc_rigid_actor >;

		template< bc_physics_api TApi >
		class bc_platform_rigid_body;
		using bc_rigid_body = bc_platform_rigid_body< g_current_physics_api >;
		using bc_rigid_body_ref = bc_physics_ref< bc_rigid_body >;

		template< bc_physics_api TApi >
		class bc_platform_rigid_static;
		using bc_rigid_static = bc_platform_rigid_static< g_current_physics_api >;
		using bc_rigid_static_ref = bc_physics_ref<bc_rigid_static>;

		template< bc_physics_api TApi >
		class bc_platform_rigid_dynamic;
		using bc_rigid_dynamic = bc_platform_rigid_dynamic< g_current_physics_api >;
		using bc_rigid_dynamic_ref = bc_physics_ref<bc_rigid_dynamic>;

		template< bc_physics_api TApi >
		class bc_platform_aggregate;
		using bc_aggregate = bc_platform_aggregate< g_current_physics_api >;
		using bc_aggregate_ref = bc_physics_ref< bc_aggregate >;

		template< bc_physics_api TApi >
		class bc_platform_fixed_joint;
		using bc_fixed_joint = bc_platform_fixed_joint< g_current_physics_api >;
		using bc_fixed_joint_ref = bc_physics_ref<bc_fixed_joint>;

		template< bc_physics_api TApi >
		class bc_platform_distance_joint;
		using bc_distance_joint = bc_platform_distance_joint< g_current_physics_api >;
		using bc_distance_joint_ref = bc_physics_ref<bc_distance_joint>;

		template< bc_physics_api TApi >
		class bc_platform_prismatic_joint;
		using bc_prismatic_joint = bc_platform_prismatic_joint< g_current_physics_api >;
		using bc_prismatic_joint_ref = bc_physics_ref<bc_prismatic_joint>;

		template< bc_physics_api TApi >
		class bc_platform_revolute_joint;
		using bc_revolute_joint = bc_platform_revolute_joint< g_current_physics_api >;
		using bc_revolute_joint_ref = bc_physics_ref< bc_revolute_joint >;

		template< bc_physics_api TApi >
		class bc_platform_spherical_joint;
		using bc_spherical_joint = bc_platform_spherical_joint< g_current_physics_api >;
		using bc_spherical_joint_ref = bc_physics_ref<bc_spherical_joint>;

		template< bc_physics_api TApi >
		class bc_platform_d6_joint;
		using bc_d6_joint = bc_platform_d6_joint< g_current_physics_api >;
		using bc_d6_joint_ref = bc_physics_ref<bc_d6_joint>;

		template< bc_physics_api TApi >
		class bc_platform_memory_buffer;
		using bc_memory_buffer = bc_platform_memory_buffer< g_current_physics_api >;

		template< bc_physics_api TApi >
		class bc_platform_serialize_buffer;
		using bc_serialize_buffer = bc_platform_serialize_buffer< g_current_physics_api >;

		template< bc_physics_api TApi >
		struct bc_platform_physics_pack
		{
		};

		template< bc_physics_api TApi >
		class bc_platform_physics : public core::bc_initializable<core::bc_unique_ptr<bci_allocator>, core::bc_unique_ptr<bci_task_dispatcher>, core::bc_unique_ptr<bci_logger>>
		{
		public:
			using platform_pack = bc_platform_physics_pack< TApi >;

		public:
			bc_platform_physics();
			
			bc_platform_physics(bc_platform_physics&& p_other) noexcept;

			~bc_platform_physics();

			bc_platform_physics& operator=(bc_platform_physics&& p_other) noexcept;

			bc_scene_ref create_scene(bc_scene_builder p_desc);

			bcUINT32 get_scene_count() const noexcept;

			/**
			* \brief Creates a static rigid actor with the specified pose and all other fields initialized to their default values.
			* \param[in] p_pose the initial pose of the actor. Must be a valid transform
			*/
			bc_rigid_static_ref create_rigid_static(const bc_transform& p_pose);

			/**
			* \brief Creates a dynamic rigid actor with the specified pose and all other fields initialized to their default values.
			* \param[in] p_pose the initial pose of the actor. Must be a valid transform
			*/
			bc_rigid_dynamic_ref create_rigid_dynamic(const bc_transform& p_pose);

			/**
			* \brief Creates a shape which may be attached to multiple actors
			* \param[in] p_geometry the geometry for the shape
			* \param[in] p_material the material for the shape
			* \param[in] p_is_exclusive whether this shape is exclusive to a single actor or maybe be shared
			* Shared shapes are not mutable when they are attached to an actor
			*/
			bc_shape_ref create_shape(const bc_shape_geometry& p_geometry,
				const bc_material& p_material,
				bool p_is_exclusive = false);

			/**
			* \brief Creates a shape which may be attached to multiple actors
			* \param[in] p_geometry the geometry for the shape
			* \param[in] p_material the material for the shape
			* \param[in] p_shape_flags
			* \param[in] p_is_exclusive whether this shape is exclusive to a single actor or maybe be shared
			* Shared shapes are not mutable when they are attached to an actor
			*/
			bc_shape_ref create_shape(const bc_shape_geometry& p_geometry,
				const bc_material& p_material,
				bc_shape_flag p_shape_flags,
				bool p_is_exclusive = false);

			/**
			* \brief Creates a shape which may be attached to multiple actors.
			* \param[in] p_geometry the geometry for the shape
			* \param[in] p_materials the materials for the shape
			* \param[in] p_material_count the number of materials
			* \param[in] p_is_exclusive whether this shape is exclusive to a single actor or may be shared
			* Shared shapes are not mutable when they are attached to an actor
			*/
			bc_shape_ref create_shape(const bc_shape_geometry& p_geometry,
				bc_material*const* p_materials,
				bcUINT16 p_material_count,
				bool p_is_exclusive = false);

			/**
			* \brief Creates a shape which may be attached to multiple actors.
			* \param[in] p_geometry the geometry for the shape
			* \param[in] p_materials the materials for the shape
			* \param[in] p_material_count the number of materials
			* \param[in] p_shape_flags the PxShapeFlags to be set
			* \param[in] p_is_exclusive whether this shape is exclusive to a single actor or may be shared
			* Shared shapes are not mutable when they are attached to an actor
			*/
			bc_shape_ref create_shape(const bc_shape_geometry& p_geometry,
				bc_material*const* p_materials,
				bcUINT16 p_material_count,
				bc_shape_flag p_shape_flags,
				bool p_is_exclusive = false);

			bcUINT32 get_shape_count() const noexcept;

			/**
			* \brief Creates a new material with default properties.
			* \param p_static_friction the coefficient of static friction. Range [0, max_float]
			* \param p_dynamic_friction the coefficient of dynamic friction. Range [0, max_float]
			* \param p_restitution the coefficient of restitution. Range [0, 1]
			* \return The new material.
			*/
			bc_material_ref create_material(bcFLOAT p_static_friction, bcFLOAT p_dynamic_friction, bcFLOAT p_restitution);

			bcUINT32 get_material_count() const noexcept;

			/**
			 * \brief Creates an aggregate with the specified maximum size and selfCollision property.
			 * \param[in] p_max_size the maximum number of actors that may be placed in the aggregate.  This value must not exceed 128, otherwise NULL will be returned.
			 * \param[in] p_enable_self_collision whether the aggregate supports self-collision
			 * \return The new aggregate.
			 */
			bc_aggregate_ref create_aggregate(bcUINT32 p_max_size, bool p_enable_self_collision);

			bc_memory_buffer create_convex_mesh(const bc_convex_mesh_desc& p_desc);

			bc_convex_mesh_ref create_convex_mesh(bc_memory_buffer& p_buffer);

			bcUINT32 get_convex_mesh_count() const noexcept;

			bc_memory_buffer create_triangle_mesh(const bc_triangle_mesh_desc& p_desc);

			bc_triangle_mesh_ref create_triangle_mesh(bc_memory_buffer& p_buffer);

			bc_triangle_mesh_ref create_runtime_triangle_mesh(const bc_triangle_mesh_desc& p_desc);

			bcUINT32 get_triangle_mesh_count() const noexcept;

			bc_memory_buffer create_height_field(const bc_height_field_desc& p_desc);

			bc_height_field_ref create_height_field(bc_memory_buffer& p_buffer);

			bcUINT32 get_height_field_count() const noexcept;

			/**
			 * \brief Create a fixed joint.
			 * \param[in] p_actor0 an actor to which the joint is attached. NULL may be used to attach the joint to a specific point in the world frame
			 * \param[in] p_local_frame0 the position and orientation of the joint relative to actor0
			 * \param[in] p_actor1 an actor to which the joint is attached. NULL may be used to attach the joint to a specific point in the world frame
			 * \param[in] p_local_frame1 the position and orientation of the joint relative to actor1
			 */
			bc_fixed_joint_ref create_fixed_joint(bc_rigid_actor* p_actor0,
				const bc_transform& p_local_frame0,
				bc_rigid_actor* p_actor1,
				const bc_transform& p_local_frame1);

			/**
			* \brief Create a distance joint.
			* \param[in] p_actor0 an actor to which the joint is attached. NULL may be used to attach the joint to a specific point in the world frame
			* \param[in] p_local_frame0 the position and orientation of the joint relative to actor0
			* \param[in] p_actor1 an actor to which the joint is attached. NULL may be used to attach the joint to a specific point in the world frame
			* \param[in] p_local_frame1 the position and orientation of the joint relative to actor1
			*/
			bc_distance_joint_ref create_distance_joint(bc_rigid_actor* p_actor0,
				const bc_transform& p_local_frame0,
				bc_rigid_actor* p_actor1,
				const bc_transform& p_local_frame1);

			/**
			* \brief Create a prismatic joint.
			* \param[in] p_actor0 an actor to which the joint is attached. NULL may be used to attach the joint to a specific point in the world frame
			* \param[in] p_local_frame0 the position and orientation of the joint relative to actor0
			* \param[in] p_actor1 an actor to which the joint is attached. NULL may be used to attach the joint to a specific point in the world frame
			* \param[in] p_local_frame1 the position and orientation of the joint relative to actor1
			*/
			bc_prismatic_joint_ref create_prismatic_joint(bc_rigid_actor* p_actor0,
				const bc_transform& p_local_frame0,
				bc_rigid_actor* p_actor1,
				const bc_transform& p_local_frame1);

			/**
			* \brief Create a revolute joint.
			* \param[in] p_actor0 an actor to which the joint is attached. NULL may be used to attach the joint to a specific point in the world frame
			* \param[in] p_local_frame0 the position and orientation of the joint relative to actor0
			* \param[in] p_actor1 an actor to which the joint is attached. NULL may be used to attach the joint to a specific point in the world frame
			* \param[in] p_local_frame1 the position and orientation of the joint relative to actor1
			*/
			bc_revolute_joint_ref create_revolute_joint(bc_rigid_actor* p_actor0,
				const bc_transform& p_local_frame0,
				bc_rigid_actor* p_actor1,
				const bc_transform& p_local_frame1);

			/**
			* \brief Create a spherical joint.
			* \param[in] p_actor0 an actor to which the joint is attached. NULL may be used to attach the joint to a specific point in the world frame
			* \param[in] p_local_frame0 the position and orientation of the joint relative to actor0
			* \param[in] p_actor1 an actor to which the joint is attached. NULL may be used to attach the joint to a specific point in the world frame
			* \param[in] p_local_frame1 the position and orientation of the joint relative to actor1
			*/
			bc_spherical_joint_ref create_spherical_joint(bc_rigid_actor* p_actor0,
				const bc_transform& p_local_frame0,
				bc_rigid_actor* p_actor1,
				const bc_transform& p_local_frame1);

			/**
			* \brief Create a d6 joint.
			* \param[in] p_actor0 an actor to which the joint is attached. NULL may be used to attach the joint to a specific point in the world frame
			* \param[in] p_local_frame0 the position and orientation of the joint relative to actor0
			* \param[in] p_actor1 an actor to which the joint is attached. NULL may be used to attach the joint to a specific point in the world frame
			* \param[in] p_local_frame1 the position and orientation of the joint relative to actor1
			*/
			bc_d6_joint_ref create_d6_joint(bc_rigid_actor* p_actor0,
				const bc_transform& p_local_frame0,
				bc_rigid_actor* p_actor1,
				const bc_transform& p_local_frame1);

			bc_memory_buffer read_to_memory_buffer(void* p_data, bcUINT32 p_size);

			bc_serialize_buffer create_serialize_buffer();

			bc_memory_buffer serialize(bc_serialize_buffer& p_buffer);

			bc_serialize_buffer deserialize(bc_memory_buffer& p_buffer);

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		private:
			void _initialize(core::bc_unique_ptr<bci_allocator> p_allocator, 
				core::bc_unique_ptr<bci_task_dispatcher> p_task_dispatcher,
				core::bc_unique_ptr<bci_logger> p_logger) noexcept(false) override;

			void _destroy() override;

			platform_pack m_pack;
		};

		using bc_physics = bc_platform_physics< g_current_physics_api >;
	}
}