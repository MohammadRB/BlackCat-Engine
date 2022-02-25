// [02/23/2022 MRB]

#include "BoX.Game/Application/bxClientNetworkMessageVisitor.h"
#include "BoX.Game/Network/bxGameResetNetworkMessage.h"

namespace box
{
	bx_game_reset_network_message::bx_game_reset_network_message() noexcept
		: bci_network_message(message_name())
	{
	}

	bool bx_game_reset_network_message::need_acknowledgment() const noexcept
	{
		return true;
	}

	void bx_game_reset_network_message::serialize_message(const game::bc_network_message_serialization_context& p_context)
	{
	}

	void bx_game_reset_network_message::deserialize_message(const game::bc_network_message_deserialization_context& p_context)
	{
	}

	void bx_game_reset_network_message::execute(const game::bc_network_message_client_context& p_context) noexcept
	{
		auto& l_visitor = static_cast<bx_client_network_message_visitor&>(p_context.m_injected_visitor);
		l_visitor.reset_game();
	}
}
