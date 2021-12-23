// [09/30/2021 MRB]

#pragma once

#include "Game/Object/Scene/ActorComponent/bcActorNetworkController.h"
#include "App/bcExport.h"

namespace black_cat
{
	class BC_DLL bc_rigid_dynamic_network_actor_controller : public game::bc_actor_network_controller
	{
	public:
		void load_origin_network_instance(const game::bc_actor_component_network_load_context& p_context) override;
		
		void load_replicated_network_instance(const game::bc_actor_component_network_load_context& p_context) override;
		
		void write_origin_network_instance(const game::bc_actor_component_network_write_context& p_context) override;
		
		void write_replicated_network_instance(const game::bc_actor_component_network_write_context& p_context) override;

		void update_origin_instance(const game::bc_actor_component_update_content& p_context) override;

		void update_replicated_instance(const game::bc_actor_component_update_content& p_context) override;

		void handle_event(const game::bc_actor_component_event_context& p_context) override;

	protected:
		const core::bc_vector3f& get_network_position() const noexcept;
		
		const core::bc_vector3f& get_network_rotation() const noexcept;
		
	private:
		core::bc_vector3f m_network_position;
		core::bc_vector3f m_network_euler_rotation;
	};

	inline const core::bc_vector3f& bc_rigid_dynamic_network_actor_controller::get_network_position() const noexcept
	{
		return m_network_position;
	}

	inline const core::bc_vector3f& bc_rigid_dynamic_network_actor_controller::get_network_rotation() const noexcept
	{
		return m_network_euler_rotation;
	}
}