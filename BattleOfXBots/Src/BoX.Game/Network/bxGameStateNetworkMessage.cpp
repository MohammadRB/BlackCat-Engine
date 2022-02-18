// [02/14/2022 MRB]

#include "Game/bcJsonParse.h"
#include "BoX.Game/Network/bxGameStateNetworkMessage.h"

namespace box
{
	bx_game_state_network_message::bx_game_state_network_message() noexcept
		: bci_network_message(message_name()),
		m_state()
	{
	}

	bx_game_state_network_message::bx_game_state_network_message(const bx_game_state& p_state) noexcept
		: bci_network_message(message_name()),
		m_state(p_state)
	{
	}

	void bx_game_state_network_message::execute(const game::bc_network_message_client_context& p_context) noexcept
	{
		auto& l_visitor = static_cast<bx_client_network_message_visitor&>(p_context.m_injected_visitor);
		l_visitor.update_game_state(m_state);
	}

	void bx_game_state_network_message::serialize_message(const game::bc_network_message_serialization_context& p_context)
	{
		json_parse::bc_write(p_context.m_params, "tm", static_cast<bcINT32>(m_state.m_game_time));
	}

	void bx_game_state_network_message::deserialize_message(const game::bc_network_message_deserialization_context& p_context)
	{
		bcINT32 l_game_time;
		json_parse::bc_load(p_context.m_params, "tm", l_game_time);

		m_state.m_game_time = l_game_time;
	}
}
