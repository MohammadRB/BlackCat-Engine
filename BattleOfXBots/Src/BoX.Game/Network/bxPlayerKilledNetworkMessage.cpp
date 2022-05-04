// [02/12/2022 MRB]

#include "Core/Utility/bcJsonParse.h"
#include "Game/Object/Scene/ActorComponent/bcActor.hpp"
#include "Game/Object/Scene/Component/bcControllerComponent.h"
#include "BoX.Game/Network/bxPlayerKilledNetworkMessage.h"
#include "BoX.Game/Game/bxPlayerActorController.h"
#include "BoX.Game/Game/bxNetworkPlayerActorController.h"

namespace box
{
	bx_player_killed_network_message::bx_player_killed_network_message() noexcept
		: bci_network_message(message_name()),
		m_killer_client_id(game::bc_network_client::invalid_id),
		m_actor_net_id(game::g_invalid_actor_network_id),
		m_is_self_replicate(false)
	{
	}

	bx_player_killed_network_message::bx_player_killed_network_message(game::bc_network_client_id p_killer_client_id, 
		const game::bc_actor& p_actor, 
		core::bc_string p_body_part_force, 
		const core::bc_vector3f& p_force) noexcept
		: bci_network_message(message_name()),
		m_killer_client_id(p_killer_client_id),
		m_body_part_force(std::move(p_body_part_force)),
		m_force(p_force),
		m_is_self_replicate(false)
	{
		const auto* l_actor_network_component = p_actor.get_component<game::bc_network_component>();
		m_actor_net_id = l_actor_network_component->get_network_id();
	}

	bool bx_player_killed_network_message::need_acknowledgment() const noexcept
	{
		return true;
	}

	void bx_player_killed_network_message::execute(const game::bc_network_message_client_context& p_context) noexcept
	{
		if (m_actor == nullptr)
		{
			return;
		}

		const auto* l_controller_component = m_actor.get_component<game::bc_controller_component>();

		if(m_is_self_replicate)
		{
			auto* l_xbot_controller = static_cast<bx_player_actor_controller*>(l_controller_component->get_controller());
			l_xbot_controller->killed(m_killer_client_id, std::move(m_body_part_force), m_force);
		}
		else
		{
			auto* l_xbot_controller = static_cast<bx_network_player_actor_controller*>(l_controller_component->get_controller());
			l_xbot_controller->killed(m_killer_client_id, std::move(m_body_part_force), m_force);
		}
	}

	void bx_player_killed_network_message::serialize_message(const game::bc_network_message_serialization_context& p_context)
	{
		json_parse::bc_write(p_context.m_params, "kid", m_killer_client_id);
		json_parse::bc_write(p_context.m_params, "nid", m_actor_net_id);
		json_parse::bc_write(p_context.m_params, "frc", m_force);
		json_parse::bc_write(p_context.m_params, "prt", m_body_part_force);
	}

	void bx_player_killed_network_message::deserialize_message(const game::bc_network_message_deserialization_context& p_context)
	{
		json_parse::bc_load(p_context.m_params, "kid", m_killer_client_id);
		json_parse::bc_load(p_context.m_params, "nid", m_actor_net_id);
		json_parse::bc_load(p_context.m_params, "prt", m_body_part_force);
		json_parse::bc_load(p_context.m_params, "frc", m_force);

		std::tie(m_actor, m_is_self_replicate) = p_context.m_visitor.get_actor(m_actor_net_id);
	}
}
