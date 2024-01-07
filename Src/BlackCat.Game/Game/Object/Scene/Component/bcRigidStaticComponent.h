// [07/01/2017 MRB]

#pragma once

#include "PhysicsImp/Body/bcRigidStatic.h"
#include "Game/Object/Scene/Component/bcRigidBodyComponent.h"

namespace black_cat::game
{
	class BC_GAME_DLL bc_rigid_static_component : public bc_rigid_body_component
	{
		BC_COMPONENT(rgd_stc, true, false)

	public:
		bc_rigid_static_component(bc_actor_id p_actor_id, bc_actor_component_id p_id) noexcept;

		bc_rigid_static_component(bc_rigid_static_component&&) noexcept;

		~bc_rigid_static_component() override;

		bc_rigid_static_component& operator=(bc_rigid_static_component&&) noexcept;
		
		physics::bc_actor_type get_body_type() const noexcept override;
			
		physics::bc_rigid_body get_body() const noexcept override;

		physics::bc_rigid_static get_static_body() const noexcept;

		void initialize_entity(const bc_actor_component_initialize_entity_context& p_context) override;
			
		void handle_event(const bc_actor_component_event_context& p_context) override;

		void debug_draw(const bc_actor_component_debug_draw_context& p_context) override;

	private:
		physics::bc_rigid_static_ref m_px_actor_ref;
	};

	inline physics::bc_actor_type bc_rigid_static_component::get_body_type() const noexcept
	{
		return physics::bc_actor_type::rigid_static;
	}
		
	inline physics::bc_rigid_body bc_rigid_static_component::get_body() const noexcept
	{
		return static_cast<physics::bc_rigid_body>(m_px_actor_ref.get());
	}

	inline physics::bc_rigid_static bc_rigid_static_component::get_static_body() const noexcept
	{
		return m_px_actor_ref.get();
	}
}
