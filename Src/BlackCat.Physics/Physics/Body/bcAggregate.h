// [01/12/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Physics/bcPhysicsApi.h"
#include "Physics/bcPhysicsReference.h"
#include "Physics/bcPhysicsRef.h"

namespace black_cat
{
	namespace physics
	{
		template< bc_physics_api TApi >
		class bc_platform_actor;
		using bc_actor = bc_platform_actor< g_current_physics_api >;

		template< bc_physics_api TApi >
		struct bc_platform_aggregate_pack
		{
		};

		/**
		 * \brief Class to aggregate actors into a single broad phase entry.
		 * An aggregate object is a collection of actors, which will exist as a single entry in the broad-phase structures.
		 */
		template< bc_physics_api TApi >
		class bc_platform_aggregate : public bc_platform_physics_reference< TApi >
		{
		public:
			using platform_pack = bc_platform_aggregate_pack< TApi >;
			constexpr static bcUINT32 s_max_actor_count = platform_pack::s_max_actor_count;

		public:
			bc_platform_aggregate() noexcept;

			explicit bc_platform_aggregate(platform_pack& p_pack) noexcept;

			bc_platform_aggregate(const bc_platform_aggregate&) noexcept;

			~bc_platform_aggregate();

			bc_platform_aggregate& operator=(const bc_platform_aggregate&) noexcept;

			bool add_actor(bc_actor& p_actor) noexcept;

			bool remove_actor(bc_actor& p_actor) noexcept;

			bcUINT32 get_actor_count() const noexcept;

			/**
			 * \brief Retrieve all actors contained in the aggregate.
			 * \param p_buffer 
			 * \param p_buffer_size 
			 * \return Number of actor pointers written to the buffer.
			 */
			bcUINT32 get_actors(bc_actor* p_buffer, bcUINT32 p_buffer_size, bcUINT32 p_start_index = 0) const noexcept;

			bool is_valid() const noexcept override;

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

		using bc_aggregate = bc_platform_aggregate< g_current_physics_api >;
		using bc_aggregate_ref = bc_physics_ref< bc_aggregate >;
	}
}