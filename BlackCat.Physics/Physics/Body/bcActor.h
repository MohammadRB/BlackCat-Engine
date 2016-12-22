// [12/01/2016 MRB]

#pragma once

#include "Core/Utility/bcEnumOperand.h"
#include "Physics/bcPhysicsApi.h"
#include "Physics/bcPhysicsReference.h"
#include "Physics/bcPhysicsRef.h"
#include "Physics/Shape/bcBoundBox.h"

namespace black_cat
{
	namespace physics
	{
		template< bc_physics_api TApi >
		class bc_platform_aggregate;
		using bc_aggregate = bc_platform_aggregate<g_current_physics_api>;

		enum class bc_actor_type
		{
			rigid_static,
			rigid_dynamic
		};

		enum class bc_actor_flag
		{
			visualization = core::bc_enum::value(0),
			disable_gravity = core::bc_enum::value(1),
			send_sleep_wake = core::bc_enum::value(2),
			disable_simulation = core::bc_enum::value(3)
		};

		template<bc_physics_api TApi>
		struct bc_platform_actor_pack
		{
		};

		template< bc_physics_api TApi >
		class bc_platform_actor : public bc_platform_physics_reference<TApi>
		{
		public:
			using platform_pack = bc_platform_actor_pack<TApi>;

		public:
			bc_platform_actor() noexcept;

			bc_platform_actor(const bc_platform_actor&) noexcept;

			virtual ~bc_platform_actor();

			bc_platform_actor& operator=(const bc_platform_actor&) noexcept;

			bc_actor_type get_type() const noexcept;

			bool is_rigid_static() const noexcept;

			bool is_rigid_dynamic() const noexcept;

			/**
			 * \brief Retrieves the axis aligned bounding box enclosing the actor.
			 * \return The actor's bounding box.
			 */
			bc_bound_box get_world_bound() const noexcept;

			bc_actor_flag get_actor_flags() const noexcept;

			void set_actor_flag(bc_actor_flag p_flag, bool p_value) noexcept;

			/**
			 * \brief If this actor belong to a aggregate return aggregate of this actor otherwise null
			 * \return 
			 */
			bc_aggregate get_aggregate() const noexcept;

			bool is_valid() const noexcept override;

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		protected:

		private:
			platform_pack m_pack;
		};

		using bc_actor = bc_platform_actor< g_current_physics_api >;
		using bc_actor_ref = bc_physics_ref< bc_actor >;
	}
}