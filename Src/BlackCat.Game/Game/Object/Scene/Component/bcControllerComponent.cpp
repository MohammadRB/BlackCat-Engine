// [07/04/2022 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManagerContainer.h"
#include "Game/Object/Scene/Component/bcControllerComponent.h"

namespace black_cat::game
{
	bc_controller_component::bc_controller_component(bc_actor_id p_actor_id, bc_actor_component_id p_id)
		: bci_actor_component(p_actor_id, p_id)
	{
	}

	void bc_controller_component::set_controller(core::bc_unique_ptr<bci_actor_controller> p_controller, const bc_actor_component_initialize_context& p_context) noexcept
	{
		m_controller = std::move(p_controller);
		m_controller->initialize(p_context);
	}
	
	void bc_controller_component::load_instance(const bc_actor_component_load_context& p_context)
	{
		m_controller->load_instance(p_context);
	}

	void bc_controller_component::write_instance(const bc_actor_component_write_context& p_context)
	{
		m_controller->write_instance(p_context);
	}

	void bc_controller_component::load_network_instance(const bc_actor_component_network_load_context& p_context)
	{
		m_controller->load_network_instance(p_context);
	}

	void bc_controller_component::write_network_instance(const bc_actor_component_network_write_context& p_context)
	{
		m_controller->write_network_instance(p_context);
	}

	void bc_controller_component::update(const bc_actor_component_update_content& p_context)
	{
		m_controller->update(p_context);
	}

	void bc_controller_component::handle_event(const bc_actor_component_event_context& p_context)
	{
		m_controller->_handle_event(p_context);
	}

	void bc_controller_component::debug_draw(const bc_actor_component_debug_draw_context& p_context)
	{
		m_controller->debug_draw(p_context);
	}
}
