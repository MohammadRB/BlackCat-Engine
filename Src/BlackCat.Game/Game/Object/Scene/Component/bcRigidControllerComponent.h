// [20/04/2021 MRB]

#pragma once

#include "PhysicsImp/Fundation/bcCController.h"
#include "PhysicsImp/Body/bcRigidDynamic.h"
#include "Game/Object/Scene/Component/bcRigidBodyComponent.h"

namespace black_cat::game
{
	class bc_material_manager;
	class bc_physics_system;

	class BC_GAME_DLL bc_rigid_controller_component : public bc_rigid_body_component
	{
		BC_COMPONENT(rgd_ctr, true, false)

	public:
		bc_rigid_controller_component(bc_actor_id p_actor_id, bc_actor_component_id p_id) noexcept;

		bc_rigid_controller_component(bc_rigid_controller_component&&) noexcept;

		~bc_rigid_controller_component() override;

		bc_rigid_controller_component& operator=(bc_rigid_controller_component&&) noexcept;

		bc_actor get_actor() const noexcept override;

		physics::bc_actor_type get_body_type() const noexcept override;
			
		physics::bc_rigid_body get_body() const noexcept override;

		physics::bc_ccontroller get_controller() const noexcept;

		physics::bc_rigid_dynamic get_dynamic_body() const noexcept;

		void initialize_entity(const bc_actor_component_initialize_entity_context& p_context) override;

		void reset_controller(physics::bc_ccontroller_ref p_controller);
			
		void reset_controller();
			
		void handle_event(const bc_actor_component_event_context& p_context) override;

		void debug_draw(const bc_actor_component_debug_draw_context& p_context) override;

	private:
		physics::bc_rigid_dynamic_ref m_px_body;
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

	inline physics::bc_rigid_dynamic bc_rigid_controller_component::get_dynamic_body() const noexcept
	{
		return *m_px_body;
	}
}
