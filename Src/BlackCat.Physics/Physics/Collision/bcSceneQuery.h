// [12/06/2016 MRB]

#pragma once

#include "Core/Utility/bcEnumOperand.h"
#include "Core/Utility/bcDelegate.h"
#include "Physics/bcPhysicsApi.h"
#include "Physics/Collision/bcShapeQuery.h"
#include "Physics/Body/bcRigidActor.h"
#include "Physics/Shape/bcShape.h"

namespace black_cat
{
	namespace physics
	{
		enum class bc_query_flags
		{
			statics = core::bc_enum::value(0),		// Include static actors in query result
			dynamics = core::bc_enum::value(1),		// Include dynamic actors in query result
			any_hit = core::bc_enum::value(4)		// Abort traversal as soon as any hit is found and return it via callback.block.
													// Helps query performance. Both eTOUCH and eBLOCK hitTypes are considered hits with this flag.
		};

		enum class bc_query_hit_type
		{
			none = 0,
			touch = 1,
			block = 2
		};

		struct bc_scene_query_post_filter_data
		{
			bc_actor m_actor;
			bc_shape m_shape;
			bcUINT32 m_face_index;
		};
		using bc_scene_query_post_filter_callback = core::bc_delegate < bc_query_hit_type(bc_scene_query_post_filter_data&) >;
		
		template< bc_physics_api TApi, class THit >
		struct bc_platform_scene_query_buffer_pack
		{
		};

		template< bc_physics_api TApi, class THit >
		class bc_platform_scene_query_buffer
		{
		public:
			using platform_pack = bc_platform_scene_query_buffer_pack< TApi, THit >;

		public:
			explicit bc_platform_scene_query_buffer(bcUINT32 p_touching_hit_count);

			bc_platform_scene_query_buffer(const bc_platform_scene_query_buffer&) noexcept;

			~bc_platform_scene_query_buffer();

			bc_platform_scene_query_buffer& operator=(const bc_platform_scene_query_buffer&) noexcept;

			bool has_block() const noexcept;

			THit get_block() const noexcept;

			bcUINT32 get_touch_count() const noexcept;

			THit get_touch(bcUINT32 p_index) const noexcept;

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		private:
			platform_pack m_pack;
		};

		template< class THit >
		using bc_scene_query_buffer = bc_platform_scene_query_buffer< g_current_physics_api, THit >;

		using bc_scene_ray_query_buffer = bc_scene_query_buffer< bc_ray_hit >;
		using bc_scene_overlap_query_buffer = bc_scene_query_buffer< bc_overlap_hit >;
		using bc_scene_sweep_query_buffer = bc_scene_query_buffer< bc_sweep_hit >;
	}
}