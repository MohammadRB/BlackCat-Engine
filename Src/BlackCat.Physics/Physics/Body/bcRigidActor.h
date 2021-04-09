// [12/01/2016 MRB]

#pragma once

#include "Core/Utility/bcEnumOperand.h"
#include "Physics/bcPhysicsApi.h"
#include "Physics/Fundation/bcTransform.h"
#include "Physics/Shape/bcShape.h"
#include "Physics/Body/bcActor.h"
#include "Physics/Collision/bcCollisionGroup.h"
#include "Physics/Collision/bcQueryGroup.h"

namespace black_cat
{
	namespace physics
	{
		template<bc_physics_api TApi>
		struct bc_platform_rigid_actor_pack
		{
		};

		template<bc_physics_api TApi>
		class bc_platform_rigid_actor : public bc_platform_actor<TApi>
		{
		public:
			using platform_pack = bc_platform_rigid_actor_pack<TApi>;

		public:
			bc_platform_rigid_actor() noexcept;
			
			explicit bc_platform_rigid_actor(platform_pack& p_pack) noexcept;

			bc_platform_rigid_actor(const bc_platform_rigid_actor&) noexcept;

			virtual ~bc_platform_rigid_actor() override;

			bc_platform_rigid_actor& operator=(const bc_platform_rigid_actor&);

			/**
			 * \brief Get actor world space transform.
			 * \return 
			 */
			bc_transform get_global_pose() const noexcept;

			/**
			 * \brief Set actor world space transform.
			 * This method is mainly for dynamic rigid bodies and even with dynamic bodies there are restrictions:
			 * 
			 * moving actors into other actors, thus causing overlap (an invalid physical state)
			 * moving an actor that is connected by a joint to another away from the other (thus causing joint error)
			 * 
			 * \param p_transform 
			 */
			void set_global_pose(const bc_transform& p_transform) noexcept;

			/**
			 * \brief Add a new shape to actor shapes collection
			 * \param p_shape 
			 * \param p_material 
			 * \param p_flags 
			 * \return 
			 */
			bc_shape create_shape(const bc_shape_geometry& p_shape,
				const bc_material& p_material,
				bc_shape_flag p_flags = core::bc_enum::mask_or({ bc_shape_flag::simulation, bc_shape_flag::query, bc_shape_flag::visualization }));

			/**
			 * \brief Add a new shape to actor shapes collection
			 * Waking: No
			 * \param p_shape 
			 * \param p_materials 
			 * \param p_material_count 
			 * \param p_flags 
			 * \return 
			 */
			bc_shape create_shape(const bc_shape_geometry& p_shape,
				const bc_material* p_materials,
				bcUINT32 p_material_count,
				bc_shape_flag p_flags = core::bc_enum::mask_or({ bc_shape_flag::simulation, bc_shape_flag::query, bc_shape_flag::visualization }));

			/**
			 * \brief Attach a shape to actor
			 * Waking: No
			 * \param p_shape 
			 */
			void attach_shape(bc_shape& p_shape) noexcept;

			/**
			 * \brief detach shape from actor
			 * Waking: No
			 * \param p_shape 
			 */
			void detach_shape(bc_shape& p_shape) noexcept;

			bcUINT32 get_shape_count() const noexcept;

			/**
			 * \brief Retrieve all the shape pointers belonging to the actor.
			 * \param p_buffer 
			 * \param p_buffer_size 
			 * \param p_start_index
			 * \return Number of shape pointers written to the buffer.
			 */
			bcUINT32 get_shapes(bc_shape* p_buffer, bcUINT32 p_buffer_size, bcUINT32 p_start_index = 0) const noexcept;

			/**
			 * \brief Set collision group and mask for all shapes included in this actor
			 * \param p_filter 
			 */
			void set_collision_group(bc_collision_filter p_filter) noexcept;

			/**
			* \brief Set query group for all shapes included in this actor
			* \param p_filter
			*/
			void set_query_group(bc_query_group p_filter) noexcept;

			/**
			 * \brief Set notify flag for all shapes included in this actor
			 * \param p_flag 
			 * \param p_value 
			 */
			void set_notify_flag(bc_shape_notify_flag p_flag, bool p_value) noexcept;
		};

		using bc_rigid_actor = bc_platform_rigid_actor< g_current_physics_api >;
		using bc_rigid_actor_ref = bc_physics_ref< bc_rigid_actor >;
	}
}