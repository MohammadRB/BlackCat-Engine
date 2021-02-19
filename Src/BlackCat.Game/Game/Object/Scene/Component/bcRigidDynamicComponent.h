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
			bc_rigid_dynamic_component(bc_actor_index p_actor_index, bc_actor_component_index p_index) noexcept;

			bc_rigid_dynamic_component(bc_rigid_dynamic_component&&) noexcept;

			~bc_rigid_dynamic_component();

			bc_rigid_dynamic_component& operator=(bc_rigid_dynamic_component&&) noexcept;

			bc_actor get_actor() const noexcept override;

			physics::bc_rigid_body& get_body() noexcept override;

			physics::bc_rigid_dynamic get_dynamic_body() const noexcept;

			void initialize(const bc_actor_component_initialize_context& p_context) override;

			void handle_event(const bc_actor_component_event_context& p_context) override;

			void debug_draw(const bc_actor_component_debug_draw_context& p_context) override;
			
		private:
			physics::bc_rigid_dynamic_ref m_px_actor_ref;
		};

		inline physics::bc_rigid_body& bc_rigid_dynamic_component::get_body() noexcept
		{
			return m_px_actor_ref.get();
		}

		inline physics::bc_rigid_dynamic bc_rigid_dynamic_component::get_dynamic_body() const noexcept
		{
			return m_px_actor_ref.get();
		}
	}
}