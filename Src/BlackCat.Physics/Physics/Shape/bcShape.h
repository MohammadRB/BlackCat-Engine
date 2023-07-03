// [29/11/2016 MRB]

#pragma once

#include "Core/Utility/bcEnumOperand.h"
#include "Physics/bcPhysicsApi.h"
#include "Physics/bcPhysicsReference.h"
#include "Physics/bcPhysicsRef.h"
#include "Physics/Fundation/bcTransform.h"
#include "Physics/Fundation/bcMaterial.h"
#include "Physics/Shape/bcShapeGeometry.h"
#include "Physics/Shape/bcShapeSphere.h"
#include "Physics/Shape/bcShapeBox.h"
#include "Physics/Shape/bcShapeCapsule.h"
#include "Physics/Shape/bcShapePlane.h"
#include "Physics/Shape/bcShapeConvexMesh.h"
#include "Physics/Shape/bcShapeTriangleMesh.h"
#include "Physics/Shape/bcShapeHeightField.h"
#include "Physics/Collision/bcCollisionGroup.h"
#include "Physics/Collision/bcQueryGroup.h"

namespace black_cat
{
	namespace physics
	{
		enum class bc_shape_notify_flag
		{
			notify_touch = core::bc_enum::value(0),			// Enable collision notification callback
			notify_modify = core::bc_enum::value(1),		// Enable collision modify callback
		};

		enum class bc_shape_query_flag
		{
			touching = 1,									// Treat as touching object
			blocking = 2									// Treat as blocking object.By default shapes are blocking
		};

		enum class bc_shape_flag
		{
			simulation = core::bc_enum::value(0),			// The shape will partake in collision in the physical simulation.
			query = core::bc_enum::value(1),				// The shape will partake in scene queries.
			trigger = core::bc_enum::value(2),				// The shape is a trigger which can send reports whenever other shapes enter/leave its volume.
			visualization = core::bc_enum::value(3),		// The shape will be visualized
			default_v = simulation | query | visualization
		};

		template<bc_physics_api TApi>
		struct bc_platform_shape_pack
		{
		};

		template<bc_physics_api TApi>
		class bc_platform_shape : public bc_platform_physics_reference<TApi>
		{
		public:
			using platform_pack = bc_platform_shape_pack< TApi >;

		public:
			bc_platform_shape();

			explicit bc_platform_shape(platform_pack& p_pack);

			bc_platform_shape(const bc_platform_shape&) noexcept;

			~bc_platform_shape() override;

			bc_platform_shape& operator=(const bc_platform_shape&) noexcept;

			bc_shape_type get_type() const noexcept;

			std::pair<bool, bc_shape_sphere> as_sphere() const noexcept;

			std::pair<bool, bc_shape_box> as_box() const noexcept;

			std::pair<bool, bc_shape_capsule> as_capsule() const noexcept;

			std::pair<bool, bc_shape_plane> as_plane() const noexcept;

			std::pair<bool, bc_shape_convex_mesh> as_convex_mesh() const noexcept;

			std::pair<bool, bc_shape_triangle_mesh> as_triangle_mesh() const noexcept;

			std::pair<bool, bc_shape_height_field> as_height_field() const noexcept;

			bc_transform get_local_pose() const noexcept;

			/*void set_collision_filter(bc_collision_filter p_filter) noexcept;

			bc_collision_filter get_collision_filter() const noexcept;*/

			/**
			 * \brief Sets the pose of the shape in actor space, i.e. relative to the actors to which they are attached.
			 * Does not automatically update the inertia properties of the owning actor.
			 * Waking: No.
			 * \param p_pose 
			 */
			void set_local_pose(const bc_transform& p_pose) noexcept;

			bcUINT32 get_material_count() const noexcept;

			bcUINT32 get_materials(bc_material* p_buffer, bcUINT32 p_buffer_size) const noexcept;

			/**
			 * \brief 
			 * Waking: No.
			 * \param p_materials 
			 * \param p_count 
			 */
			void set_materials(const bc_material* p_materials, bcUINT32 p_count);

			/**
			 * \brief Retrieve material from given triangle index.
			 * his function is only useful for triangle meshes or height-fields, which have per-triangle materials. 
			 * For other shapes the function returns the single material associated with the shape, regardless of the index.
			 * \param p_face_index 
			 * \return 
			 */
			bc_material get_material_from_face(bcUINT32 p_face_index) const;

			bcFLOAT get_contact_offset() const noexcept;

			/**
			* \brief Sets the contact offset.
			* Shapes whose distance is less than the sum of their contactOffset values will generate contacts.
			* The contact offset must be positive and greater than the rest offset.
			* Having a contactOffset greater than than the restOffset allows the collision detection system to
			* predicatively enforce the contact constraint even when the objects are slightly separated.
			* This prevents jitter that would occur if the constraint were enforced only when shapes were within the rest distance.
			* Waking: No.
			* Default: 0.02f
			* \return
			*/
			void set_contact_offset(bcFLOAT p_offset) noexcept;

			bcFLOAT get_rest_offset() const noexcept;

			/**
			 * \brief Sets the rest offset. 
			 * Two shapes will come to rest at a distance equal to the sum of their restOffset values. 
			 * If the restOffset is 0, they should converge to touching exactly.
			 * Having a restOffset greater than zero is useful to have objects slide smoothly, 
			 * so that they do not get hung up on irregularities of each others' surfaces.
			 * Default: 0.0f
			 */
			void set_rest_offset(bcFLOAT p_offset) noexcept;

			bc_shape_flag get_flags() const noexcept;

			void set_flags(bc_shape_flag p_flags) noexcept;
			
			void set_flag(bc_shape_flag p_flag, bool p_value) noexcept;

			bc_collision_filter get_collision_group() const noexcept;

			void set_collision_group(bc_collision_filter p_filter) noexcept;

			bc_shape_notify_flag get_notify_flags() const noexcept;

			void set_notify_flags(bc_shape_notify_flag p_flags) noexcept;
			
			void set_notify_flag(bc_shape_notify_flag p_flag, bool p_value) noexcept;

			bc_query_group get_query_group() const noexcept;

			void set_query_group(bc_query_group p_filter) noexcept;

			bc_shape_query_flag get_query_flags() const noexcept;

			void set_query_flags(bc_shape_query_flag p_flags) noexcept;
			
			void set_query_flag(bc_shape_query_flag p_flag, bool p_value) noexcept;

			bool get_high_detail_query_shape() const noexcept;

			void set_high_detail_query_shape(bool p_value) noexcept;
			
			bool is_exclusive() const noexcept;

			bool is_valid() const noexcept override;

			void set_data(void* p_user_data) noexcept;

			void* get_data() const noexcept;

			platform_pack& get_platform_pack() noexcept override
			{
				return m_pack;
			}

			const platform_pack& get_platform_pack() const noexcept override
			{
				return m_pack;
			}

		private:
			platform_pack m_pack;
		};

		using bc_shape = bc_platform_shape< g_current_physics_api >;
		using bc_shape_ref = bc_physics_ref<bc_shape>;
	}
}