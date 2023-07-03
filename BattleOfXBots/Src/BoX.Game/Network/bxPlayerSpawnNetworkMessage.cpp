// [11/02/2022 MRB]

#include "Game/bcJsonParse.h"
#include "BoX.Game/Network/bxPlayerSpawnNetworkMessage.h"
#include "BoX.Game/Application/bxClientNetworkMessageVisitor.h"

namespace box
{
	bx_player_spawn_network_message::bx_player_spawn_network_message() noexcept
		: bci_network_message(message_name()),
		m_team()
	{
	}

	bx_player_spawn_network_message::bx_player_spawn_network_message(const core::bc_vector3f& p_position, bx_team p_team) noexcept
		: bci_network_message(message_name()),
		m_position(p_position),
		m_team(p_team)
	{
	}

	bool bx_player_spawn_network_message::need_acknowledgment() const noexcept
	{
		return true;
	}

	void bx_player_spawn_network_message::execute(const game::bc_network_message_client_context& p_context) noexcept
	{
		auto& l_visitor = static_cast<bx_client_network_message_visitor&>(p_context.m_injected_visitor);
		l_visitor.spawn_player(m_position, m_team);
	}

	void bx_player_spawn_network_message::serialize_message(const game::bc_network_message_serialization_context& p_context)
	{
		json_parse::bc_write(p_context.m_params, "pos", m_position);
		json_parse::bc_write(p_context.m_params, "tm", static_cast<bcINT32>(m_team));
	}

	void bx_player_spawn_network_message::deserialize_message(const game::bc_network_message_deserialization_context& p_context)
	{
		bcINT32 l_team_value;

		json_parse::bc_load(p_context.m_params, "pos", m_position);
		json_parse::bc_load(p_context.m_params, "tm", l_team_value);

		m_team = static_cast<bx_team>(l_team_value);
	}
}
