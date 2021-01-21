// [01/07/2017 MRB]

#pragma once

#include "PhysicsImp/Body/bcRigidDynamic.h"
#include "Game/Object/Scene/Component/bcRigidBodyComponent.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_rigid_dynamic_component : public bc_rigid_body_component
		{
			BC_COMPONENT(rgd_dyn, true, false)

		public:
			explicit bc_rigid_dynamic_component(bc_actor_component_index p_index) noexcept;

			bc_rigid_dynamic_component(bc_rigid_dynamic_component&&) noexcept;

			~bc_rigid_dynamic_component();

			bc_rigid_dynamic_component& operator=(bc_rigid_dynamic_component&&) noexcept;

			bc_actor get_actor() const noexcept override;

			physics::bc_rigid_body& get_body() noexcept override;

			physics::bc_rigid_dynamic get_dynamic_body() const noexcept;

			void initialize(bc_actor_component_initialize_context& p_context) override;

			void handle_event(bc_actor_component_event_context& p_context) override;

		private:
			physics::bc_rigid_dynamic_ref m_px_actor_ref;
		};
	}
}