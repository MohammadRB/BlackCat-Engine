// [01/07/2017 MRB]

#pragma once

#include "PhysicsImp/Body/bcRigidBody.h"
#include "Game/Object/Scene/bcActorComponent.h"

namespace black_cat
{
	namespace game
	{
		class bc_rigid_body_component : public bc_iactor_component
		{
			BC_ABSTRACT_COMPONENT(rigid_body);

		public:
			virtual ~bc_rigid_body_component();

			virtual physics::bc_rigid_body& get_body() noexcept = 0;

		protected:
			explicit bc_rigid_body_component(bc_actor_component_index p_index) noexcept;

			bc_rigid_body_component(bc_rigid_body_component&&) noexcept;

			bc_rigid_body_component& operator=(bc_rigid_body_component&&) noexcept;

		private:
		};

		inline bc_rigid_body_component::~bc_rigid_body_component() = default;

		inline bc_rigid_body_component::bc_rigid_body_component(bc_actor_component_index p_index) noexcept
			: bc_iactor_component(p_index)
		{
		}

		inline bc_rigid_body_component::bc_rigid_body_component(bc_rigid_body_component&&) noexcept = default;

		inline bc_rigid_body_component& bc_rigid_body_component::operator=(bc_rigid_body_component&&) noexcept = default;
	}
}