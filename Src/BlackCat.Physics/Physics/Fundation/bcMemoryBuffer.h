// [12/07/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Physics/bcPhysicsApi.h"

namespace black_cat
{
	namespace physics
	{
		template< bc_physics_api TApi >
		class bc_platform_physics;

		template< bc_physics_api TApi >
		struct bc_platform_mesh_buffer_pack
		{
		};

		/**
		 * \brief An intermediate format when creating triangle mesh or convex mesh from input data.
		 * \tparam TApi 
		 */
		template< bc_physics_api TApi >
		class bc_platform_memory_buffer
		{
		public:
			using platform_pack = bc_platform_mesh_buffer_pack< TApi >;
			friend bc_platform_physics< TApi >;

		public:
			bc_platform_memory_buffer(bc_platform_memory_buffer&&) noexcept;

			~bc_platform_memory_buffer();

			bc_platform_memory_buffer& operator=(bc_platform_memory_buffer&&) noexcept;

			bool is_valid() const noexcept;

			void* get_buffer_pointer() const noexcept;

			bcUINT32 get_buffer_size() const noexcept;

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		private:
			bc_platform_memory_buffer();

			platform_pack m_pack;
		};

		using bc_memory_buffer = bc_platform_memory_buffer< g_current_physics_api >;
	}
}