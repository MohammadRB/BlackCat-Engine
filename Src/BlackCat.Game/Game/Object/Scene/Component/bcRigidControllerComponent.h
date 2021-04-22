// [04/20/2021 MRB]

#pragma once

#include "PhysicsImp/Fundation/bcCController.h"
#include "PhysicsImp/Body/bcRigidStatic.h"
#include "Game/Object/Scene/Component/bcRigidBodyComponent.h"

namespace black_cat
{
	namespace game
	{
		class bc_material_manager;
		class bc_physics_system;

		class BC_GAME_DLL bc_rigid_controller_component : public bc_rigid_body_component
		{
			BC_COMPONENT(rgd_ctr, true, false)

		public:
			bc_rigid_controller_component(bc_actor_index p_actor_index, bc_actor_component_index p_index) noexcept;

			bc_rigid_controller_component(bc_rigid_controller_component&&) noexcept;

			~bc_rigid_controller_component() override;

			bc_rigid_controller_component& operator=(bc_rigid_controller_component&&) noexcept;

			bc_actor get_actor() const noexcept override;

			physics::bc_actor_type get_body_type() const noexcept override;
			
			physics::bc_rigid_body get_body() const noexcept override;

			physics::bc_ccontroller get_controller() const noexcept;

			void initialize(const bc_actor_component_initialize_context& p_context) override;

			void initialize_entity(const bc_actor_component_initialize_entity_context& p_context) override;

			void reset_controller(physics::bc_ccontroller_ref p_controller);
			
			void reset_controller();
			
			void handle_event(const bc_actor_component_event_context& p_context) override;

			void debug_draw(const bc_actor_component_debug_draw_context& p_context) override;

		private:
			physics::bc_rigid_static_ref m_px_body; // TODO convert to rigid dynamic
			physics::bc_ccontroller_ref m_px_controller;
		};

		inline physics::bc_actor_type bc_rigid_controller_component::get_body_type() const noexcept
		{
			return physics::bc_actor_type::rigid_dynamic;
		}
		
		inline physics::bc_rigid_body bc_rigid_controller_component::get_body() const noexcept
		{
			return *m_px_body;
		}

		inline physics::bc_ccontroller bc_rigid_controller_component::get_controller() const noexcept
		{
			return m_px_controller.get();
		}
	}
}