// [02/12/2022 MRB]

#include "Game/bcJsonParse.h"
#include "BoX.Game/Network/bxTeamSelectNetworkMessage.h"
#include "BoX.Game/Application/bxClientNetworkMessageVisitor.h"
#include "BoX.Game/Application/bxServerNetworkMessageVisitor.h"

namespace box
{
	bx_team_select_network_message::bx_team_select_network_message() noexcept
		: bci_network_message(message_name()),
		m_selected_team(bx_team::blue)
	{
	}

	bx_team_select_network_message::bx_team_select_network_message(bx_team p_selected_team) noexcept
		: bci_network_message(message_name()),
		m_selected_team(p_selected_team)
	{
	}

	bool bx_team_select_network_message::need_acknowledgment() const noexcept
	{
		return true;
	}

	core::bc_string bx_team_select_network_message::get_acknowledgment_data() const noexcept
	{
		return m_error_message;
	}

	void bx_team_select_network_message::execute(const game::bc_network_message_server_context& p_context) noexcept
	{
		auto& l_visitor = static_cast<bx_server_network_message_visitor&>(p_context.m_injected_visitor);
		m_error_message = l_visitor.change_player_team(p_context.m_client.m_id, m_selected_team);
	}

	void bx_team_select_network_message::acknowledge(const game::bc_network_message_client_acknowledge_context& p_context) noexcept
	{
		if(p_context.m_ack_data.empty())
		{
			return;
		}

		auto& l_visitor = static_cast<bx_client_network_message_visitor&>(p_context.m_injected_visitor);
		l_visitor.team_change_rejected(p_context.m_ack_data);
	}

	void bx_team_select_network_message::serialize_message(const game::bc_network_message_serialization_context& p_context)
	{
		json_parse::bc_write(p_context.m_params, "tm", static_cast<bcINT32>(m_selected_team));
	}

	void bx_team_select_network_message::deserialize_message(const game::bc_network_message_deserialization_context& p_context)
	{
		bcINT32 l_team_value;
		json_parse::bc_load(p_context.m_params, "tm", l_team_value);

		m_selected_team = static_cast<bx_team>(l_team_value);
	}
}
