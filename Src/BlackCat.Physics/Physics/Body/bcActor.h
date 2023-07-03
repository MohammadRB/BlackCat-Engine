// [01/12/2016 MRB]

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
		template<bc_physics_api TApi>
		class bc_platform_aggregate;
		using bc_aggregate = bc_platform_aggregate<g_current_physics_api>;

		template<bc_physics_api TApi>
		class bc_platform_rigid_actor;
		using bc_rigid_actor = bc_platform_rigid_actor<g_current_physics_api>;

		template<bc_physics_api TApi>
		class bc_platform_rigid_body;
		using bc_rigid_body = bc_platform_rigid_body<g_current_physics_api>;

		template<bc_physics_api TApi>
		class bc_platform_rigid_static;
		using bc_rigid_static = bc_platform_rigid_static<g_current_physics_api>;

		template<bc_physics_api TApi>
		class bc_platform_rigid_dynamic;
		using bc_rigid_dynamic = bc_platform_rigid_dynamic<g_current_physics_api>;

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

		template<bc_physics_api TApi>
		class bc_platform_actor : public bc_platform_physics_reference<TApi>
		{
		public:
			using platform_pack = bc_platform_actor_pack<TApi>;

		public:
			bc_platform_actor() noexcept;

			explicit bc_platform_actor(platform_pack& p_pack) noexcept;

			bc_platform_actor(const bc_platform_actor&) noexcept;

			virtual ~bc_platform_actor() override;

			bc_platform_actor& operator=(const bc_platform_actor&) noexcept;

			bc_actor_type get_type() const noexcept;

			bc_rigid_actor is_rigid_actor() const noexcept;

			bc_rigid_body is_rigid_body() const noexcept;

			bc_rigid_static is_rigid_static() const noexcept;

			bc_rigid_dynamic is_rigid_dynamic() const noexcept;

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

		using bc_actor = bc_platform_actor<g_current_physics_api>;
		using bc_actor_ref = bc_physics_ref<bc_actor>;
	}
}